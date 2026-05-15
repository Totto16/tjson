

#pragma once

#include <stddef.h>

#include <tstr.h>

#include "./fuse_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FUSEHandleImpl FUSEHandle;

GENERATE_VARIANT_ALL_FUSE_CREATE_RESULT()

[[nodiscard]] FuseCreateResult create_new_fuse_file(char* dir, FuseFile* files, size_t file_amount,
                                                    bool debug);

[[nodiscard]] int clear_fuse_file(FUSEHandle* handle);

#ifdef __cplusplus
}
#endif
