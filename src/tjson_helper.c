#include "./tjson_helper.h"

#include <tstr_builder.h>
#include <tvec.h>

#include "./_impl/utf8.h"
#include "./_impl/utils.h"

typedef struct {
	Utf8CodepointArr value;
} JsonPathSegmentObjectKey;

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

static void free_json_path_segment_object_key(JsonPathSegmentObjectKey* object_key) {

	TVEC_FREE(Utf8Codepoint, &(object_key->value));
}

TJSON_NODISCARD static bool
json_path_segment_object_key_eq(const JsonPathSegmentObjectKey* const object_key1,
                                const JsonPathSegmentObjectKey* const object_key2) {
	return utf8_codpoints_eq(&(object_key1->value), &(object_key2->value));
}

TJSON_NODISCARD bool json_path_add_object_key_moved(JsonPath* path, TJSON_MOVED(tstr) key) {

	tstr key_normal = *TJSON_GET_MOVED_VALUE(key);
	*TJSON_GET_MOVED_VALUE(key) = tstr_null();

#define FREE_AT_END() \
	do { \
		tstr_free(&key_normal); \
	} while(false)

	tstr_view str_view = tstr_as_view(&key_normal);

	const Utf8DataResult result = get_utf8_string(str_view);

	IF_UTF8_DATA_RESULT_IS_ERROR_IGN(result) {
		FREE_AT_END();
		return false;
	}

	JsonPathSegmentObjectKey object_key = { .value = TVEC_EMPTY(Utf8Codepoint) };

	const Utf8Data data = utf8_data_result_get_as_ok(result);

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&key_normal); \
		free_utf8_data(data); \
		free_json_path_segment_object_key(&object_key); \
	} while(false)

	for(size_t i = 0; i < data.size; ++i) {
		const Utf8Codepoint codepoint = data.data[i];

		const TvecResult add_result = TVEC_PUSH(Utf8Codepoint, &(object_key.value), codepoint);

		if(add_result != TvecResultOk) {
			FREE_AT_END();
			return false;
		}
	}

	free_utf8_data(data);

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&key_normal); \
		free_json_path_segment_object_key(&object_key); \
	} while(false)

	TvecResult push_result =
	    TVEC_PUSH(JsonPathSegment, &(path->segments), new_json_path_segment_object_key(object_key));

	if(push_result != TvecResultOk) {
		FREE_AT_END();
		return false;
	}

	return true;
}

#undef FREE_AT_END

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
				return json_path_segment_object_key_eq(object1, object2);
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

TJSON_NODISCARD bool json_path_is_parent_of(const JsonPath* parent, const JsonPath* path) {

	const size_t parent_len = TVEC_LENGTH(JsonPathSegment, parent->segments);
	const size_t path_len = TVEC_LENGTH(JsonPathSegment, path->segments);

	if(path_len != (parent_len + 1)) {
		return false;
	}

	for(size_t i = 0; i < parent_len; ++i) {
		const JsonPathSegment parent_value = TVEC_AT(JsonPathSegment, parent->segments, i);
		const JsonPathSegment path_value = TVEC_AT(JsonPathSegment, path->segments, i);

		if(!path_segment_eq(&parent_value, &path_value)) {
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

static tstr
get_tstr_from_json_path_segment_object_key_escaped(const JsonPathSegmentObjectKey* const object_key,
                                                   const JsonSerializeEscapeOption escape) {

	return get_normalized_string_from_codepoints_json_escaped(object_key->value, escape);
}

static bool json_path_segment_object_key_is_name_first(const Utf8Codepoint codepoint) {

	// see: https://www.rfc-editor.org/info/rfc9535/#section-2.5.1.1

	// name-first          = ALPHA /
	//                       "_"   /
	//                       %x80-D7FF /
	//                          ; skip surrogate code points
	//                       %xE000-10FFFF

	// ALPHA               = %x41-5A / %x61-7A    ; A-Z / a-z

	if(codepoint >= 'A' && codepoint <= 'Z') {
		return true;
	}

	if(codepoint >= 'a' && codepoint <= 'z') {
		return true;
	}

	if(codepoint == '_') {
		return true;
	}

	if(codepoint >= 0x80 && codepoint <= 0xD7FF) {
		return true;
	}

	if(codepoint >= 0xE000 && codepoint <= 0x10FFFF) {
		return true;
	}

	return false;
}

static bool json_path_segment_object_key_is_name_char(const Utf8Codepoint codepoint) {

	// see: https://www.rfc-editor.org/info/rfc9535/#section-2.5.1.1

	// name-char           = name-first / DIGIT

	// DIGIT               = %x30-39              ; 0-9

	if(json_path_segment_object_key_is_name_first(codepoint)) {
		return true;
	}

	if(codepoint >= '0' && codepoint <= '9') {
		return true;
	}

	return false;
}

static bool
json_path_segment_object_key_can_be_compact(const JsonPathSegmentObjectKey* object_key) {
	// see: https://www.rfc-editor.org/info/rfc9535/#section-2.5.1.1

	// child-segment       = bracketed-selection /
	//                       ("."
	//                        (wildcard-selector /
	//                         member-name-shorthand))

	// bracketed-selection = "[" S selector *(S "," S selector) S "]"

	// member-name-shorthand = name-first *name-char

	const size_t length = TVEC_LENGTH(Utf8Codepoint, object_key->value);

	if(length == 0) {
		return false;
	}

	for(size_t i = 0; i < length; ++i) {

		const Utf8Codepoint codepoint = TVEC_AT(Utf8Codepoint, object_key->value, i);

		if(i == 0) {
			if(!json_path_segment_object_key_is_name_first(codepoint)) {
				return false;
			}
		} else {
			if(!json_path_segment_object_key_is_name_char(codepoint)) {
				return false;
			}
		}
	}

	return true;
}

static void json_path_segment_to_string_impl(StringBuilder* const string_builder,
                                             const JsonPathSegment* segment,
                                             const JsonPathStringOptions options) {

	SWITCH_JSON_PATH_SEGMENT(*segment) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(segment) {

			const bool can_use_compact =
			    options.compact ? json_path_segment_object_key_can_be_compact(object_key) : false;

			// see: https://www.rfc-editor.org/info/rfc9535/#section-2.3.1.1
			// we use the normal json escape function, as it does what we expect

			// Double-quoted strings follow the JSON string syntax (Section 7 of [RFC8259]); [...]

			if(can_use_compact) {
				tstr name = get_tstr_from_json_path_segment_object_key_escaped(
				    object_key, JsonSerializeEscapeOptionMost);

				if(tstr_is_null(&name)) {
					OOM_ASSERT(false, "error in formatting path segment");
				}

				STRING_BUILDER_APPENDF( // GCOVR_EXCL_BR_WITHOUT_HIT: 6/12
				    string_builder, OOM_ASSERT(false, "error in formatting path segment");
				    , TSTR_FMT, TSTR_FMT_ARGS(name))

				tstr_free(&name);

			} else {
				// NOTE we can use '' or "", but use ", as the escaping expects that

				tstr name = get_tstr_from_json_path_segment_object_key_escaped(
				    object_key, JsonSerializeEscapeOptionMost);

				if(tstr_is_null(&name)) {
					OOM_ASSERT(false, "error in formatting path segment");
				}

				STRING_BUILDER_APPENDF( // GCOVR_EXCL_BR_WITHOUT_HIT: 6/12
				    string_builder, OOM_ASSERT(false, "error in formatting path segment");
				    , "[\"" TSTR_FMT "\"]", TSTR_FMT_ARGS(name))

				tstr_free(&name);
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
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(segment) {
			return json_path_segment_object_key_can_be_compact(object_key);
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

		if(options.compact) {
			if(json_path_segment_need_separator(&value)) {
				string_builder_append_tstr(string_builder, &separator);
			}
		}

		json_path_segment_to_string_impl(string_builder, &value, options);
	}

	return string_builder_release_into_tstr(&string_builder);
}

TJSON_NODISCARD tstr json_path_to_str(const JsonPath* path) {
	return json_path_to_str_advanced(path, (JsonPathStringOptions){ .compact = true });
}

static void free_json_path_segment(JsonPathSegment segment) {
	SWITCH_JSON_PATH_SEGMENT(segment) {
		CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF(&segment) {
			free_json_path_segment_object_key(object_key);
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
