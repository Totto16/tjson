#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json_schema.hpp"

#include <tjson_schema.h>

#include "helpers/string_maker.hpp"

namespace {

struct JsonSchemaValidateTestCaseSingle {
	JsonValue value;
	std::optional<std::string> result;
};

struct JsonSchemaValidateTestCase {
	JsonSchema schema;
	std::vector<JsonSchemaValidateTestCaseSingle> tests;
};

} // namespace

TEST_SUITE_BEGIN("json_schema" * doctest::description("json schema tests") * doctest::timeout(2.0));

TEST_CASE("testing validation of json schemas <json_schema_validate>") {

	std::vector<JsonSchemaValidateTestCase> json_schema_validate_tests = {

	};

	CAutoFreePtr<std::vector<JsonSchemaValidateTestCase>> defer_tests = {
		&json_schema_validate_tests,
		[](std::vector<JsonSchemaValidateTestCase>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));

			    for(size_t j = 0; j < value->tests.size(); ++j) {
				    auto* const test = &(value->tests.at(i));

				    free_json_value(&(test->value));
			    }

			    free_json_schema(&(value->schema));
		    }
		}
	};

	for(const auto& test_case : json_schema_validate_tests) {

		INFO("Test case: ", test_case.schema);

		for(const auto& subtest : test_case.tests) {

			auto validate_result = json_schema_validate_data(&test_case.schema, &subtest.value);

			CAutoFreePtr<tstr> defer_result = { &validate_result, [](tstr* const value) -> void {
				                                   if(!tstr_is_null(value)) {
					                                   tstr_free(value);
				                                   }
				                               } };

			if(!subtest.result.has_value()) {

				REQUIRE_EQ(TstrIsNull{}, validate_result);

				continue;
			}

			const auto expected_result = subtest.result.value();

			auto actual_result = string_from_tstr(validate_result);

			REQUIRE_EQ(actual_result, expected_result);
		}
	}
}

TEST_SUITE_END();
