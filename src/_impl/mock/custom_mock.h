

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
#define CUSTOM_MOCK_WRAP_NAME_UNWRAP(name) unwrap_##name
#define CUSTOM_MOCK_WRAP_WRAPPED_NAME(name) wrapped_fn_##name
#define CUSTOM_MOCK_WRAP_COUNT_NAME(name) wrapped_calls_count_##name

#define CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(name, return_type, params, fn_type) \
	return_type CUSTOM_MOCK_WRAP_WRAPPED_NAME(name) params; \
	bool CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(name)(void); \
	bool CUSTOM_MOCK_WRAP_NAME_WRAP(name)(fn_type fun); \
	void CUSTOM_MOCK_WRAP_NAME_UNWRAP(name)(void); \
	size_t CUSTOM_MOCK_WRAP_COUNT_NAME(name)(void);

typedef int (*fstat_function_type)(int fd, struct stat* statbuf);

typedef ssize_t (*read_function_type)(int fd, void* buf, size_t count);

typedef int (*close_function_type)(int fd);

typedef int (*open_function_type)(const char* path, int flags, mode_t mode);

#ifdef TJSON_HAVE_FILE_RELATED_64_FUNCTIONS

typedef int (*open64_function_type)(const char* path, int flags, mode_t mode);

typedef struct stat64 struct_stat64_impl;

	#define CUSTOM_MOCK_STATE64_STRUCT struct_stat64_impl

typedef int (*fstat64_function_type)(int fd, CUSTOM_MOCK_STATE64_STRUCT* statbuf);

#endif

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(fstat, int, (int fd, struct stat* statbuf),
                                        fstat_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(read, ssize_t, (int fd, void* buf, size_t count),
                                        read_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(close, int, (int fd), close_function_type)

#ifdef TJSON_HAVE_FILE_RELATED_64_FUNCTIONS

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(fstat64, int, (int fd, CUSTOM_MOCK_STATE64_STRUCT* statbuf),
                                        fstat64_function_type)

#endif

// note open and open64 is special, as it is variadic

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(open, int, (const char* path, int flags, ...),
                                        open_function_type)

#ifdef TJSON_HAVE_FILE_RELATED_64_FUNCTIONS

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_HEADER(open64, int, (const char* path, int flags, ...),
                                        open64_function_type)

#endif

#ifdef __cplusplus
}
#endif
