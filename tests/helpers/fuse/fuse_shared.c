#include "./fuse_shared.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

[[nodiscard]] FuseState fuse_state_uninitialized(void) {
	return (FuseState){ .type = FuseStateTypeUninitialized, .data = {} };
}

[[nodiscard]] FuseState fuse_state_error(tstr_static const error) {
	return (FuseState){ .type = FuseStateTypeInitializedErr, .data = { .error = error } };
}

[[nodiscard]] FuseState fuse_state_ok(void) {
	return (FuseState){ .type = FuseStateTypeInitializedOk, .data = {} };
}

[[nodiscard]] bool fuse_shared_state_set_state(FuseSharedState* const shared_state,
                                               FuseState state) {
	int result = pthread_mutex_lock(&(shared_state->mutex));
	if(result != 0) {
		return false;
	}

	shared_state->fuse_state = state;

	result = pthread_mutex_unlock(&(shared_state->mutex));
	if(result != 0) {
		return false;
	}

	return true;
}

[[nodiscard]] bool fuse_shared_state_get_state(FuseSharedState* const shared_state,
                                               FuseState* state) {
	int result = pthread_mutex_lock(&(shared_state->mutex));
	if(result != 0) {
		return false;
	}

	*state = shared_state->fuse_state;

	result = pthread_mutex_unlock(&(shared_state->mutex));
	if(result != 0) {
		return false;
	}

	return true;
}

void fuse_shared_state_set_session_finished(FuseSharedState* const shared_state) {
	shared_state->session_finished = true;
}

static void* get_random_data(size_t length) {

	int fd = open("/dev/urandom", O_RDONLY);

	if(fd < 0) {
		return NULL;
	}

	uint8_t* data = (uint8_t*)malloc(length);

	// NOTE: only supports one read, no multi read gets
	ssize_t result = read(fd, data, length);

	if(result < 0) {
		free(data);
		return NULL;
	}

	if((size_t)result != length) {
		free(data);
		return NULL;
	}

	int close_res = close(fd);

	if(close_res != 0) {
		free(data);
		return NULL;
	}

	return data;
}

typedef struct {
	void* ptr;
	size_t size;
} MmapData;

struct SharedAllocatorImpl {
	char* path;
	MmapData mmap;
	int shm_fd;
	bool created_shm;
};

#define SHARED_ALLOCATOR_ERROR() \
	((SharedAllocatorResult){ \
	    .allocator = NULL, \
	    .memory = (FuseSharedMemory){ .state = NULL, .rest = { .ptr = NULL, .size = 0 } } })

#define SHARED_MEMORY_NAME_LENGTH 256

[[nodiscard]] SharedAllocatorResult new_shared_allocator(size_t additional_data_size) {
	SharedAllocator* allocator = (SharedAllocator*)malloc(sizeof(SharedAllocator));

	if(allocator == NULL) {
		return SHARED_ALLOCATOR_ERROR();
	}

#define FREE_AT_END() \
	do { \
		free(allocator); \
	} while(false)

	void* data = get_random_data(SHARED_MEMORY_NAME_LENGTH + 2);

	if(data == NULL) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	char* path = data;
	path[0] = '/';
	path[SHARED_MEMORY_NAME_LENGTH + 1] = '\0';

	for(size_t i = 0; ++i < SHARED_MEMORY_NAME_LENGTH; ++i) {
		char* value = (path + i + 1);
		if(!isprint(*value)) {
			*value = '_';
		}
	}

	allocator->path = path;

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free(path); \
		free(allocator); \
	} while(false)

	static_assert((sizeof(FuseSharedState) % 8) == 0);

	size_t size = sizeof(FuseSharedState) + additional_data_size;

	int fd = shm_open(path, O_CREAT | O_RDWR, 0666);

	if(fd < 0) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	allocator->shm_fd = fd;
	allocator->created_shm = true;

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		close(fd); \
		free(path); \
		free(allocator); \
	} while(false)

	int result = ftruncate(fd, (off_t)size);

	if(result != 0) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);

	if(ptr == MAP_FAILED) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	allocator->mmap = (MmapData){ .ptr = ptr, .size = size };

	FuseSharedMemory memory = { .state = ptr,
		                        .rest = { .ptr = ((uint8_t*)ptr) + sizeof(FuseSharedState),
		                                  .size = additional_data_size } };

	return ((SharedAllocatorResult){ .allocator = allocator, .memory = memory });
}

#undef FREE_AT_END

[[nodiscard]] SharedAllocatorResult shared_allocator_from_path(const char* path) {
	SharedAllocator* allocator = (SharedAllocator*)malloc(sizeof(SharedAllocator));

	if(allocator == NULL) {
		return SHARED_ALLOCATOR_ERROR();
	}

#define FREE_AT_END() \
	do { \
		free(allocator); \
	} while(false)

	char* path_str = strdup(path);

	if(path_str == NULL) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	allocator->path = path_str;

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free(path_str); \
		free(allocator); \
	} while(false)

	int fd = shm_open(path, O_RDWR, 0666);

	if(fd < 0) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	allocator->shm_fd = fd;
	allocator->created_shm = false;

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		close(fd); \
		free(path_str); \
		free(allocator); \
	} while(false)

	struct stat statbuf;

	int result = fstat(fd, &statbuf);

	if(result != 0) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	off_t raw_size = statbuf.st_size;

	if(raw_size < 0) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	static_assert((sizeof(FuseSharedState) % 8) == 0);

	const size_t size = (size_t)statbuf.st_size;

	if(size < sizeof(FuseSharedState)) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	const size_t additional_data_size = size - sizeof(FuseSharedState);

	void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);

	if(ptr == MAP_FAILED) {
		FREE_AT_END();
		return SHARED_ALLOCATOR_ERROR();
	}

	allocator->mmap = (MmapData){ .ptr = ptr, .size = size };

	FuseSharedMemory memory = { .state = ptr,
		                        .rest = { .ptr = ((uint8_t*)ptr) + sizeof(FuseSharedState),
		                                  .size = additional_data_size } };

	return ((SharedAllocatorResult){ .allocator = allocator, .memory = memory });
}

#undef FREE_AT_END

[[nodiscard]] char* shared_allocator_get_path(const SharedAllocator* allocator) {
	return allocator->path;
}

void shared_allocator_deinit(SharedAllocator* allocator) {

	int result = munmap(allocator->mmap.ptr, allocator->mmap.size);

	if(result != 0) {
		// TODO: handle error
		fprintf(stderr, "ERROR in munmap: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	close(allocator->shm_fd);

	if(allocator->created_shm) {
		result = shm_unlink(allocator->path);

		if(result != 0) {
			// TODO: handle error
			fprintf(stderr, "ERROR in shm_unlink: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	free(allocator->path);

	free(allocator);
}

[[nodiscard]] static size_t serialize_str_size(const char* const str) {
	return strlen(str) + 1;
}

[[nodiscard]] static size_t serialize_bool_size(bool) {
	return sizeof(bool);
}

[[nodiscard]] static size_t serialize_u64_size(uint64_t) {
	return sizeof(uint64_t);
}

[[nodiscard]] static size_t serialize_fuse_file_size(const FuseFile* file);

[[nodiscard]] static size_t serialize_fuse_mock_flags_size(const FuseFileMockFlags* flags);

[[nodiscard]] static size_t serialize_fuse_buffer_size(const FuseBuffer* buffer);

static_assert(sizeof(uint64_t) == sizeof(size_t));

#define SERIALIZE_FIELD_SIZE(value) \
	_Generic((value), \
	    bool: serialize_bool_size, \
	    size_t: serialize_u64_size, \
	    char*: serialize_str_size, \
	    const FuseFile*: serialize_fuse_file_size, \
	    const FuseBuffer*: serialize_fuse_buffer_size, \
	    const FuseFileMockFlags*: serialize_fuse_mock_flags_size)(value)

[[nodiscard]] static size_t serialize_fuse_file_size(const FuseFile* const file) {

	size_t size = 0;

	size += SERIALIZE_FIELD_SIZE(file->name);

	size += SERIALIZE_FIELD_SIZE(&(file->content));

	size += SERIALIZE_FIELD_SIZE(&(file->flags));

	return size;
}

[[nodiscard]] static size_t serialize_fuse_mock_flags_size(const FuseFileMockFlags* flags) {
	size_t size = 0;

	size += SERIALIZE_FIELD_SIZE(flags->allow_stat);

	size += SERIALIZE_FIELD_SIZE(flags->allow_read);

	return size;
}

[[nodiscard]] static size_t serialize_fuse_buffer_size(const FuseBuffer* buffer) {
	size_t size = 0;

	size += SERIALIZE_FIELD_SIZE(buffer->size);

	size += buffer->size;

	return size;
}

[[nodiscard]] size_t get_serialize_size_for_static_data(const FuseStaticData* const data) {

	size_t size = 0;

	size += SERIALIZE_FIELD_SIZE(data->dir_path);

	size += SERIALIZE_FIELD_SIZE(data->files.size);

	for(size_t i = 0; i < data->files.size; ++i) {
		const FuseFile* file = &(data->files.data[i]);

		size += SERIALIZE_FIELD_SIZE(file);
	}

	size += SERIALIZE_FIELD_SIZE(data->debug);

	return size;
}

#undef SERIALIZE_FIELD_SIZE

//

static void memory_advance(MemoryBlock* block, size_t size) {
	if(block->size >= size) {
		fprintf(stderr, "ERROR in memory_advance: not enough memory to advance: %zu < %zu\n",
		        block->size, size);
		exit(EXIT_FAILURE);
	}

	block->size -= size;
	block->ptr = ((uint8_t*)block->ptr) + size;
}

[[nodiscard]] static tstr_static serialize_slice(MemoryBlock* block, size_t size,
                                                 const void* const data) {
	if(block->size < size) {
		return TSTR_STATIC_LIT("Not enough memory");
	}

	memcpy(block->ptr, data, size);

	memory_advance(block, size);

	return tstr_static_null();
}

[[nodiscard]] static tstr_static serialize_char(MemoryBlock* block, char value) {
	return serialize_slice(block, sizeof(char), &value);
}

[[nodiscard]] static tstr_static serialize_str(MemoryBlock* block, const char* const str) {
	tstr_static result = serialize_slice(block, strlen(str), (const void*)str);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = serialize_char(block, '\0');

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] static tstr_static serialize_bool(MemoryBlock* block, bool value) {
	return serialize_slice(block, sizeof(bool), &value);
}

[[nodiscard]] static tstr_static serialize_u64(MemoryBlock* block, uint64_t value) {
	return serialize_slice(block, sizeof(uint64_t), &value);
}

[[nodiscard]] static tstr_static serialize_fuse_file(MemoryBlock* block, const FuseFile* file);

[[nodiscard]] static tstr_static serialize_fuse_mock_flags(MemoryBlock* block,
                                                           const FuseFileMockFlags* flags);

[[nodiscard]] static tstr_static serialize_fuse_buffer(MemoryBlock* block,
                                                       const FuseBuffer* buffer);

static_assert(sizeof(uint64_t) == sizeof(size_t));

#define SERIALIZE_FIELD(block, value) \
	_Generic((value), \
	    bool: serialize_bool, \
	    size_t: serialize_u64, \
	    char*: serialize_str, \
	    const FuseFile*: serialize_fuse_file, \
	    const FuseBuffer*: serialize_fuse_buffer, \
	    const FuseFileMockFlags*: serialize_fuse_mock_flags)(block, value)

[[nodiscard]] static tstr_static serialize_fuse_file(MemoryBlock* block,
                                                     const FuseFile* const file) {

	tstr_static result = SERIALIZE_FIELD(block, file->name);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = SERIALIZE_FIELD(block, &(file->content));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = SERIALIZE_FIELD(block, &(file->flags));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] static tstr_static serialize_fuse_mock_flags(MemoryBlock* block,
                                                           const FuseFileMockFlags* flags) {

	tstr_static result = SERIALIZE_FIELD(block, flags->allow_stat);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = SERIALIZE_FIELD(block, flags->allow_read);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] static tstr_static serialize_fuse_buffer(MemoryBlock* block,
                                                       const FuseBuffer* buffer) {

	tstr_static result = SERIALIZE_FIELD(block, buffer->size);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = serialize_slice(block, buffer->size, buffer->data);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] tstr_static serialize_static_data(const MemoryBlock memory,
                                                const FuseStaticData* const data) {

	MemoryBlock block = memory;

	tstr_static result = SERIALIZE_FIELD(&block, data->dir_path);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = SERIALIZE_FIELD(&block, data->files.size);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	for(size_t i = 0; i < data->files.size; ++i) {
		const FuseFile* file = &(data->files.data[i]);

		result = SERIALIZE_FIELD(&block, file);

		if(!tstr_static_is_null(result)) {
			return result;
		}
	}

	result = SERIALIZE_FIELD(&block, data->debug);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	if(block.size != 0) {
		return TSTR_STATIC_LIT("memory size was too much");
	}

	return tstr_static_null();
}

#undef SERIALIZE_FIELD

//

[[nodiscard]] static tstr_static deserialize_slice(MemoryBlock* block, size_t size,
                                                   void** const data) {
	if(block->size < size) {
		return TSTR_STATIC_LIT("Not enough memory");
	}

	memcpy(data, block->ptr, size);

	memory_advance(block, size);

	return tstr_static_null();
}

[[nodiscard]] static tstr_static deserialize_str(MemoryBlock* block, char** const str) {
	if(block->size < 1) {
		return TSTR_STATIC_LIT("Not enough memory for string");
	}

	// because we serialize the string in place, this works

	char* const str_ptr = block->ptr;

	size_t str_len = strlen(str_ptr);

	if(block->size < (str_len + 1)) {
		return TSTR_STATIC_LIT("Not enough memory for whole null terminated string");
	}

	memory_advance(block, str_len + 1);

	*str = str_ptr;

	return tstr_static_null();
}

[[nodiscard]] static tstr_static deserialize_bool(MemoryBlock* block, bool* value) {
	return deserialize_slice(block, sizeof(bool), (void**)&value);
}

[[nodiscard]] static tstr_static deserialize_u64(MemoryBlock* block, uint64_t* value) {
	return deserialize_slice(block, sizeof(uint64_t), (void**)&value);
}

[[nodiscard]] static tstr_static deserialize_fuse_file(MemoryBlock* block, FuseFile* file);

[[nodiscard]] static tstr_static deserialize_fuse_mock_flags(MemoryBlock* block,
                                                             FuseFileMockFlags* flags);

[[nodiscard]] static tstr_static deserialize_fuse_buffer(MemoryBlock* block, FuseBuffer* buffer);

static_assert(sizeof(uint64_t) == sizeof(size_t));

#define DESERIALIZE_FIELD(block, value) \
	_Generic((value), \
	    bool*: deserialize_bool, \
	    size_t*: deserialize_u64, \
	    char**: deserialize_str, \
	    FuseFile*: deserialize_fuse_file, \
	    FuseBuffer*: deserialize_fuse_buffer, \
	    FuseFileMockFlags*: deserialize_fuse_mock_flags)(block, value)

[[nodiscard]] static tstr_static deserialize_fuse_file(MemoryBlock* block, FuseFile* const file) {

	tstr_static result = DESERIALIZE_FIELD(block, &(file->name));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = DESERIALIZE_FIELD(block, &(file->content));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = DESERIALIZE_FIELD(block, &(file->flags));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] static tstr_static deserialize_fuse_mock_flags(MemoryBlock* block,
                                                             FuseFileMockFlags* flags) {

	tstr_static result = DESERIALIZE_FIELD(block, &(flags->allow_stat));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = DESERIALIZE_FIELD(block, &(flags->allow_read));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] static tstr_static deserialize_fuse_buffer(MemoryBlock* block, FuseBuffer* buffer) {

	tstr_static result = DESERIALIZE_FIELD(block, &buffer->size);

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = deserialize_slice(block, buffer->size, &(buffer->data));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	return tstr_static_null();
}

[[nodiscard]] tstr_static deserialize_static_data(const MemoryBlock memory, FuseStaticData* data,
                                                  AllocatedDataArray* allocated_things) {
	MemoryBlock block = memory;

	tstr_static result = DESERIALIZE_FIELD(&block, &(data->dir_path));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	result = DESERIALIZE_FIELD(&block, &(data->files.size));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	FuseFile* files = malloc(data->files.size * sizeof(FuseFile));

	if(files == NULL) {
		return TSTR_STATIC_LIT("OOM in additional data allocation");
	}

	TvecResult push_res =
	    TVEC_PUSH(AllocatedData, allocated_things, (AllocatedData){ .data = files });

	if(push_res != TvecResultOk) {
		return TSTR_STATIC_LIT("OOM in additional data allocation");
	}

	for(size_t i = 0; i < data->files.size; ++i) {
		FuseFile* file = &(data->files.data[i]);

		result = DESERIALIZE_FIELD(&block, file);

		if(!tstr_static_is_null(result)) {
			return result;
		}
	}

	result = DESERIALIZE_FIELD(&block, &(data->debug));

	if(!tstr_static_is_null(result)) {
		return result;
	}

	if(block.size != 0) {
		return TSTR_STATIC_LIT("memory size was too much");
	}

	return tstr_static_null();
}

void free_allocated_data(AllocatedDataArray* allocated_things) {
	for(size_t i = 0; i < TVEC_LENGTH(AllocatedData, *allocated_things); ++i) {
		AllocatedData data = TVEC_AT(AllocatedData, *allocated_things, i);
		free(data.data);
	}
	TVEC_FREE(AllocatedData, allocated_things);
}

TVEC_IMPLEMENT_VEC_TYPE(AllocatedData)
