
#include "./json_helper.h"

TVEC_IMPLEMENT_VEC_TYPE(TestJsonStructArrayElement)

TRTTI_IMPLEMENTATION_FOR_TYPE(TestJsonStruct)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(tstr)
TRTTI_DEFINE_TYPE_AS_SUPPORTED(uint32_t)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TestJsonStructNested)
TRTTI_DEFINE_TYPE_AS_SUPPORTED_EXTENDED(TestJsonStructNested*, TestJsonStructNestedOptional)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TestJsonStructArrayElement)

JsonIterateResult test_iterate_cb(const JsonPath* path, RTTIAnnotatedValue parent,
                                  JsonIterateValue value) {

	if(json_path_is_root(path)) {

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			if(parent.ptr != NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err =
				        TSTR_STATIC_LIT("implementation error, first RTTI value vas not NULL") });
			}

			TestJsonStruct* allocated_struct =
			    (TestJsonStruct*)TJSON_MALLOC(sizeof(TestJsonStruct));

			if(allocated_struct == NULL) {
				return new_json_iterate_result_error(
				    (JsonIterateError){ .err = TSTR_STATIC_LIT("OOM") });
			}

			*allocated_struct = (TestJsonStruct){
				.number = 0,
				.optional_arrray = NULL,
				.name = tstr_null(),
			};

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TestJsonStruct, allocated_struct);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_END(value) {

			if(!TRTTI_ANNOTATED_VALUE_IS(TestJsonStruct, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT("implementation error, root object end RTTI value vas "
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
				    .err = TSTR_STATIC_LIT("implementation error, root object key RTTI value vas "
				                           "not of type 'TestJsonStruct'") });
			}

			TestJsonStruct* object = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, parent);

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("number"))) {

				RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(uint32_t, &(object->number));

				return new_json_iterate_result_ok(result);
			}

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("optional_arrray"))) {

				RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TestJsonStructNestedOptional,
				                                                      &(object->optional_arrray));

				return new_json_iterate_result_ok(result);
			}

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("name"))) {

				RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(tstr, &(object->name));

				return new_json_iterate_result_ok(result);
			}

			return new_json_iterate_result_error((JsonIterateError){
			    .err = TSTR_STATIC_LIT("Error: unhandled object key in root object") });
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_STATIC_LIT("Error: unhandled iterate value in root path") });
	}

	//

	if(TRTTI_ANNOTATED_VALUE_IS(tstr, parent)) {

		IF_JSON_ITERATE_VALUE_IS_STRING_CONST(value) {

			tstr str_value = json_string_get_as_str(string);

			if(tstr_is_null(&str_value)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT("error in getting tstr from json string") });
			}

			tstr* dest = TRTTI_ANNOTATED_VALUE_CAST(tstr, parent);

			*dest = str_value;

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_STATIC_LIT("Error: invalid json value for type that expected: 'tstr'") });
	}

	if(TRTTI_ANNOTATED_VALUE_IS(uint32_t, parent)) {

		IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST(value) {

			uint32_t num_value = (uint32_t)number.value;

			uint32_t* dest = TRTTI_ANNOTATED_VALUE_CAST(uint32_t, parent);

			*dest = num_value;

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err =
		        TSTR_STATIC_LIT("Error: invalid json value for type that expected: 'uint32_t'") });
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructNestedOptional, parent)) {

		TestJsonStructNested** const dest =
		    TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructNestedOptional, parent);

		IF_JSON_ITERATE_VALUE_IS_NULL(value) {

			if(*dest != NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT("implementation error, another value for "
				                           "'optional<TestJsonStructNested>' value was given, "
				                           "duplicate object key!") });
			}

			*dest = NULL;

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_START(value) {

			TestJsonStructNested* allocated_array =
			    (TestJsonStructNested*)TJSON_MALLOC(sizeof(TestJsonStructNested));

			if(allocated_array == NULL) {
				return new_json_iterate_result_error(
				    (JsonIterateError){ .err = TSTR_STATIC_LIT("OOM") });
			}

			if(*dest != NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT("implementation error, another value for "
				                           "'optional<TestJsonStructNested>' value was given, "
				                           "duplicate object key!") });
			}

			*dest = allocated_array;

			*allocated_array =
			    (TestJsonStructNested){ .array = TVEC_EMPTY(TestJsonStructArrayElement) };

			RTTIAnnotatedValue result =
			    TRTTI_ANNOTATED_VALUE_GET(TestJsonStructNested, allocated_array);

			return new_json_iterate_result_ok(result);
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_STATIC_LIT("Error: invalid json value for type that expected: "
		                           "'optional<TestJsonStructNested>'") });
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructNested, parent)) {

		TestJsonStructNested* const dest = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructNested, parent);

		IF_JSON_ITERATE_VALUE_IS_ARRAY_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type has
			// all fields set to a valid value

			// NOTE: return empty, as this can be a subparser, where the return value has to be
			// null!
			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST(value) {

			IF_JSON_VALUE_IS_BOOLEAN_IGN(*(array_push.entry.value)) {

				if(dest == NULL) {
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_STATIC_LIT("Error: tried to push on empty optional array") });
				}

				TestJsonStructArrayElement* push_slot =
				    TVEC_PUSH_SLOT(TestJsonStructArrayElement, &(dest->array));

				if(push_slot == NULL) {
					return new_json_iterate_result_error((JsonIterateError){
					    .err = TSTR_STATIC_LIT(
					        "Error: error on allocating space for the array push") });
				}

				RTTIAnnotatedValue result =
				    TRTTI_ANNOTATED_VALUE_GET(TestJsonStructArrayElement, push_slot);

				return new_json_iterate_result_ok(result);
			}

			return new_json_iterate_result_error((JsonIterateError){
			    .err = TSTR_STATIC_LIT("Error: invalid json value for type that expected: "
			                           "'TestJsonStructArrayElement'") });
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_STATIC_LIT("Error: invalid json value for type that expected: "
		                           "'optional<TestJsonStructNested>'") });
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TestJsonStructArrayElement, parent)) {

		IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST(value) {

			TestJsonStructArrayElement wrapper_value = { .value = boolean.value };

			TestJsonStructArrayElement* dest =
			    TRTTI_ANNOTATED_VALUE_CAST(TestJsonStructArrayElement, parent);

			*dest = wrapper_value;

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err =
		        TSTR_STATIC_LIT("Error: invalid json value for type that expected: 'uint32_t'") });
	}

	return new_json_iterate_result_error((JsonIterateError){
	    .err = TSTR_STATIC_LIT("unhandled convertor for type below root path") });
}
