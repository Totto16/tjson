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

TEST_SUITE_BEGIN("json_helper" * doctest::description("json helper tests") * doctest::timeout(2.0));

TEST_CASE("testing simple json iterator example <json_iterator_simple>") {

	JsonValue value = json::object({});

	RTTIAnnotatedValue empty = { .type = {}, .ptr = NULL };

	JsonIterateResult actual_result = json_value_iterate(&value, test_iterate_cb, empty);

	REQUIRE_IS_NOT_ERROR(actual_result);

	RTTIAnnotatedValue struct_value = json_iterate_result_get_as_ok(actual_result);

	RTTIAnnotatedValue expected_value = TRTTI_ANNOTATED_VALUE_GET(TestJsonStruct, NULL);

	REQUIRE_EQ(struct_value, expected_value);

	TestJsonStruct* result = TRTTI_ANNOTATED_VALUE_CAST(TestJsonStruct, struct_value);

	REQUIRE_NE(result, nullptr);
}

TEST_SUITE_END();
