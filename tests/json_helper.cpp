#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/compat.hpp"
#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"
#include "./helpers/json_helper.h"
#include "./helpers/json_helper.hpp"

#include <tjson_helper.h>
#include <tvec.h>

#include <functional>

#include "helpers/string_maker.hpp"

namespace {

void free_json_test_struct(TestJsonStruct test_struct) {

	if(test_struct.optional_array != NULL) {

		TVEC_FREE(TestJsonStructArrayElement, &(test_struct.optional_array->array));

		free(test_struct.optional_array);
	}

	tstr_free(&test_struct.name);
}

struct JsonIteratorProcessingTest {
	JsonValue input;
	TestJsonStruct expected;
};

struct JsonIteratorErrorTest {
	JsonValue input;
	JsonIterateErrorCpp error;
};

} // namespace

[[nodiscard]] static bool operator==(const TestJsonStructArrayElement& s1,
                                     const TestJsonStructArrayElement& s2) {
	return s1.value == s2.value;
}

[[nodiscard]] static bool operator==(const TestJsonStructOptionalArray& s1,
                                     const TestJsonStructOptionalArray& s2) {
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

	if(s1.optional_array == NULL || s2.optional_array == NULL) {
		if(s1.optional_array != s2.optional_array) {
			return false;
		}
	} else {
		if(*s1.optional_array != *s2.optional_array) {
			return false;
		}
	}

	return tstr_eq(&s1.name, &s2.name);
}

static std::ostream& operator<<(std::ostream& os, const TestJsonStructOptionalArray& test_struct) {

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

	if(test_struct.optional_array == NULL) {
		os << "<None>, ";
	} else {
		os << *test_struct.optional_array << ", ";
	}

	os << test_struct.name << " }";

	return os;
}

[[nodiscard]] static TestJsonStructOptionalArray*
get_optional_array(std::initializer_list<bool> values) {

	TestJsonStructOptionalArray* ptr =
	    (TestJsonStructOptionalArray*)malloc(sizeof(TestJsonStructOptionalArray));

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
		                                                    { "optional_array", json::null() },
		                                                    { "name", json::string("string1") } }),
		                            .expected =
		                                TestJsonStruct{
		                                    .number = 1,
		                                    .optional_array = nullptr,
		                                    .name = "string1"_tstr,
		                                } },
		JsonIteratorProcessingTest{
		    .input = json::object(
		        { { "number", json::number((int64_t)2) },
		          { "optional_array", json::array({ json::boolean(true), json::boolean(true),
		                                            json::boolean(false) }) },
		          { "name", json::string("string2") } }),
		    .expected =
		        TestJsonStruct{
		            .number = 2,
		            .optional_array = get_optional_array({ true, true, false }),
		            .name = "string2"_tstr,
		        } },
		JsonIteratorProcessingTest{ .input = json::object({}),
		                            .expected =
		                                TestJsonStruct{
		                                    .number = 0,
		                                    .optional_array = nullptr,
		                                    .name = tstr_null(),
		                                } },
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

	std::vector<JsonIteratorErrorTest> json_iterator_cases = {};

	{
		json_iterator_cases.emplace_back(
		    json::array({}),

		    JsonIterateErrorCpp{ "Error: unhandled iterate value in root path: array_start",
		                         json::path::path() });
		json_iterator_cases.emplace_back(
		    json::object({ { "number", json::null() } }),
		    JsonIterateErrorCpp{ "Error: invalid json value for type that expected: 'uint32_t'",
		                         json::path::path() / json::path::object("number") });

		json_iterator_cases.emplace_back(
		    json::object(
		        { { "number", json::number((int64_t)2) },
		          { "optional_array", json::array({ json::boolean(true), json::boolean(true),
		                                            json::boolean(false), json::null() }) },
		          { "name", json::string("string2") } }),
		    JsonIterateErrorCpp{
		        "Error: unhandled iterate value in TestJsonStructOptionalArray: null",
		        json::path::path() / json::path::object("optional_array") / json::path::array(3) });
	}

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

		JsonIterateError actual_error = json_iterate_result_get_as_error(actual_result).error;

		CAutoFreePtr<JsonIterateError> defer_error = { &actual_error, free_json_iterate_error };

		REQUIRE_EQ(actual_error, test_case.error);
	}
}

TEST_SUITE_END();
