

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FUSEHandleImpl FUSEHandle;

[[nodiscard]] FUSEHandle* create_new_fuse_file(const char* file, const void* data,
                                               size_t data_size);

void clear_fuse_file(FUSEHandle* handle);

#ifdef __cplusplus
}
#endif
