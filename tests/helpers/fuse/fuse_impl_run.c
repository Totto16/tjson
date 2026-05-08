
#include "./fuse_impl_run.h"

#include <errno.h>
#include <signal.h>

typedef FuseFiles UserData;

static void fuse_lowlevel_op_init(void* userdata, struct fuse_conn_info* conn) {

	(void)userdata;

	// Disable the receiving and processing of FUSE_INTERRUPT requests
	conn->no_interrupt = 1;
}

#define INO_ROOT_FOLDER 1
#define INO_START_FILES 2

[[nodiscard]] static int stat_helper_folder_impl(fuse_ino_t ino, struct stat* stbuf,
                                                 const UserData* const userdata) {
	stbuf->st_ino = ino;
	switch(ino) {
		case INO_ROOT_FOLDER: {

			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1 + userdata->size;
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

static int stat_helper_ino_impl(fuse_ino_t ino, struct stat* stbuf,
                                const UserData* const userdata) {
	stbuf->st_ino = ino;
	switch(ino) {
		case 1: return stat_helper_folder_impl(ino, stbuf, userdata);

		default: {
			if(ino <= INO_ROOT_FOLDER) {
				return -1;
			}

			if(ino >= INO_START_FILES + userdata->size) {
				return -1;
			}

			const size_t i = ino - INO_START_FILES;

			if(i >= userdata->size) {
				fuse_log(FUSE_LOG_EMERG,
				         "ino calculation implementation error: %zu is out of bounds %zu\n", i,
				         userdata->size);
				return -1;
			}

			const FuseFile file = userdata->data[i];

			return stat_helper_file_impl(ino, stbuf, &file.content);
		}
	}
	return 0;
}

static void fuse_lowlevel_op_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "getattr called\n");

	struct stat stbuf;

	(void)fi;

	const UserData* const userdata = fuse_req_userdata(req);

	memset(&stbuf, 0, sizeof(stbuf));
	if(stat_helper_ino_impl(ino, &stbuf, userdata) == -1) {
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

	const UserData* const userdata = fuse_req_userdata(req);

	for(size_t i = 0; i < userdata->size; ++i) {
		const FuseFile file = userdata->data[i];

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
	fuse_log(FUSE_LOG_EMERG, "readdir not yet implemented\n");

	(void)ino;
	(void)size;
	(void)off;
	(void)fi;

	fuse_reply_err(req, ENOTDIR);
}

static void fuse_lowlevel_op_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {

	fuse_log(FUSE_LOG_DEBUG, "open called\n");

	if(ino == INO_ROOT_FOLDER) {
		fuse_reply_err(req, EISDIR);
		return;
	}

	if(ino <= INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const UserData* const userdata = fuse_req_userdata(req);

	if(ino >= INO_START_FILES + userdata->size) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const size_t i = ino - INO_START_FILES;

	if(i >= userdata->size) {
		fuse_log(FUSE_LOG_EMERG, "ino calculation implementation error: %zu is out of bounds %zu\n",
		         i, userdata->size);

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
	fuse_log(FUSE_LOG_DEBUG, "read called\n");

	(void)fi;

	if(ino == INO_ROOT_FOLDER) {
		fuse_reply_err(req, EISDIR);
		return;
	}

	if(ino <= INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const UserData* const userdata = fuse_req_userdata(req);

	if(ino >= INO_START_FILES + userdata->size) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const size_t i = ino - INO_START_FILES;

	if(i >= userdata->size) {
		fuse_log(FUSE_LOG_EMERG, "ino calculation implementation error: %zu is out of bounds %zu\n",
		         i, userdata->size);

		fuse_reply_err(req, ENOENT);
		return;
	}

	if((fi->flags & O_ACCMODE) != O_RDONLY) {
		fuse_reply_err(req, EACCES);
		return;
	}

	const FuseFile file = userdata->data[i];

	if(!file.flags.allow_read) {
		fuse_reply_err(req, EACCES);
		return;
	}

	reply_buf_limited(req, &file.content, size, off);
}

static void fuse_lowlevel_op_getxattr(fuse_req_t req, fuse_ino_t ino, const char* name,
                                      size_t size) {

	fuse_log(FUSE_LOG_DEBUG, "getxattr called\n");
	fuse_log(FUSE_LOG_EMERG, "getxattr not yet implemented\n");

	(void)ino;
	(void)size;
	(void)name;

	fuse_reply_err(req, ENOTSUP);
}

static void fuse_lowlevel_op_setxattr(fuse_req_t req, fuse_ino_t ino, const char* name,
                                      const char* value, size_t size, int flags) {

	fuse_log(FUSE_LOG_DEBUG, "setxattr called\n");
	fuse_log(FUSE_LOG_EMERG, "setxattr not yet implemented\n");

	(void)ino;
	(void)size;
	(void)name;
	(void)value;
	(void)flags;

	fuse_reply_err(req, ENOTSUP);
}

static void fuse_lowlevel_op_removexattr(fuse_req_t req, fuse_ino_t ino, const char* name) {

	fuse_log(FUSE_LOG_DEBUG, "removexattr called\n");
	fuse_log(FUSE_LOG_EMERG, "removexattr not yet implemented\n");

	(void)ino;
	(void)name;

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

[[nodiscard]] static struct fuse_session*
fuse_initialize_impl(const FuseStaticData* const fuse_data, struct fuse_args* const args,
                     tstr_static* const error) {

	const UserData* const userdata = &(fuse_data->files);

	struct fuse_session* session =
	    fuse_session_new(args, &fuse_lowlevel_operations, sizeof(fuse_lowlevel_operations),
	                     (void*)((uintptr_t)userdata));

	if(session == NULL) {
		*error = TSTR_STATIC_LIT("session new failed");
		return NULL;
	}

	if(fuse_set_signal_handlers(session) != 0) {
		*error = TSTR_STATIC_LIT("setting signal handlers failed");
		return NULL;
	}

	if(fuse_session_mount(session, fuse_data->dir_path) != 0) {
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
[[nodiscard]] FuseHandleResult fuse_start_fn(FuseSharedState* const shared_state,
                                             const FuseStaticData* const data) {

	// setup logging

	if(data->debug) {
		fuse_set_log_func(fuse_log_debug_impl);
	} else {
		fuse_set_log_func(fuse_log_normal_impl);
	}

	// initialize

	tstr_static error = tstr_static_null();

	const size_t argv_count = 1;

	char** const argv = malloc((argv_count + 1) * sizeof(char*));

	if(argv == NULL) {
		return PROCESS_ERROR;
	}

	argv[0] = strdup("fuse_impl_dummy_argv0");
	argv[argv_count] = NULL;

	struct fuse_args args = { .argc = (int)argv_count, .argv = argv, .allocated = (int)false };

	struct fuse_session* session = fuse_initialize_impl(data, &args, &error);

	// setup signals, this accesses global data
	bool signal_res = setup_signals(session);

	if(!signal_res) {
		return PROCESS_ERROR;
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

	bool state_success = fuse_shared_state_set_state(shared_state, state);

	if(!state_success) {
		return PROCESS_ERROR;
	}

	if(session == NULL) {
		return PROCESS_ERROR;
	}

	// loop until we are finished

	/* Block until SIGINT  or fuse_session_exit */
	int ret = fuse_session_loop(session);

	// we are finished, set the atomic bool, so that we stop spamming SIGPIPEs
	fuse_shared_state_set_session_finished(shared_state);

	fuse_session_unmount(session);

	fuse_remove_signal_handlers(session);

	remove_signals();

	fuse_session_destroy(session);

	fuse_opt_free_args(&args);

	{ // free argv
		for(size_t i = 0; i < argv_count; ++i) {
			// use free, as we use strdup
			free(argv[i]);
		}
		free((void*)argv);
	}

	if(ret != 0) {
		return PROCESS_ERROR;
	}

	return PROCESS_SUCCESS;
}
