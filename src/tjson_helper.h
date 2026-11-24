
#pragma once

#include "./tjson.h"

#include <trtti.h>

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: this doesn't support every json path syntax according to spec, only the needed one!

typedef struct JsonPathImpl JsonPath;

typedef struct JsonPathSegmentRefImpl JsonPathSegmentRef;

TJSON_NODISCARD JsonPath* json_path_get_root(void);

// NOTE: this tstr is the raw value, no escapes are possible, we only support constructing json
// paths, not parsing them!
TJSON_NODISCARD bool json_path_add_object_key_dup(JsonPath* path, const tstr* key);

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, TJSON_MOVED(tstr) key);

TJSON_NODISCARD bool json_path_add_array_index(JsonPath* path, size_t index);

TJSON_NODISCARD bool json_path_remove_last_segment(JsonPath* path);

// NOTE: this ref is only valid until the next modification of the path, so just use it briefly!
TJSON_NODISCARD JsonPathSegmentRef* json_path_get_last_segment(const JsonPath* path);

TJSON_NODISCARD bool json_path_segment_is_object_key(const JsonPathSegmentRef* segment_ref,
                                                     const tstr* key);

TJSON_NODISCARD bool json_path_segment_is_object_key_view(const JsonPathSegmentRef* segment_ref,
                                                          tstr_view key);

TJSON_NODISCARD bool json_path_segment_is_array_index(const JsonPathSegmentRef* segment_ref,
                                                      size_t index);

void free_json_path_segment_ref(JsonPathSegmentRef* segment_ref);

TJSON_NODISCARD bool json_path_is_root(const JsonPath* path);

TJSON_NODISCARD bool json_path_is_parent_of(const JsonPath* parent, const JsonPath* path);

TJSON_NODISCARD bool json_path_is_eq(const JsonPath* path1, const JsonPath* path2);

TJSON_NODISCARD tstr json_path_to_str(const JsonPath* path);

typedef struct {
	bool compact;
} JsonPathStringOptions;

TJSON_NODISCARD tstr json_path_to_str_advanced(const JsonPath* path, JsonPathStringOptions options);

void free_json_path(JsonPath* path);

typedef struct {
	tstr err;
	JsonPath* path;
} JsonIterateError;

void free_json_iterate_error(JsonIterateError* path);

TJSON_NODISCARD tstr json_iterate_error_to_str(const JsonIterateError* error);

TJSON_NODISCARD tstr json_iterate_error_to_str_advanced(const JsonIterateError* error,
                                                        JsonPathStringOptions options);

GENERATE_VARIANT_ALL_JSON_ITERATE_RESULT() // GCOVR_EXCL_LINE

typedef struct {
	RTTIAnnotatedValue handle;
} JsonIterateResolvedValue;

typedef struct {
	const tstr* key;
	RTTIAnnotatedValue value;
} JsonIterateObjectEntry;

typedef struct {
	RTTIAnnotatedValue value;
} JsonIterateArrayEntry;

GENERATE_VARIANT_ALL_JSON_ITERATE_VALUE()

typedef JsonIterateResult (*JsonValueIterateCallback)(const JsonPath* path,
                                                      RTTIAnnotatedValue parent,
                                                      JsonIterateValue value,
                                                      RTTIAnnotatedValue userdata);

typedef void (*JsonIterateFreeCallback)(RTTIAnnotatedValue data);

TJSON_NODISCARD JsonIterateResult json_value_iterate(const JsonValue* json_value,
                                                     JsonValueIterateCallback iterate_callback,
                                                     JsonIterateFreeCallback free_callback,
                                                     RTTIAnnotatedValue start,
                                                     RTTIAnnotatedValue userdata);

#define RETURN_JSON_ITERATE_ERROR_DYNAMIC_EXTENDED(path_arg, free_expr, raw_err_msg, err_msg_fmt, \
                                                   ...) \
	do { \
		tstr fmt_str = tstr_from(""); \
		if(tstr_is_null(&fmt_str)) { \
			free_expr; \
			return new_json_iterate_result_error((JsonIterateError){ \
			    .err = TSTR_LIT(raw_err_msg), \
			    .path = (path_arg), \
			}); \
		} \
		TStrResult res = tstr_fmt(&fmt_str, err_msg_fmt, __VA_ARGS__); \
		if(res != TStrResultOk) { \
			free_expr; \
			return new_json_iterate_result_error((JsonIterateError){ \
			    .err = TSTR_LIT(raw_err_msg), \
			    .path = (path_arg), \
			}); \
		} \
		free_expr; \
		return new_json_iterate_result_error((JsonIterateError){ \
		    .err = fmt_str, \
		    .path = (path_arg), \
		}); \
	} while(false)

#define RETURN_JSON_ITERATE_ERROR_DYNAMIC_WITH_PATH(path_arg, raw_err_msg, err_msg_fmt, ...) \
	RETURN_JSON_ITERATE_ERROR_DYNAMIC_EXTENDED( \
	    path_arg, do {} while(false), raw_err_msg, err_msg_fmt, __VA_ARGS__)

#define RETURN_JSON_ITERATE_ERROR_DYNAMIC(raw_err_msg, err_msg_fmt, ...) \
	RETURN_JSON_ITERATE_ERROR_DYNAMIC_WITH_PATH(NULL, raw_err_msg, err_msg_fmt, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
