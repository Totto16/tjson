#include <allocator.h>

#include "./fuse_impl.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "./fuse_impl_run.h"

typedef _Atomic bool AtomicBool;

[[nodiscard]] static inline FuseCreateResult fuse_create_result_error(tstr_static const error) {
	return (FuseCreateResult){ .is_error = true, .data = { .error = error } };
}

[[nodiscard]] static inline FuseCreateResult fuse_create_result_ok(FUSEHandle* const ok) {
	return (FuseCreateResult){ .is_error = false, .data = { .ok = ok } };
}

[[nodiscard]] FuseCreateResult create_new_fuse_file(const char* dir, const FuseFile* files,
                                                    size_t file_amount, bool debug) {

	FUSEHandle* handle = (FUSEHandle*)allocate_shared(sizeof(FUSEHandle));

	if(handle == NULL) {
		return fuse_create_result_error(TSTR_STATIC_LIT("allocate error"));
	}

#define FREE_AT_END() \
	do { \
		free_shared(handle); \
	} while(false)

	handle->state = (FuseStaticState){
		.dir_path = dir, .files = files, .files_size = file_amount, .debug = debug
	};

	FuseRunHandle* const run_in = fuse_run_in_init(fuse_start_fn, handle);

	if(run_in == NULL) {
		FREE_AT_END();
		return fuse_create_result_error(TSTR_STATIC_LIT("run_in handle create error"));
	}

	handle->run_in = run_in;

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		auto _ = fuse_run_in_deinit(handle->run_in); \
		(void)_; \
		free_shared(handle); \
	} while(false)

	// wait for fuse initialization

	{

		FuseState state = fuse_state_uninitialized();

		while(state.type == FuseStateTypeUninitialized) {

			FuseState new_state = state;

			bool get_ok = fuse_state_get(handle->run_in, &new_state);

			if(new_state.type != FuseStateTypeUninitialized) {
				state = new_state;
			}

			if(!get_ok) {
				FREE_AT_END();
				return fuse_create_result_error(TSTR_STATIC_LIT("mutex unlock error"));
			}
		}

		if(state.type != FuseStateTypeInitializedOk) {

			FREE_AT_END();
			if(state.type == FuseStateTypeInitializedErr) {
				return fuse_create_result_error(state.data.error);
			} else {
				return fuse_create_result_error(TSTR_STATIC_LIT("invalid fuse state"));
			}
		}
	}

	return fuse_create_result_ok(handle);
}

#undef FREE_AT_END

[[nodiscard]] int clear_fuse_file(FUSEHandle* const handle) {

	const int run_result = fuse_run_in_deinit(handle->run_in);

	if(run_result != 0) {
		return run_result;
	}

	free_shared(handle);
	return 0;
}

[[nodiscard]] FuseState fuse_state_uninitialized(void) {
	return (FuseState){ .type = FuseStateTypeUninitialized, .data = {} };
}

[[nodiscard]] FuseState fuse_state_error(tstr_static const error) {
	return (FuseState){ .type = FuseStateTypeInitializedErr, .data = { .error = error } };
}

[[nodiscard]] FuseState fuse_state_ok(void) {
	return (FuseState){ .type = FuseStateTypeInitializedOk, .data = {} };
}

#if FUSE_RUN_FILESYSTEM_IN == 0
struct FuseRunHandleImpl {
	pthread_mutex_t mutex;
	FuseState fuse_state;
	//
	pthread_t handler_thread;
	//
	AtomicBool session_finished;
};

[[nodiscard]] FuseRunHandle* fuse_run_in_init(FuseHandleFn start_fn, UserData* const userdata) {

	FuseRunHandle* handle = (FuseRunHandle*)allocate_shared(sizeof(FuseRunHandle));

	if(handle == NULL) {
		return NULL;
	}

	#define FREE_AT_END() \
		do { \
			free_shared(handle); \
		} while(false)

	int result = pthread_mutex_init(&(handle->mutex), NULL);
	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	handle->session_finished = false;
	handle->fuse_state = fuse_state_uninitialized();

	result = pthread_create(&(handle->handler_thread), NULL, (void* (*)(void*))start_fn,
	                        (void*)userdata);

	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	return handle;
}

	#undef FREE_AT_END

[[nodiscard]] int fuse_run_in_deinit(FuseRunHandle* handle) {
	switch(handle->fuse_state.type) {
		case FuseStateTypeUninitialized: {
			break;
		}
		case FuseStateTypeInitializedErr: {
			break;
		}
		case FuseStateTypeInitializedOk: {

			// first exit the session, signal that via a signal
			int result = pthread_kill(handle->handler_thread, SIGNAL_FOR_FUSE_EXIT_REQUEST);
			if(result != 0) {
				return -1;
			}

			// now force the blocking function to wake up
			int pthread_res = pthread_kill(handle->handler_thread, SIGPIPE);
			if(pthread_res != 0) {
				return -2;
			}

			FuseHandleResult return_value = THREAD_SUCCESS;
			result = pthread_join(handle->handler_thread, &return_value);
			if(result != 0) {
				return -3;
			}

			if(return_value != THREAD_SUCCESS) {
				return -4;
			}

			break;
		}
		default: {
			return -5;
		}
	}

	int result = pthread_mutex_destroy(&(handle->mutex));
	if(result != 0) {
		return -6;
	}

	free_shared(handle);

	return 0;
}

void fuse_set_session_finished(FuseRunHandle* const handle) {
	handle->session_finished = true;
}

[[nodiscard]] void* allocate_shared(size_t size) {
	return malloc(size);
}

void free_shared(void* data) {
	free(data);
}

#else
typedef struct {
	pid_t pid;
} ProcessInfo;

struct FuseRunHandleImpl {
	pthread_mutex_t mutex;
	FuseState fuse_state;
	//
	ProcessInfo process_info;
	//
	AtomicBool session_finished;
};

typedef FuseHandleResult(ProcessCreateFn)(UserData* const);

[[nodiscard]] static int create_process(ProcessInfo* out_info, ProcessCreateFn create_fn,
                                        UserData* const userdata) {

	pid_t result = fork();

	if(result == 0) {
		// we are in the child
		FuseHandleResult fn_res = create_fn(userdata);
		exit((int)fn_res);
	}

	if(result == -1) {
		return -1;
	}

	*out_info = (ProcessInfo){ .pid = result };

	return 0;
}

static void destroy_process_info(ProcessInfo info) {
	(void)info;
	// NOOP
}

[[nodiscard]] FuseRunHandle* fuse_run_in_init(FuseHandleFn start_fn, UserData* const userdata) {

	FuseRunHandle* handle = (FuseRunHandle*)allocate_shared(sizeof(FuseRunHandle));

	if(handle == NULL) {
		return NULL;
	}

	#define FREE_AT_END() \
		do { \
			free_shared(handle); \
		} while(false)

	pthread_mutexattr_t attr = {};

	int result = pthread_mutexattr_init(&attr);
	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	result = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	result = pthread_mutex_init(&(handle->mutex), &attr);
	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	handle->session_finished = false;
	handle->fuse_state = fuse_state_uninitialized();

	result = create_process(&(handle->process_info), start_fn, userdata);

	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	return handle;
}

	#undef FREE_AT_END

	#define SIGPIPE_INTERVAL_USEC (10 * 1000)

[[nodiscard]] int fuse_run_in_deinit(FuseRunHandle* const handle) {
	switch(handle->fuse_state.type) {
		case FuseStateTypeUninitialized: {
			break;
		}
		case FuseStateTypeInitializedErr: {
			break;
		}
		case FuseStateTypeInitializedOk: {

			// first exit the session, signal that via a signal
			int result = kill(handle->process_info.pid, SIGNAL_FOR_FUSE_EXIT_REQUEST);
			if(result != 0) {
				return -1;
			}

			// now force the blocking function to wake up
			int pthread_res = kill(handle->process_info.pid, SIGPIPE);
			if(pthread_res != 0) {
				return -2;
			}

			int return_status = 0;
			// periodically check if the child exited otherwise send a SIGPIPE again
			while(true) {

				result = waitpid(handle->process_info.pid, &return_status, WNOHANG);

				if(result == handle->process_info.pid) {
					break;
				}

				if(result != 0) {
					return -3;
				}

				if(handle->session_finished) {
					// the session finished, we wait until the rest is finished too
					result = waitpid(handle->process_info.pid, &return_status, 0);

					if(result != handle->process_info.pid) {
						return -4;
					}

					break;
				}

				pthread_res = kill(handle->process_info.pid, SIGPIPE);
				if(pthread_res != 0) {
					return -5;
				}

				result = usleep(SIGPIPE_INTERVAL_USEC);
				if(result != 0) {
					return -6;
				}
			}

			if(!(WIFEXITED(return_status))) {
				return -7;
			}

			if(WIFSIGNALED(return_status)) {
				return -8;
			}

			FuseHandleResult return_value = (FuseHandleResult)(WEXITSTATUS(return_status));

			if(return_value != THREAD_SUCCESS) {
				return -9;
			}

			break;
		}
		default: {
			return -10;
		}
	}

	int result = pthread_mutex_destroy(&(handle->mutex));
	if(result != 0) {
		return -11;
	}

	destroy_process_info(handle->process_info);

	free_shared(handle);

	return 0;
}

void fuse_set_session_finished(FuseRunHandle* const handle) {
	handle->session_finished = true;
}

typedef struct {
	size_t length;
} SharedAllocHeader;

static_assert((sizeof(SharedAllocHeader) % 8) == 0);

[[nodiscard]] void* allocate_shared(size_t size) {

	const size_t length = sizeof(SharedAllocHeader) + size;

	void* ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(ptr == MAP_FAILED) {
		return NULL;
	}

	SharedAllocHeader* header = (SharedAllocHeader*)ptr;

	*header = (SharedAllocHeader){ .length = length };

	return (void*)(header + 1);
}

void free_shared(void* data) {

	SharedAllocHeader* header = (((SharedAllocHeader*)data) - 1);

	size_t length = header->length;

	int result = munmap((void*)header, length);

	if(result != 0) {
		// TODO: handle error
		fprintf(stderr, "ERROR in munmap: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

#endif

[[nodiscard]] bool fuse_state_set(FuseRunHandle* const handle, FuseState state) {
	int result = pthread_mutex_lock(&(handle->mutex));
	if(result != 0) {
		return false;
	}

	handle->fuse_state = state;

	result = pthread_mutex_unlock(&(handle->mutex));
	if(result != 0) {
		return false;
	}

	return true;
}

[[nodiscard]] bool fuse_state_get(FuseRunHandle* handle, FuseState* state) {
	int result = pthread_mutex_lock(&(handle->mutex));
	if(result != 0) {
		return false;
	}

	*state = handle->fuse_state;

	result = pthread_mutex_unlock(&(handle->mutex));
	if(result != 0) {
		return false;
	}

	return true;
}
