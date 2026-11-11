

#include "./custom_mock.h"

#include <stdarg.h>

#define CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name) name##_state_impl__
#define CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name) g_##name##_state_variable_impl__
#define CUSTOM_MOCK_REAL_NAME(name) __real_##name

#define CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(name, return_type, params, param_names, \
                                                        fn_type) \
	typedef struct { \
		fn_type wrapped_fn_ptr; \
		size_t call_count; \
	} CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name); \
\
	return_type CUSTOM_MOCK_WRAP_NAME(name) params; \
\
	return_type CUSTOM_MOCK_WRAP_WRAPPED_NAME(name) params { \
		return CUSTOM_MOCK_WRAP_NAME(name) param_names; \
	} \
\
	[[maybe_unused]] static CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name) \
	    CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name) = { \
		    .wrapped_fn_ptr = NULL, \
		    .call_count = 0, \
	    }; \
\
	return_type CUSTOM_MOCK_REAL_NAME(name) params; \
\
	return_type CUSTOM_MOCK_WRAP_NAME(name) params { \
		if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).wrapped_fn_ptr != NULL) { \
			CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).call_count++; \
			return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).wrapped_fn_ptr param_names; \
		} \
\
		return CUSTOM_MOCK_REAL_NAME(name) param_names; \
	} \
\
	bool CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(name)(void) { \
		return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).wrapped_fn_ptr != NULL; \
	} \
\
	bool CUSTOM_MOCK_WRAP_NAME_WRAP(name)(fn_type fun) { \
		if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).wrapped_fn_ptr != NULL) { \
			return false; \
		} \
\
		if(fun == NULL) { \
			return false; \
		} \
\
		CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name) = \
		    (CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name)){ .wrapped_fn_ptr = fun, .call_count = 0 }; \
		return true; \
	} \
\
	void CUSTOM_MOCK_WRAP_NAME_UNWRAP(name)(void) { \
		CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name) = \
		    (CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name)){ .wrapped_fn_ptr = NULL, .call_count = 0 }; \
	} \
\
	size_t CUSTOM_MOCK_WRAP_COUNT_NAME(name)(void) { \
		return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).call_count; \
	}

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(fstat, int, (int fd, struct stat* statbuf),
                                                (fd, statbuf), fstat_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(read, ssize_t, (int fd, void* buf, size_t count),
                                                (fd, buf, count), read_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(close, int, (int fd), (fd), close_function_type)

#ifdef TJSON_HAVE_FILE_RELATED_64_FUNCTIONS

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(fstat64, int,
                                                (int fd, CUSTOM_MOCK_STATE64_STRUCT* statbuf),
                                                (fd, statbuf), fstat64_function_type)
#endif

// note open and open64 is special, as it is variadic

// open helper functions

static int _wrap_open_helper_impl__(open_function_type fun, const char* pathname, int flags,
                                    va_list args) {
	mode_t mode = 0;

	if(flags & O_CREAT) {
		mode = va_arg(args, mode_t);

		return fun(pathname, flags, mode);
	}

	return fun(pathname, flags, 0);
}

typedef int (*open_function_type_orig)(const char* path, int flags, ...);

static int _wrap_open_helper_orig_impl__(open_function_type_orig fun, const char* pathname,
                                         int flags, va_list args) {
	mode_t mode = 0;

	if(flags & O_CREAT) {
		mode = va_arg(args, mode_t);

		return fun(pathname, flags, mode);
	}

	return fun(pathname, flags);
}

// impl

typedef struct {
	open_function_type wrapped_fn_ptr;
	size_t call_count;
} CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open);

[[maybe_unused]] static CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open)
    CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open) = {
	    .wrapped_fn_ptr = NULL,
	    .call_count = 0,
    };

int CUSTOM_MOCK_REAL_NAME(open)(const char* path, int flags, ...);

int CUSTOM_MOCK_WRAP_NAME(open)(const char* path, int flags, ...);

int CUSTOM_MOCK_WRAP_WRAPPED_NAME(open)(const char* path, int flags, ...) {
	va_list args;
	va_start(args, flags);

	const int result =
	    _wrap_open_helper_orig_impl__(CUSTOM_MOCK_WRAP_NAME(open), path, flags, args);

	va_end(args);
	return result;
}

int CUSTOM_MOCK_WRAP_NAME(open)(const char* path, int flags, ...) {
	va_list args;
	va_start(args, flags);

	if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open).wrapped_fn_ptr != NULL) {
		CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open).call_count++;
		const int result = _wrap_open_helper_impl__(
		    CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open).wrapped_fn_ptr, path, flags, args);

		va_end(args);
		return result;
	}

	const int result =
	    _wrap_open_helper_orig_impl__(CUSTOM_MOCK_REAL_NAME(open), path, flags, args);

	va_end(args);
	return result;
}

bool CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(open)(void) {
	return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open).wrapped_fn_ptr != NULL;
}

bool CUSTOM_MOCK_WRAP_NAME_WRAP(open)(open_function_type fun) {
	if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open).wrapped_fn_ptr != NULL) {
		return false;
	}

	if(fun == NULL) {
		return false;
	}

	CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open) =
	    (CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open)){ .wrapped_fn_ptr = fun, .call_count = 0 };
	return true;
}

void CUSTOM_MOCK_WRAP_NAME_UNWRAP(open)(void) {
	CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open) =
	    (CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open)){ .wrapped_fn_ptr = NULL, .call_count = 0 };
}

size_t CUSTOM_MOCK_WRAP_COUNT_NAME(open)(void) {
	return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open).call_count;
}

#ifdef TJSON_HAVE_FILE_RELATED_64_FUNCTIONS

typedef struct {
	open64_function_type wrapped_fn_ptr;
	size_t call_count;
} CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open64);

[[maybe_unused]] static CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open64)
    CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64) = {
	    .wrapped_fn_ptr = NULL,
	    .call_count = 0,
    };

int CUSTOM_MOCK_REAL_NAME(open64)(const char* path, int flags, ...);

int CUSTOM_MOCK_WRAP_NAME(open64)(const char* path, int flags, ...);

int CUSTOM_MOCK_WRAP_WRAPPED_NAME(open64)(const char* path, int flags, ...) {
	va_list args;
	va_start(args, flags);

	const int result =
	    _wrap_open_helper_orig_impl__(CUSTOM_MOCK_WRAP_NAME(open64), path, flags, args);

	va_end(args);
	return result;
}

int CUSTOM_MOCK_WRAP_NAME(open64)(const char* path, int flags, ...) {
	va_list args;
	va_start(args, flags);

	if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64).wrapped_fn_ptr != NULL) {
		CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64).call_count++;
		const int result = _wrap_open_helper_impl__(
		    CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64).wrapped_fn_ptr, path, flags, args);

		va_end(args);
		return result;
	}

	const int result =
	    _wrap_open_helper_orig_impl__(CUSTOM_MOCK_REAL_NAME(open64), path, flags, args);

	va_end(args);
	return result;
}

bool CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(open64)(void) {
	return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64).wrapped_fn_ptr != NULL;
}

bool CUSTOM_MOCK_WRAP_NAME_WRAP(open64)(open64_function_type fun) {
	if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64).wrapped_fn_ptr != NULL) {
		return false;
	}

	if(fun == NULL) {
		return false;
	}

	CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64) =
	    (CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open64)){ .wrapped_fn_ptr = fun, .call_count = 0 };
	return true;
}

void CUSTOM_MOCK_WRAP_NAME_UNWRAP(open64)(void) {
	CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64) =
	    (CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(open64)){ .wrapped_fn_ptr = NULL, .call_count = 0 };
}

size_t CUSTOM_MOCK_WRAP_COUNT_NAME(open64)(void) {
	return CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(open64).call_count;
}

#endif
