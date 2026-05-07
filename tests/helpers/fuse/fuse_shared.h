
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <tstr.h>

// don't use those here
#undef TJSON_MALLOC
#undef TJSON_CALLOC
#undef TJSON_REALLOC
#undef TJSON_FREE

#ifdef __cplusplus
	#include <atomic>
	#define ATOMIC(type) std::atomic<type>
extern "C" {
#else
	#define ATOMIC(type) _Atomic type
#endif

// see https://github.com/libfuse/libfuse/issues/410
// on why this is necessary

typedef uint8_t FuseHandleResult;
#define THREAD_SUCCESS ((FuseHandleResult)(0))

#define THREAD_ERROR ((FuseHandleResult)(1))

typedef struct {
	void* data;
	size_t size;
} FuseBuffer;

typedef struct {
	bool allow_stat;
	bool allow_read;
} FuseFileMockFlags;

typedef struct {
	char* name;
	FuseBuffer content;
	FuseFileMockFlags flags;
} FuseFile;

typedef struct {
	const FuseFile* data;
	size_t size;
} FuseFiles;

typedef struct {
	const char* dir_path;
	FuseFiles files;
	bool debug;
} FuseStaticData;

typedef ATOMIC(bool) AtomicBool;

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

typedef struct {
	pthread_mutex_t mutex;
	FuseState fuse_state;
	AtomicBool session_finished;
} FuseSharedState;

// just assert, that we have a non opaque etype
static_assert(sizeof(FuseSharedState) != 0);

typedef struct {
	FuseSharedState* state;
	void* rest; // is the "FuseStaticData" encoded into a flat array
} FuseSharedMemory;

[[nodiscard]] FuseState fuse_state_uninitialized(void);

[[nodiscard]] FuseState fuse_state_error(tstr_static error);

[[nodiscard]] FuseState fuse_state_ok(void);

[[nodiscard]] bool fuse_shared_state_set_state(FuseSharedState* shared_state, FuseState state);

[[nodiscard]] bool fuse_shared_state_get_state(FuseSharedState* shared_state, FuseState* state);

typedef struct SharedAllocatorImpl SharedAllocator;

void fuse_shared_state_set_session_finished(FuseSharedState* shared_state);

// shared allocator things

typedef struct {
	FuseSharedMemory memory;
	SharedAllocator* allocator;
} SharedAllocatorResult;

[[nodiscard]] SharedAllocatorResult new_shared_allocator(size_t additional_data_size);

[[nodiscard]] SharedAllocatorResult shared_allocator_from_path(const char* path);

[[nodiscard]] char* shared_allocator_get_path(const SharedAllocator* allocator);

void shared_allocator_deinit(SharedAllocator* allocator);

#define SIGNAL_FOR_FUSE_EXIT_REQUEST SIGUSR2

#ifdef __cplusplus
}
#endif
