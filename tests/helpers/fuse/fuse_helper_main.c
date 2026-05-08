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

	MemoryBlock rest_block = allocator_result.memory.rest;
	SharedAllocator* allocator = allocator_result.allocator;
	FuseSharedState* shared_state = allocator_result.memory.state;

	if(allocator == NULL || shared_state == NULL || rest_block.ptr == NULL) {
		fprintf(stderr, "shared allocator error\n");
		return EXIT_FAILURE;
	}

	FuseStaticData static_data = {};

	AllocatedDataArray allocated_things = TVEC_EMPTY(AllocatedData);

	tstr_static result = deserialize_static_data(rest_block, &static_data, &allocated_things);

	if(!tstr_static_is_null(result)) {
		fprintf(stderr, "deserialize static data failed: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(result));
		return EXIT_FAILURE;
	}

	if(static_data.debug) {
		fprintf(stdout, "Mounting custom fuse file system on path: '%s'\n", static_data.dir_path);
		fprintf(stdout, "files:\n");
		for(size_t i = 0; i < static_data.files.size; ++i) {
			const FuseFile file = static_data.files.data[i];
			fprintf(stdout, "[%zu]\n", i);
			fprintf(stdout, "\tname: %s\n", file.name);
			fprintf(stdout, "\tcontent: %.*s\n", (int)file.content.size, (char*)file.content.data);
			fprintf(stdout, "\tflags: s%c\n", get_char_for_fail_scenario(file.flags.scenario));
		}
	}

	const FuseHandleResult fuse_result = fuse_start_fn(shared_state, &static_data);

	free_allocated_data(&allocated_things);
	shared_allocator_deinit(allocator);

	return fuse_result;
}
