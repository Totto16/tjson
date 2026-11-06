
#include "./json_helper.h"

TVEC_IMPLEMENT_VEC_TYPE(TestJsonStructArrayElement)

TRTTI_IMPLEMENTATION_FOR_TYPE(TestJsonStruct)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(tstr)
TRTTI_DEFINE_TYPE_AS_SUPPORTED(uint32_t)
TRTTI_DEFINE_TYPE_AS_SUPPORTED(bool)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TestJsonStructNested)

typedef struct {
	bool _unused;
} TestJsonNull;

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TestJsonNull)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TestJsonStructArrayElement)

JsonIterateResult test_iterate_cb(const JsonPath* path, RTTIAnnotatedValue parent,
                                  JsonIterateValue value, RTTIAnnotatedValue userdata) {

	(void)userdata;

	if(json_path_is_root(path)) {

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			if(parent.ptr != NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, first RTTI value vas not NULL") });
			}

			TestJsonStruct* allocated_struct =
			    (TestJsonStruct*)TJSON_MALLOC(sizeof(TestJsonStruct));

			if(allocated_struct == NULL) {
				return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_LIT("OOM") });
			}

			*allocated_struct = (TestJsonStruct){
				.number = 0,
				.optional_array = NULL,
				.name = tstr_null(),
			};

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TestJsonStruct, allocated_struct);

			return new_json_iterate_result_ok(result);
		}

		return new_json_iterate_result_error(
		    (JsonIterateError){ .err = TSTR_LIT("Error: unhandled iterate value in root path") });
	}

	//

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStruct, parent)) {

		IF_JSON_ITERATE_VALUE_IS_STRING_CONST(value) {

			tstr str_value = json_string_get_as_str(string);

			if(tstr_is_null(&str_value)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("error in getting tstr from json string") });
			}

			tstr* dest = TRTTI_ANNOTATED_VALUE_CAST(tstr, parent);

			*dest = str_value;

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST(value) {

			uint32_t num_value = (uint32_t)number.value;

			uint32_t* dest = TRTTI_ANNOTATED_VALUE_CAST(uint32_t, parent);

			*dest = num_value;

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		IF_JSON_ITERATE_VALUE_IS_NULL(value) {

			TestJsonNull* null = (TestJsonNull*)TJSON_MALLOC(sizeof(TestJsonNull));

			if(null == NULL) {
				return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_LIT("OOM") });
			}

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TestJsonNull, null);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type
			// has all fields set to a valid value

			// NOTE: return empty, as this can be a subparser, where the return value has to be
			// null!
			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_START(value) {

			TestJsonStructNested* allocated_array =
			    (TestJsonStructNested*)TJSON_MALLOC(sizeof(TestJsonStructNested));

			if(allocated_array == NULL) {
				return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_LIT("OOM") });
			}

			*allocated_array =
			    (TestJsonStructNested){ .array = TVEC_EMPTY(TestJsonStructArrayElement) };

			RTTIAnnotatedValue result =
			    TRTTI_ANNOTATED_VALUE_GET(TestJsonStructNested, allocated_array);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_END(value) {

			if(!TRTTI_ANNOTATED_VALUE_IS(TestJsonStruct, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, root object end RTTI value vas "
				                    "not of type 'TestJsonStruct'") });
			}

			// NOTE: here we could check some properties of the final result, alias if the type has
			// all fields set to a valid value

			// NOTE: return NON empty, as this is the toplevel parser, so the return value of this
			// is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(value) {

			if(!TRTTI_ANNOTATED_VALUE_IS(TestJsonStruct, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, root object key RTTI value vas "
				                    "not of type 'TestJsonStruct'") });
			}

			TestJsonStruct* object = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, parent);

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("number"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(uint32_t, object_entry.entry.value)) {

					uint32_t* number =
					    TRTTI_ANNOTATED_VALUE_CAST(uint32_t, object_entry.entry.value);

					if(number == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content") });
					}

					object->number = *number;
					TJSON_FREE(number);

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				return new_json_iterate_result_error((JsonIterateError){
				    .err =
				        TSTR_LIT("Error: invalid json value for type that expected: 'uint32_t'") });
			}

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("optional_array"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(TestJsonNull, object_entry.entry.value)) {

					TestJsonNull* null_wrapper =
					    TRTTI_ANNOTATED_VALUE_CAST(TestJsonNull, object_entry.entry.value);

					if(null_wrapper == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content") });
					}

					object->optional_array = NULL;
					TJSON_FREE(null_wrapper);

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructNested, object_entry.entry.value)) {

					TestJsonStructNested* nested_struct =
					    TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructNested, object_entry.entry.value);

					if(nested_struct == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content") });
					}

					// we don't free nested_struct, as we moved it
					object->optional_array = nested_struct;

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				return new_json_iterate_result_error((JsonIterateError){
				    .err =
				        TSTR_LIT("Error: invalid json value for type that expected: 'uint32_t'") });
			}

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("name"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(tstr, object_entry.entry.value)) {

					tstr* string = TRTTI_ANNOTATED_VALUE_CAST(tstr, object_entry.entry.value);

					if(string == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content") });
					}

					object->name = *string;
					TJSON_FREE(string);

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT(
				        "Error: invalid json value for type that expected: 'tstr'") });
			}

			RETURN_JSON_ITERATE_ERROR_DYNAMIC(
			    "Error: unhandled object key in root object: <no more info>",
			    "Error: unhandled object key in root object: " TSTR_FMT,
			    TSTR_FMT_ARGS(*object_entry.entry.key));
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_LIT("Error: unhandled iterate value in TestJsonStruct") });
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructNested, parent)) {

		IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST(value) {

			TestJsonStructNested* const dest =
			    TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructNested, parent);

			if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructArrayElement, array_push.entry.value)) {

				if(dest == NULL) {
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_LIT("Error: tried to push on empty optional array") });
				}

				const TestJsonStructArrayElement* element =
				    TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructArrayElement, array_push.entry.value);

				TvecResult push_result =
				    TVEC_PUSH(TestJsonStructArrayElement, &(dest->array), *element);

				if(push_result != TvecResultOk) {
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_LIT(
					        "Error: error on allocating space for the array push") });
				}

				RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

				return new_json_iterate_result_ok(empty);
			}

			return new_json_iterate_result_error((JsonIterateError){
			    .err = TSTR_LIT("Error: invalid value for type that expected: "
			                           "'TestJsonStructArrayElement'") });
		}

		IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST(value) {

			bool* bool_wrapper = (bool*)TJSON_MALLOC(sizeof(bool));

			if(bool_wrapper == NULL) {
				return new_json_iterate_result_error(
				    (JsonIterateError){ .err = TSTR_LIT("OOM") });
			}

			*bool_wrapper = boolean.value;

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(bool, bool_wrapper);

			return new_json_iterate_result_ok(result);
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_LIT("Error: invalid json value for type that expected: "
		                           "'TestJsonStructNested'") });
	}

	return new_json_iterate_result_error(
	    (JsonIterateError){ .err = TSTR_LIT("unhandled convertor for type below root path") });
}

void test_iterate_free(RTTIAnnotatedValue data) {

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStruct, data)) {

		TestJsonStruct* dest = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, data);

		tstr_free(&(dest->name));

		if(dest->optional_array != NULL) {

			TVEC_FREE(TestJsonStructArrayElement, &(dest->optional_array->array));

			free(dest->optional_array);
		}

		free(dest);

		return;
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructNested, data)) {

		TestJsonStructNested* dest = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructNested, data);

		TVEC_FREE(TestJsonStructArrayElement, &(dest->array));

		*dest = (TestJsonStructNested){ .array = TVEC_EMPTY(TestJsonStructArrayElement) };

		return;
	}

	fprintf(stderr, "ERROR: don't know how to free: " TRTTI_TYPE_NAME_FMT "\n",
	        TRTTI_TYPE_NAME_FMT_ARGS(data.type.name));
	exit(EXIT_FAILURE);
}
