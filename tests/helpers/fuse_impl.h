

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

typedef struct {
	const void* data;
	size_t size;
} FuseBuffer;

typedef struct {
	const char* name;
	FuseBuffer content;
} FuseFile;

[[nodiscard]] FuseCreateResult create_new_fuse_file(const char* dir, const FuseFile* files,
                                                    size_t file_amount);

[[nodiscard]] bool clear_fuse_file(FUSEHandle* handle);

#ifdef __cplusplus
}
#endif
