#include <allocator.h>

#include "./fuse_impl.h"

#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "helpers/fuse/fuse_helper.h"

[[nodiscard]] static inline FuseCreateResult fuse_create_result_error(tstr_static const error) {
	return (FuseCreateResult){ .is_error = true, .data = { .error = error } };
}

[[nodiscard]] static inline FuseCreateResult fuse_create_result_ok(FUSEHandle* const ok) {
	return (FuseCreateResult){ .is_error = false, .data = { .ok = ok } };
}

typedef struct {
	pid_t pid;
} ProcessInfo;

struct FUSEHandleImpl {
	FuseSharedState* state;
	SharedAllocator* allocator;
	FuseStaticData data;
	ProcessInfo process_info;
};

[[nodiscard]] int fuse_shared_init(const SharedAllocator* allocator, FuseSharedState* state,
                                   ProcessInfo* process_info);

[[nodiscard]] int fuse_shared_state_deinit(FuseSharedState* shared_state,
                                           const ProcessInfo* process_info);

[[nodiscard]] FuseCreateResult create_new_fuse_file(const char* dir, const FuseFile* files,
                                                    size_t file_amount, bool debug) {

	const size_t TODO = (size_t)-1;

	SharedAllocatorResult allocator_result = new_shared_allocator(TODO);

	if(allocator_result.allocator == NULL || allocator_result.memory.state == NULL) {
		return fuse_create_result_error(TSTR_STATIC_LIT("shared allocator error"));
	}

	SharedAllocator* allocator = allocator_result.allocator;

	FUSEHandle* handle = (FUSEHandle*)malloc(sizeof(FUSEHandle));

	if(handle == NULL) {
		return fuse_create_result_error(TSTR_STATIC_LIT("allocate error"));
	}

	handle->state = allocator_result.memory.state;
	handle->allocator = allocator;
	handle->process_info = (ProcessInfo){ .pid = 0 };

#define FREE_AT_END() \
	do { \
		shared_allocator_deinit(allocator); \
		free(handle); \
	} while(false)

	handle->data = (FuseStaticData){ .dir_path = dir,
		                             .files = (FuseFiles){ .data = files, .size = file_amount },
		                             .debug = debug };

	int result = fuse_shared_init(handle->allocator, handle->state, &(handle->process_info));

	if(result != 0) {
		FREE_AT_END();
		return fuse_create_result_error(TSTR_STATIC_LIT("shared state create error"));
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		auto _ = fuse_shared_state_deinit(handle->state, &(handle->process_info)); \
		(void)_; \
		shared_allocator_deinit(allocator); \
		free(handle); \
	} while(false)

	// wait for fuse initialization

	{

		FuseState state = fuse_state_uninitialized();

		while(state.type == FuseStateTypeUninitialized) {

			FuseState new_state = state;

			bool get_ok = fuse_shared_state_get_state(handle->state, &new_state);

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

	int result = fuse_shared_state_deinit(handle->state, &(handle->process_info));

	if(result != 0) {
		return result;
	}

	shared_allocator_deinit(handle->allocator);

	free(handle);
	return 0;
}

[[nodiscard]] static int create_process(ProcessInfo* out_info,
                                        const SharedAllocator* const allocator) {

	pid_t result = fork();

	if(result == 0) {
		// we are in the child

		const char* program = FUSE_PROGRAM_PATH;

		char* allocator_path = shared_allocator_get_path(allocator);

		char* program_dup = strdup(program);

		char* args[] = { program_dup, allocator_path, NULL };

		execv(args[0], args);
		exit(EXIT_FAILURE);
		abort();
		return -3;
	}

	if(result == -1) {
		return -1;
	}

	*out_info = (ProcessInfo){ .pid = result };

	return 0;
}

[[nodiscard]] int fuse_shared_init(const SharedAllocator* const allocator,
                                   FuseSharedState* const state, ProcessInfo* const process_info) {

	pthread_mutexattr_t attr = {};

	int result = pthread_mutexattr_init(&attr);
	if(result != 0) {
		return -1;
	}

	result = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if(result != 0) {
		return -2;
	}

	result = pthread_mutex_init(&(state->mutex), &attr);
	if(result != 0) {
		return -3;
	}

	state->session_finished = false;
	state->fuse_state = fuse_state_uninitialized();

	result = create_process(process_info, allocator);

	if(result != 0) {
		return -4;
	}

	return 0;
}

#undef FREE_AT_END

#define SIGPIPE_INTERVAL_USEC (10 * 1000)

[[nodiscard]] int fuse_shared_state_deinit(FuseSharedState* const shared_state,
                                           const ProcessInfo* const process_info) {
	switch(shared_state->fuse_state.type) {
		case FuseStateTypeUninitialized: {
			break;
		}
		case FuseStateTypeInitializedErr: {
			break;
		}
		case FuseStateTypeInitializedOk: {

			// first exit the session, signal that via a signal
			int result = kill(process_info->pid, SIGNAL_FOR_FUSE_EXIT_REQUEST);
			if(result != 0) {
				return -1;
			}

			// now force the blocking function to wake up
			int pthread_res = kill(process_info->pid, SIGPIPE);
			if(pthread_res != 0) {
				return -2;
			}

			int return_status = 0;
			// periodically check if the child exited otherwise send a SIGPIPE again
			while(true) {

				result = waitpid(process_info->pid, &return_status, WNOHANG);

				if(result == process_info->pid) {
					break;
				}

				if(result != 0) {
					return -3;
				}

				if(shared_state->session_finished) {
					// the session finished, we wait until the rest is finished too
					result = waitpid(process_info->pid, &return_status, 0);

					if(result != process_info->pid) {
						return -4;
					}

					break;
				}

				pthread_res = kill(process_info->pid, SIGPIPE);
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

	int result = pthread_mutex_destroy(&(shared_state->mutex));
	if(result != 0) {
		return -11;
	}

	return 0;
}
