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
	JsonSchemaCpp schema;
	std::vector<JsonSchemaValidateTestCaseSingle> tests;
};

} // namespace

TEST_SUITE_BEGIN("json_schema" * doctest::description("json schema tests") * doctest::timeout(2.0));

TEST_CASE("testing validation of json schemas <json_schema_validate>") {

	std::vector<JsonSchemaValidateTestCase> json_schema_validate_tests = {
		JsonSchemaValidateTestCase{
		    .schema = json_schema::null(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::null(),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::boolean(false),
		                                              .result = "JsonValue is not null" },
		        },

		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::boolean(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::boolean(false),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::null(),
		                                              .result = "JsonValue is not a boolean" },
		        },

		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::number(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::number(1.0),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::null(),
		                                              .result = "JsonValue is not a number" },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::string("hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::null(),
		                                              .result = "JsonValue is not a string" },
		        },

		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().nonempty().get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = JsonValueCpp::string("hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = JsonValueCpp::string(""),
		                .result = "string size (0) is smaller than the min size (1)" },
		        },

		}
	};

	CAutoFreePtr<std::vector<JsonSchemaValidateTestCase>> defer_tests = {
		&json_schema_validate_tests,
		[](std::vector<JsonSchemaValidateTestCase>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));

			    for(size_t j = 0; j < value->tests.size(); ++j) {
				    auto* const test = &(value->tests.at(j));

				    free_json_value(&(test->value));
			    }
		    }
		}
	};

	for(const auto& test_case : json_schema_validate_tests) {

		INFO("Test case: ", test_case.schema);

		for(const auto& subtest : test_case.tests) {

			auto validate_result =
			    json_schema_validate_data(test_case.schema.ptr(), &subtest.value);

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
