#pragma once

#include <tjson_helper.h>
#include <tvec.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	bool value;
} TestJsonStructArrayElement;

TVEC_DEFINE_VEC_TYPE(TestJsonStructArrayElement)

typedef TVEC_TYPENAME(TestJsonStructArrayElement) TestJsonStructArray;

typedef struct {
	TestJsonStructArray array;
} TestJsonStructNested;

typedef struct {
	uint32_t number1;
	double number2;
	TestJsonStructNested* optional;
	tstr name;
} TestJsonStruct;

TRTTI_DECLARE_TYPE_AS_SUPPORTED(TestJsonStruct)

JsonIterateResult test_iterate_cb(const JsonPath* path, RTTIAnnotatedValue parent,
                                  JsonIterateValue value);

#ifdef __cplusplus
}
#endif
