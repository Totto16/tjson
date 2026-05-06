#include "./fuse_impl.h"

#include <allocator.h>

#include <pthread.h>

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

/* static int hello_stat(fuse_ino_t ino, struct stat* stbuf) {
    stbuf->st_ino = ino;
    switch(ino) {
        case 1:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;

        case 2:
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(hello_str);
            break;

        default: return -1;
    }
    return 0;
}

static void hello_ll_init(void* userdata, struct fuse_conn_info* conn) {
    (void)userdata;

    // Disable the receiving and processing of FUSE_INTERRUPT requests
conn->no_interrupt = 1;

// Test setting flags the old way
conn->want = FUSE_CAP_ASYNC_READ;
conn->want &= ~FUSE_CAP_ASYNC_READ;
}

static void hello_ll_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
    struct stat stbuf;

    (void)fi;

    memset(&stbuf, 0, sizeof(stbuf));
    if(hello_stat(ino, &stbuf) == -1)
        fuse_reply_err(req, ENOENT);
    else
        fuse_reply_attr(req, &stbuf, 1.0);
}

static void hello_ll_lookup(fuse_req_t req, fuse_ino_t parent, const char* name) {
    struct fuse_entry_param e;

    if(parent != 1 || strcmp(name, hello_name) != 0)
        fuse_reply_err(req, ENOENT);
    else {
        memset(&e, 0, sizeof(e));
        e.ino = 2;
        e.attr_timeout = 1.0;
        e.entry_timeout = 1.0;
        hello_stat(e.ino, &e.attr);

        fuse_reply_entry(req, &e);
    }
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
    fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf, b->size);
}

static int reply_buf_limited(fuse_req_t req, const char* buf, size_t bufsize, off_t off,
                             size_t maxsize) {
    if(off < bufsize)
        return fuse_reply_buf(req, buf + off, min(bufsize - off, maxsize));
    else
        return fuse_reply_buf(req, NULL, 0);
}

static void hello_ll_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                             struct fuse_file_info* fi) {
    (void)fi;

    if(ino != 1)
        fuse_reply_err(req, ENOTDIR);
    else {
        struct dirbuf b;

        memset(&b, 0, sizeof(b));
        dirbuf_add(req, &b, ".", 1);
        dirbuf_add(req, &b, "..", 1);
        dirbuf_add(req, &b, hello_name, 2);
        reply_buf_limited(req, b.p, b.size, off, size);
        free(b.p);
    }
}

static void hello_ll_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info* fi) {
    if(ino != 2)
        fuse_reply_err(req, EISDIR);
    else if((fi->flags & O_ACCMODE) != O_RDONLY)
        fuse_reply_err(req, EACCES);
    else
        fuse_reply_open(req, fi);
}

static void hello_ll_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                          struct fuse_file_info* fi) {
    (void)fi;

    assert(ino == 2);
    reply_buf_limited(req, hello_str, strlen(hello_str), off, size);
}

static void hello_ll_getxattr(fuse_req_t req, fuse_ino_t ino, const char* name, size_t size) {
    (void)size;
    assert(ino == 1 || ino == 2);
    if(strcmp(name, "hello_ll_getxattr_name") == 0) {
        const char* buf = "hello_ll_getxattr_value";
        fuse_reply_buf(req, buf, strlen(buf));
    } else {
        fuse_reply_err(req, ENOTSUP);
    }
}

static void hello_ll_setxattr(fuse_req_t req, fuse_ino_t ino, const char* name, const char* value,
                              size_t size, int flags) {
    (void)flags;
    (void)size;
    assert(ino == 1 || ino == 2);
    const char* exp_val = "hello_ll_setxattr_value";
    if(strcmp(name, "hello_ll_setxattr_name") == 0 && strlen(exp_val) == size &&
       strncmp(value, exp_val, size) == 0) {
        fuse_reply_err(req, 0);
    } else {
        fuse_reply_err(req, ENOTSUP);
    }
}

static void hello_ll_removexattr(fuse_req_t req, fuse_ino_t ino, const char* name) {
    assert(ino == 1 || ino == 2);
    if(strcmp(name, "hello_ll_removexattr_name") == 0) {
        fuse_reply_err(req, 0);
    } else {
        fuse_reply_err(req, ENOTSUP);
    }
}
*/

static const struct fuse_lowlevel_ops fuse_lowlevel_operations = {
	.init = hello_ll_init,
	.lookup = hello_ll_lookup,
	.getattr = hello_ll_getattr,
	.readdir = hello_ll_readdir,
	.open = hello_ll_open,
	.read = hello_ll_read,
	.setxattr = hello_ll_setxattr,
	.getxattr = hello_ll_getxattr,
	.removexattr = hello_ll_removexattr,
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

void clear_fuse_file(FUSEHandle* const handle) {

	// TODO

	TJSON_FREE(handle);
}
