

#include "./custom_mock.h"

#define CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name) name##_state_impl__
#define CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name) g_##name##_state_variable_impl__
#define CUSTOM_MOCK_REAL_NAME(name) __real_##name

#define CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(name, return_type, params, param_names, \
                                                        fn_type) \
	typedef struct { \
		fn_type wrapped_fn_ptr; \
	} CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name); \
\
	[[maybe_unused]] static CUSTOM_MOCK_STRUCT_STATE_IMPL_NAME(name) \
	    CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name) = { .wrapped_fn_ptr = NULL }; \
\
	return_type CUSTOM_MOCK_REAL_NAME(name) params; \
\
	return_type CUSTOM_MOCK_WRAP_NAME(name) params { \
		if(CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).wrapped_fn_ptr != NULL) { \
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
		CUSTOM_MOCK_STRUCT_STATE_GLOBAL_NAME(name).wrapped_fn_ptr = fun; \
		return true; \
	}

CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(open, int,
                                                (const char* path, int flags, mode_t mode),
                                                (path, flags, mode), open_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(fstat, int, (int fd, struct stat* statbuf),
                                                (fd, statbuf), fstat_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(read, ssize_t, (int fd, void* buf, size_t count),
                                                (fd, buf, count), read_function_type)
CUSTOM_MOCK_DEFINE_MOCK_FUNCTION_IMPLEMENTATION(close, int, (int fd), (fd), close_function_type)
