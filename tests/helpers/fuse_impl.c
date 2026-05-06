#include <allocator.h>

#include "./fuse_impl.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define FUSE_USE_VERSION FUSE_MAKE_VERSION(3, 12)
#include <fuse3/fuse.h>

#include <fuse3/fuse_lowlevel.h>

typedef struct {
	const void* data;
	size_t size;
} Buffer;

typedef enum {
	FuseStateTypeUninitialized = 0,
	FuseStateTypeInitializedOk,
	FuseStateTypeInitializedErr,
} FuseStateType;

// manual "variant", but only used internally, so it's fine
typedef struct {
	FuseStateType type;
	union {
		tstr_static error;
	} data;
} FuseState;

[[nodiscard]] static inline FuseState fuse_state_uninitialized(void) {
	return (FuseState){ .type = FuseStateTypeUninitialized, .data = {} };
}

[[nodiscard]] static inline FuseState fuse_state_error(tstr_static const error) {
	return (FuseState){ .type = FuseStateTypeInitializedErr, .data = { .error = error } };
}

[[nodiscard]] static inline FuseState fuse_state_ok(void) {
	return (FuseState){ .type = FuseStateTypeInitializedOk, .data = {} };
}

struct FUSEHandleImpl {
	const char* file_path;
	Buffer content;
	//
	pthread_mutex_t mutex;
	pthread_t thread;
	//
	FuseState fuse_state;
};

// TODO: remove
#define UNUSED(v) ((void)(v))

static void fuse_lowlevel_op_init(void* userdata, struct fuse_conn_info* conn) {
	// TODO: is this correct

	(void)userdata;

	// Disable the receiving and processing of FUSE_INTERRUPT requests
	conn->no_interrupt = 1;

	// Test setting flags the old way
	// TODO
	//  conn->want = FUSE_CAP_ASYNC_READ;
	//  conn->want &= ~FUSE_CAP_ASYNC_READ;
}

static void fuse_lowlevel_op_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
	// TODO
	UNUSED(ino);
	UNUSED(fi);
	fuse_reply_err(req, ENOENT);
}

static void fuse_lowlevel_op_lookup(fuse_req_t req, fuse_ino_t parent, const char* name) {
	// TODO
	UNUSED(parent);
	UNUSED(name);
	fuse_reply_err(req, ENOENT);
}

static void fuse_lowlevel_op_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                     struct fuse_file_info* fi) {
	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(off);
	UNUSED(fi);

	fuse_reply_err(req, ENOTDIR);
}

static void fuse_lowlevel_op_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
	if(ino != 2)
		fuse_reply_err(req, EISDIR);
	else if((fi->flags & O_ACCMODE) != O_RDONLY)
		fuse_reply_err(req, EACCES);
	else
		fuse_reply_err(req, ENODEV);
}

static void fuse_lowlevel_op_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                  struct fuse_file_info* fi) {
	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(off);
	UNUSED(fi);

	fuse_reply_err(req, ENODEV);
}

static void fuse_lowlevel_op_getxattr(fuse_req_t req, fuse_ino_t ino, const char* name,
                                      size_t size) {

	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(name);

	fuse_reply_err(req, ENOTSUP);
}

static void fuse_lowlevel_op_setxattr(fuse_req_t req, fuse_ino_t ino, const char* name,
                                      const char* value, size_t size, int flags) {

	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(name);
	UNUSED(value);
	UNUSED(flags);

	fuse_reply_err(req, ENOTSUP);
}

static void fuse_lowlevel_op_removexattr(fuse_req_t req, fuse_ino_t ino, const char* name) {
	// TODO
	UNUSED(ino);
	UNUSED(name);

	fuse_reply_err(req, ENOTSUP);
}

static const struct fuse_lowlevel_ops fuse_lowlevel_operations = {
	.init = fuse_lowlevel_op_init,
	.lookup = fuse_lowlevel_op_lookup,
	.getattr = fuse_lowlevel_op_getattr,
	.readdir = fuse_lowlevel_op_readdir,
	.open = fuse_lowlevel_op_open,
	.read = fuse_lowlevel_op_read,
	.setxattr = fuse_lowlevel_op_setxattr,
	.getxattr = fuse_lowlevel_op_getxattr,
	.removexattr = fuse_lowlevel_op_removexattr,
};

[[nodiscard]] static struct fuse_session* fuse_initialize_impl(FUSEHandle* const handle,
                                                               struct fuse_args* const args,
                                                               tstr_static* const error) {

	struct fuse_session* session = fuse_session_new(
	    args, &fuse_lowlevel_operations, sizeof(fuse_lowlevel_operations), (void*)handle);

	if(session == NULL) {
		*error = TSTR_STATIC_LIT("session new failed");
		return NULL;
	}

	if(fuse_set_signal_handlers(session) != 0) {
		*error = TSTR_STATIC_LIT("setting signal handlers failed");
		return NULL;
	}

	if(fuse_session_mount(session, handle->file_path) != 0) {
		*error = TSTR_STATIC_LIT("session mount failed");
		return NULL;
	}

	*error = tstr_static_null();
	return session;
}

#define THREAD_SUCCESS ((void*)(20))

#define THREAD_ERROR ((void*)(21))

// runs on a new thread
[[nodiscard]] static void* fuse_thread_fn(void* const thread_arg) {

	FUSEHandle* handle = (FUSEHandle*)thread_arg;

	tstr_static error = tstr_static_null();

	const size_t argv_count = 2;

	char** const argv = TJSON_MALLOC((argv_count + 1) * sizeof(char*));

	if(argv == NULL) {
		return THREAD_ERROR;
	}

	argv[0] = strdup("fuse_impl_dummy_argv0");
	argv[1] = strdup(handle->file_path);
	argv[argv_count] = NULL;

	struct fuse_args dummy_args = { .argc = 2, .argv = argv, .allocated = (int)false };

	struct fuse_session* session = fuse_initialize_impl(handle, &dummy_args, &error);

	int result = pthread_mutex_lock(&handle->mutex);
	if(result != 0) {
		return THREAD_ERROR;
	}
	if(session == NULL) {
		if(tstr_static_is_null(error)) {
			handle->fuse_state = fuse_state_error(TSTR_STATIC_LIT("Unkown error"));
		} else {
			handle->fuse_state = fuse_state_error(error);
		}
	} else {
		handle->fuse_state = fuse_state_ok();
	}

	result = pthread_mutex_unlock(&handle->mutex);
	if(result != 0) {
		return THREAD_ERROR;
	}

	if(session == NULL) {
		return THREAD_ERROR;
	}

	// loop untile we are finished

	/* Block until SIGINT */
	int ret = fuse_session_loop(session);

	fuse_session_unmount(session);

	fuse_remove_signal_handlers(session);

	fuse_session_destroy(session);

	{ // free argv
		for(size_t i = 0; i < argv_count; ++i) {
			// use free, as we use strdup
			free(argv[i]);
		}
		TJSON_FREE(argv);
	}

	if(ret != 0) {
		return THREAD_ERROR;
	}

	return THREAD_SUCCESS;
}

[[nodiscard]] static inline FuseCreateResult fuse_create_result_error(tstr_static const error) {
	return (FuseCreateResult){ .is_error = true, .data = { .error = error } };
}

[[nodiscard]] static inline FuseCreateResult fuse_create_result_ok(FUSEHandle* const ok) {
	return (FuseCreateResult){ .is_error = false, .data = { .ok = ok } };
}

[[nodiscard]] FuseCreateResult create_new_fuse_file(const char* const file, const void* const data,
                                                    const size_t data_size) {

	FUSEHandle* handle = (FUSEHandle*)TJSON_MALLOC(sizeof(FUSEHandle));

	if(handle == NULL) {
		return fuse_create_result_error(TSTR_STATIC_LIT("malloc error"));

#define FREE_AT_END() \
	do { \
		TJSON_FREE(handle); \
	} while(false)
	}

	handle->file_path = file;
	handle->content = (Buffer){ .data = data, .size = data_size };

	int result = pthread_mutex_init(&handle->mutex, NULL);
	if(result != 0) {
		FREE_AT_END();
		return fuse_create_result_error(TSTR_STATIC_LIT("mutex init error"));
	}

	handle->fuse_state = fuse_state_uninitialized();

	result = pthread_create(&(handle->thread), NULL, fuse_thread_fn, handle);

	if(result != 0) {
		FREE_AT_END();
		return fuse_create_result_error(TSTR_STATIC_LIT("pthread create error"));
	}

	// wait for fuse initialization

	{

		FuseState state = fuse_state_uninitialized();

		while(state.type == FuseStateTypeUninitialized) {

			result = pthread_mutex_lock(&handle->mutex);
			if(result != 0) {
				FREE_AT_END();
				return fuse_create_result_error(TSTR_STATIC_LIT("mutex lock error"));
			}

			const FuseState new_state = handle->fuse_state;

			if(new_state.type != FuseStateTypeUninitialized) {
				state = new_state;
			}

			result = pthread_mutex_unlock(&handle->mutex);
			if(result != 0) {
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

[[nodiscard]] bool clear_fuse_file(FUSEHandle* const handle) {

	int pthread_res = pthread_kill(handle->thread, SIGINT);
	if(pthread_res != 0) {
		return false;
	}

	void* return_value = THREAD_SUCCESS;
	int result = pthread_join(handle->thread, &return_value);
	if(result != 0) {
		return false;
	}

	if(return_value != THREAD_SUCCESS) {
		return false;
	}

	result = pthread_mutex_destroy(&handle->mutex);
	if(result != 0) {
		return false;
	}

	TJSON_FREE(handle);
	return true;
}
