#include "./fuse_impl_run.h"

#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

typedef struct timespec Time;

#define EMPTY_TIME() ((Time){ .tv_sec = 0, .tv_nsec = 0 })

typedef struct {
	Time access_time;
} FileMetadata;

#define EMPTY_FILE_METADATA() ((FileMetadata){ .access_time = EMPTY_TIME() })

typedef struct {
	FileMetadata* data;
	size_t size;
} FileMetadatas;

typedef struct {
	FileMetadatas metadata;
	Time start_time;
	gid_t user_gid;
	uid_t user_uid;
} FuseData;

typedef struct {
	const FuseFiles* files;
	FuseData* data;
} UserData;

[[nodiscard]] static Time get_current_time(void) {

	Time time = {};

	int result = clock_gettime(CLOCK_REALTIME, &time);

	if(result != 0) {
		return EMPTY_TIME();
	}

	return time;
}

[[nodiscard]] static FileMetadata get_file_metadata_for_ino(const FileMetadatas* const metadatas,
                                                            fuse_ino_t ino) {

	if(ino == 0) {
		return EMPTY_FILE_METADATA();
	}

	if(ino > metadatas->size) {
		return EMPTY_FILE_METADATA();
	}

	return metadatas->data[ino - 1];
}

static void set_file_metadata_for_ino(FileMetadatas* const metadatas, fuse_ino_t ino) {

	if(ino == 0) {
		return;
	}

	if(ino > metadatas->size) {
		return;
	}

	FileMetadata* metadata = &(metadatas->data[ino - 1]);

	metadata->access_time = get_current_time();
}

//

static void fuse_lowlevel_op_init(void* userdata_arg, struct fuse_conn_info* conn) {

	UserData* const userdata = userdata_arg;

	FuseData* data = malloc(sizeof(FuseData));
	assert(data);

	userdata->data = data;

	Time current_time = get_current_time();

	data->start_time = current_time;

	const size_t metadatas_size = userdata->files->size + 1;
	FileMetadata* metadatas_ptr = malloc(sizeof(FileMetadata) * metadatas_size);
	assert(metadatas_ptr);

	for(size_t i = 0; i < metadatas_size; ++i) {
		metadatas_ptr[i] = (FileMetadata){ .access_time = current_time };
	}

	data->metadata = (FileMetadatas){ .data = metadatas_ptr, .size = metadatas_size };

	data->user_gid = getgid();
	data->user_uid = getuid();

	// Disable the receiving and processing of FUSE_INTERRUPT requests
	conn->no_interrupt = 1;
}

static void fuse_lowlevel_op_destroy(void* userdata_arg) {

	UserData* const userdata = userdata_arg;

	free(userdata->data->metadata.data);
	free(userdata->data);
	free(userdata);
}

#define INO_ROOT_FOLDER ((fuse_ino_t)(1))
#define INO_START_FILES ((fuse_ino_t)(2))

#define FOLDER_PERMISSIONS (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

[[nodiscard]] static int stat_helper_folder_impl(fuse_ino_t ino, struct stat* stbuf,
                                                 const UserData* const userdata) {
	stbuf->st_ino = ino;
	switch(ino) {
		case INO_ROOT_FOLDER: {

			stbuf->st_mode = S_IFDIR | FOLDER_PERMISSIONS;

			stbuf->st_nlink = 1 + userdata->files->size;
			stbuf->st_size = 0;

			FileMetadata metadata = get_file_metadata_for_ino(&(userdata->data->metadata), ino);

			stbuf->st_atim = metadata.access_time;
			stbuf->st_ctim = userdata->data->start_time;
			stbuf->st_mtim = userdata->data->start_time;

			stbuf->st_gid = userdata->data->user_gid;
			stbuf->st_uid = userdata->data->user_uid;
			break;
		}

		default: return -1;
	}
	return 0;
}

#define FILE_PERMISSIONS (S_IRUSR | S_IRGRP | S_IROTH)

[[nodiscard]] static int stat_helper_file_impl(fuse_ino_t ino, struct stat* stbuf,
                                               const FuseFile* const file,
                                               const FuseData* const data) {

	stbuf->st_ino = ino;
	switch(ino) {
		case INO_ROOT_FOLDER: return -1;

		default: {
			stbuf->st_mode = S_IFREG | FILE_PERMISSIONS;

			stbuf->st_nlink = 1;

			if(file->flags.scenario == FailScenarioStatNegativeFileSize) {
				stbuf->st_size = -1;
			} else {
				stbuf->st_size = (off_t)file->content.size;
			}

			FileMetadata metadata = get_file_metadata_for_ino(&(data->metadata), ino);

			stbuf->st_atim = metadata.access_time;
			stbuf->st_ctim = data->start_time;
			stbuf->st_mtim = data->start_time;

			stbuf->st_gid = data->user_gid;
			stbuf->st_uid = data->user_uid;

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

			if(ino >= INO_START_FILES + userdata->files->size) {
				return -1;
			}

			const size_t i = ino - INO_START_FILES;

			if(i >= userdata->files->size) {
				fuse_log(FUSE_LOG_EMERG,
				         "ino calculation implementation error: %zu is out of bounds %zu\n", i,
				         userdata->files->size);
				return -1;
			}

			const FuseFile file = userdata->files->data[i];

			return stat_helper_file_impl(ino, stbuf, &file, userdata->data);
		}
	}
	return 0;
}

static void fuse_lowlevel_op_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "getattr called: inode: %zu\n", ino);

	struct stat stbuf;

	(void)fi;

	UserData* const userdata = fuse_req_userdata(req);

	memset(&stbuf, 0, sizeof(stbuf));
	if(stat_helper_ino_impl(ino, &stbuf, userdata) == -1) {
		fuse_reply_err(req, ENOENT);
	} else {
		fuse_reply_attr(req, &stbuf, 1.0);
	}
}

static void fuse_lowlevel_op_setattr(fuse_req_t req, fuse_ino_t ino, struct stat* attr, int to_set,
                                     struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "setattr called\n");
	fuse_log(FUSE_LOG_EMERG, "setattr not yet implemented\n");

	(void)ino;
	(void)attr;
	(void)to_set;
	(void)fi;

	fuse_reply_err(req, ENOTDIR);
}

static void fuse_lowlevel_op_lookup(fuse_req_t req, fuse_ino_t parent, const char* name) {
	fuse_log(FUSE_LOG_DEBUG, "lookup called: parent: %zu name: %s\n", parent, name);

	if(parent != INO_ROOT_FOLDER) {
		// the top level directory, where we mounted is inode 1, we only support files inside
		// that

		fuse_reply_err(req, ENOENT);
		return;
	}

	UserData* const userdata = fuse_req_userdata(req);

	for(size_t i = 0; i < userdata->files->size; ++i) {
		const FuseFile file = userdata->files->data[i];

		if(strcmp(name, file.name) == 0) {
			struct fuse_entry_param e;
			memset(&e, 0, sizeof(e));
			e.ino = INO_START_FILES + i;
			e.attr_timeout = 1.0;
			e.entry_timeout = 1.0;
			if(stat_helper_file_impl(e.ino, &e.attr, &file, userdata->data) != 0) {
				fuse_reply_err(req, ENOENT);
				return;
			}

			fuse_reply_entry(req, &e);
			return;
		}
	}

	fuse_reply_err(req, ENOENT);
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

struct dirbuf {
	char* p;
	size_t size;
};

static void dirbuf_add(fuse_req_t req, struct dirbuf* b, const char* name, fuse_ino_t ino) {
	struct stat stbuf;

	size_t oldsize = b->size;

	b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);

	b->p = (char*)realloc(b->p, b->size);

	memset(&stbuf, 0, sizeof(stbuf));

	stbuf.st_ino = ino;

	fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf, (off_t)b->size);
}

static void fuse_lowlevel_op_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                     struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "readdir called: inode: %zu\n", ino);

	(void)fi;

	if(ino != INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOTDIR);
		return;
	}

	UserData* const userdata = fuse_req_userdata(req);

	struct dirbuf b;
	memset(&b, 0, sizeof(b));

	dirbuf_add(req, &b, ".", INO_ROOT_FOLDER);
	dirbuf_add(req, &b, "..", INO_ROOT_FOLDER);

	for(size_t i = 0; i < userdata->files->size; ++i) {
		const FuseFile file = userdata->files->data[i];

		dirbuf_add(req, &b, file.name, INO_START_FILES + i);
	}

	FuseBuffer final_buffer = { .data = b.p, .size = b.size };

	reply_buf_limited(req, &final_buffer, size, off);

	set_file_metadata_for_ino(&(userdata->data->metadata), ino);

	free(b.p);
}

static void fuse_lowlevel_op_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {

	fuse_log(FUSE_LOG_DEBUG, "open called: inode: %zu\n", ino);

	if(ino == INO_ROOT_FOLDER) {
		fuse_reply_err(req, EISDIR);
		return;
	}

	if(ino <= INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	UserData* const userdata = fuse_req_userdata(req);

	if(ino >= INO_START_FILES + userdata->files->size) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const size_t i = ino - INO_START_FILES;

	if(i >= userdata->files->size) {
		fuse_log(FUSE_LOG_EMERG, "ino calculation implementation error: %zu is out of bounds %zu\n",
		         i, userdata->files->size);

		fuse_reply_err(req, ENOENT);
		return;
	}

	if((fi->flags & O_ACCMODE) != O_RDONLY) {
		fuse_reply_err(req, EACCES);
		return;
	}

	fuse_reply_open(req, fi);
}

static void fuse_lowlevel_op_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                                  struct fuse_file_info* fi) {
	fuse_log(FUSE_LOG_DEBUG, "read called: inode: %zu\n", ino);

	if(ino == INO_ROOT_FOLDER) {
		fuse_reply_err(req, EISDIR);
		return;
	}

	if(ino <= INO_ROOT_FOLDER) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	UserData* const userdata = fuse_req_userdata(req);

	if(ino >= INO_START_FILES + userdata->files->size) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	const size_t i = ino - INO_START_FILES;

	if(i >= userdata->files->size) {
		fuse_log(FUSE_LOG_EMERG, "ino calculation implementation error: %zu is out of bounds %zu\n",
		         i, userdata->files->size);

		fuse_reply_err(req, ENOENT);
		return;
	}

	if((fi->flags & O_ACCMODE) != O_RDONLY) {
		fuse_reply_err(req, EACCES);
		return;
	}

	const FuseFile file = userdata->files->data[i];

	if(file.flags.scenario == FailScenarioReadFailsGeneric) {
		fuse_reply_err(req, EACCES);
		return;
	}

	set_file_metadata_for_ino(&(userdata->data->metadata), ino);

	if(file.flags.scenario == FailScenarioReadFailsLessData) {
		FuseBuffer content = file.content;
		if(content.size == 0) {
			fuse_log(FUSE_LOG_EMERG, "can't decrement size of file, as size is already 0\n");

			fuse_reply_err(req, ENOENT);
			return;
		}

		content.size -= 1;

		reply_buf_limited(req, &content, size, off);
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

	fuse_reply_err(req, ENOSYS);
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

	fuse_reply_err(req, ENOSYS);
}

static void fuse_lowlevel_op_removexattr(fuse_req_t req, fuse_ino_t ino, const char* name) {

	fuse_log(FUSE_LOG_DEBUG, "removexattr called\n");
	fuse_log(FUSE_LOG_EMERG, "removexattr not yet implemented\n");

	(void)ino;
	(void)name;

	fuse_reply_err(req, ENOSYS);
}

static void fuse_lowlevel_op_lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence,
                                   struct fuse_file_info* fi) {

	fuse_log(FUSE_LOG_DEBUG, "lseek called\n");
	fuse_log(FUSE_LOG_EMERG, "lseek not yet implemented\n");

	(void)ino;
	(void)off;
	(void)whence;
	(void)fi;

	fuse_reply_err(req, ENOSYS);
}

// see: https://libfuse.github.io/doxygen/structfuse__lowlevel__ops.html
static const struct fuse_lowlevel_ops fuse_lowlevel_operations = {
	.init = fuse_lowlevel_op_init,
	.destroy = fuse_lowlevel_op_destroy,
	.lookup = fuse_lowlevel_op_lookup,
	// forget,
	.getattr = fuse_lowlevel_op_getattr,
	.setattr = fuse_lowlevel_op_setattr,
	// readlink,
	// mknod,
	// mkdir,
	// unlink,
	// rmdir,
	// symlink,
	// rename,
	// link,
	.open = fuse_lowlevel_op_open,
	.read = fuse_lowlevel_op_read,
	// write,
	// flush,
	// release,
	// fsync,
	// opendir,
	.readdir = fuse_lowlevel_op_readdir,
	// releasedir,
	// fsyncdir,
	// statfs,
	.setxattr = fuse_lowlevel_op_setxattr,
	.getxattr = fuse_lowlevel_op_getxattr,
	// listxattr,
	.removexattr = fuse_lowlevel_op_removexattr,
	// access,
	// create,
	// getlk,
	// setlk,
	// bmap,
	// ioctl,
	// poll,
	// write_buf,
	// retrieve_reply,
	// forget_multi,
	// flock,
	// fallocate,
	// readdirplus,
	// copy_file_range,
	.lseek = fuse_lowlevel_op_lseek,
	// tmpfile,
	// statx,
};

[[nodiscard]] static struct fuse_session*
fuse_initialize_impl(const FuseStaticData* const fuse_data, struct fuse_args* const args,
                     tstr_static* const error) {

	UserData* const userdata = malloc(sizeof(UserData));
	userdata->files = &(fuse_data->files);
	userdata->data = NULL;

	struct fuse_session* session = fuse_session_new(
	    args, &fuse_lowlevel_operations, sizeof(fuse_lowlevel_operations), (void*)userdata);

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

		fprintf(stderr, "file system hierarchy:\n");
		fprintf(stderr, "/ (%zu)\n", INO_ROOT_FOLDER);
		for(size_t i = 0; i < data->files.size; ++i) {
			const FuseFile file = data->files.data[i];
			fprintf(stderr, "\t%s (%zu) [%zu] s%c\n", file.name, INO_START_FILES + i,
			        file.content.size, get_char_for_fail_scenario(file.flags.scenario));
		}
		fprintf(stderr, "\n");

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
