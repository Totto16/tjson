#include <allocator.h>

#include "./fuse_impl.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define FUSE_USE_VERSION FUSE_MAKE_VERSION(3, 12)
#include <fuse3/fuse.h>

#include <fuse3/fuse_lowlevel.h>

// don't use those here
#undef TJSON_MALLOC
#undef TJSON_CALLOC
#undef TJSON_REALLOC
#undef TJSON_FREE

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

// Define the type, that we run fuse in, 0 means another thread, 1 means another process
#define FUSE_RUN_FILESYSTEM_IN 1

// see https://github.com/libfuse/libfuse/issues/410
// on why this is necessary

#if !defined(FUSE_RUN_FILESYSTEM_IN)
	#error "need 'FUSE_RUN_FILESYSTEM_IN' to be defined"
#elif FUSE_RUN_FILESYSTEM_IN == 0

typedef void* FuseHandleResult;
	#define THREAD_SUCCESS ((FuseHandleResult)(20))

	#define THREAD_ERROR ((FuseHandleResult)(21))

#elif FUSE_RUN_FILESYSTEM_IN == 1
typedef uint8_t FuseHandleResult;
	#define THREAD_SUCCESS ((FuseHandleResult)(0))

	#define THREAD_ERROR ((FuseHandleResult)(1))
#else
	#error "'FUSE_RUN_FILESYSTEM_IN' can only be 0 or 1"
#endif

typedef struct FuseRunHandleImpl FuseRunHandle;

typedef FUSEHandle UserData;

typedef FuseHandleResult (*FuseHandleFn)(UserData* const data);

typedef _Atomic bool AtomicBool;

[[nodiscard]] FuseRunHandle* fuse_run_in_init(FuseHandleFn start_fn, UserData* const userdata);

[[nodiscard]] bool fuse_state_set(FuseRunHandle* handle, FuseState state);

[[nodiscard]] bool fuse_state_get(FuseRunHandle* handle, FuseState* state);

[[nodiscard]] bool fuse_run_in_deinit(FuseRunHandle* handle);

void fuse_set_session_finished(FuseRunHandle* handle);

[[nodiscard]] void* allocate_shared(size_t size);

void free_shared(void* data);

typedef struct {
	const char* dir_path;
	const FuseFile* files;
	size_t files_size;
	bool debug;
} FuseStaticState;

struct FUSEHandleImpl {
	FuseStaticState state;
	FuseRunHandle* run_in;
};

// TODO: remove
#define UNUSED(v) ((void)(v))

static void fuse_lowlevel_op_init(void* userdata, struct fuse_conn_info* conn) {

	(void)userdata;

	// Disable the receiving and processing of FUSE_INTERRUPT requests
	conn->no_interrupt = 1;
}

#define INO_ROOT_FOLDER 1
#define INO_START_FILES 2

[[nodiscard]] static int stat_helper_folder_impl(fuse_ino_t ino, struct stat* stbuf,
                                                 UserData* const handle) {
	stbuf->st_ino = ino;
	switch(ino) {
		case INO_ROOT_FOLDER: {

			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1 + handle->state.files_size;
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

static int stat_helper_ino_impl(fuse_ino_t ino, struct stat* stbuf, UserData* handle) {
	stbuf->st_ino = ino;
	switch(ino) {
		case 1: return stat_helper_folder_impl(ino, stbuf, handle);

		default: {
			if(ino <= INO_ROOT_FOLDER) {
				return -1;
			}

			if(ino >= INO_START_FILES + handle->state.files_size) {
				return -1;
			}

			const size_t i = ino - INO_START_FILES;

			if(i >= handle->state.files_size) {
				fuse_log(FUSE_LOG_EMERG,
				         "ino calculation implementation error: %zu is out of bounds %zu\n", i,
				         handle->state.files_size);
				return -1;
			}

			const FuseFile file = handle->state.files[i];

			return stat_helper_file_impl(ino, stbuf, &file.content);
		}
	}
	return 0;
}

static void fuse_lowlevel_op_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "getattr called\n");

	struct stat stbuf;

	(void)fi;

	UserData* handle = fuse_req_userdata(req);

	memset(&stbuf, 0, sizeof(stbuf));
	if(stat_helper_ino_impl(ino, &stbuf, handle) == -1) {
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

	UserData* handle = fuse_req_userdata(req);

	for(size_t i = 0; i < handle->state.files_size; ++i) {
		const FuseFile file = handle->state.files[i];

		if(strcmp(name, file.name) == 0) {
			struct fuse_entry_param e;
			memset(&e, 0, sizeof(e));
			e.ino = INO_START_FILES + i;
			e.attr_timeout = 1.0;
			e.entry_timeout = 1.0;
			if(stat_helper_file_impl(e.ino, &e.attr, &file.content) != 0) {
				fuse_reply_err(req, ENOENT);
				return;
			}

			fuse_reply_entry(req, &e);
			return;
		}
	}

	fuse_reply_err(req, ENOENT);
}

static void fuse_lowlevel_op_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                     struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "readdir called\n");
	// TODO
	UNUSED(ino);
	UNUSED(size);
	UNUSED(off);
	UNUSED(fi);
	fuse_log(FUSE_LOG_DEBUG, "readdir not yet implemented\n");

	fuse_reply_err(req, ENOTDIR);
}

static void fuse_lowlevel_op_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {

	fuse_log(FUSE_LOG_DEBUG, "op_open called\n");

	if(ino == INO_ROOT_FOLDER) {
		fuse_reply_err(req, EISDIR);
		return;
	}

	if(ino <= INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	UserData* handle = fuse_req_userdata(req);

	if(ino >= INO_START_FILES + handle->state.files_size) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const size_t i = ino - INO_START_FILES;

	if(i >= handle->state.files_size) {
		fuse_log(FUSE_LOG_EMERG, "ino calculation implementation error: %zu is out of bounds %zu\n",
		         i, handle->state.files_size);

		fuse_reply_err(req, ENOENT);
		return;
	}

	if((fi->flags & O_ACCMODE) != O_RDONLY) {
		fuse_reply_err(req, EACCES);
		return;
	}

	fuse_reply_open(req, fi);
}

[[nodiscard]] static size_t min(size_t x, size_t y) {
	return ((x) < (y) ? (x) : (y));
}

static int reply_buf_limited(fuse_req_t req, const FuseBuffer* const buf, size_t maxsize,
                             off_t off) {

	if(off < 0) {
		fuse_reply_err(req, EFAULT);
		return -EFAULT;
	}

	const size_t off_s = (size_t)off;

	if(off_s < buf->size) {
		return fuse_reply_buf(req, (char*)buf->data + off_s, min(buf->size - off_s, maxsize));
	}

	return fuse_reply_buf(req, NULL, 0);
}

static void fuse_lowlevel_op_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                  struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "op_read called\n");

	(void)fi;

	if(ino == INO_ROOT_FOLDER) {
		fuse_reply_err(req, EISDIR);
		return;
	}

	if(ino <= INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	UserData* handle = fuse_req_userdata(req);

	if(ino >= INO_START_FILES + handle->state.files_size) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const size_t i = ino - INO_START_FILES;

	if(i >= handle->state.files_size) {
		fuse_log(FUSE_LOG_EMERG, "ino calculation implementation error: %zu is out of bounds %zu\n",
		         i, handle->state.files_size);

		fuse_reply_err(req, ENOENT);
		return;
	}

	if((fi->flags & O_ACCMODE) != O_RDONLY) {
		fuse_reply_err(req, EACCES);
		return;
	}

	const FuseFile file = handle->state.files[i];

	if(!file.flags.allow_read) {
		fuse_reply_err(req, EACCES);
		return;
	}

	reply_buf_limited(req, &file.content, size, off);
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

	if(fuse_session_mount(session, handle->state.dir_path) != 0) {
		*error = TSTR_STATIC_LIT("session mount failed");
		return NULL;
	}

	*error = tstr_static_null();
	return session;
}

static void fuse_log_impl(bool debug, enum fuse_log_level level, const char* fmt, va_list ap) {

	if(!debug) {
		if(level > FUSE_LOG_WARNING) {
			return;
		}
	}

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
	fflush(stderr);
}

static void fuse_log_debug_impl(enum fuse_log_level level, const char* fmt, va_list ap) {
	fuse_log_impl(true, level, fmt, ap);
}

static void fuse_log_normal_impl(enum fuse_log_level level, const char* fmt, va_list ap) {
	fuse_log_impl(false, level, fmt, ap);
}

typedef struct {
	struct sigaction old_sa;
	struct fuse_session* session;
} GlobalSignalState;

static volatile GlobalSignalState g_signal_state = { .old_sa = {}, .session = NULL };

// only setting the volatile sig_atomic_t g_signal_received' in here
static void fuse_exit_signal_received(int signal_number) {
	(void)signal_number;

	if(g_signal_state.session == NULL) {
		return;
	}

	fuse_session_exit(g_signal_state.session);
}

#define SIGNAL_FOR_FUSE_EXIT_REQUEST SIGUSR2

[[nodiscard]] static bool setup_signals(struct fuse_session* session) {

	// set up the signal handler
	// just create a sigaction structure, then add the handler
	struct sigaction action = {};

	action.sa_handler = fuse_exit_signal_received;
	// initialize the mask to be empty
	int empty_set_result = sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGNAL_FOR_FUSE_EXIT_REQUEST);

	struct sigaction old_sa = {};

	int result_act = sigaction(SIGNAL_FOR_FUSE_EXIT_REQUEST, &action, &old_sa);
	if(result_act < 0 || empty_set_result < 0) {
		return false;
	}

	g_signal_state = (GlobalSignalState){ .old_sa = old_sa, .session = session };

	return true;
}

static void remove_signals(void) {

	const struct sigaction old_sa = g_signal_state.old_sa;

	sigaction(SIGNAL_FOR_FUSE_EXIT_REQUEST, &old_sa, NULL);

	g_signal_state = (GlobalSignalState){ .old_sa = {}, .session = NULL };
}

// runs on a new thread
[[nodiscard]] static FuseHandleResult fuse_start_fn(UserData* const handle) {

	// setup logging

	if(handle->state.debug) {
		fuse_set_log_func(fuse_log_debug_impl);
	} else {
		fuse_set_log_func(fuse_log_normal_impl);
	}

	// initialize

	tstr_static error = tstr_static_null();

	const size_t argv_count = 1;

	char** const argv = malloc((argv_count + 1) * sizeof(char*));

	if(argv == NULL) {
		return THREAD_ERROR;
	}

	argv[0] = strdup("fuse_impl_dummy_argv0");
	argv[argv_count] = NULL;

	struct fuse_args dummy_args = { .argc = (int)argv_count,
		                            .argv = argv,
		                            .allocated = (int)false };

	struct fuse_session* session = fuse_initialize_impl(handle, &dummy_args, &error);

	// setup signals, this accesses global data
	bool signal_res = setup_signals(session);

	if(!signal_res) {
		return THREAD_ERROR;
	}

	FuseState state = fuse_state_error(TSTR_STATIC_LIT("Unkown error"));

	if(session == NULL) {
		if(tstr_static_is_null(error)) {
			state = fuse_state_error(TSTR_STATIC_LIT("Unkown error"));
		} else {
			state = fuse_state_error(error);
		}
	} else {
		state = fuse_state_ok();
	}

	bool state_success = fuse_state_set(handle->run_in, state);

	if(!state_success) {
		return THREAD_ERROR;
	}

	if(session == NULL) {
		return THREAD_ERROR;
	}

	// loop until we are finished

	/* Block until SIGINT  or fuse_session_exit */
	int ret = fuse_session_loop(session);

	// we are finished, set the atomic bool, so that we stop spamming SIGPIPEs
	fuse_set_session_finished(handle->run_in);

	fuse_session_unmount(session);

	fuse_remove_signal_handlers(session);

	remove_signals();

	fuse_session_destroy(session);

	{ // free argv
		for(size_t i = 0; i < argv_count; ++i) {
			// use free, as we use strdup
			free(argv[i]);
		}
		free((void*)argv);
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
		return fuse_create_result_error(TSTR_STATIC_LIT("run inm handle create error"));
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

[[nodiscard]] bool clear_fuse_file(FUSEHandle* const handle) {

	if(!fuse_run_in_deinit(handle->run_in)) {
		return false;
	}

	free_shared(handle);
	return true;
}

#if FUSE_RUN_FILESYSTEM_IN == 0
struct FuseRunHandleImpl {
	pthread_mutex_t mutex;
	FuseState fuse_state;
	//
	pthread_t handler_thread;
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

[[nodiscard]] bool fuse_run_in_deinit(FuseRunHandle* handle) {
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
				return false;
			}

			// now force the blocking function to wake up
			int pthread_res = pthread_kill(handle->handler_thread, SIGPIPE);
			if(pthread_res != 0) {
				return false;
			}

			FuseHandleResult return_value = THREAD_SUCCESS;
			result = pthread_join(handle->handler_thread, &return_value);
			if(result != 0) {
				return false;
			}

			if(return_value != THREAD_SUCCESS) {
				return false;
			}

			break;
		}
		default: {
			return false;
		}
	}

	int result = pthread_mutex_destroy(&(handle->mutex));
	if(result != 0) {
		return false;
	}

	free_shared(handle);

	return true;
}

[[nodiscard]] void* allocate_shared(size_t size) {
	return malloc(size);
}

void free_shared(void* data) {
	free(data);
}

#else
struct FuseRunHandleImpl {
	pthread_mutex_t mutex;
	FuseState fuse_state;
	//
	pid_t handler_process;
	//
	AtomicBool session_finished;
};

typedef FuseHandleResult(ProcessCreateFn)(UserData* const);

[[nodiscard]] static int create_process(pid_t* process_pid, ProcessCreateFn create_fn,
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

	*process_pid = result;

	return 0;
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

	result = create_process(&(handle->handler_process), start_fn, userdata);

	if(result != 0) {
		FREE_AT_END();
		return NULL;
	}

	return handle;
}

	#undef FREE_AT_END

	#define SIGPIPE_INTERVAL_USEC (10 * 1000)

[[nodiscard]] bool fuse_run_in_deinit(FuseRunHandle* const handle) {
	switch(handle->fuse_state.type) {
		case FuseStateTypeUninitialized: {
			break;
		}
		case FuseStateTypeInitializedErr: {
			break;
		}
		case FuseStateTypeInitializedOk: {

			// first exit the session, signal that via a signal
			int result = kill(handle->handler_process, SIGNAL_FOR_FUSE_EXIT_REQUEST);
			if(result != 0) {
				return false;
			}

			// now force the blocking function to wake up
			int pthread_res = kill(handle->handler_process, SIGPIPE);
			if(pthread_res != 0) {
				return false;
			}

			int return_status = 0;
			// periodically check if the child exited otherwise send a SIGPIPE again
			while(true) {

				result = waitpid(handle->handler_process, &return_status, WNOHANG);

				if(result == handle->handler_process) {
					break;
				}

				if(result != 0) {
					return false;
				}

				if(handle->session_finished) {
					// the session finished, we wait until the rest is finished too
					result = waitpid(handle->handler_process, &return_status, 0);

					if(result != handle->handler_process) {
						return false;
					}
				}

				pthread_res = kill(handle->handler_process, SIGPIPE);
				if(pthread_res != 0) {
					return false;
				}

				result = usleep(SIGPIPE_INTERVAL_USEC);
				if(result != 0) {
					return false;
				}
			}

			if(!(WIFEXITED(return_status))) {
				return false;
			}

			if(WIFSIGNALED(return_status)) {
				return false;
			}

			FuseHandleResult return_value = (FuseHandleResult)(WEXITSTATUS(return_status));

			if(return_value != THREAD_SUCCESS) {
				return false;
			}

			break;
		}
		default: {
			return false;
		}
	}

	int result = pthread_mutex_destroy(&(handle->mutex));
	if(result != 0) {
		return false;
	}

	free_shared(handle);

	return true;
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
