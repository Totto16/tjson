#include "./tjson_helper.h"

//  https://en.wikipedia.org/wiki/JSONPath
// https://www.rfc-editor.org/rfc/rfc9535

struct JsonPathImpl {
	int todo;
};

TJSON_NODISCARD JsonPath* json_path_from_tstr_view(tstr_view path) {
	// TODO
	(void)path;
	return NULL;
}

TJSON_NODISCARD JsonPath* json_path_from_tstr(const tstr* path) {
	// TODO
	(void)path;
	return NULL;
}

TJSON_NODISCARD JsonPath* json_path_get_root(void) {
	// TODO
	return NULL;
}

TJSON_NODISCARD bool json_path_add_object_key(JsonPath* path, const tstr* key) {
	// TODO
	(void)path;
	(void)key;
	return false;
}

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, tstr* key_moved) {
	// TODO
	(void)path;
	(void)key_moved;
	return false;
}

TJSON_NODISCARD bool json_path_remove_last_object(JsonPath* path) {
	// TODO
	(void)path;
	return false;
}

TJSON_NODISCARD bool json_path_is_root(const JsonPath* path) {
	// TODO
	(void)path;
	return false;
}

void free_json_path(JsonPath* path) {
	// TODO
	(void)path;
	if(path == (void*)0x1) {
		return;
	}

	TJSON_UNREACHABLE();
}

//

TJSON_NODISCARD static JsonIterateResult
json_value_iterate_impl(const JsonValue* json_value, JsonValueIterateCallback iterate_callback,
                        RTTIAnnotatedValue start, JsonPath* json_path) {

	SWITCH_JSON_VALUE(*json_value) {
		CASE_JSON_VALUE_IS_OBJECT_CONST(*json_value) {

			JsonIterateValue obj_start = new_json_iterate_value_object_start();

			JsonIterateResult start_result = iterate_callback(json_path, start, obj_start);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(start_result) {
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue obj_handle = json_iterate_result_get_as_ok(start_result);

			{
				JsonObjectIter* iter = json_object_get_iterator(object.obj);

				if(iter == NULL) {
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_STATIC_LIT("json object iterator couldn't be allocated") });
				}

#define FREE_AT_END() \
	do { \
		json_object_free_iterator(iter); \
	} while(false)

				while(true) {

					const JsonObjectEntry* const next_entry = json_object_iterator_next(iter);

					if(next_entry == NULL) {
						break;
					}

					const JsonString* const key = json_object_entry_get_key(next_entry);

					tstr key_tstr = json_string_get_as_str(key);

					if(tstr_is_null(&key_tstr)) {
						FREE_AT_END();
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_STATIC_LIT("json key string convert error") });
					}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		json_object_free_iterator(iter); \
		tstr_free(&key_tstr); \
	} while(false)

					JsonIterateValue obj_entry = new_json_iterate_value_object_entry(
					    (JsonIterateObjectEntry){ .key = &key_tstr });

					JsonIterateResult entry_result =
					    iterate_callback(json_path, obj_handle, obj_entry);

					IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(entry_result) {
						FREE_AT_END();
						return new_json_iterate_result_error(error.error);
					}

					RTTIAnnotatedValue entry_handle = json_iterate_result_get_as_ok(start_result);

					const JsonValue value = json_object_entry_get_value(next_entry);

					bool add_result = json_path_add_object_key_moved(json_path, &key_tstr);

					if(!add_result) {
						FREE_AT_END();
						return new_json_iterate_result_error(
						    (JsonIterateError){ .err = TSTR_STATIC_LIT("json path add failed") });
					}

					key_tstr = tstr_null();

					JsonIterateResult recursive_result =
					    json_value_iterate_impl(&value, iterate_callback, entry_handle, json_path);

					IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(recursive_result) {
						FREE_AT_END();
						return new_json_iterate_result_error(error.error);
					}

					// note: the "recursive_result" RTTI value is reused for the object handle, if
					// it isn't NULL, so that you can swap this out or just return NULL and
					// everything still works
					RTTIAnnotatedValue result_handle =
					    json_iterate_result_get_as_ok(recursive_result);

					if(result_handle.ptr != NULL) {
						obj_handle = result_handle;
					}

					bool remove_result = json_path_remove_last_object(json_path);

					if(!remove_result) {
						FREE_AT_END();
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_STATIC_LIT("json path remove failed") });
					}

#undef FREE_AT_END

					tstr_free(&key_tstr);
				}

				json_object_free_iterator(iter);
			}

			JsonIterateValue obj_end = new_json_iterate_value_object_end();

			JsonIterateResult end_result = iterate_callback(json_path, obj_handle, obj_end);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(end_result) {
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue end_handle = json_iterate_result_get_as_ok(start_result);

			return new_json_iterate_result_ok(end_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_ARRAY_CONST(*json_value) {
			(void)array;
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: array") });
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_NUMBER_CONST(*json_value) {
			(void)number;
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: number") });
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_STRING_CONST(*json_value) {
			(void)string;
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: string") });
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_BOOLEAN_CONST(*json_value) {
			(void)boolean;
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_STATIC_LIT("boolean") });
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_NULL() {
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: null") });
		}
		VARIANT_CASE_END();
		default: {
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_STATIC_LIT("json value has invalid state") });
		}
	}
}

#undef FREE_AT_END

TJSON_NODISCARD JsonIterateResult json_value_iterate(const JsonValue* const json_value,
                                                     JsonValueIterateCallback iterate_callback,
                                                     RTTIAnnotatedValue start) {

	JsonPath* json_path = json_path_get_root();

	if(json_path == NULL) {
		return new_json_iterate_result_error(
		    (JsonIterateError){ .err = TSTR_STATIC_LIT("json path allocation error") });
	}

	JsonIterateResult result =
	    json_value_iterate_impl(json_value, iterate_callback, start, json_path);

	free_json_path(json_path);

	return result;
}
