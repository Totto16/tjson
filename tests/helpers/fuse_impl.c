#include "./fuse_impl.h"

#include <allocator.h>

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
	FuseStateUninitialized = 0,
	FuseStateInitializedOk,
	FuseStateInitializedErr,
} FuseState;

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

[[nodiscard]] static struct fuse_session* fuse_initialize_impl(FUSEHandle* const handle) {

	struct fuse_args dummy_args = { .argc = 0, .argv = NULL, .allocated = (int)false };

	struct fuse_session* se = fuse_session_new(&dummy_args, &fuse_lowlevel_operations,
	                                           sizeof(fuse_lowlevel_operations), (void*)handle);

	if(se == NULL) {
		return NULL;
	}

	if(fuse_set_signal_handlers(se) != 0) {
		return NULL;
	}

	if(fuse_session_mount(se, handle->file_path) != 0) {
		return NULL;
	}

	return se;
}

#define THREAD_SUCCESS ((void*)(20))

#define THREAD_ERROR ((void*)(21))

// runs on a new thread
[[nodiscard]] static void* fuse_thread_fn(void* const thread_arg) {

	FUSEHandle* handle = (FUSEHandle*)thread_arg;

	struct fuse_session* session = fuse_initialize_impl(handle);

	int result = pthread_mutex_lock(&handle->mutex);
	if(result != 0) {
		return THREAD_ERROR;
	}

	handle->fuse_state = session == NULL ? FuseStateInitializedErr : FuseStateInitializedOk;

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

	if(ret != 0) {
		return THREAD_ERROR;
	}

	return THREAD_SUCCESS;
}

[[nodiscard]] FUSEHandle* create_new_fuse_file(const char* const file, const void* const data,
                                               const size_t data_size) {

	FUSEHandle* handle = (FUSEHandle*)TJSON_MALLOC(sizeof(FUSEHandle));

	if(handle == NULL) {
		return NULL;

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
		return NULL;
	}

	handle->fuse_state = FuseStateUninitialized;

	result = pthread_create(&(handle->thread), NULL, fuse_thread_fn, handle);

	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	// wait for fuse initialization

	{

		FuseState state = FuseStateUninitialized;

		while(state == FuseStateUninitialized) {

			result = pthread_mutex_lock(&handle->mutex);
			if(result != 0) {
				FREE_AT_END();
				return NULL;
			}

			const FuseState new_state = handle->fuse_state;

			if(new_state != FuseStateUninitialized) {
				state = new_state;
			}

			result = pthread_mutex_unlock(&handle->mutex);
			if(result != 0) {
				FREE_AT_END();
				return NULL;
			}
		}

		if(state != FuseStateInitializedOk) {

			FREE_AT_END();
			return NULL;
		}
	}

	return handle;
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
