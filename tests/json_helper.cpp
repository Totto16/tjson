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

void free_json_test_struct(TestJsonStruct test_struct) {

	if(test_struct.optional_arrray != NULL) {

		TVEC_FREE(TestJsonStructArrayElement, &(test_struct.optional_arrray->array));

		free(test_struct.optional_arrray);
	}
}

struct JsonIteratorProcessingTest {
	JsonValue input;
	TestJsonStruct expected;
};

struct JsonIteratorErrorTest {
	JsonValue input;
	std::string error;
};

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

	if(s1.number != s2.number) {
		return false;
	}

	if(s1.optional_arrray == NULL || s2.optional_arrray == NULL) {
		if(s1.optional_arrray != s2.optional_arrray) {
			return false;
		}
	} else {
		if(*s1.optional_arrray != *s2.optional_arrray) {
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

	os << "{ " << test_struct.number << ", ";

	if(test_struct.optional_arrray == NULL) {
		os << "<None>, ";
	} else {
		os << *test_struct.optional_arrray << ", ";
	}

	os << test_struct.name << " }";

	return os;
}

[[nodiscard]] static TestJsonStructNested* get_optional_array(std::initializer_list<bool> values) {

	TestJsonStructNested* ptr = (TestJsonStructNested*)malloc(sizeof(TestJsonStructNested));

	if(ptr == nullptr) {
		throw std::runtime_error("OOM");
	}

	*ptr = { .array = TVEC_EMPTY(TestJsonStructArrayElement) };

	for(auto& value : values) {
		auto res = TVEC_PUSH(TestJsonStructArrayElement, &(ptr->array), { .value = value });

		if(res != TvecResultOk) {
			throw std::runtime_error("push error");
		}
	}

	return ptr;
}

namespace doctest {

template <> struct StringMaker<TestJsonStruct> {
	static String convert(const TestJsonStruct& result) {
		return ::os_stream_formattable_to_doctest(result);
	}
};

} // namespace doctest

TEST_SUITE_BEGIN("json_helper" * doctest::description("json helper tests") * doctest::timeout(2.0));

TEST_CASE("testing json iterator processing <json_iterator_processing>") {

	std::vector<JsonIteratorProcessingTest> json_iterator_cases = {
		JsonIteratorProcessingTest{ .input = json::object({ { "number", json::number((int64_t)1) },
		                                                    { "optional_arrray", json::null() },
		                                                    { "name", json::string("string1") } }),
		                            .expected =
		                                TestJsonStruct{
		                                    .number = 1,
		                                    .optional_arrray = nullptr,
		                                    .name = "string1"_tstr,
		                                } },
		JsonIteratorProcessingTest{
		    .input = json::object(
		        { { "number", json::number((int64_t)2) },
		          { "optional_arrray", json::array({ json::boolean(true), json::boolean(true),
		                                             json::boolean(false) }) },
		          { "name", json::string("string2") } }),
		    .expected =
		        TestJsonStruct{
		            .number = 2,
		            .optional_arrray = get_optional_array({ true, true, false }),
		            .name = "string2"_tstr,
		        } }
	};

	CAutoFreePtr<std::vector<JsonIteratorProcessingTest>> defer_tests = {
		&json_iterator_cases,
		[](std::vector<JsonIteratorProcessingTest>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));
			    free_json_value(&(value->input));
			    free_json_test_struct(value->expected);
		    }
		}
	};

	for(const auto& test_case : json_iterator_cases) {

		INFO("Test case: ", test_case.input);

		RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

		JsonIterateResult actual_result =
		    json_value_iterate(&test_case.input, test_iterate_cb, test_iterate_free, empty, empty);

		REQUIRE_IS_NOT_ERROR(actual_result);

		RTTIAnnotatedValue struct_value = json_iterate_result_get_as_ok(actual_result);

		RTTIAnnotatedValue expected_value = TRTTI_ANNOTATED_VALUE_GET(TestJsonStruct, NULL);

		REQUIRE_EQ(struct_value, expected_value);

		TestJsonStruct* result = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, struct_value);

		CAutoFreePtr<TestJsonStruct> defer_result = { result,
			                                          [](TestJsonStruct* const value) -> void {
			                                              if(value == NULL) {
				                                              return;
			                                              }

			                                              free_json_test_struct(*value);
			                                              free(value);
			                                          } };

		REQUIRE_EQ(*result, test_case.expected);
	}
}

TEST_CASE("testing json iterator errors <json_iterator_error>") {

	std::vector<JsonIteratorErrorTest> json_iterator_cases = {
		JsonIteratorErrorTest{ .input = json::array({}),
		                       .error = "Error: unhandled iterate value in root path" },
		JsonIteratorErrorTest{
		    .input = json::object(
		        { { "number", json::number((int64_t)2) },
		          { "optional_arrray", json::array({ json::boolean(true), json::boolean(true),
		                                             json::boolean(false), json::null() }) },
		          { "name", json::string("string2") } }),
		    .error = "Error: invalid json value for type that expected: "
		             "'TestJsonStructArrayElement'" }
	};

	CAutoFreePtr<std::vector<JsonIteratorErrorTest>> defer_tests = {
		&json_iterator_cases,
		[](std::vector<JsonIteratorErrorTest>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));
			    free_json_value(&(value->input));
		    }
		}
	};

	for(const auto& test_case : json_iterator_cases) {

		INFO("Test case: ", test_case.input);

		RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

		JsonIterateResult actual_result =
		    json_value_iterate(&test_case.input, test_iterate_cb, test_iterate_free, empty, empty);

		REQUIRE_IS_ERROR(actual_result);

		JsonIterateError error = json_iterate_result_get_as_error(actual_result).error;

		std::string actual_error = string_from_tstr_static(error.err);

		REQUIRE_EQ(actual_error, test_case.error);
	}
}

TEST_SUITE_END();
