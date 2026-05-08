
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <tstr.h>
#include <tvec.h>

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
#define PROCESS_SUCCESS ((FuseHandleResult)(0))

#define PROCESS_ERROR ((FuseHandleResult)(1))

typedef struct {
	void* data;
	size_t size;
} FuseBuffer;

typedef enum {
	FailScenarioNone = 0,
	FailScenarioReadFailsGeneric,
	FailScenarioStatNegativeFileSize,
	FailScenarioReadFailsLessData,
} FailScenario;

[[nodiscard]] char get_char_for_fail_scenario(FailScenario scenario);

typedef struct {
	FailScenario scenario;
} FuseFileMockFlags;

typedef struct {
	char* name;
	FuseBuffer content;
	FuseFileMockFlags flags;
} FuseFile;

typedef struct {
	FuseFile* data;
	size_t size;
} FuseFiles;

typedef struct {
	char* dir_path;
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
	void* ptr;
	size_t size;
} MemoryBlock;

typedef struct {
	FuseSharedState* state;
	MemoryBlock rest; // is the "FuseStaticData" encoded into a flat array
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

// serialize / deserialize static data

[[nodiscard]] size_t get_serialize_size_for_static_data(const FuseStaticData* data);

[[nodiscard]] tstr_static serialize_static_data(MemoryBlock memory, const FuseStaticData* data);

typedef struct {
	void* data;
} AllocatedData;

TVEC_DEFINE_VEC_TYPE(AllocatedData)

typedef TVEC_TYPENAME(AllocatedData) AllocatedDataArray;

[[nodiscard]] tstr_static deserialize_static_data(MemoryBlock memory, FuseStaticData* data,
                                                  AllocatedDataArray* allocated_things);

void free_allocated_data(AllocatedDataArray* allocated_things);

#ifdef __cplusplus
}
#endif
