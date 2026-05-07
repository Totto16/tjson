#include <stdio.h>
#include <stdlib.h>

#include "./fuse_impl_run.h"
#include "./fuse_shared.h"

int main(int argc, char** argv) {

	if(argc != 2) {
		fprintf(stderr, "Usage %s <shm_path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char* path = argv[1];

	SharedAllocatorResult allocator_result = shared_allocator_from_path(path);

	if(allocator_result.allocator == NULL || allocator_result.memory.state == NULL) {
		fprintf(stderr, "shared allocator error\n");
		return EXIT_FAILURE;
	}

	MemoryBlock rest_block = allocator_result.memory.rest;
	SharedAllocator* allocator = allocator_result.allocator;
	FuseSharedState* shared_state = allocator_result.memory.state;

	FuseStaticData static_data = {};

	AllocatedDataArray allocated_things = TVEC_EMPTY(AllocatedData);

	tstr_static result = deserialize_static_data(rest_block, &static_data, &allocated_things);

	if(tstr_static_is_null(result)) {
		fprintf(stderr, "deserialize static data failed: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(result));
		return EXIT_FAILURE;
	}

	const FuseHandleResult fuse_result = fuse_start_fn(shared_state, &static_data);

	free_allocated_data(&allocated_things);
	shared_allocator_deinit(allocator);

	return fuse_result;
}
