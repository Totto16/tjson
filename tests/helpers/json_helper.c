
#include "./json_helper.h"

TVEC_IMPLEMENT_VEC_TYPE(TestJsonStructArrayElement)

TRTTI_IMPLEMENTATION_FOR_TYPE(TestJsonStruct)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(tstr)

TRTTI_DEFINE_TYPE_AS_SUPPORTED_EXTENDED(TestJsonStructNested*, TestJsonStructNestedPtr)

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
				.number1 = 0,
				.optional = NULL,
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

			return new_json_iterate_result_ok(parent);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(value) {

			if(!TRTTI_ANNOTATED_VALUE_IS(TestJsonStruct, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT("implementation error, root object key RTTI value vas "
				                           "not of type 'TestJsonStruct'") });
			}

			TestJsonStruct* object = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, parent);

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("name"))) {

				RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(tstr, &(object->name));

				return new_json_iterate_result_ok(result);
			}

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("optional"))) {

				RTTIAnnotatedValue result =
				    TRTTI_ANNOTATED_VALUE_GET(TestJsonStructNestedPtr, &(object->optional));

				return new_json_iterate_result_ok(result);
			}

			return new_json_iterate_result_error((JsonIterateError){
			    .err = TSTR_STATIC_LIT("Error unhandled object key in root object") });
		}

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_STATIC_LIT("Error unhandled iterate value in root path") });
	}

	const JsonPath* root_parent_path = json_path_get_root();

	if(json_path_is_parent_of(path, root_parent_path)) {

		return new_json_iterate_result_error(
		    (JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: 3") });
	}

	return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: 2") });
}
