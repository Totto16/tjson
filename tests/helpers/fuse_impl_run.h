
#pragma once

#define FUSE_USE_VERSION FUSE_MAKE_VERSION(3, 12)
#include <fuse3/fuse.h>

#include <fuse3/fuse_lowlevel.h>

#include "./fuse_impl.h"

// don't use those here
#undef TJSON_MALLOC
#undef TJSON_CALLOC
#undef TJSON_REALLOC
#undef TJSON_FREE

typedef FUSEHandle UserData;

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

typedef struct {
	const char* dir_path;
	const FuseFile* files;
	size_t files_size;
	bool debug;
} FuseStaticState;

typedef struct FuseRunHandleImpl FuseRunHandle;

struct FUSEHandleImpl {
	FuseStaticState state;
	FuseRunHandle* run_in;
};

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

[[nodiscard]] FuseState fuse_state_uninitialized(void);

[[nodiscard]] FuseState fuse_state_error(tstr_static const error);

[[nodiscard]] FuseState fuse_state_ok(void);

typedef FuseHandleResult (*FuseHandleFn)(UserData* const data);

[[nodiscard]] FuseRunHandle* fuse_run_in_init(FuseHandleFn start_fn, UserData* const userdata);

[[nodiscard]] bool fuse_state_set(FuseRunHandle* handle, FuseState state);

[[nodiscard]] bool fuse_state_get(FuseRunHandle* handle, FuseState* state);

[[nodiscard]] int fuse_run_in_deinit(FuseRunHandle* handle);

void fuse_set_session_finished(FuseRunHandle* handle);

[[nodiscard]] void* allocate_shared(size_t size);

void free_shared(void* data);

#define SIGNAL_FOR_FUSE_EXIT_REQUEST SIGUSR2

// main fn
[[nodiscard]] FuseHandleResult fuse_start_fn(UserData* const handle);
