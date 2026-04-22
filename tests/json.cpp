#include <doctest.h>

#include "./helpers/generic.hpp"
#include "./helpers/helpers.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include "helpers/string_maker.hpp"

namespace {

struct JsonParseTestCaseSuccess {
	std::string input;
	JsonValue expected;
};

struct JsonParseTestCaseError {
	std::string input;
	JsonErrorCpp expected_error;
};

struct JsonStringifyTest {
	std::string expected;
	JsonValue input;
};

} // namespace

TEST_SUITE_BEGIN("json" * doctest::description("json tests") * doctest::timeout(2.0));

TEST_CASE("testing parsing of json values <json_parser>") {

	std::vector<JsonParseTestCaseSuccess> json_parse_test_cases = {
		JsonParseTestCaseSuccess{ .input = "null", .expected = JsonValueCpp::null() },
		JsonParseTestCaseSuccess{ .input = "   null   ", .expected = JsonValueCpp::null() },
		JsonParseTestCaseSuccess{ .input = "\t			null   ",
		                          .expected = JsonValueCpp::null() },
		JsonParseTestCaseSuccess{ .input = "true", .expected = JsonValueCpp::boolean(true) },
		JsonParseTestCaseSuccess{ .input = "false", .expected = JsonValueCpp::boolean(false) },
		JsonParseTestCaseSuccess{ .input = "100", .expected = JsonValueCpp::number((int64_t)100) },
		JsonParseTestCaseSuccess{ .input = "-100",
		                          .expected = JsonValueCpp::number((int64_t)-100) },
		JsonParseTestCaseSuccess{ .input = "-100.01", .expected = JsonValueCpp::number(-100.01) },
		JsonParseTestCaseSuccess{ .input = "100.43", .expected = JsonValueCpp::number(100.43) },
		JsonParseTestCaseSuccess{ .input = "1e2", .expected = JsonValueCpp::number((int64_t)100) },
		JsonParseTestCaseSuccess{ .input = "1.2e3",
		                          .expected = JsonValueCpp::number((int64_t)1200) },
		JsonParseTestCaseSuccess{ .input = "1.3E+3",
		                          .expected = JsonValueCpp::number((int64_t)1300) },
		JsonParseTestCaseSuccess{ .input = "1.5E-2", .expected = JsonValueCpp::number(0.015) },
		JsonParseTestCaseSuccess{ .input = R"("hello world")",
		                          .expected = JsonValueCpp::string("hello world") },
		JsonParseTestCaseSuccess{ .input = R"("hello world\n\"\f\t")",
		                          .expected = JsonValueCpp::string("hello world\n\"\f\t") },
		JsonParseTestCaseSuccess{
		    .input = R"([null,  	1,2,   true ])",
		    .expected = JsonValueCpp::array(
		        { JsonValueCpp::null(), JsonValueCpp::number((int64_t)1),
		          JsonValueCpp::number((int64_t)2), JsonValueCpp::boolean(true) }) },
		JsonParseTestCaseSuccess{
		    .input =
		        R"({"key1": "hello", "key2": null, "nested": { "nested_key"   : {"nested_key2":
		   true, "array": []}}})",
		    .expected = JsonValueCpp::object(
		        { { "key1", JsonValueCpp::string("hello") },
		          { "key2", JsonValueCpp::null() },
		          { "nested",
		            JsonValueCpp::object({
		                { "nested_key", JsonValueCpp::object({
		                                    { "nested_key2", JsonValueCpp::boolean(true) },
		                                    { "array", JsonValueCpp::array({}) },
		                                }) },
		            }

		                                 ) } }) },
	};
	CAutoFreePtr<std::vector<JsonParseTestCaseSuccess>> defer_tests = {
		&json_parse_test_cases,
		[](std::vector<JsonParseTestCaseSuccess>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));
			    free_json_value(&(value->expected));
		    }
		}
	};

	for(const auto& test_case : json_parse_test_cases) {

		INFO("Test case: ", test_case.input);

		const tstr_view str_view = helpers::tstr_view_from_str(test_case.input);

		const auto parse_result = json_value_parse_from_str(str_view);

		REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result), JsonParseResultTypeOk);

		JsonValue result = json_parse_result_get_as_ok(parse_result);
		CAutoFreePtr<JsonValue> defer = { &result, free_json_value };

		REQUIRE_EQ(result, test_case.expected);
	}
}

TEST_CASE("testing parse errors of json values <json_parser_error>") {

	// just here as a dummy tstr_view
	const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");

	std::vector<JsonParseTestCaseError> json_parse_test_cases = {
		JsonParseTestCaseError{
		    .input = "not_null xD",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not null", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 0 }) },
		JsonParseTestCaseError{
		    .input = "for_sure_not_false ",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not a boolean", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 0 }) },
		JsonParseTestCaseError{
		    .input = "trivially_not_true ",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not a boolean", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 0 }) },
		JsonParseTestCaseError{
		    .input = "  trivially_not_true ",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not a boolean", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{
		    .input = "  \ntrivially_not_true ",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not a boolean", dummy_str_view, JsonSourcePosition{ .line = 1, .col = 0 }) },
		JsonParseTestCaseError{
		    .input = "\n  trivially_not_true ",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not a boolean", dummy_str_view, JsonSourcePosition{ .line = 1, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"({"key1": 1, "key1": 2 })",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "json object has duplicate key", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 21 }) },
		JsonParseTestCaseError{ .input = R"({"key1": 1,)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty object member: missing member after 'value-separator'",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 11 }) },
		JsonParseTestCaseError{ .input = R"({"key1": 1, ")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty string: <EOF> after '\"'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 13 }) },
		JsonParseTestCaseError{
		    .input = R"({"key1")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "empty object member: missing 'name-separator' after member name", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{ .input = R"({"key1" -)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "wrong name-separator: expected ':'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 8 }) },
		JsonParseTestCaseError{ .input = R"({"key1" :)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty object member: missing value after 'name-separator'",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 9 }) },
		JsonParseTestCaseError{ .input = R"({"key1" : )",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty value: expected value but got <EOF>", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 10 }) },

	};

	for(const auto& test_case : json_parse_test_cases) {

		INFO("Test case: ", test_case.input);

		const tstr_view str_view = helpers::tstr_view_from_str(test_case.input);

		const auto parse_result = json_value_parse_from_str(str_view);

		REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result),
		           JsonParseResultTypeError);

		JsonError result = json_parse_result_get_as_error(parse_result);

		const auto actual_error = JsonErrorCpp{ result };

		REQUIRE_EQ(actual_error, test_case.expected_error);
	}
}

TEST_CASE("testing helper functions of the json parser <json_parser_helper_fn>") {

	SUBCASE("null source handling") {
		[]() -> void {
			const auto null_src = json_source_location_get_null();

			REQUIRE_TRUE(json_source_location_is_null(null_src));

			const tstr_static nonnull_src_str = "nothing"_tstr_static;

			const auto nonnull_src =
			    JsonSourceLocation{ .source = new_json_source_string(JsonStringSource{
				                        .data = tstr_static_as_view(nonnull_src_str) }),
				                    .pos = JsonSourcePosition{ .line = 0, .col = 0 } };

			REQUIRE_FALSE(json_source_location_is_null(nonnull_src));
		}();
	}

	SUBCASE("object add entry") {
		[]() -> void {
			JsonObject* const object = json_object_get_empty();

			if(object == nullptr) {
				throw std::runtime_error("JSON object initialization failed");
			}

			{
				const auto key = "key_null"_tstr;

				const auto add_result =
				    json_object_add_entry_tstr(object, &key, JsonValueCpp::null());
				if(!tstr_static_is_null(add_result)) {
					throw std::runtime_error(
					    std::string{ "JSON object entry addition failed for key: " } +
					    string_from_tstr_static(add_result));
				}
			}

			{

				const auto key = "key_2";

				const auto add_result =
				    json_object_add_entry_cstr(object, key, JsonValueCpp::number((int64_t)2));
				if(!tstr_static_is_null(add_result)) {
					throw std::runtime_error(
					    std::string{ "JSON object entry addition failed for key: " } +
					    string_from_tstr_static(add_result));
				}
			}

			auto json_value = new_json_value_object_rc(object);

			auto expected_value = JsonValueCpp::object({
			    { "key_null", JsonValueCpp::null() },
			    { "key_2", JsonValueCpp::number((int64_t)2) },
			});

			CAutoFreePtr<JsonValue> defer = { &json_value, free_json_value };
			CAutoFreePtr<JsonValue> defer2 = { &expected_value, free_json_value };

			REQUIRE_EQ(json_value, expected_value);
		}();
	}
}

TEST_CASE("testing stringification of json values <json_parser_stringify>") {

	std::vector<JsonStringifyTest> json_stringify_test_case = {
		JsonStringifyTest{ .expected = "null", .input = JsonValueCpp::null() },
		JsonStringifyTest{ .expected = "true", .input = JsonValueCpp::boolean(true) },
		JsonStringifyTest{ .expected = "false", .input = JsonValueCpp::boolean(false) },
		JsonStringifyTest{ .expected = "100", .input = JsonValueCpp::number((int64_t)100) },
		JsonStringifyTest{ .expected = "-100", .input = JsonValueCpp::number((int64_t)-100) },
		JsonStringifyTest{ .expected = "-100.01", .input = JsonValueCpp::number(-100.01) },
		JsonStringifyTest{ .expected = "100.43", .input = JsonValueCpp::number(100.43) },
		JsonStringifyTest{ .expected = R"("hello world")",
		                   .input = JsonValueCpp::string("hello world") },
		JsonStringifyTest{ .expected = R"("hello world\n\"\f\t")",
		                   .input = JsonValueCpp::string("hello world\n\"\f\t") },
		JsonStringifyTest{
		    .expected = R"("smiley: 🙃 is not escapable as it is U+1F643")",
		    .input = JsonValueCpp::string("smiley: 🙃 is not escapable as it is U+1F643") },
		JsonStringifyTest{ .expected = R"([null, 1, 2, true])",
		                   .input = JsonValueCpp::array(
		                       { JsonValueCpp::null(), JsonValueCpp::number((int64_t)1),
		                         JsonValueCpp::number((int64_t)2), JsonValueCpp::boolean(true) }) },
		JsonStringifyTest{
		    .expected =
		        R"({"key1": "hello", "key2": null, "nested": {"nested_key": {"array": [], "nested_key2": true}}})",
		    .input = JsonValueCpp::object(
		        { { "key1", JsonValueCpp::string("hello") },
		          { "key2", JsonValueCpp::null() },
		          { "nested",
		            JsonValueCpp::object({
		                { "nested_key", JsonValueCpp::object({
		                                    { "nested_key2", JsonValueCpp::boolean(true) },
		                                    { "array", JsonValueCpp::array({}) },
		                                }) },
		            }

		                                 ) } }) },
	};
	CAutoFreePtr<std::vector<JsonStringifyTest>> defer_tests = {
		&json_stringify_test_case,
		[](std::vector<JsonStringifyTest>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));
			    free_json_value(&(value->input));
		    }
		}
	};

	for(const auto& test_case : json_stringify_test_case) {

		INFO("Test case: ", test_case.input);

		auto stringify_result = json_value_to_string(&test_case.input);
		CAutoFreePtr<tstr> defer = { &stringify_result, tstr_free };

		REQUIRE_FALSE(tstr_is_null(&stringify_result));

		std::string actual_result = string_from_tstr(stringify_result);

		REQUIRE_EQ(actual_result, test_case.expected);

		{ // extra check, check if parsing results in the value too!

			const tstr_view str_view = tstr_as_view(&stringify_result);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result),
			           JsonParseResultTypeOk);

			JsonValue result = json_parse_result_get_as_ok(parse_result);
			CAutoFreePtr<JsonValue> defer2 = { &result, free_json_value };

			REQUIRE_EQ(result, test_case.input);
		}
	}
}

TEST_CASE("manual json test cases <json_manual>") {

	SUBCASE("json_object_add_entry_dup frees memory correctly") {
		[]() -> void {
			JsonObject* object = json_object_get_empty();

			REQUIRE_NE(object, nullptr);
			CAutoFreePtr<JsonObject> defer_tests = { object, free_json_object };

			JsonString* key_string = json_get_string_from_cstr("key1");

			auto res1 = json_object_add_entry_dup(object, key_string, new_json_value_null());

			REQUIRE_TRUE(tstr_static_is_null(res1));

			auto res2 = json_object_add_entry_dup(object, key_string, new_json_value_null());

			REQUIRE_FALSE(tstr_static_is_null(res2));

			std::string actual_error = string_from_tstr_static(res2);

			std::string expected_error = "json object has duplicate key";

			REQUIRE_EQ(expected_error, actual_error);
		}();
	}
}

// TODO: compare with nhlohmann json!

TEST_SUITE_END();
