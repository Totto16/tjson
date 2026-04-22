#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include "helpers/allocators.hpp"
#include "helpers/string_maker.hpp"

TEST_SUITE_BEGIN("json_oom" * doctest::description("json oom tests") * doctest::timeout(2.0));

TEST_CASE("testing oom behaviour of json functions <json_oom_tester>") {

	SUBCASE("first alloc fails") {
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
			    "Internal OOM error", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 1 });

			REQUIRE_EQ(actual_error, expected_error);
		}();
	}
}

TEST_SUITE_END();
