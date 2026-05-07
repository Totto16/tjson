

#pragma once

#include <stddef.h>

#include <tstr.h>

#include "./fuse_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FUSEHandleImpl FUSEHandle;

// manual "variant", but only used internally, so it's fine
typedef struct {
	bool is_error;
	union {
		FUSEHandle* ok;
		tstr_static error;
	} data;
} FuseCreateResult;

[[nodiscard]] FuseCreateResult create_new_fuse_file(char* dir, FuseFile* files, size_t file_amount,
                                                    bool debug);

[[nodiscard]] int clear_fuse_file(FUSEHandle* handle);

#ifdef __cplusplus
}
#endif
