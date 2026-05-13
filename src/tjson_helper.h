
#pragma once

#include "./tjson.h"

#include <trtti.h>

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: this doesn't support every json path syntax according to spec, only the needed one!

typedef struct JsonPathImpl JsonPath;

TJSON_NODISCARD JsonPath* json_path_from_tstr_view(tstr_view path);

TJSON_NODISCARD JsonPath* json_path_from_tstr(const tstr* path);

TJSON_NODISCARD JsonPath* json_path_get_root(void);

TJSON_NODISCARD bool json_path_add_object_key(JsonPath* path, const tstr* key);

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, tstr* key);

TJSON_NODISCARD bool json_path_remove_last_object(JsonPath* path);


TJSON_NODISCARD bool json_path_is_root(const JsonPath* path);

void free_json_path(JsonPath* path);

typedef struct {
	tstr_static err;
} JsonIterateError;

GENERATE_VARIANT_ALL_JSON_ITERATE_RESULT()

typedef struct {
	const tstr* key;
} JsonIterateObjectEntry;

GENERATE_VARIANT_ALL_JSON_ITERATE_VALUE()

typedef JsonIterateResult (*JsonValueIterateCallback)(const JsonPath* path,
                                                      RTTIAnnotatedValue parent,
                                                      JsonIterateValue value);

TJSON_NODISCARD JsonIterateResult json_value_iterate(const JsonValue* json_value,
                                                     JsonValueIterateCallback iterate_callback,
                                                     RTTIAnnotatedValue start);

#ifdef __cplusplus
}
#endif
