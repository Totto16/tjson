#include <allocator.h>

#include "./fuse_impl.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define FUSE_USE_VERSION FUSE_MAKE_VERSION(3, 12)
#include <fuse3/fuse.h>

#include <fuse3/fuse_lowlevel.h>

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
	const char* dir_path;
	const FuseFile* files;
	size_t files_size;
	//
	pthread_mutex_t mutex;
	pthread_t thread;
	//
	FuseState fuse_state;
};

typedef FUSEHandle UserData;

// TODO: remove
#define UNUSED(v) ((void)(v))

static void fuse_lowlevel_op_init(void* userdata, struct fuse_conn_info* conn) {

	(void)userdata;

	// Disable the receiving and processing of FUSE_INTERRUPT requests
	conn->no_interrupt = 1;
}

#define INO_ROOT_FOLDER 1
#define INO_START_FILES 2

[[nodiscard]] static int stat_helper_folder_impl(fuse_ino_t ino, struct stat* stbuf) {
	stbuf->st_ino = ino;
	switch(ino) {
		case INO_ROOT_FOLDER: {

			struct fuse_context* ctx = fuse_get_context();

			UserData* handle = ctx->private_data;

			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1 + handle->files_size;
			break;
		}

		default: return -1;
	}
	return 0;
}

[[nodiscard]] static int stat_helper_file_impl(fuse_ino_t ino, struct stat* stbuf,
                                               const FuseBuffer* const buf) {
	stbuf->st_ino = ino;
	switch(ino) {
		case INO_ROOT_FOLDER: return -1;

		default: {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = (off_t)buf->size;
			break;
		}
	}
	return 0;
}

static int stat_helper_ino_impl(fuse_ino_t ino, struct stat* stbuf) {
	stbuf->st_ino = ino;
	switch(ino) {
		case 1: return stat_helper_folder_impl(ino, stbuf);

		default: {
			if(ino <= INO_ROOT_FOLDER) {
				return -1;
			}

			struct fuse_context* ctx = fuse_get_context();

			UserData* handle = ctx->private_data;

			if(ino >= INO_START_FILES + handle->files_size) {
				return -1;
			}

			const size_t i = ino - INO_START_FILES;

			if(i >= handle->files_size) {
				fuse_log(FUSE_LOG_EMERG,
				         "ino calculation implementation error: %zu is out of bounds %zu\n", i,
				         handle->files_size);
				return -1;
			}

			const FuseFile file = handle->files[i];

			return stat_helper_file_impl(ino, stbuf, &file.content);
		}
	}
	return 0;
}

static void fuse_lowlevel_op_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "getattr called\n");

	struct stat stbuf;

	(void)fi;

	memset(&stbuf, 0, sizeof(stbuf));
	if(stat_helper_ino_impl(ino, &stbuf) == -1) {
		fuse_reply_err(req, ENOENT);
	} else {
		fuse_reply_attr(req, &stbuf, 1.0);
	}
}

static void fuse_lowlevel_op_lookup(fuse_req_t req, fuse_ino_t parent, const char* name) {
	fuse_log(FUSE_LOG_DEBUG, "lookup called\n");

	if(parent != INO_ROOT_FOLDER) {
		// the top level directory, where we mounted is inode 1, we only support files inside
		// that

		fuse_reply_err(req, ENOENT);
		return;
	}

	struct fuse_context* ctx = fuse_get_context();

	UserData* handle = ctx->private_data;

	for(size_t i = 0; i < handle->files_size; ++i) {
		const FuseFile file = handle->files[i];
		if(strcmp(name, file.name) != 0) {
			struct fuse_entry_param e;
			memset(&e, 0, sizeof(e));
			e.ino = INO_START_FILES + i;
			e.attr_timeout = 1.0;
			e.entry_timeout = 1.0;
			if(stat_helper_file_impl(e.ino, &e.attr, &file.content) != 0) {
				fuse_reply_err(req, ENOENT);
				fuse_log(FUSE_LOG_DEBUG, "lookup returned error : \n");
				return;
			}

			fuse_reply_entry(req, &e);
			fuse_log(FUSE_LOG_DEBUG, "lookup returned valid file: %s\n", file.name);
			return;
		}
	}

	fuse_reply_err(req, ENOENT);
}

static void fuse_lowlevel_op_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                     struct fuse_file_info* fi) {
	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(off);
	UNUSED(fi);

	fuse_log(FUSE_LOG_DEBUG, "readdir called\n");

	fuse_reply_err(req, ENOTDIR);
}

static void fuse_lowlevel_op_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {

	fuse_log(FUSE_LOG_DEBUG, "op_open called\n");

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

	fuse_log(FUSE_LOG_DEBUG, "op_read called\n");

	fuse_reply_err(req, ENODEV);
}

static void fuse_lowlevel_op_getxattr(fuse_req_t req, fuse_ino_t ino, const char* name,
                                      size_t size) {

	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(name);

	fuse_log(FUSE_LOG_DEBUG, "getxattr called\n");

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

	fuse_log(FUSE_LOG_DEBUG, "setxattr called\n");

	fuse_reply_err(req, ENOTSUP);
}

static void fuse_lowlevel_op_removexattr(fuse_req_t req, fuse_ino_t ino, const char* name) {
	// TODO
	UNUSED(ino);
	UNUSED(name);

	fuse_log(FUSE_LOG_DEBUG, "removexattr called\n");

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

[[nodiscard]] static struct fuse_session* fuse_initialize_impl(UserData* const handle,
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

	if(fuse_session_mount(session, handle->dir_path) != 0) {
		*error = TSTR_STATIC_LIT("session mount failed");
		return NULL;
	}

	*error = tstr_static_null();
	return session;
}

static void fuse_log_impl(enum fuse_log_level level, const char* fmt, va_list ap) {
	switch(level) {
		case FUSE_LOG_EMERG: {
			fprintf(stderr, "EMERG: ");
			break;
		}
		case FUSE_LOG_ALERT: {
			fprintf(stderr, "ALERT: ");
			break;
		}
		case FUSE_LOG_CRIT: {
			fprintf(stderr, "CRIT: ");
			break;
		}
		case FUSE_LOG_ERR: {
			fprintf(stderr, "ERR: ");
			break;
		}
		case FUSE_LOG_WARNING: {
			fprintf(stderr, "WARNING: ");
			break;
		}
		case FUSE_LOG_NOTICE: {
			fprintf(stderr, "NOTICE: ");
			break;
		}
		case FUSE_LOG_INFO: {
			fprintf(stderr, "INFO: ");
			break;
		}
		case FUSE_LOG_DEBUG: {
			fprintf(stderr, "DEBUG: ");
			break;
		}
		default: {
			break;
		}
	}
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
}

#define THREAD_SUCCESS ((void*)(20))

#define THREAD_ERROR ((void*)(21))

// runs on a new thread
[[nodiscard]] static void* fuse_thread_fn(void* const thread_arg) {

	FUSEHandle* handle = (UserData*)thread_arg;

	// setup logging

	fuse_set_log_func(fuse_log_impl);

	// initialize

	tstr_static error = tstr_static_null();

	const size_t argv_count = 1;

	char** const argv = TJSON_MALLOC((argv_count + 1) * sizeof(char*));

	if(argv == NULL) {
		return THREAD_ERROR;
	}

	argv[0] = strdup("fuse_impl_dummy_argv0");
	argv[argv_count] = NULL;

	struct fuse_args dummy_args = { .argc = (int)argv_count,
		                            .argv = argv,
		                            .allocated = (int)false };

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

	// loop until we are finished

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

[[nodiscard]] FuseCreateResult create_new_fuse_file(const char* dir, const FuseFile* files,
                                                    size_t file_amount) {

	FUSEHandle* handle = (FUSEHandle*)TJSON_MALLOC(sizeof(FUSEHandle));

	if(handle == NULL) {
		return fuse_create_result_error(TSTR_STATIC_LIT("malloc error"));

#define FREE_AT_END() \
	do { \
		TJSON_FREE(handle); \
	} while(false)
	}

	handle->dir_path = dir;
	handle->files = files;
	handle->files_size = file_amount;

	int result = pthread_mutex_init(&handle->mutex, NULL);
	if(result != 0) {
		FREE_AT_END();
		return fuse_create_result_error(TSTR_STATIC_LIT("mutex init error"));
	}

	handle->fuse_state = fuse_state_uninitialized();

	result = pthread_create(&(handle->thread), NULL, fuse_thread_fn, (void*)handle);

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
