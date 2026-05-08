#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include <tjson_coverage_allocators.h>

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

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

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

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

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

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

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

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

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

	SUBCASE("parsing empty array fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "[]";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "Internal OOM error: array allocation failed while parsing an empty array",
			    dummy_str_view, JsonSourcePosition{ .line = 0, .col = 2 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing non-empty array fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "[1,2]";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "Internal OOM error: array allocation failed while parsing an array",
			    dummy_str_view, JsonSourcePosition{ .line = 0, .col = 1 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing: array entry add fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.realloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = "[1]";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "json array add error", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 2 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}

	SUBCASE("parsing: string parsing fails") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res = mock_allocator.realloc().always_fail();
			REQUIRE_TRUE(mock_res);

			std::string json_str = R"("hello world")";

			const tstr_view str_view = helpers::tstr_view_from_str(json_str);

			auto parse_result = json_value_parse_from_str(str_view);

			CAutoFreePtr<JsonParseResult> defer_parse_result = { &parse_result, [](JsonParseResult* res) -> void {
				                                                    // noop, as the error is a
				                                                    // tstr_static
				                                                    free_json_parse_result(*res);
				                                                } };

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			// just here as a dummy tstr_view
			const auto dummy_str_view = tstr_view_from("__dummy_str_view__impl__");
			auto expected_error = JsonErrorCpp::with_string_loc(
			    "json string add error", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 2 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}
}

namespace {
struct AllocTest {
	uint64_t value_1;
	bool value_2;
	char value_3[7];
};
} // namespace

TEST_CASE("testing oom mock implementation <oom_mock_impl>") {

	SUBCASE("testing fail after") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const std::vector<size_t> fail_amounts = { 1, 30, 200 };

			for(const auto& fail_amount : fail_amounts) {

				const bool mock_res = mock_allocator.calloc().fail_after(fail_amount);
				REQUIRE_TRUE(mock_res);

				for(size_t i = 0; i < fail_amount; ++i) {
					AllocTest* allocated = (AllocTest*)TJSON_CALLOC(2, sizeof(AllocTest));

					REQUIRE_NE(allocated, nullptr);
					TJSON_FREE(allocated);
				}

				AllocTest* allocated = (AllocTest*)TJSON_CALLOC(2, sizeof(AllocTest));

				REQUIRE_EQ(allocated, nullptr);
			}
		}();
	}

	SUBCASE("testing duplicate retireval of handle") {
		[]() -> void {
			auto* handle1 = tjson_coverage_allocator_get_handle(AllocatorFunctionTypeMalloc);

			REQUIRE_NE(handle1, nullptr);

			auto* handle2 = tjson_coverage_allocator_get_handle(AllocatorFunctionTypeMalloc);

			REQUIRE_EQ(handle2, nullptr);

			tjson_coverage_allocator_free_handle(handle1);
		}();
	}

	SUBCASE("testing recovery with fail never") {
		[]() -> void {
			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const bool mock_res1 = mock_allocator.malloc().always_fail();
			REQUIRE_TRUE(mock_res1);

			AllocTest* allocated1 = (AllocTest*)TJSON_MALLOC(sizeof(AllocTest));

			REQUIRE_EQ(allocated1, nullptr);

			const bool mock_res2 = mock_allocator.malloc().never_fail();
			REQUIRE_TRUE(mock_res2);

			AllocTest* allocated2 = (AllocTest*)TJSON_MALLOC(sizeof(AllocTest));

			REQUIRE_NE(allocated2, nullptr);
		}();
	}
}

TEST_SUITE_END();
