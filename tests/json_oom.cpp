#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include "helpers/allocators.hpp"
#include "helpers/string_maker.hpp"

TEST_SUITE_BEGIN("json_oom" * doctest::description("json oom tests") * doctest::timeout(2.0));

TEST_CASE("testing oom behaviour of json functions <json_oom_tester>") {

	SUBCASE("parsing string fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "\"hello\"";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result),
			           JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "Internal OOM error: string allocation failed while parsing one", dummy_str_view,
			    JsonSourcePosition{ .line = 0, .col = 1 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing empty object fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "{}";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result),
			           JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "Internal OOM error: object allocation failed while parsing an empty object",
			    dummy_str_view, JsonSourcePosition{ .line = 0, .col = 2 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing non-empty object fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "{\"hello\"}";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result),
			           JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "Internal OOM error: object allocation failed while parsing an object",
			    dummy_str_view, JsonSourcePosition{ .line = 0, .col = 1 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing: object entry add fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.calloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "{\"hello\":null}";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(get_current_tag_type_for_json_parse_result(parse_result),
			           JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error =
			    JsonErrorCpp::with_string_loc("json object add error", dummy_str_view,
			                                  JsonSourcePosition{ .line = 0, .col = 13 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing: object entry (tstr) add manual fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			JsonObject* object = json_object_get_empty();

			REQUIRE_NE(object, nullptr);
			CAutoFreePtr<JsonObject> defer_tests = { object, free_json_object };

			tstr key = TSTR_LIT("key1");

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			auto res1 = json_object_add_entry_tstr(object, &key, new_json_value_null());

			std::string actual_error = string_from_tstr_static(res1);

			std::string expected_error =
			    "Internal OOM error: string allocation failed while adding an object entry by tstr";

			REQUIRE_EQ(expected_error, actual_error);
		}();
	}

	SUBCASE("parsing: object entry (cstr) add manual fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			JsonObject* object = json_object_get_empty();

			REQUIRE_NE(object, nullptr);
			CAutoFreePtr<JsonObject> defer_tests = { object, free_json_object };

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			auto res1 = json_object_add_entry_cstr(object, "key1", new_json_value_null());

			std::string actual_error = string_from_tstr_static(res1);

			std::string expected_error =
			    "Internal OOM error: string allocation failed while adding an object entry by cstr";

			REQUIRE_EQ(expected_error, actual_error);
		}();
	}
}

TEST_SUITE_END();
