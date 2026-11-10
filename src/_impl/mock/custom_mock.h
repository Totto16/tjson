

#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CUSTOM_MOCK_WRAP_NAME(name) __wrap_##name
#define CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(name) name##_is_wrapped
#define CUSTOM_MOCK_WRAP_NAME_WRAP(name) wrap_##name

#define CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(name, return_type, params, fn_type) \
	return_type CUSTOM_MOCK_WRAP_NAME(name) params; \
	bool CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(name)(void); \
	bool CUSTOM_MOCK_WRAP_NAME_WRAP(name)(fn_type fun);

typedef int (*open_function_type)(const char* path, int flags, mode_t mode);

typedef int (*fstat_function_type)(int fd, struct stat* statbuf);

typedef ssize_t (*read_function_type)(int fd, void* buf, size_t count);

typedef int (*close_function_type)(int fd);

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(open, int, (const char* path, int flags, mode_t mode),
                                        open_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(fstat, int, (int fd, struct stat* statbuf),
                                        fstat_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(read, ssize_t, (int fd, void* buf, size_t count),
                                        read_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(close, int, (int fd), close_function_type)

#ifdef __cplusplus
}
#endif
