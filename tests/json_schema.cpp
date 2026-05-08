#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json_schema.hpp"

#include <tjson_schema.h>

#include <functional>

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

struct JsonSchemaErrorThrows {
	std::function<JsonSchemaCpp(void)> schema_fn;
	std::string throw_msg;
};

} // namespace

TEST_SUITE_BEGIN("json_schema" * doctest::description("json schema tests") * doctest::timeout(2.0));

TEST_CASE("testing validation of json schemas <json_schema_validate>") {

	std::vector<JsonSchemaValidateTestCase> json_schema_validate_tests = {
		JsonSchemaValidateTestCase{
		    .schema = json_schema::null(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::null(),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::boolean(false),
		                                              .result = "JsonValue is not null" },
		        },

		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::boolean(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::boolean(false),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::null(),
		                                              .result = "JsonValue is not a boolean" },
		        },

		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::number(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::number(1.0),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::null(),
		                                              .result = "JsonValue is not a number" },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::null(),
		                                              .result = "JsonValue is not a string" },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().nonempty().get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string(""),
		                .result = "string size (0) is smaller than the min size (1)" },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().min(2).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string("1"),
		                .result = "string size (1) is smaller than the min size (2)" },
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("12"),
		                                              .result = std::nullopt },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().max(2).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("1"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string("123"),
		                .result = "string size (3) is larger than the max size (2)" },
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("12"),
		                                              .result = std::nullopt },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().min(2).max(3).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("12"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("123"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string("1234"),
		                .result = "string size (4) is larger than the max size (3)" },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string("1"),
		                .result = "string size (1) is smaller than the min size (2)" },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().regex("^a$").get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("a"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("ba"),
		                                              .result =
		                                                  "string 'ba' doesn't match regex '^a$'" },

		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::string().regex("^[A-Z][a-z]*$").get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("Hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string("HelloW"),
		                .result = "string 'HelloW' doesn't match regex '^[A-Z][a-z]*$'" },

		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::literal("hello"),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{ .value = json::boolean(true),
		                                              .result =
		                                                  "JsonValue is not a string (literal)" },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string("Hello"),
		                .result = "string 'Hello' doesn't match literal 'hello'" },
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("hello"),
		                                              .result = std::nullopt },

		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::one_of({
		        json_schema::null(),
		        json_schema::number(),
		        json_schema::string().nonempty().get(),
		    }),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue doesn't match one of the 3 subschemas" },
		            JsonSchemaValidateTestCaseSingle{ .value = json::string("Hello"),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::string(""),
		                .result = "JsonValue doesn't match one of the 3 subschemas" },
		            JsonSchemaValidateTestCaseSingle{ .value = json::null(),
		                                              .result = std::nullopt },
		            JsonSchemaValidateTestCaseSingle{ .value = json::number(1.0),
		                                              .result = std::nullopt },

		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::array(json_schema::null(), true).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an array",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({}),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::boolean(true) }),
		                .result = "Item at index 1 in array is incorrect: JsonValue is not null",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::array(json_schema::null(), true).min(1).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an array",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({}),
		                .result = "array length (0) is smaller than the min length (1)",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::boolean(true) }),
		                .result = "Item at index 1 in array is incorrect: JsonValue is not null",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::array(json_schema::null(), true).max(2).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an array",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({}),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::null(), json::null() }),
		                .result = "array length (3) is larger than the max length (2)",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::boolean(true) }),
		                .result = "Item at index 1 in array is incorrect: JsonValue is not null",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::array(json_schema::null(), true).min(1).max(2).get(),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an array",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({}),
		                .result = "array length (0) is smaller than the min length (1)",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::null() }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::null(), json::null() }),
		                .result = "array length (3) is larger than the max length (2)",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::array({ json::null(), json::boolean(true) }),
		                .result = "Item at index 1 in array is incorrect: JsonValue is not null",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::object(true, {}),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an object",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({}),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = std::nullopt,
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::object(false, {}),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an object",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({}),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = "object can't have additional properties: but got key 'obj_key'",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::object(false, { json_schema::JsonSchemaObjectEntryCpp{
		                                             .key = "obj_key",
		                                             .value = json_schema::null(),
		                                             .required = true,
		                                         } }),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an object",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({}),
		                .result = "object is missing required key 'obj_key'",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::number(1.0) } }),
		                .result =
		                    "Value in object at key 'obj_key' is incorrect: JsonValue is not null",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() },
		                                        { "additional_key", json::boolean(true) } }),
		                .result =
		                    "object can't have additional properties: but got key 'additional_key'",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::object(true, { json_schema::JsonSchemaObjectEntryCpp{
		                                            .key = "obj_key",
		                                            .value = json_schema::null(),
		                                            .required = true,
		                                        } }),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an object",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({}),
		                .result = "object is missing required key 'obj_key'",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() },
		                                        { "additional_key", json::boolean(true) } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::number(1.0) } }),
		                .result =
		                    "Value in object at key 'obj_key' is incorrect: JsonValue is not null",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema = json_schema::object(false,
		                                  {
		                                      json_schema::JsonSchemaObjectEntryCpp{
		                                          .key = "obj_key",
		                                          .value = json_schema::null(),
		                                          .required = true,
		                                      },
		                                      json_schema::JsonSchemaObjectEntryCpp{
		                                          .key = "key2",
		                                          .value = json_schema::boolean(),
		                                          .required = false,
		                                      },
		                                  }),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an object",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({}),
		                .result = "object is missing required key 'obj_key'",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object(
		                    { { "obj_key", json::null() }, { "key2", json::boolean(true) } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value =
		                    json::object({ { "obj_key", json::null() }, { "key2", json::null() } }),
		                .result = "Value in object at key 'key2' is incorrect: JsonValue is not a "
		                          "boolean",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::number(1.0) } }),
		                .result =
		                    "Value in object at key 'obj_key' is incorrect: JsonValue is not null",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() },
		                                        { "additional_key", json::boolean(true) } }),
		                .result =
		                    "object can't have additional properties: but got key 'additional_key'",
		            },
		        },
		},
		JsonSchemaValidateTestCase{
		    .schema =
		        json_schema::object(true,
		                            {
		                                json_schema::JsonSchemaObjectEntryCpp{
		                                    .key = "obj_key",
		                                    .value = json_schema::null(),
		                                    .required = true,
		                                },
		                                json_schema::JsonSchemaObjectEntryCpp{
		                                    .key = "key2",
		                                    .value = json_schema::boolean(),
		                                    .required = false,
		                                },
		                            }),
		    .tests =
		        std::vector<JsonSchemaValidateTestCaseSingle>{
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::boolean(true),
		                .result = "JsonValue is not an object",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({}),
		                .result = "object is missing required key 'obj_key'",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::null() },
		                                        { "additional_key", json::boolean(true) } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object(
		                    { { "obj_key", json::null() }, { "key2", json::boolean(true) } }),
		                .result = std::nullopt,
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value =
		                    json::object({ { "obj_key", json::null() }, { "key2", json::null() } }),
		                .result = "Value in object at key 'key2' is incorrect: JsonValue is not a "
		                          "boolean",
		            },
		            JsonSchemaValidateTestCaseSingle{
		                .value = json::object({ { "obj_key", json::number(1.0) } }),
		                .result =
		                    "Value in object at key 'obj_key' is incorrect: JsonValue is not null",
		            },
		        },
		},
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

TEST_CASE("testing invalid json schema construction <json_schema_errors>") {

	std::vector<JsonSchemaErrorThrows> json_schema_error_tests = {
		JsonSchemaErrorThrows{
		    .schema_fn = []() -> JsonSchemaCpp { return json_schema::string().regex("\\").get(); },
		    .throw_msg = "Regex was invalid!" },
		//
		JsonSchemaErrorThrows{ .schema_fn = []() -> JsonSchemaCpp {
		                          return json_schema::string().min(2).max(1).get();
		                      },
		                       .throw_msg = "string prop error: max is smaller than min!" },
		JsonSchemaErrorThrows{ .schema_fn = []() -> JsonSchemaCpp {
		                          return json_schema::string().max(1).min(2).get();
		                      },
		                       .throw_msg = "string prop error: min is larger than max!" },
		JsonSchemaErrorThrows{ .schema_fn = []() -> JsonSchemaCpp {
		                          auto result = json_schema::string().min(1).max(1).get();
		                          throw std::runtime_error("Nothing wrong");
		                      },
		                       .throw_msg = "Nothing wrong" },
		//
		JsonSchemaErrorThrows{
		    .schema_fn = []() -> JsonSchemaCpp {
		        return json_schema::array(json_schema::null(), true).min(2).max(1).get();
		    },
		    .throw_msg = "array prop error: max is smaller than min!" },
		JsonSchemaErrorThrows{
		    .schema_fn = []() -> JsonSchemaCpp {
		        return json_schema::array(json_schema::null(), true).max(1).min(2).get();
		    },
		    .throw_msg = "array prop error: min is larger than max!" },
		JsonSchemaErrorThrows{
		    .schema_fn = []() -> JsonSchemaCpp {
		        auto result = json_schema::array(json_schema::null(), true).min(1).max(1).get();
		        throw std::runtime_error("Nothing wrong");
		    },
		    .throw_msg = "Nothing wrong" },
		//
		JsonSchemaErrorThrows{
		    .schema_fn = []() -> JsonSchemaCpp {
		        return json_schema::array(json_schema::null(), true).max(1).max(1).get();
		    },
		    .throw_msg = "array prop error: max already set!" },
		JsonSchemaErrorThrows{
		    .schema_fn = []() -> JsonSchemaCpp {
		        return json_schema::array(json_schema::null(), true).min(1).min(1).get();
		    },
		    .throw_msg = "array prop error: min already set!" },
		//
		JsonSchemaErrorThrows{ .schema_fn = []() -> JsonSchemaCpp {
		                          return json_schema::string().max(1).max(1).get();
		                      },
		                       .throw_msg = "string prop error: max already set!" },
		JsonSchemaErrorThrows{ .schema_fn = []() -> JsonSchemaCpp {
		                          return json_schema::string().min(1).min(1).get();
		                      },
		                       .throw_msg = "string prop error: min already set!" },
		JsonSchemaErrorThrows{ .schema_fn = []() -> JsonSchemaCpp {
		                          return json_schema::string().regex("^a$").regex("^a$").get();
		                      },
		                       .throw_msg = "string prop error: pattern already set!" },
	};

	for(const auto& test_case : json_schema_error_tests) {

		INFO("Test case: ", test_case.throw_msg);

		REQUIRE_THROWS_WITH_AS(test_case.schema_fn(), test_case.throw_msg, std::runtime_error);
	}
}

TEST_SUITE_END();
