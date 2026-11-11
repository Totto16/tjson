#include "./tjson_helper.h"

#include <tstr_builder.h>
#include <tvec.h>

#include "./_impl/utils.h"

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

TJSON_NODISCARD JsonPath* json_path_get_root(void) {
	JsonPath* path = TJSON_NEW(JsonPath);

	if(path == NULL) {
		return NULL;
	}

	*path = (JsonPath){ .segments = TVEC_EMPTY(JsonPathSegment) };

	return path;
}

TJSON_NODISCARD bool json_path_add_object_key_dup(JsonPath* const path, const tstr* key) {

	tstr key_duped = tstr_dup(key);

	if(tstr_is_null(&key_duped)) {
		return false;
	}

	return json_path_add_object_key_moved(path, TJSON_MOVE(tstr, key_duped));
}

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, TJSON_MOVED(tstr) key) {

	const tstr key_normal = *TJSON_GET_MOVED_VALUE(key);

	TvecResult result =
	    TVEC_PUSH(JsonPathSegment, &(path->segments), new_json_path_segment_object_key(key_normal));

	if(result != TvecResultOk) {

		*TJSON_GET_MOVED_VALUE(key) = tstr_null();
		tstr_free(TJSON_GET_MOVED_VALUE(key));

		return false;
	}

	*TJSON_GET_MOVED_VALUE(key) = tstr_null();

	return true;
}

TJSON_NODISCARD bool json_path_add_array_index(JsonPath* path, size_t index) {
	TvecResult result =
	    TVEC_PUSH(JsonPathSegment, &(path->segments), new_json_path_segment_array_index(index));

	return (result == TvecResultOk);
}

static void free_json_path_segment(JsonPathSegment segment);

TJSON_NODISCARD bool json_path_remove_last_segment(JsonPath* path) {
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

TJSON_NODISCARD static bool path_segment_eq(const JsonPathSegment* segment1,
                                            const JsonPathSegment* segment2) {

	SWITCH_JSON_PATH_SEGMENT(*segment1) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(segment1, object1) {

			IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(segment2, object2) {
				return tstr_eq(&(object1->name), &(object2->name));
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF(segment1, array1) {

			IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF(segment2, array2) {
				return array1->index == array2->index;
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
		const JsonPathSegment path_value = TVEC_AT(JsonPathSegment, path->segments, i);
		const JsonPathSegment parent_value = TVEC_AT(JsonPathSegment, parent->segments, i);

		if(!path_segment_eq(&path_value, &parent_value)) {
			return false;
		}
	}

	return true;
}

TJSON_NODISCARD bool json_path_is_eq(const JsonPath* path1, const JsonPath* path2) {

	if(path1 == NULL || path2 == NULL) {
		return path1 == path2;
	}

	const size_t len1 = TVEC_LENGTH(JsonPathSegment, path1->segments);

	const size_t len2 = TVEC_LENGTH(JsonPathSegment, path2->segments);

	if(len1 != len2) {
		return false;
	}

	for(size_t i = 0; i < len1; ++i) {
		const JsonPathSegment value1 = TVEC_AT(JsonPathSegment, path1->segments, i);
		const JsonPathSegment value2 = TVEC_AT(JsonPathSegment, path2->segments, i);

		if(!path_segment_eq(&value1, &value2)) {
			return false;
		}
	}

	return true;
}

static void json_path_segment_to_string_impl(StringBuilder* const string_builder,
                                             const JsonPathSegment* segment,
                                             const JsonPathStringOptions options) {

	SWITCH_JSON_PATH_SEGMENT(*segment) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(segment) {

			// TODO: figure out if we can use the compact version (e.g. no spaces etc)

			// TODO: escape properly!

			if(options.compact) {
				STRING_BUILDER_APPENDF( // GCOVR_EXCL_BR_WITHOUT_HIT: 6/12
				    string_builder, OOM_ASSERT(false, "error in formatting path segment");
				    , TSTR_FMT, TSTR_FMT_ARGS(object_key->name))
			} else {
				// NOTE we can use '' or ""

				STRING_BUILDER_APPENDF( // GCOVR_EXCL_BR_WITHOUT_HIT: 6/12
				    string_builder, OOM_ASSERT(false, "error in formatting path segment");
				    , "['" TSTR_FMT "']", TSTR_FMT_ARGS(object_key->name))
			}

			return;
		}
		VARIANT_CASE_END();
		CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF(segment) {

			STRING_BUILDER_APPENDF( // GCOVR_EXCL_BR_WITHOUT_HIT: 6/12
			    string_builder, OOM_ASSERT(false, "error in formatting path segment");
			    , "[%zu]", array_index->index)

			return;
		}
		VARIANT_CASE_END();
		default: {
			UNREACHABLE();
		}
	}
}

TJSON_NODISCARD static bool json_path_segment_need_separator(const JsonPathSegment* segment) {

	SWITCH_JSON_PATH_SEGMENT(*segment) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_IGN() {
			return true;
		}
		VARIANT_CASE_END();
		CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_IGN() {

			return false;
		}
		VARIANT_CASE_END();
		default: {
			UNREACHABLE();
		}
	}
}

TJSON_NODISCARD tstr json_path_to_str_advanced(const JsonPath* path,
                                               const JsonPathStringOptions options) {

	StringBuilder* string_builder = string_builder_init();

	if(string_builder == NULL) {
		return tstr_null();
	}

	// See:
	// https://en.wikipedia.org/wiki/JSONPath
	// https://www.rfc-editor.org/rfc/rfc9535

	const tstr root = TSTR_LIT("$");

	const tstr separator = TSTR_LIT(".");

	string_builder_append_tstr(string_builder, &root);

	const size_t length = TVEC_LENGTH(JsonPathSegment, path->segments);

	for(size_t i = 0; i < length; ++i) {
		JsonPathSegment value = TVEC_AT(JsonPathSegment, path->segments, i);
		json_path_segment_to_string_impl(string_builder, &value, options);

		if(i + 1 < length) {
			if(!options.compact) {
				JsonPathSegment next = TVEC_AT(JsonPathSegment, path->segments, i + 1);
				if(json_path_segment_need_separator(&next)) {
					string_builder_append_tstr(string_builder, &separator);
				}
			}
		}
	}

	return string_builder_release_into_tstr(&string_builder);
}

TJSON_NODISCARD tstr json_path_to_str(const JsonPath* path) {
	return json_path_to_str_advanced(path, (JsonPathStringOptions){ .compact = true });
}

static void free_json_path_segment(JsonPathSegment segment) {
	SWITCH_JSON_PATH_SEGMENT(segment) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF(&segment) {
			tstr_free(&(object_key->name));
		}
		break;
		VARIANT_CASE_END();
		CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_IGN() {
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

	TJSON_FREE(path);
}

//

TJSON_NODISCARD static JsonIterateResult json_value_iterate_impl( // NOLINT(misc-no-recursion)
    const JsonValue* json_value, JsonValueIterateCallback iterate_callback,
    JsonIterateFreeCallback free_callback, RTTIAnnotatedValue annotated_value_start,
    RTTIAnnotatedValue userdata, JsonPath* json_path) {

	SWITCH_JSON_VALUE(*json_value) {
		CASE_JSON_VALUE_IS_OBJECT_CONST(*json_value) {

			JsonIterateValue obj_start = new_json_iterate_value_object_start();

#define FREE_AT_END() \
	do { \
		free_callback(annotated_value_start); \
	} while(false)

			JsonIterateResult start_result =
			    iterate_callback(json_path, annotated_value_start, obj_start, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(start_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			const RTTIAnnotatedValue obj_handle = json_iterate_result_get_as_ok(start_result);

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(obj_handle); \
		free_callback(annotated_value_start); \
	} while(false)

			{
				JsonObjectIter* iter = json_object_get_iterator(object.obj);

				if(iter == NULL) {
					FREE_AT_END();
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_LIT("json object iterator couldn't be allocated") });
				}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(obj_handle); \
		free_callback(annotated_value_start); \
		json_object_free_iterator(iter); \
	} while(false)

				while(true) {

					const JsonObjectEntry* const next_entry = json_object_iterator_next(iter);

					if(next_entry == NULL) {
						break;
					}

					const JsonString* const key = json_object_entry_get_key(next_entry);

					if(key == NULL) {
						FREE_AT_END();
						return new_json_iterate_result_error(
						    (JsonIterateError){ .err = TSTR_LIT("json key string convert error") });
					}

					tstr key_tstr = json_string_get_as_str(key);

					if(tstr_is_null(&key_tstr)) {
						FREE_AT_END();
						return new_json_iterate_result_error(
						    (JsonIterateError){ .err = TSTR_LIT("json key string convert error") });
					}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&key_tstr); \
		free_callback(obj_handle); \
		free_callback(annotated_value_start); \
		json_object_free_iterator(iter); \
	} while(false)

					const JsonValue value = json_object_entry_get_value(next_entry);

					bool add_result = json_path_add_object_key_dup(json_path, &key_tstr);

					if(!add_result) {
						FREE_AT_END();
						return new_json_iterate_result_error(
						    (JsonIterateError){ .err = TSTR_LIT("json path add failed") });
					}

					JsonIterateResult recursive_result = json_value_iterate_impl(
					    &value, iterate_callback, free_callback, obj_handle, userdata, json_path);

					IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(recursive_result) {

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&key_tstr); \
		free_callback(annotated_value_start); \
		json_object_free_iterator(iter); \
	} while(false)

						FREE_AT_END();
						return new_json_iterate_result_error(error.error);
					}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&key_tstr); \
		free_callback(obj_handle); \
		free_callback(recursive_handle); \
		free_callback(annotated_value_start); \
		json_object_free_iterator(iter); \
	} while(false)

					const RTTIAnnotatedValue recursive_handle =
					    json_iterate_result_get_as_ok(recursive_result);

					JsonIterateValue obj_entry = new_json_iterate_value_object_entry(
					    (JsonIterateObjectEntry){ .key = &key_tstr, .value = recursive_handle });

					JsonIterateResult entry_result =
					    iterate_callback(json_path, obj_handle, obj_entry, userdata);

					tstr_free(&key_tstr);

					IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(entry_result) {

						FREE_AT_END();
						return new_json_iterate_result_error(error.error);
					}

					RTTIAnnotatedValue entry_handle = json_iterate_result_get_as_ok(entry_result);

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&key_tstr); \
		free_callback(obj_handle); \
		free_callback(annotated_value_start); \
		json_object_free_iterator(iter); \
	} while(false)

					// note: the "entry_handle" RTTI value has to be NULL, as it isn't needed
					// anymore, the ptr that is used as the obj_handle should be modified by "sub
					// iterators", we don't allow returning anything new in subparsers, set the ptr
					// up to be modified in place, even if it's a double ptr!
					if(entry_handle.ptr != NULL) {
						FREE_AT_END();
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT("sub iterator returned non NULL rtti type") });
					}

					bool remove_result = json_path_remove_last_segment(json_path);

					if(!remove_result) {
						FREE_AT_END();
						return new_json_iterate_result_error(
						    (JsonIterateError){ .err = TSTR_LIT("json path remove failed") });
					}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(obj_handle); \
		free_callback(annotated_value_start); \
	} while(false)

					tstr_free(&key_tstr);
				}

				json_object_free_iterator(iter);
			}

			JsonIterateValue obj_end = new_json_iterate_value_object_end();

			JsonIterateResult end_result =
			    iterate_callback(json_path, obj_handle, obj_end, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(end_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			const RTTIAnnotatedValue end_handle = json_iterate_result_get_as_ok(end_result);

			return new_json_iterate_result_ok(end_handle);
		}
		VARIANT_CASE_END();

#undef FREE_AT_END

		CASE_JSON_VALUE_IS_ARRAY_CONST(*json_value) {

			JsonIterateValue arr_start = new_json_iterate_value_array_start();

#define FREE_AT_END() \
	do { \
		free_callback(annotated_value_start); \
	} while(false)

			JsonIterateResult start_result =
			    iterate_callback(json_path, annotated_value_start, arr_start, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(start_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			const RTTIAnnotatedValue arr_handle = json_iterate_result_get_as_ok(start_result);

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(arr_handle); \
		free_callback(annotated_value_start); \
	} while(false)

			for(size_t i = 0; i < json_array_get_size(array.arr); ++i) {

				const JsonValue* const value = json_array_get_at(array.arr, i);

				if(value == NULL) {
					FREE_AT_END();
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_LIT("implementation error: empty json value") });
				}

				bool add_result = json_path_add_array_index(json_path, i);

				if(!add_result) {
					FREE_AT_END();
					return new_json_iterate_result_error(
					    (JsonIterateError){ .err = TSTR_LIT("json path add failed") });
				}

				JsonIterateResult recursive_result = json_value_iterate_impl(
				    value, iterate_callback, free_callback, arr_handle, userdata, json_path);

				IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(recursive_result) {

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(annotated_value_start); \
	} while(false)

					FREE_AT_END();
					return new_json_iterate_result_error(error.error);
				}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(arr_handle); \
		free_callback(recursive_handle); \
		free_callback(annotated_value_start); \
	} while(false)

				const RTTIAnnotatedValue recursive_handle =
				    json_iterate_result_get_as_ok(recursive_result);

				JsonIterateValue arr_entry = new_json_iterate_value_array_push(
				    (JsonIterateArrayEntry){ .value = recursive_handle });

				JsonIterateResult entry_result =
				    iterate_callback(json_path, arr_handle, arr_entry, userdata);

				IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(entry_result) {
					FREE_AT_END();
					return new_json_iterate_result_error(error.error);
				}

				RTTIAnnotatedValue entry_handle = json_iterate_result_get_as_ok(entry_result);

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_callback(arr_handle); \
		free_callback(annotated_value_start); \
	} while(false)

				// note: the "entry_handle" RTTI value has to be NULL, as it isn't needed
				// anymore, the ptr that is used as the obj_handle should be modified by "sub
				// iterators", we don't allow returning anything new in subparsers, set the ptr
				// up to be modified in place, even if it's a double ptr!
				if(entry_handle.ptr != NULL) {
					FREE_AT_END();
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_LIT("sub iterator returned non NULL rtti type") });
				}

				bool remove_result = json_path_remove_last_segment(json_path);

				if(!remove_result) {
					FREE_AT_END();
					return new_json_iterate_result_error(
					    (JsonIterateError){ .err = TSTR_LIT("json path remove failed") });
				}
			}

			JsonIterateValue arr_end = new_json_iterate_value_array_end();

			JsonIterateResult end_result =
			    iterate_callback(json_path, arr_handle, arr_end, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(end_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			const RTTIAnnotatedValue end_handle = json_iterate_result_get_as_ok(end_result);

			return new_json_iterate_result_ok(end_handle);
		}
		VARIANT_CASE_END();

#undef FREE_AT_END

		CASE_JSON_VALUE_IS_NUMBER_CONST(*json_value) {
			JsonIterateValue iterate_value = new_json_iterate_value_number(number);

#define FREE_AT_END() \
	do { \
		free_callback(annotated_value_start); \
	} while(false)

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_STRING_CONST(*json_value) {

			JsonIterateValue iterate_value = new_json_iterate_value_string(string);

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_BOOLEAN_CONST(*json_value) {
			JsonIterateValue iterate_value = new_json_iterate_value_boolean(boolean);

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_NULL() {
			JsonIterateValue iterate_value = new_json_iterate_value_null();

			JsonIterateResult iterate_result =
			    iterate_callback(json_path, annotated_value_start, iterate_value, userdata);

			IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_result) {
				FREE_AT_END();
				return new_json_iterate_result_error(error.error);
			}

			RTTIAnnotatedValue iterate_handle = json_iterate_result_get_as_ok(iterate_result);

			return new_json_iterate_result_ok(iterate_handle);
		}
		VARIANT_CASE_END();
		default: {
			return new_json_iterate_result_error(
			    (JsonIterateError){ .err = TSTR_LIT("json value has invalid state") });
		}
	}
}

#undef FREE_AT_END

TJSON_NODISCARD JsonIterateResult json_value_iterate(const JsonValue* const json_value,
                                                     JsonValueIterateCallback iterate_callback,
                                                     JsonIterateFreeCallback free_callback,
                                                     RTTIAnnotatedValue start,
                                                     RTTIAnnotatedValue userdata) {

	JsonPath* json_path = json_path_get_root();

	if(json_path == NULL) {
		return new_json_iterate_result_error(
		    (JsonIterateError){ .err = TSTR_LIT("json path allocation error") });
	}

	JsonIterateResult result = json_value_iterate_impl(json_value, iterate_callback, free_callback,
	                                                   start, userdata, json_path);

	free_json_path(json_path);

	return result;
}
