#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/compat.hpp"
#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"
#include "./helpers/json_helper.h"

#include <tjson_helper.h>
#include <tvec.h>

#include <functional>

#include "helpers/string_maker.hpp"

namespace {

void free_json_test_struct(TestJsonStruct* test_struct) {

	if(test_struct->optional != NULL) {

		TVEC_FREE(TestJsonStructArrayElement, &(test_struct->optional->array));

		free(test_struct->optional);
	}
}

} // namespace

[[nodiscard]] static bool operator==(const TestJsonStructArrayElement& s1,
                                     const TestJsonStructArrayElement& s2) {
	return s1.value == s2.value;
}

[[nodiscard]] static bool operator==(const TestJsonStructNested& s1,
                                     const TestJsonStructNested& s2) {
	const size_t len1 = TVEC_LENGTH(TestJsonStructArrayElement, s1.array);
	const size_t len2 = TVEC_LENGTH(TestJsonStructArrayElement, s2.array);

	if(len1 != len2) {
		return false;
	}

	for(size_t i = 0; i < len1; ++i) {
		TestJsonStructArrayElement elem1 = TVEC_AT(TestJsonStructArrayElement, s1.array, i);
		TestJsonStructArrayElement elem2 = TVEC_AT(TestJsonStructArrayElement, s2.array, i);

		if(elem1 != elem2) {
			return false;
		}
	}

	return true;
}

[[nodiscard]] static bool operator==(const TestJsonStruct& s1, const TestJsonStruct& s2) {

	if(s1.number1 != s2.number1) {
		return false;
	}

	if(s1.optional == NULL || s2.optional == NULL) {
		if(s1.optional != s2.optional) {
			return false;
		}
	} else {
		if(*s1.optional != *s2.optional) {
			return false;
		}
	}

	return tstr_eq(&s1.name, &s2.name);
}

static std::ostream& operator<<(std::ostream& os, const TestJsonStructNested& test_struct) {

	const size_t len = TVEC_LENGTH(TestJsonStructArrayElement, test_struct.array);

	os << "[ ";

	for(size_t i = 0; i < len; ++i) {
		TestJsonStructArrayElement elem = TVEC_AT(TestJsonStructArrayElement, test_struct.array, i);

		os << elem.value;

		if((i + 1) != len) {
			os << ", ";
		}
	}

	os << " ]";

	return os;
}

static std::ostream& operator<<(std::ostream& os, const TestJsonStruct& test_struct) {

	os << "{ " << test_struct.number1 << ", ";

	if(test_struct.optional == NULL) {
		os << "<None>, ";
	} else {
		os << *test_struct.optional << ", ";
	}

	os << test_struct.name << " }";

	return os;
}

namespace doctest {

template <> struct StringMaker<TestJsonStruct> {
	static String convert(const TestJsonStruct& result) {
		return ::os_stream_formattable_to_doctest(result);
	}
};

} // namespace doctest

TEST_SUITE_BEGIN("json_helper" * doctest::description("json helper tests") * doctest::timeout(2.0));

TEST_CASE("testing simple json iterator example <json_iterator_simple>") {

	JsonValue value = json::object({ { "name", json::string("string") } });

	CAutoFreePtr<JsonValue> defer_value = { &value, free_json_value };

	RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

	JsonIterateResult actual_result = json_value_iterate(&value, test_iterate_cb, empty);

	REQUIRE_IS_NOT_ERROR(actual_result);

	RTTIAnnotatedValue struct_value = json_iterate_result_get_as_ok(actual_result);

	RTTIAnnotatedValue expected_value = TRTTI_ANNOTATED_VALUE_GET(TestJsonStruct, NULL);

	REQUIRE_EQ(struct_value, expected_value);

	TestJsonStruct* result = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, struct_value);

	TestJsonStruct expected_result = {

	};

	CAutoFreePtr<TestJsonStruct> defer_result = { &expected_result, free_json_test_struct };

	REQUIRE_EQ(*result, expected_result);
}

TEST_SUITE_END();
