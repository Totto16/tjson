#include "./tjson_helper.h"

#include <tvec.h>

GENERATE_VARIANT_ALL_JSON_PATH_SEGMENT()

/* NOLINTBEGIN(misc-use-internal-linkage,totto-function-passing-type,totto-use-fixed-width-types-var)
 */
// GCOVR_EXCL_START (external library)
TVEC_DEFINE_AND_IMPLEMENT_VEC_TYPE(JsonPathSegment)
// GCOVR_EXCL_STOP
/* NOLINTEND(misc-use-internal-linkage,totto-function-passing-type,totto-use-fixed-width-types-var)
 */

typedef TVEC_TYPENAME(JsonPathSegment) JsonPathSegmentsArray;

//  https://en.wikipedia.org/wiki/JSONPath
// https://www.rfc-editor.org/rfc/rfc9535

struct JsonPathImpl {
	JsonPathSegmentsArray segments;
};

TJSON_NODISCARD JsonPath* json_path_from_tstr_view(tstr_view path) {

	JsonPath* result = json_path_get_root();

	if(result == NULL) {
		return NULL;
	}

	(void)path;
	// TODO
	return NULL;
}

TJSON_NODISCARD JsonPath* json_path_from_tstr(const tstr* path) {
	// TODO
	(void)path;
	return NULL;
}

TJSON_NODISCARD JsonPath* json_path_get_root(void) {
	JsonPath* path = malloc(sizeof(JsonPath));

	if(path == NULL) {
		return NULL;
	}

	*path = (JsonPath){ .segments = TVEC_EMPTY(JsonPathSegment) };

	return path;
}

TJSON_NODISCARD bool json_path_add_object_key(JsonPath* const path, const tstr* key) {

	tstr key_duped = tstr_dup(key);

	if(tstr_is_null(&key_duped)) {
		return false;
	}

	return json_path_add_object_key_moved(path, &key_duped);
}

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, tstr* key_moved) {

	TvecResult result =
	    TVEC_PUSH(JsonPathSegment, &(path->segments), new_json_path_segment_object_key(*key_moved));

	if(result != TvecResultOk) {

		tstr_free(key_moved);
		*key_moved = tstr_null();

		return false;
	}

	*key_moved = tstr_null();

	return true;
}

static void free_json_path_segment(JsonPathSegment segment);

TJSON_NODISCARD bool json_path_remove_last_object(JsonPath* path) {
	if(TVEC_LENGTH(JsonPathSegment, path->segments) == 0) {
		return false;
	}

	JsonPathSegment segment = TVEC_POP_GET(JsonPathSegment, &(path->segments));

	free_json_path_segment(segment);

	return true;
}

TJSON_NODISCARD bool json_path_is_root(const JsonPath* path) {
	const size_t len = TVEC_LENGTH(JsonPathSegment, path->segments);

	return len == 0;
}

TJSON_NODISCARD static bool path_segment_eq(JsonPathSegment segment1, JsonPathSegment segment2) {

	SWITCH_JSON_PATH_SEGMENT(segment1) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT(segment1, object1) {

			IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST(segment2, object2) {
				return tstr_eq(&object1.name, &object2.name);
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_PATH_SEGMENT_IS_ARRAY_KEY_CONST(segment1, array1) {

			IF_JSON_PATH_SEGMENT_IS_ARRAY_KEY_CONST(segment2, array2) {
				return array1.index == array2.index;
			}

			return false;
		}
		VARIANT_CASE_END();
		default: {
			return false;
		}
	}
}

TJSON_NODISCARD bool json_path_is_parent_of(const JsonPath* path, const JsonPath* parent) {

	const size_t path_len = TVEC_LENGTH(JsonPathSegment, path->segments);
	const size_t parent_len = TVEC_LENGTH(JsonPathSegment, parent->segments);

	if(path_len != (parent_len + 1)) {
		return false;
	}

	for(size_t i = 0; i < parent_len; ++i) {
		JsonPathSegment path_value = TVEC_AT(JsonPathSegment, path->segments, i);
		JsonPathSegment parent_value = TVEC_AT(JsonPathSegment, parent->segments, i);

		if(!path_segment_eq(path_value, parent_value)) {
			return false;
		}
	}

	return true;
}

static void free_json_path_segment(JsonPathSegment segment) {
	SWITCH_JSON_PATH_SEGMENT(segment) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT(segment) {
			tstr_free(&object_key.name);
		}
		break;
		VARIANT_CASE_END();
		CASE_JSON_PATH_SEGMENT_IS_ARRAY_KEY_IGN() {
			// noop
		}
		break;
		VARIANT_CASE_END();
		default: {
			break;
		}
	}
}

void free_json_path(JsonPath* path) {

	for(size_t i = 0; i < TVEC_LENGTH(JsonPathSegment, path->segments); ++i) {
		JsonPathSegment value = TVEC_AT(JsonPathSegment, path->segments, i);
		free_json_path_segment(value);
	}
	TVEC_FREE(JsonPathSegment, &(path->segments));

	free(path);
}

//

TJSON_NODISCARD static JsonIterateResult
json_value_iterate_impl(const JsonValue* json_value, JsonValueIterateCallback iterate_callback,
                        RTTIAnnotatedValue annotated_value_start, JsonPath* json_path) {

	SWITCH_JSON_VALUE(*json_value) {
		CASE_JSON_VALUE_IS_OBJECT_CONST(*json_value) {

			JsonIterateValue obj_start = new_json_iterate_value_object_start();

			JsonIterateResult start_result =
			    iterate_callback(json_path, annotated_value_start, obj_start);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(start_result) {
				return new_json_iterate_result_error(error.error);
			}

			const RTTIAnnotatedValue obj_handle = json_iterate_result_get_as_ok(start_result);

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

					RTTIAnnotatedValue entry_handle = json_iterate_result_get_as_ok(entry_result);

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

					// note: the "recursive_result" RTTI value has to be NULL, as it isn't needed
					// anymore, the ptr that is used as the obj_handle should be modified by "sub
					// iterators", we don't allow returning anything new in subparsers, set the ptr
					// up to be modified in place, even if it's a double ptr!
					RTTIAnnotatedValue result_handle =
					    json_iterate_result_get_as_ok(recursive_result);

					if(result_handle.ptr != NULL) {
						FREE_AT_END();
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_STATIC_LIT("sub iterator returned non NULL rtti type") });
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

			RTTIAnnotatedValue end_handle = json_iterate_result_get_as_ok(end_result);

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
			JsonIterateValue iterate_value = new_json_iterate_value_number(number);

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_STRING_CONST(*json_value) {

			JsonIterateValue iterate_value = new_json_iterate_value_string(string);

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_BOOLEAN_CONST(*json_value) {
			JsonIterateValue iterate_value = new_json_iterate_value_boolean(boolean);

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_NULL() {
			JsonIterateValue iterate_value = new_json_iterate_value_null();

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
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
