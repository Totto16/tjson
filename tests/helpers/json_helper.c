
#include "./json_helper.h"

TVEC_IMPLEMENT_VEC_TYPE(TestJsonStructArrayElement)

TRTTI_IMPLEMENTATION_FOR_TYPE(TestJsonStruct)

JsonIterateResult test_iterate_cb(const JsonPath* path, RTTIAnnotatedValue parent,
                                  JsonIterateValue value) {
	//
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
				.number2 = 0.0,
				.optional = NULL,
				.name = tstr_null(),
			};

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TestJsonStruct, allocated_struct);

			return new_json_iterate_result_ok(result);
		}

		return new_json_iterate_result_error(
		    (JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: 1") });
	}

	return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_STATIC_LIT("TODO: 2") });
}
