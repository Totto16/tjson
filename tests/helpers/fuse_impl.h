

#pragma once

#include <stddef.h>

#include <tstr.h>

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

[[nodiscard]] FuseCreateResult create_new_fuse_file(const char* file, const void* data,
                                                    size_t data_size);

[[nodiscard]] bool clear_fuse_file(FUSEHandle* handle);

#ifdef __cplusplus
}
#endif
