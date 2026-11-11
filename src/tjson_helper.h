
#pragma once

#include "./tjson.h"

#include <trtti.h>

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: this doesn't support every json path syntax according to spec, only the needed one!

typedef struct JsonPathImpl JsonPath;

TJSON_NODISCARD JsonPath* json_path_get_root(void);

TJSON_NODISCARD bool json_path_add_object_key_dup(JsonPath* path, const tstr* key);

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, TJSON_MOVED(tstr) key);

TJSON_NODISCARD bool json_path_add_array_index(JsonPath* path, size_t index);

TJSON_NODISCARD bool json_path_remove_last_segment(JsonPath* path);

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
} JsonIterateError;

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

#define RETURN_JSON_ITERATE_ERROR_DYNAMIC(raw_err_msg, err_msg_fmt, ...) \
	do { \
		tstr fmt_str = tstr_from(""); \
		if(tstr_is_null(&fmt_str)) { \
			return new_json_iterate_result_error( \
			    (JsonIterateError){ .err = TSTR_LIT(raw_err_msg) }); \
		} \
		TStrResult res = tstr_fmt(&fmt_str, err_msg_fmt, __VA_ARGS__); \
		if(res != TStrResultOk) { \
			return new_json_iterate_result_error( \
			    (JsonIterateError){ .err = TSTR_LIT(raw_err_msg) }); \
		} \
		return new_json_iterate_result_error((JsonIterateError){ .err = fmt_str }); \
	} while(false)

#ifdef __cplusplus
}
#endif
