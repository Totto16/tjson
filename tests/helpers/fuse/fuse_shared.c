#include "./fuse_shared.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
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
	((SharedAllocatorResult){ .allocator = NULL, \
	                          .memory = (FuseSharedMemory){ .state = NULL, .rest = NULL } })

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

	FuseSharedMemory memory = { .state = ptr, .rest = ((uint8_t*)ptr) + sizeof(FuseSharedState) };

	return ((SharedAllocatorResult){ .allocator = allocator, .memory = memory });
}

#undef FREE_AT_END

[[nodiscard]] SharedAllocatorResult shared_allocator_from_path(const char* path) {
	// TODO
	(void)path;
	return SHARED_ALLOCATOR_ERROR();
}

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
