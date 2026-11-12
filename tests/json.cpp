#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/compat.hpp"
#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"
#include "./helpers/json_helper.hpp"
#include "./helpers/mock_file.hpp"

#include <tjson.h>

#include "helpers/string_maker.hpp"

#include <expected>

#include <nlohmann/json.hpp>

#include <_impl/simple_regex.h>

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

struct JsonEscapeStringifyTest {
	std::array<std::string, 3> expected_values;
	JsonValue input;
};

struct JsonParseTestCaseCompat {
	std::string input;
};

using JsonParseResultCpp = std::expected<JsonValue, JsonErrorCpp>;

struct JsonFileParseTestCase {
	std::filesystem::path file;
	JsonParseResultCpp expected;
	MockFile* raw_lock_ptr;
};

} // namespace

TEST_SUITE_BEGIN("json" * doctest::description("json tests") * doctest::timeout(2.0));

TEST_CASE("testing parsing of json values <json_parser>") {

	std::vector<JsonParseTestCaseSuccess> json_parse_test_cases = {
		JsonParseTestCaseSuccess{ .input = "null", .expected = json::null() },
		JsonParseTestCaseSuccess{ .input = "   null   ", .expected = json::null() },
		JsonParseTestCaseSuccess{ .input = "\t			null   ", .expected = json::null() },
		JsonParseTestCaseSuccess{ .input = "true", .expected = json::boolean(true) },
		JsonParseTestCaseSuccess{ .input = "false", .expected = json::boolean(false) },
		JsonParseTestCaseSuccess{ .input = "100", .expected = json::number((int64_t)100) },
		JsonParseTestCaseSuccess{ .input = "-100", .expected = json::number((int64_t)-100) },
		JsonParseTestCaseSuccess{ .input = "-100.01", .expected = json::number(-100.01) },
		JsonParseTestCaseSuccess{ .input = "100.43", .expected = json::number(100.43) },
		JsonParseTestCaseSuccess{ .input = "1e2", .expected = json::number((int64_t)100) },
		JsonParseTestCaseSuccess{ .input = "-1e2", .expected = json::number((int64_t)-100) },
		JsonParseTestCaseSuccess{ .input = "1e20", .expected = json::number(1e20) },
		JsonParseTestCaseSuccess{ .input = "1E20", .expected = json::number(1E20) },
		JsonParseTestCaseSuccess{ .input = "1.2e3", .expected = json::number((int64_t)1200) },
		JsonParseTestCaseSuccess{ .input = "0", .expected = json::number((int64_t)0) },
		JsonParseTestCaseSuccess{ .input = "1.3E+3", .expected = json::number((int64_t)1300) },
		JsonParseTestCaseSuccess{ .input = "1.5E-2", .expected = json::number(0.015) },
		JsonParseTestCaseSuccess{ .input = "1.5E10",
		                          .expected = json::number((int64_t)15000000000) },
		JsonParseTestCaseSuccess{ .input = "8.98846567431158e307", // 2^1023 exactly
		                          .expected = json::number(8.98846567431158e307) },
		JsonParseTestCaseSuccess{ .input = "1e0", .expected = json::number(1.0) },
		JsonParseTestCaseSuccess{ .input = R"("hello world")",
		                          .expected = json::string("hello world") },
		JsonParseTestCaseSuccess{ .input = R"("hello world\n\"\f\t")",
		                          .expected = json::string("hello world\n\"\f\t") },
		JsonParseTestCaseSuccess{
		    .input =
		        R"("a really long string that is longer than 1024 characters and has some unicode chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる 白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる 川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる 朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく 赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる 弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる 父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる 赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる 川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる 銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる 白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる 空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる 風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる 黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 日 雨 川 のる きく 君 友")",
		    .expected = json::string(
		        "a really long string that is longer than 1024 characters and has some unicode "
		        "chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる "
		        "白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 "
		        "んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる "
		        "川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 "
		        "しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる "
		        "朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる "
		        "でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく "
		        "赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 "
		        "める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 "
		        "ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 "
		        "なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 "
		        "ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる "
		        "せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる "
		        "のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 "
		        "かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある "
		        "んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる "
		        "弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり "
		        "りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる "
		        "父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる "
		        "赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる "
		        "よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 "
		        "黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 "
		        "ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 "
		        "銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる "
		        "川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 "
		        "夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる "
		        "銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 "
		        "月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 "
		        "める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう "
		        "はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 "
		        "んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 "
		        "でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる "
		        "白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 "
		        "よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 "
		        "わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 "
		        "へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 "
		        "れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる "
		        "空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる "
		        "風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 "
		        "兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく "
		        "ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる "
		        "黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 "
		        "緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり "
		        "ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 "
		        "日 雨 川 のる きく 君 友") },

		JsonParseTestCaseSuccess{
		    .input = R"("smiley: 🙃 is not escapable as it is U+1F643")",
		    .expected = json::string("smiley: 🙃 is not escapable as it is U+1F643"),
		},
		JsonParseTestCaseSuccess{
		    .input =
		        R"("unicode surrogate: \uD83C\uDF0D should resolve to 🌍 and \ud83d\ude43 as 🙃 and \uD834\uDD1E as 𝄞")",
		    .expected =
		        json::string("unicode surrogate: 🌍 should resolve to 🌍 and 🙃 as 🙃 and 𝄞 as 𝄞"),
		},
		JsonParseTestCaseSuccess{
		    .input = R"("")",
		    .expected = json::string(""),
		},
		JsonParseTestCaseSuccess{
		    .input = R"("unicode related things: 퀀 ힰ  is the same as \uD000 \uD7B0 \uE000")",
		    .expected =
		        json::string("unicode related things: 퀀 ힰ  is the same as 퀀 ힰ "),
		},
		JsonParseTestCaseSuccess{ .input = R"("escape chars \\\/\b\r::\u0010\u000A\u000a")",
		                          .expected = json::string("escape chars \\/\b\r::\x10\n\n") },
		JsonParseTestCaseSuccess{ .input = R"({})", .expected = json::object({}) },
		JsonParseTestCaseSuccess{
		    .input = R"([null,  	1,-2,   true ])",
		    .expected = json::array({ json::null(), json::number((int64_t)1),
		                              json::number((int64_t)-2), json::boolean(true) }) },
		JsonParseTestCaseSuccess{
		    .input = R"([1e10, -2e10, 1e-10, -2e-10, -1.0, 1.0, 1.25e-10, -2.25e-10])",
		    .expected = json::array({ json::number(1e10), json::number(-2e10), json::number(1e-10),
		                              json::number(-2e-10), json::number(-1.0), json::number(1.0),
		                              json::number(1.25e-10), json::number(-2.25e-10) }) },
		JsonParseTestCaseSuccess{
		    .input =
		        R"({"key1": "hello", "key2": null, "nested": { "nested_key"   : {"nested_key2":
		   true, "array": []}}})",
		    .expected = json::object(
		        { { "key1", json::string("hello") },
		          { "key2", json::null() },
		          { "nested", json::object({
		                          { "nested_key", json::object({
		                                              { "nested_key2", json::boolean(true) },
		                                              { "array", json::array({}) },
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

		REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

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
		JsonParseTestCaseError{
		    .input = R"({)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "empty object: missing 'member' or 'end-object' after 'begin-object'",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 1 }) },
		JsonParseTestCaseError{ .input = R"({ "key2": null)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty object: missing 'member' or 'end-object'",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 14 }) },
		JsonParseTestCaseError{ .input = R"({ "key2": null - )",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid continuation of member in object: expected ','",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 15 }) },
		JsonParseTestCaseError{
		    .input = R"([)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "empty array: missing 'value' or 'end-array' after 'begin-array'", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 1 }) },
		JsonParseTestCaseError{
		    .input = R"([null,)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "empty array value", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 6 }) },
		JsonParseTestCaseError{
		    .input = R"([null,not_null)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not null", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 6 }) },
		JsonParseTestCaseError{
		    .input = R"([not_null)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "not null", dummy_str_view, JsonSourcePosition{ .line = 0, .col = 1 }) },
		JsonParseTestCaseError{ .input = R"([null)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty array: missing 'value' or 'end-array'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 5 }) },
		JsonParseTestCaseError{ .input = R"([null - )",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid continuation of values in array: expected ','",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 6 }) },
		JsonParseTestCaseError{ .input = R"(-)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty number: <EOF> after '-'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 1 }) },
		JsonParseTestCaseError{ .input = R"(-A)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number int part: incorrect start", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 1 }) },
		JsonParseTestCaseError{ .input = R"(-#)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number int part: incorrect start", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 1 }) },
		JsonParseTestCaseError{ .input = R"(-3151325312532575675757575757575775757575)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number int part: value overflowed a 64 bit number!",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 22 }) },
		JsonParseTestCaseError{ .input = R"(13124124512491246156139813265081326513205861320)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number int part: value overflowed a 64 bit number!",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 21 }) },
		JsonParseTestCaseError{ .input = R"(1.)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty number frac part: <EOF> after '.'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"(1.#)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number frac part: incorrect start", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"(1.A)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number frac part: incorrect start", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"(1.1#)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "Didn't reach the end, invalid data at the end", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{ .input = R"(1e)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty number exp part: <EOF> after 'e'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"(1e+)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty number exp part: no values after 'e' and optional sign",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{ .input = R"(1e#)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number exp part: incorrect start", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"(1eA)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid number exp part: incorrect start", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"(1e1#)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "Didn't reach the end, invalid data at the end", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{ .input = R"(1e1A)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "Didn't reach the end, invalid data at the end", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{
		    .input = R"(1e1000)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid number exp part: value overflowed the maximum allowed exponent 308!",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 6 }) },
		JsonParseTestCaseError{
		    .input = R"(1e-1000)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid number exp part: value overflowed the maximum allowed exponent 308!",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{
		    .input = R"(1.1e-1000)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid number exp part: value overflowed the maximum allowed exponent 308!",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 9 }) },
		JsonParseTestCaseError{ .input = "\"invalid utf8: \x80\"",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "Invalid UTF-8 string", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 15 }) },
		JsonParseTestCaseError{ .input = R"({"key1": 1, -})",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "wrong quotation-mark: expected '\"'", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 12 }) },
		JsonParseTestCaseError{ .input = R"({"ke)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty string: expected '\"' or string-char but got <EOF>",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 4 }) },
		JsonParseTestCaseError{ .input = "\"invalid utf8 (underflows 0): \xFF\"",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "Invalid UTF-8 string", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 30 }) },
		JsonParseTestCaseError{ .input = "\"invalid utf8: \x10\"",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string char: range [0, 0x20)", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 16 }) },
		JsonParseTestCaseError{ .input = R"("\)",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "empty string escape sequence", dummy_str_view,
		                            JsonSourcePosition{ .line = 0, .col = 2 }) },
		JsonParseTestCaseError{ .input = R"("\u")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: unicode escape is missing "
		                            "values, it requires at least 4 chars after it",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{ .input = R"("\o")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: not a recognized escape char",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{
		    .input = R"("\u#000")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: unicode escape has invalid digits", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{
		    .input = R"("\uZ000")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: unicode escape has invalid digits", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{
		    .input = R"("\uz000")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: unicode escape has invalid digits", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 3 }) },
		JsonParseTestCaseError{ .input = R"("\uDEAD")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: the unicode value is reserved "
		                            "for the second part of a surrogate pair",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{ .input = R"("\uD83D")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: the unicode surrogate pair is "
		                            "missing its second value",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{
		    .input = R"("\uD83D-----")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: the unicode surrogate pair has an invalid second "
		        "value: it needs a \\uYYYY escape sequence but is missing the '\\'",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{
		    .input = R"("\uD83D++++++")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: the unicode surrogate pair has an invalid second "
		        "value: it needs a \\uYYYY escape sequence but is missing the '\\'",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{
		    .input = R"("\uD83D\i||||")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: the unicode surrogate pair has an invalid second "
		        "value: it needs a \\uYYYY escape sequence but is missing the 'u'",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{ .input = R"("\uD83D\uxxxx")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: the unicode surrogate pair "
		                            "has an invalid second value: it needs a \\uYYYY escape "
		                            "sequence but the escape sequence has invalid digits",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{ .input = R"("\uD83D\u#000")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: the unicode surrogate pair "
		                            "has an invalid second value: it needs a \\uYYYY escape "
		                            "sequence but the escape sequence has invalid digits",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{ .input = R"("\uD83D\uZ000")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: the unicode surrogate pair "
		                            "has an invalid second value: it needs a \\uYYYY escape "
		                            "sequence but the escape sequence has invalid digits",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{ .input = R"("\uD83D\uz000")",
		                        .expected_error = JsonErrorCpp::with_string_loc(
		                            "invalid string escape sequence: the unicode surrogate pair "
		                            "has an invalid second value: it needs a \\uYYYY escape "
		                            "sequence but the escape sequence has invalid digits",
		                            dummy_str_view, JsonSourcePosition{ .line = 0, .col = 7 }) },
		JsonParseTestCaseError{
		    .input = R"("\uD83D\uAAAA")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: the unicode surrogate pair has an invalid second "
		        "value: it has to be in the correct range",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 13 }) },
		JsonParseTestCaseError{
		    .input = R"("\uD83D\uE000")",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid string escape sequence: the unicode surrogate pair has an invalid second "
		        "value: it has to be in the correct range",
		        dummy_str_view, JsonSourcePosition{ .line = 0, .col = 13 }) },
		JsonParseTestCaseError{
		    .input = R"(#)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid value: no value type was detected based on the start-char", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 0 }) },
		JsonParseTestCaseError{
		    .input = R"(z)",
		    .expected_error = JsonErrorCpp::with_string_loc(
		        "invalid value: no value type was detected based on the start-char", dummy_str_view,
		        JsonSourcePosition{ .line = 0, .col = 0 }) },
	};

	for(const auto& test_case : json_parse_test_cases) {

		INFO("Test case: ", test_case.input);

		const tstr_view str_view = helpers::tstr_view_from_str(test_case.input);

		const auto parse_result = json_value_parse_from_str(str_view);

		REQUIRE_EQ(parse_result, JsonParseResultTypeError);

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

			REQUIRE_NE(object, nullptr);

			{
				const auto key = "key_null"_tstr;

				const auto add_result = json_object_add_entry_tstr(object, &key, json::null());
				if(!tstr_static_is_null(add_result)) {
					throw std::runtime_error(
					    std::string{ "JSON object entry addition failed for key: " } +
					    string_from_tstr_static(add_result));
				}
			}

			{

				const auto key = "key_2";

				const auto add_result =
				    json_object_add_entry_cstr(object, key, json::number((int64_t)2));
				if(!tstr_static_is_null(add_result)) {
					throw std::runtime_error(
					    std::string{ "JSON object entry addition failed for key: " } +
					    string_from_tstr_static(add_result));
				}
			}

			auto json_value = new_json_value_object_rc(object);

			auto expected_value = json::object({
			    { "key_null", json::null() },
			    { "key_2", json::number((int64_t)2) },
			});

			CAutoFreePtr<JsonValue> defer = { &json_value, free_json_value };
			CAutoFreePtr<JsonValue> defer2 = { &expected_value, free_json_value };

			REQUIRE_EQ(json_value, expected_value);
		}();
	}

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

	SUBCASE("json_string_starts_with cases") {
		[]() -> void {
			JsonString* str1 = json_get_string_from_cstr("string1");

			REQUIRE_NE(str1, nullptr);
			CAutoFreePtr<JsonString> defer1 = { str1, free_json_string };

			JsonString* str2 = json_get_string_from_cstr("string2");

			REQUIRE_NE(str2, nullptr);
			CAutoFreePtr<JsonString> defer2 = { str2, free_json_string };

			auto res1 = json_string_starts_with(str1, str2);
			REQUIRE_FALSE(res1);

			JsonString* str3 = json_get_string_from_cstr("long string");

			REQUIRE_NE(str3, nullptr);
			CAutoFreePtr<JsonString> defer3 = { str3, free_json_string };

			auto res2 = json_string_starts_with(str2, str3);
			REQUIRE_FALSE(res2);

			JsonString* str4 = json_get_string_from_cstr("string");

			REQUIRE_NE(str4, nullptr);
			CAutoFreePtr<JsonString> defer4 = { str4, free_json_string };

			auto res3 = json_string_starts_with(str2, str4);
			REQUIRE_TRUE(res3);
		}();
	}

	SUBCASE("json_object_get_entry_by_key on null key") {
		[]() -> void {
			JsonObject* object = json_object_get_empty();

			REQUIRE_NE(object, nullptr);

			{
				const auto key = "key_null"_tstr;

				const auto add_result = json_object_add_entry_tstr(object, &key, json::null());
				if(!tstr_static_is_null(add_result)) {
					throw std::runtime_error(
					    std::string{ "JSON object entry addition failed for key: " } +
					    string_from_tstr_static(add_result));
				}
			}

			CAutoFreePtr<JsonObject> defer = { object, free_json_object };

			const JsonObjectEntry* obj_entry = json_object_get_entry_by_key(object, nullptr);

			REQUIRE_EQ(obj_entry, nullptr);
		}();
	}

	SUBCASE("json_format_source_location works as expected") {
		[]() -> void {
			{
				const tstr_static nonnull_src_str = "nothing"_tstr_static;

				const auto nonnull_src =
				    JsonSourceLocation{ .source = new_json_source_string(JsonStringSource{
					                        .data = tstr_static_as_view(nonnull_src_str) }),
					                    .pos = JsonSourcePosition{ .line = 0, .col = 0 } };

				tstr res = json_format_source_location(nonnull_src);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("<string source>:1:1", res);
			}

			{
				const tstr_static nonnull_src_str = "nothing"_tstr_static;

				const auto nonnull_src =
				    JsonSourceLocation{ .source = new_json_source_string(JsonStringSource{
					                        .data = tstr_static_as_view(nonnull_src_str) }),
					                    .pos = JsonSourcePosition{ .line = 146, .col = 13 } };

				tstr res = json_format_source_location(nonnull_src);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("<string source>:147:14", res);
			}

			{
				tstr nonnull_file = "nothing.c"_tstr;

				CAutoFreePtr<tstr> defer1 = { &nonnull_file, tstr_free };

				const auto nonnull_src = JsonSourceLocation{
					.source = new_json_source_file(JsonFileSource{ .file_path = &nonnull_file }),
					.pos = JsonSourcePosition{ .line = 0, .col = 0 }
				};

				tstr res = json_format_source_location(nonnull_src);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("nothing.c:1:1", res);
			}

			{
				tstr nonnull_file = "nothing.c"_tstr;

				CAutoFreePtr<tstr> defer1 = { &nonnull_file, tstr_free };

				const auto nonnull_src = JsonSourceLocation{
					.source = new_json_source_file(JsonFileSource{ .file_path = &nonnull_file }),
					.pos = JsonSourcePosition{ .line = 146, .col = 13 }
				};

				tstr res = json_format_source_location(nonnull_src);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("nothing.c:147:14", res);
			}

			{

				const auto null_src = json_source_location_get_null();

				tstr res = json_format_source_location(null_src);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("<Nowhere>", res);
			}
		}();
	}

	SUBCASE("json_format_error works as expected") {
		[]() -> void {
			{
				const tstr_static nonnull_src_str = "nothing"_tstr_static;

				const auto nonnull_src =
				    JsonSourceLocation{ .source = new_json_source_string(JsonStringSource{
					                        .data = tstr_static_as_view(nonnull_src_str) }),
					                    .pos = JsonSourcePosition{ .line = 0, .col = 0 } };

				JsonError err = {
					.message = "message 1"_tstr_static,
					.loc = nonnull_src,
				};

				tstr res = json_format_error(err);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("message 1: <string source>:1:1", res);
			}

			{
				const tstr_static nonnull_src_str = "nothing"_tstr_static;

				const auto nonnull_src =
				    JsonSourceLocation{ .source = new_json_source_string(JsonStringSource{
					                        .data = tstr_static_as_view(nonnull_src_str) }),
					                    .pos = JsonSourcePosition{ .line = 146, .col = 13 } };

				JsonError err = {
					.message = "message 2"_tstr_static,
					.loc = nonnull_src,
				};

				tstr res = json_format_error(err);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("message 2: <string source>:147:14", res);
			}

			{
				tstr nonnull_file = "nothing.c"_tstr;

				CAutoFreePtr<tstr> defer1 = { &nonnull_file, tstr_free };

				const auto nonnull_src = JsonSourceLocation{
					.source = new_json_source_file(JsonFileSource{ .file_path = &nonnull_file }),
					.pos = JsonSourcePosition{ .line = 0, .col = 0 }
				};

				JsonError err = {
					.message = "message 3"_tstr_static,
					.loc = nonnull_src,
				};

				tstr res = json_format_error(err);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("message 3: nothing.c:1:1", res);
			}

			{
				tstr nonnull_file = "nothing.c"_tstr;

				CAutoFreePtr<tstr> defer1 = { &nonnull_file, tstr_free };

				const auto nonnull_src = JsonSourceLocation{
					.source = new_json_source_file(JsonFileSource{ .file_path = &nonnull_file }),
					.pos = JsonSourcePosition{ .line = 146, .col = 13 }
				};

				JsonError err = {
					.message = "message 4"_tstr_static,
					.loc = nonnull_src,
				};

				tstr res = json_format_error(err);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("message 4: nothing.c:147:14", res);
			}

			{

				const auto null_src = json_source_location_get_null();

				JsonError err = {
					.message = "message 5"_tstr_static,
					.loc = null_src,
				};

				tstr res = json_format_error(err);

				REQUIRE_NE(TstrIsNull{}, res);

				CAutoFreePtr<tstr> defer = { &res, tstr_free };

				REQUIRE_EQ("message 5: <Nowhere>", res);
			}
		}();
	}

	SUBCASE("get_utf8_string: invalid utf8") {
		[]() -> void {
			std::array<char, 1> json_invalid_utf8 = { '\xFF' };

			const tstr_view str_view =
			    tstr_view{ .data = json_invalid_utf8.data(), .len = json_invalid_utf8.size() };

			const Utf8DataResult parse_result = get_utf8_string(str_view);

			REQUIRE_EQ(parse_result, Utf8DataResultTypeError);

			tstr_static actual_error = utf8_data_result_get_as_error(parse_result);

			REQUIRE_EQ(actual_error, "Invalid UTF-8 string"_tstr_static);
		}();
	}

	SUBCASE("null source handling") {
		[]() -> void {
			{
				bool res = simple_regex_match(nullptr, nullptr);

				REQUIRE_FALSE(res);
			}

			{

				tstr regex_str = TSTR_LIT("^hello$");

				CAutoFreePtr<tstr> defer = { &regex_str, tstr_free };

				SimpleRegexResult regex_result = simple_regex_compile(&regex_str);

				REQUIRE_EQ(get_current_tag_type_for_simple_regex_result(regex_result),
				           SimpleRegexResultTypeOk);

				SimpleRegex regex = simple_regex_result_get_as_ok(regex_result);

				CAutoFreePtr<SimpleRegex> defer2 = { &regex, free_simple_regex };

				bool res = simple_regex_match(&regex, nullptr);

				REQUIRE_FALSE(res);
			}
		}();
	}

	SUBCASE("json path constructing") {
		[]() -> void {
			auto complex_path =
			    json::path::path() / json::path::object("hello") / json::path::array(0);

			REQUIRE_NE(complex_path, json::path::path());
		}();
	}

	SUBCASE("json path eq") {
		[]() -> void {
			auto complex_path1 =
			    json::path::path() / json::path::object("hello") / json::path::array(0);

			auto complex_path2 =
			    json::path::path() / json::path::object("hello") / json::path::array(0);

			REQUIRE_EQ(complex_path1, complex_path2);

			auto complex_path3 =
			    json::path::path() / json::path::array(0) / json::path::object("hello");

			REQUIRE_NE(complex_path1, complex_path3);
		}();
	}

	SUBCASE("json path is parent of") {
		[]() -> void {
			auto complex_path1 = json::path::path() / json::path::object("hello");

			auto complex_path2 =
			    json::path::path() / json::path::object("hello") / json::path::array(0);

			REQUIRE_TRUE(complex_path1.is_parent_of(complex_path2));

			auto complex_path3 =
			    json::path::path() / json::path::array(0) / json::path::object("hello");

			REQUIRE_FALSE(complex_path1.is_parent_of(complex_path3));
		}();
	}

	SUBCASE("json path long string") {
		[]() -> void {
			auto complex_path1 =
			    json::path::path() / json::path::object("large_string_that_is_allocated");

			REQUIRE_EQ(complex_path1.string(), "$.large_string_that_is_allocated");
		}();
	}

	SUBCASE("json path to string") {
		[]() -> void {
			auto complex_path1 = json::path::path() / json::path::object("hello");

			auto complex_path2 = json::path::path() / json::path::object("hello") /
			                     json::path::array(0) / json::path::object("second");

			auto complex_path3 =
			    json::path::path() / json::path::object("hello") / json::path::object("second");

			REQUIRE_EQ(complex_path1.string(), "$.hello");

			REQUIRE_EQ(complex_path2.string(), "$.hello[0].second");

			REQUIRE_EQ(complex_path3.string(), "$.hello.second");
		}();
	}

	SUBCASE("json path to string: rfc") {
		[]() -> void {
			auto complex_path1 = json::path::path() / json::path::object("store") /
			                     json::path::object("book") / json::path::array(0) /
			                     json::path::object("title");

			REQUIRE_EQ(complex_path1.string(), "$.store.book[0].title");

			REQUIRE_EQ(complex_path1.string(JsonPathStringOptions{ .compact = false }),
			           R"($["store"]["book"][0]["title"])");
		}();
	}

	SUBCASE("json path to string with valid escapes") {
		[]() -> void {
			auto complex_path1 = json::path::path() / json::path::object("hello with a whitespace");

			auto complex_path2 =
			    json::path::path() / json::path::object("hello with a whitespace") /
			    json::path::array(0) / json::path::object("second with a \x10 and \x07 char");

			auto complex_path3 =
			    json::path::path() / json::path::object("hello with a whitespace") /
			    json::path::object("third with a smiley: 🙃") / json::path::object("") /
			    json::path::object("test") / json::path::object("1");

			REQUIRE_EQ(complex_path1.string(), R"($["hello with a whitespace"])");

			REQUIRE_EQ(
			    complex_path2.string(),
			    R"($["hello with a whitespace"][0]["second with a \u0010 and \u0007 char"])");

			REQUIRE_EQ(
			    complex_path3.string(),
			    R"($["hello with a whitespace"]["third with a smiley: \uD83D\uDE43"][""].test["1"])");
		}();
	}

	SUBCASE("json error to string") {
		[]() -> void {
			auto path = json_path_get_root();
			assert(path != nullptr);

			tstr obj_key = "large_string_that_is_allocated"_tstr;

			CAutoFreePtr<tstr> defer_obj_key{ &obj_key, tstr_free };

			bool ok = json_path_add_object_key_dup(path, &obj_key);
			assert(ok);

			ok = json_path_add_array_index(path, 1);
			assert(ok);

			JsonIterateError error{ .err = "test error 123"_tstr, .path = path };

			CAutoFreePtr<JsonIterateError> defer_error{ &error, free_json_iterate_error };

			tstr val = json_iterate_error_to_str(&error);

			CAutoFreePtr<tstr> defer_val{ &val, tstr_free };

			REQUIRE_EQ(val, "test error 123 at $.large_string_that_is_allocated[1]");
		}();
	}

	SUBCASE("json error to string (advanced)") {
		[]() -> void {
			auto path = json_path_get_root();
			assert(path != nullptr);

			tstr obj_key = "large_string_that_is_allocated"_tstr;

			CAutoFreePtr<tstr> defer_obj_key{ &obj_key, tstr_free };

			bool ok = json_path_add_object_key_dup(path, &obj_key);
			assert(ok);

			ok = json_path_add_array_index(path, 1);
			assert(ok);

			JsonIterateError error{ .err = "test error 123"_tstr, .path = path };

			CAutoFreePtr<JsonIterateError> defer_error{ &error, free_json_iterate_error };

			tstr val = json_iterate_error_to_str_advanced(&error, { .compact = false });

			CAutoFreePtr<tstr> defer_val{ &val, tstr_free };

			REQUIRE_EQ(val, R"(test error 123 at $["large_string_that_is_allocated"][1])");
		}();
	}
}

TEST_CASE("testing stringification of json values <json_parser_stringify>") {

	std::vector<JsonStringifyTest> json_stringify_test_case = {
		JsonStringifyTest{ .expected = "null", .input = json::null() },
		JsonStringifyTest{ .expected = "true", .input = json::boolean(true) },
		JsonStringifyTest{ .expected = "false", .input = json::boolean(false) },
		JsonStringifyTest{ .expected = "100", .input = json::number((int64_t)100) },
		JsonStringifyTest{ .expected = "-100", .input = json::number((int64_t)-100) },
		JsonStringifyTest{ .expected = "-100.01", .input = json::number(-100.01) },
		JsonStringifyTest{ .expected = "100.43", .input = json::number(100.43) },
		JsonStringifyTest{ .expected = R"("hello world")", .input = json::string("hello world") },
		JsonStringifyTest{ .expected = R"("hello world\n\"\f\t\b\r\\/")",
		                   .input = json::string("hello world\n\"\f\t\b\r\\/") },
		JsonStringifyTest{
		    .expected =
		        R"("a really long string that is longer than 1024 characters and has some unicode chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる 白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる 川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる 朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく 赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる 弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる 父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる 赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる 川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる 銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる 白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる 空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる 風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる 黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 日 雨 川 のる きく 君 友")",
		    .input = json::string(
		        "a really long string that is longer than 1024 characters and has some unicode "
		        "chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる "
		        "白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 "
		        "んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる "
		        "川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 "
		        "しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる "
		        "朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる "
		        "でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく "
		        "赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 "
		        "める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 "
		        "ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 "
		        "なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 "
		        "ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる "
		        "せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる "
		        "のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 "
		        "かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある "
		        "んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる "
		        "弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり "
		        "りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる "
		        "父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる "
		        "赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる "
		        "よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 "
		        "黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 "
		        "ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 "
		        "銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる "
		        "川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 "
		        "夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる "
		        "銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 "
		        "月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 "
		        "める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう "
		        "はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 "
		        "んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 "
		        "でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる "
		        "白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 "
		        "よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 "
		        "わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 "
		        "へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 "
		        "れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる "
		        "空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる "
		        "風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 "
		        "兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく "
		        "ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる "
		        "黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 "
		        "緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり "
		        "ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 "
		        "日 雨 川 のる きく 君 友") },
		JsonStringifyTest{ .expected = R"("smiley: 🙃 is not escapable as it is U+1F643")",
		                   .input = json::string("smiley: 🙃 is not escapable as it is U+1F643") },
		JsonStringifyTest{ .expected = R"("")", .input = json::string("") },
		JsonStringifyTest{
		    .expected = R"("unicode surrogate, none of these is escaped: 🌍, 🙃 and 𝄞")",
		    .input = json::string("unicode surrogate, none of these is escaped: 🌍, 🙃 and 𝄞") },
		JsonStringifyTest{ .expected = R"("\u0007")", .input = json::string("\x07") },
		JsonStringifyTest{ .expected = R"([null, 1, 2, true])",
		                   .input =
		                       json::array({ json::null(), json::number((int64_t)1),
		                                     json::number((int64_t)2), json::boolean(true) }) },
		JsonStringifyTest{
		    .expected =
		        R"({"key1": "hello", "key2": null, "nested": {"nested_key": {"array": [], "nested_key2": true}}})",
		    .input = json::object(
		        { { "key1", json::string("hello") },
		          { "key2", json::null() },
		          { "nested", json::object({
		                          { "nested_key", json::object({
		                                              { "nested_key2", json::boolean(true) },
		                                              { "array", json::array({}) },
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

		REQUIRE_NE(TstrIsNull{}, stringify_result);

		std::string actual_result = string_from_tstr(stringify_result);

		REQUIRE_EQ(actual_result, test_case.expected);

		{ // extra check, check if parsing results in the value too!

			const tstr_view str_view = tstr_as_view(&stringify_result);

			const auto parse_result = json_value_parse_from_str(str_view);

			REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

			JsonValue result = json_parse_result_get_as_ok(parse_result);
			CAutoFreePtr<JsonValue> defer2 = { &result, free_json_value };

			REQUIRE_EQ(result, test_case.input);
		}
	}
}

TEST_CASE("testing forced escaped stringification of json values <json_parser_escaped_stringify>") {

	std::vector<JsonEscapeStringifyTest> json_escape_stringify_test_cases = {
		JsonEscapeStringifyTest{ .expected_values = { "null", "null", "null" },
		                         .input = json::null() },
		JsonEscapeStringifyTest{ .expected_values = { "true", "true", "true" },
		                         .input = json::boolean(true) },
		JsonEscapeStringifyTest{ .expected_values = { "false", "false", "false" },
		                         .input = json::boolean(false) },
		JsonEscapeStringifyTest{ .expected_values = { "100", "100", "100" },
		                         .input = json::number((int64_t)100) },
		JsonEscapeStringifyTest{ .expected_values = { "-100", "-100", "-100" },
		                         .input = json::number((int64_t)-100) },
		JsonEscapeStringifyTest{ .expected_values = { "-100.01", "-100.01", "-100.01" },
		                         .input = json::number(-100.01) },
		JsonEscapeStringifyTest{ .expected_values = { "100.43", "100.43", "100.43" },
		                         .input = json::number(100.43) },
		JsonEscapeStringifyTest{
		    .expected_values = { R"("hello world")", R"("hello world")", R"("hello world")" },
		    .input = json::string("hello world") },
		JsonEscapeStringifyTest{ .expected_values = { R"("hello world\n\"\f\t\b\r\\\/")",
		                                              R"("hello world\n\"\f\t\b\r\\\/")",
		                                              R"("hello world\n\"\f\t\b\r\\\/")" },
		                         .input = json::string("hello world\n\"\f\t\b\r\\/") },
		JsonEscapeStringifyTest{
		    .expected_values = { R"("a really long string that is longer than 1024 characters and has some unicode chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる 白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる 川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる 朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく 赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる 弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる 父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる 赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる 川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる 銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる 白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる 空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる 風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる 黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 日 雨 川 のる きく 君 友")", R"("a really long string that is longer than 1024 characters and has some unicode chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる 白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる 川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる 朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく 赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる 弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる 父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる 赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる 川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる 銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる 白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる 空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる 風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる 黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 日 雨 川 のる きく 君 友")",
		                         R"("a really long string that is longer than 1024 characters and has some unicode chars like \u00F6\u00E4\u00FC\u00DF and also some japanese characters: \u305D\u3046 \u59B9 \u3067\u308B \u59B9 \u663C \u304A\u308B \u3078\u308B \u767D \u3090\u308B \u53CB \u3086\u308B \u3093\u308B \u5B50 \u6708 \u9752 \u3044\u308B \u306E\u308B \u9EC4 \u9EC4 \u3078\u308B \u6642 \u6728 \u304B\u304F \u4EBA \u308F\u308B \u571F \u9752 \u6BCD \u3093\u308B \u7A7A \u306C\u308B \u96E8 \u6843 \u3068\u308B \u307F\u308B \u3057\u304F \u3090\u308B \u305B\u308B \u706B \u9EC4 \u590F \u591C \u3080\u308B \u5DDD \u590F \u6BCD \u5144 \u5F1F \u305B\u308B \u5DDD \u7D2B \u9752 \u96EA \u307E\u308B \u7D2B \u5B50 \u3091\u308B \u307B\u308B \u3086\u308B \u590F \u7D2B \u3067\u308B \u305B\u308B \u65E5 \u8D64 \u5B50 \u3042\u308A \u3044\u308B \u3089\u308B \u9031 \u3057\u304F \u8D64 \u541B \u9280 \u3072\u308B \u3081\u308B \u7A7A \u6708 \u3059\u308B \u304A\u308B \u3059\u308B \u3080\u308B \u6625 \u7D2B \u79CB \u3084\u308B \u6642 \u306F\u308B \u3080\u308B \u305B\u308B \u671D \u3081\u308B \u5C71 \u305F\u308A \u3093\u308B \u96EA \u3080\u308B \u5C71 \u3042\u308A \u3044\u308B \u6625 \u3044\u308B \u541B \u6708 \u59C9 \u671D \u5DDD \u3078\u308B \u96E8 \u3082\u308B \u3067\u308B \u306B\u308B \u6728 \u306F\u308B \u4EBA \u671D \u663C \u306D\u308B \u3091\u308B \u3075\u308B \u5C71 \u3067\u308B \u306A\u308B \u308C\u308B \u706B \u7AF9 \u3090\u308B \u3057\u304F \u304B\u304F \u8D64 \u308C\u308B \u51AC \u3082\u308B \u53CB \u671D \u306B\u308B \u91D1 \u9752 \u306E\u308B \u6642 \u3089\u308B \u7AF9 \u5C71 \u5E74 \u77F3 \u306D\u308B \u3081\u308B \u51AC \u6642 \u59C9 \u9031 \u3081\u308B \u541B \u5DDD \u7236 \u53CB \u7DD1 \u308C\u308B \u307E\u308B \u308A\u308B \u3075\u308B \u6843 \u663C \u5DDD \u6708 \u51AC \u91D1 \u65E5 \u5C71 \u7A7A \u308D\u308B \u591C \u96E8 \u306C\u308B \u82B1 \u306C\u308B \u6843 \u308F\u308B \u3081\u308B \u6BCD \u5E74 \u3078\u308B \u91D1 \u3086\u308B \u304A\u308B \u59C9 \u3082\u308B \u82B1 \u59C9 \u6708 \u663C \u6BCD \u3081\u308B \u9EC4 \u306A\u308B \u306D\u308B \u307E\u308B \u305D\u3046 \u3088\u308B \u304D\u304F \u3088\u308B \u307B\u308B \u3042\u308A \u306C\u308B \u51AC \u3059\u308B \u65E5 \u6843 \u590F \u3080\u308B \u53CB \u82B1 \u5DDD \u3042\u308B \u6625 \u3042\u308A \u307E\u308B \u305B\u308B \u305B\u308B \u59B9 \u304D\u304F \u79CB \u304B\u304F \u3081\u308B \u304B\u304F \u305F\u308A \u304B\u304F \u9ED2 \u3059\u308B \u3055\u304F \u3091\u308B \u305B\u308B \u306C\u308B \u77F3 \u304B\u304F \u4EBA \u3088\u308B \u9031 \u77F3 \u305D\u3046 \u8D64 \u3042\u308A \u65E5 \u96E8 \u3091\u308B \u3088\u308B \u6642 \u77F3 \u306B\u308B \u9031 \u306E\u308B \u306E\u308B \u307F\u308B \u307E\u308B \u591C \u6708 \u306E\u308B \u7AF9 \u5C71 \u3092\u308B \u3084\u308B \u59C9 \u571F \u5F1F \u7236 \u304D\u304F \u79CB \u304A\u308B \u305D\u3046 \u59B9 \u9752 \u304B\u304F \u59B9 \u304B\u304F \u9ED2 \u3091\u308B \u6708 \u6211 \u5B50 \u305F\u308A \u7A7A \u5DDD \u6728 \u59C9 \u304D\u304F \u3090\u308B \u53CB \u9280 \u671D \u304A\u308B \u96EA \u3042\u308B \u3093\u308B \u706B \u3044\u308B \u96EA \u79CB \u671D \u308D\u308B \u3088\u308B \u305D\u3046 \u590F \u6843 \u6625 \u308A\u308B \u306A\u308B \u306F\u308B \u308F\u308B \u591C \u9ED2 \u9752 \u308D\u308B \u5F1F \u3072\u308B \u571F \u3057\u304F \u9752 \u706B \u767D \u308B\u308B \u304D\u304F \u305F\u308A \u6BCD \u59C9 \u3057\u304F \u306A\u308B \u3090\u308B \u307B\u308B \u663C \u3059\u308B \u3042\u308A \u308A\u308B \u305F\u308A \u591C \u305F\u308A \u7236 \u3067\u308B \u6728 \u6728 \u91D1 \u6728 \u9280 \u541B \u307F\u308B \u304B\u304F \u3059\u308B \u3091\u308B \u304A\u308B \u305B\u308B \u5F1F \u308F\u308B \u7236 \u3090\u308B \u77F3 \u590F \u3081\u308B \u3067\u308B \u77F3 \u9EC4 \u308C\u308B \u306B\u308B \u59B9 \u7DD1 \u9280 \u9ED2 \u59C9 \u304D\u304F \u306C\u308B \u308A\u308B \u51AC \u6BCD \u3075\u308B \u8D64 \u5E74 \u5B50 \u3057\u304F \u3088\u308B \u306C\u308B \u706B \u65E5 \u6211 \u3055\u304F \u6843 \u91D1 \u305F\u308A \u3093\u308B \u51AC \u6642 \u5E74 \u7DD1 \u7236 \u3084\u308B \u5DDD \u308F\u308B \u3088\u308B \u591C \u6728 \u3068\u308B \u3092\u308B \u79CB \u77F3 \u308B\u308B \u671D \u541B \u7236 \u6728 \u79CB \u304B\u304F \u53CB \u9280 \u671D \u3092\u308B \u307E\u308B \u3057\u304F \u6708 \u5F1F \u9EC4 \u59C9 \u305D\u3046 \u91D1 \u3088\u308B \u59C9 \u304D\u304F \u9280 \u3059\u308B \u706B \u6BCD \u590F \u4EBA \u306F\u308B \u4EBA \u3055\u304F \u9EC4 \u308A\u308B \u3064\u304F \u767D \u6843 \u307B\u308B \u6843 \u6728 \u3091\u308B \u305F\u308A \u96EA \u304D\u304F \u59B9 \u59B9 \u51AC \u308D\u308B \u3081\u308B \u706B \u308D\u308B \u591C \u5C71 \u7236 \u307E\u308B \u671D \u7DD1 \u6BCD \u9ED2 \u9280 \u306E\u308B \u6642 \u307E\u308B \u3057\u304F \u7A7A \u5144 \u308D\u308B \u6708 \u7D2B \u77F3 \u6728 \u3042\u308A \u6708 \u591C \u591C \u3042\u308B \u7AF9 \u3082\u308B \u3086\u308B \u3075\u308B \u5DDD \u3082\u308B \u3072\u308B \u3081\u308B \u6625 \u304D\u304F \u306E\u308B \u306E\u308B \u663C \u9EC4 \u3055\u304F \u6211 \u3072\u308B \u663C \u5DDD \u308D\u308B \u3057\u304F \u7D2B \u3055\u304F \u671D \u591C \u3086\u308B \u3081\u308B \u3092\u308B \u307E\u308B \u3088\u308B \u308C\u308B \u306C\u308B \u65E5 \u9752 \u305F\u308A \u671D \u663C \u3084\u308B \u3082\u308B \u3075\u308B \u96EA \u571F \u308B\u308B \u9280 \u590F \u3092\u308B \u6642 \u308C\u308B \u306A\u308B \u59C9 \u3086\u308B \u3059\u308B \u3064\u304F \u65E5 \u304A\u308B \u3068\u308B \u9280 \u6708 \u3055\u304F \u6BCD \u307E\u308B \u3057\u304F \u59C9 \u6708 \u9752 \u305B\u308B \u96E8 \u6843 \u304D\u304F \u3092\u308B \u5B50 \u306F\u308B \u304A\u308B \u82B1 \u9280 \u307B\u308B \u3057\u304F \u767D \u308A\u308B \u3064\u304F \u308D\u308B \u79CB \u4EBA \u3081\u308B \u591C \u6708 \u307B\u308B \u3086\u308B \u3093\u308B \u98A8 \u6211 \u7D2B \u7236 \u79CB \u3068\u308B \u706B \u306C\u308B \u7236 \u306B\u308B \u98A8 \u7236 \u6708 \u9752 \u305D\u3046 \u306F\u308B \u5B50 \u307E\u308B \u306D\u308B \u3075\u308B \u59C9 \u308B\u308B \u77F3 \u304A\u308B \u767D \u5C71 \u306D\u308B \u65E5 \u9280 \u6625 \u3055\u304F \u98A8 \u3089\u308B \u9031 \u8D64 \u6C34 \u3093\u308B \u305F\u308A \u6BCD \u6708 \u306A\u308B \u91D1 \u3042\u308A \u308B\u308B \u3067\u308B \u5DDD \u3072\u308B \u3067\u308B \u590F \u3091\u308B \u541B \u79CB \u590F \u5C71 \u9280 \u7236 \u590F \u3067\u308B \u6625 \u308C\u308B \u3093\u308B \u9EC4 \u308B\u308B \u6708 \u663C \u9EC4 \u3084\u308B \u7D2B \u96EA \u91D1 \u3091\u308B \u6728 \u91D1 \u7236 \u79CB \u7D2B \u6708 \u91D1 \u306A\u308B \u767D \u98A8 \u308B\u308B \u51AC \u706B \u5DDD \u571F \u98A8 \u305F\u308A \u3042\u308A \u3044\u308B \u306C\u308B \u571F \u305F\u308A \u571F \u706B \u7DD1 \u3089\u308B \u5C71 \u6C34 \u6C34 \u671D \u3088\u308B \u6C34 \u306C\u308B \u3064\u304F \u304B\u304F \u6708 \u308A\u308B \u5C71 \u79CB \u3082\u308B \u304B\u304F \u77F3 \u591C \u3072\u308B \u59B9 \u308F\u308B \u7236 \u541B \u3064\u304F \u706B \u308F\u308B \u305F\u308A \u3082\u308B \u9280 \u3059\u308B \u59B9 \u9752 \u6C34 \u3075\u308B \u308C\u308B \u82B1 \u82B1 \u3064\u304F \u8D64 \u4EBA \u306C\u308B \u306C\u308B \u5DDD \u59C9 \u571F \u3078\u308B \u3042\u308A \u306B\u308B \u96EA \u3067\u308B \u308D\u308B \u6C34 \u3091\u308B \u91D1 \u306D\u308B \u3057\u304F \u4EBA \u590F \u6708 \u7DD1 \u591C \u7A7A \u307E\u308B \u7D2B \u591C \u308C\u308B \u9ED2 \u96E8 \u307F\u308B \u53CB \u6BCD \u6728 \u3088\u308B \u3080\u308B \u306D\u308B \u59C9 \u9EC4 \u3067\u308B \u304B\u304F \u3057\u304F \u8D64 \u671D \u6642 \u3089\u308B \u307E\u308B \u7A7A \u51AC \u9EC4 \u663C \u7D2B \u3090\u308B \u571F \u7D2B \u307E\u308B \u79CB \u6728 \u6625 \u591C \u3086\u308B \u9ED2 \u5E74 \u96E8 \u3078\u308B \u59C9 \u9031 \u3082\u308B \u7A7A \u306A\u308B \u98A8 \u3064\u304F \u7AF9 \u671D \u308C\u308B \u305B\u308B \u3089\u308B \u3067\u308B \u4EBA \u3055\u304F \u5B50 \u308A\u308B \u5F1F \u59B9 \u706B \u7D2B \u96EA \u3089\u308B \u7DD1 \u3059\u308B \u7236 \u5144 \u304B\u304F \u6708 \u96EA \u3057\u304F \u590F \u3088\u308B \u7DD1 \u3075\u308B \u3092\u308B \u6642 \u3055\u304F \u308A\u308B \u3081\u308B \u663C \u8D64 \u6BCD \u7AF9 \u3057\u304F \u3057\u304F \u3042\u308B \u5E74 \u5C71 \u65E5 \u53CB \u6708 \u3042\u308B \u3092\u308B \u6C34 \u8D64 \u306C\u308B \u6708 \u3092\u308B \u5C71 \u3067\u308B \u3089\u308B \u3089\u308B \u3086\u308B \u7D2B \u308F\u308B \u9ED2 \u3055\u304F \u308C\u308B \u8D64 \u306D\u308B \u3090\u308B \u7AF9 \u541B \u6708 \u7AF9 \u308C\u308B \u307F\u308B \u9ED2 \u5DDD \u663C \u306A\u308B \u304D\u304F \u590F \u59B9 \u307B\u308B \u5144 \u7DD1 \u3067\u308B \u7AF9 \u65E5 \u5144 \u571F \u571F \u590F \u304A\u308B \u3064\u304F \u767D \u8D64 \u59C9 \u6D77 \u307F\u308B \u6843 \u3091\u308B \u3086\u308B \u591C \u7236 \u3075\u308B \u3042\u308A \u306D\u308B \u305F\u308A \u308B\u308B \u541B \u3055\u304F \u3082\u308B \u306D\u308B \u7AF9 \u3078\u308B \u96E8 \u3082\u308B \u7A7A \u3082\u308B \u3091\u308B \u671D \u77F3 \u767D \u304D\u304F \u6D77 \u59B9 \u65E5 \u96E8 \u5DDD \u306E\u308B \u304D\u304F \u541B \u53CB")" },
		    .input = json::string(
		        "a really long string that is longer than 1024 characters and has some unicode "
		        "chars like öäüß and also some japanese characters: そう 妹 でる 妹 昼 おる へる "
		        "白 ゐる 友 ゆる んる 子 月 青 いる のる 黄 黄 へる 時 木 かく 人 わる 土 青 母 "
		        "んる 空 ぬる 雨 桃 とる みる しく ゐる せる 火 黄 夏 夜 むる 川 夏 母 兄 弟 せる "
		        "川 紫 青 雪 まる 紫 子 ゑる ほる ゆる 夏 紫 でる せる 日 赤 子 あり いる らる 週 "
		        "しく 赤 君 銀 ひる める 空 月 する おる する むる 春 紫 秋 やる 時 はる むる せる "
		        "朝 める 山 たり んる 雪 むる 山 あり いる 春 いる 君 月 姉 朝 川 へる 雨 もる "
		        "でる にる 木 はる 人 朝 昼 ねる ゑる ふる 山 でる なる れる 火 竹 ゐる しく かく "
		        "赤 れる 冬 もる 友 朝 にる 金 青 のる 時 らる 竹 山 年 石 ねる める 冬 時 姉 週 "
		        "める 君 川 父 友 緑 れる まる りる ふる 桃 昼 川 月 冬 金 日 山 空 ろる 夜 雨 "
		        "ぬる 花 ぬる 桃 わる める 母 年 へる 金 ゆる おる 姉 もる 花 姉 月 昼 母 める 黄 "
		        "なる ねる まる そう よる きく よる ほる あり ぬる 冬 する 日 桃 夏 むる 友 花 川 "
		        "ある 春 あり まる せる せる 妹 きく 秋 かく める かく たり かく 黒 する さく ゑる "
		        "せる ぬる 石 かく 人 よる 週 石 そう 赤 あり 日 雨 ゑる よる 時 石 にる 週 のる "
		        "のる みる まる 夜 月 のる 竹 山 をる やる 姉 土 弟 父 きく 秋 おる そう 妹 青 "
		        "かく 妹 かく 黒 ゑる 月 我 子 たり 空 川 木 姉 きく ゐる 友 銀 朝 おる 雪 ある "
		        "んる 火 いる 雪 秋 朝 ろる よる そう 夏 桃 春 りる なる はる わる 夜 黒 青 ろる "
		        "弟 ひる 土 しく 青 火 白 るる きく たり 母 姉 しく なる ゐる ほる 昼 する あり "
		        "りる たり 夜 たり 父 でる 木 木 金 木 銀 君 みる かく する ゑる おる せる 弟 わる "
		        "父 ゐる 石 夏 める でる 石 黄 れる にる 妹 緑 銀 黒 姉 きく ぬる りる 冬 母 ふる "
		        "赤 年 子 しく よる ぬる 火 日 我 さく 桃 金 たり んる 冬 時 年 緑 父 やる 川 わる "
		        "よる 夜 木 とる をる 秋 石 るる 朝 君 父 木 秋 かく 友 銀 朝 をる まる しく 月 弟 "
		        "黄 姉 そう 金 よる 姉 きく 銀 する 火 母 夏 人 はる 人 さく 黄 りる つく 白 桃 "
		        "ほる 桃 木 ゑる たり 雪 きく 妹 妹 冬 ろる める 火 ろる 夜 山 父 まる 朝 緑 母 黒 "
		        "銀 のる 時 まる しく 空 兄 ろる 月 紫 石 木 あり 月 夜 夜 ある 竹 もる ゆる ふる "
		        "川 もる ひる める 春 きく のる のる 昼 黄 さく 我 ひる 昼 川 ろる しく 紫 さく 朝 "
		        "夜 ゆる める をる まる よる れる ぬる 日 青 たり 朝 昼 やる もる ふる 雪 土 るる "
		        "銀 夏 をる 時 れる なる 姉 ゆる する つく 日 おる とる 銀 月 さく 母 まる しく 姉 "
		        "月 青 せる 雨 桃 きく をる 子 はる おる 花 銀 ほる しく 白 りる つく ろる 秋 人 "
		        "める 夜 月 ほる ゆる んる 風 我 紫 父 秋 とる 火 ぬる 父 にる 風 父 月 青 そう "
		        "はる 子 まる ねる ふる 姉 るる 石 おる 白 山 ねる 日 銀 春 さく 風 らる 週 赤 水 "
		        "んる たり 母 月 なる 金 あり るる でる 川 ひる でる 夏 ゑる 君 秋 夏 山 銀 父 夏 "
		        "でる 春 れる んる 黄 るる 月 昼 黄 やる 紫 雪 金 ゑる 木 金 父 秋 紫 月 金 なる "
		        "白 風 るる 冬 火 川 土 風 たり あり いる ぬる 土 たり 土 火 緑 らる 山 水 水 朝 "
		        "よる 水 ぬる つく かく 月 りる 山 秋 もる かく 石 夜 ひる 妹 わる 父 君 つく 火 "
		        "わる たり もる 銀 する 妹 青 水 ふる れる 花 花 つく 赤 人 ぬる ぬる 川 姉 土 "
		        "へる あり にる 雪 でる ろる 水 ゑる 金 ねる しく 人 夏 月 緑 夜 空 まる 紫 夜 "
		        "れる 黒 雨 みる 友 母 木 よる むる ねる 姉 黄 でる かく しく 赤 朝 時 らる まる "
		        "空 冬 黄 昼 紫 ゐる 土 紫 まる 秋 木 春 夜 ゆる 黒 年 雨 へる 姉 週 もる 空 なる "
		        "風 つく 竹 朝 れる せる らる でる 人 さく 子 りる 弟 妹 火 紫 雪 らる 緑 する 父 "
		        "兄 かく 月 雪 しく 夏 よる 緑 ふる をる 時 さく りる める 昼 赤 母 竹 しく しく "
		        "ある 年 山 日 友 月 ある をる 水 赤 ぬる 月 をる 山 でる らる らる ゆる 紫 わる "
		        "黒 さく れる 赤 ねる ゐる 竹 君 月 竹 れる みる 黒 川 昼 なる きく 夏 妹 ほる 兄 "
		        "緑 でる 竹 日 兄 土 土 夏 おる つく 白 赤 姉 海 みる 桃 ゑる ゆる 夜 父 ふる あり "
		        "ねる たり るる 君 さく もる ねる 竹 へる 雨 もる 空 もる ゑる 朝 石 白 きく 海 妹 "
		        "日 雨 川 のる きく 君 友") },
		JsonEscapeStringifyTest{
		    .expected_values = { R"("smiley: 🙃 is not escapable as it is U+1F643")",
		                         R"("smiley: \uD83D\uDE43 is not escapable as it is U+1F643")",
		                         R"("smiley: \uD83D\uDE43 is not escapable as it is U+1F643")" },
		    .input = json::string("smiley: 🙃 is not escapable as it is U+1F643") },
		JsonEscapeStringifyTest{
		    .expected_values = { R"("unicode surrogate, these should all be escaped as surrogate pairs: 🌍, 🙃 and 𝄞")",
		                         R"("unicode surrogate, these should all be escaped as surrogate pairs: \uD83C\uDF0D, \uD83D\uDE43 and \uD834\uDD1E")",
		                         R"("unicode surrogate, these should all be escaped as surrogate pairs: \uD83C\uDF0D, \uD83D\uDE43 and \uD834\uDD1E")" },
		    .input = json::string("unicode surrogate, these should all be escaped as surrogate "
		                          "pairs: 🌍, 🙃 and 𝄞") },
		JsonEscapeStringifyTest{
		    .expected_values = { R"("this string should be exactly 256 chars long, which triggers a special case in the resizing, but the last char has to be a surrogate char, that needs 12 chars exactly:(<-168) filler!! 🙃🙃🙃🙃🙃🌍")",
		                         R"("this string should be exactly 256 chars long, which triggers a special case in the resizing, but the last char has to be a surrogate char, that needs 12 chars exactly:(<-168) filler!! \uD83D\uDE43\uD83D\uDE43\uD83D\uDE43\uD83D\uDE43\uD83D\uDE43\uD83C\uDF0D")",R"("this string should be exactly 256 chars long, which triggers a special case in the resizing, but the last char has to be a surrogate char, that needs 12 chars exactly:(<-168) filler!! \uD83D\uDE43\uD83D\uDE43\uD83D\uDE43\uD83D\uDE43\uD83D\uDE43\uD83C\uDF0D")",  },
		    .input = json::string(
		        "this string should be exactly 256 chars long, which triggers a special case in "
		        "the resizing, but the last char has to be a surrogate char, that needs 12 chars "
		        "exactly:(<-168) filler!! 🙃🙃🙃🙃🙃🌍") },
		JsonEscapeStringifyTest{ .expected_values = { R"("\u0007")", R"("\u0007")", R"("\u0007")" },
		                         .input = json::string("\x07") },
		JsonEscapeStringifyTest{
		    .expected_values = { R"([null, 1, 2, true])", R"([null, 1, 2, true])",
		                         R"([null, 1, 2, true])" },
		    .input = json::array({ json::null(), json::number((int64_t)1), json::number((int64_t)2),
		                           json::boolean(true) }) },
		JsonEscapeStringifyTest{
		    .expected_values = { R"({"key1": "hello", "key2": null, "nested": {"nested_key": {"array": [], "nested_key2": true}}})",
		                         R"({"key1": "hello", "key2": null, "nested": {"nested_key": {"array": [], "nested_key2": true}}})",
		                         R"({"key1": "hello", "key2": null, "nested": {"nested_key": {"array": [], "nested_key2": true}}})" },
		    .input = json::object(
		        { { "key1", json::string("hello") },
		          { "key2", json::null() },
		          { "nested", json::object({
		                          { "nested_key", json::object({
		                                              { "nested_key2", json::boolean(true) },
		                                              { "array", json::array({}) },
		                                          }) },
		                      }

		                                   ) } }) },
	};
	CAutoFreePtr<std::vector<JsonEscapeStringifyTest>> defer_tests = {
		&json_escape_stringify_test_cases,
		[](std::vector<JsonEscapeStringifyTest>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));
			    free_json_value(&(value->input));
		    }
		}
	};

	for(const auto& test_case : json_escape_stringify_test_cases) {

		for(size_t i = 0; i < test_case.expected_values.size(); ++i) {

			const std::string& expected = test_case.expected_values.at(i);
			auto [escape, escape_str] =
			    ([i]() -> std::pair<JsonSerializeEscapeOption, tstr_static> {
				    switch(i) {
					    case 0: return { JsonSerializeEscapeOptionMore, "more"_tstr_static };
					    case 1: return { JsonSerializeEscapeOptionMost, "most"_tstr_static };
					    case 2: return { JsonSerializeEscapeOptionAll, "all"_tstr_static };
					    default: {
						    throw std::runtime_error("Invlaid index");
					    }
				    }
			    })();

			INFO("Test case: ", test_case.input, " escape: ", escape_str);

			auto stringify_result =
			    json_value_to_string_advanced(&test_case.input, {
			                                                        .indent_size = 0,
			                                                        .escape = escape,
			                                                    });
			CAutoFreePtr<tstr> defer = { &stringify_result, tstr_free };

			REQUIRE_NE(TstrIsNull{}, stringify_result);

			std::string actual_result = string_from_tstr(stringify_result);

			REQUIRE_EQ(actual_result, expected);

			{ // extra check, check if parsing results in the value too!

				const tstr_view str_view = tstr_as_view(&stringify_result);

				const auto parse_result = json_value_parse_from_str(str_view);

				REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

				JsonValue result = json_parse_result_get_as_ok(parse_result);
				CAutoFreePtr<JsonValue> defer2 = { &result, free_json_value };

				REQUIRE_EQ(result, test_case.input);

				{ // extra check, check if parsing of the same thing with another indentation
				  // results in the same value too

					std::array<size_t, 4> indentations = { 2, 4, 8, 16 };

					for(size_t j = 0; j < indentations.size(); ++j) {

						const auto indentation = indentations.at(j);

						auto stringify_result2 = json_value_to_string_advanced(
						    &test_case.input, {
						                          .indent_size = indentation,
						                          .escape = escape,
						                      });
						CAutoFreePtr<tstr> defer_str2 = { &stringify_result2, tstr_free };

						REQUIRE_NE(TstrIsNull{}, stringify_result2);

						const tstr_view str_view2 = tstr_as_view(&stringify_result2);

						const auto parse_result2 = json_value_parse_from_str(str_view2);

						REQUIRE_EQ(parse_result2, JsonParseResultTypeOk);

						JsonValue result2 = json_parse_result_get_as_ok(parse_result2);
						CAutoFreePtr<JsonValue> defer_str_str2 = { &result2, free_json_value };

						REQUIRE_EQ(result2, test_case.input);
					}
				}
			}
		}
	}
}

TEST_CASE("testing json compatibility with other json library (nlohmann_json) <json_compat>") {

	std::vector<JsonParseTestCaseCompat> json_compat_test_cases = {
		JsonParseTestCaseCompat{
		    .input = "null",
		},
		JsonParseTestCaseCompat{
		    .input = "   null   ",
		},
		JsonParseTestCaseCompat{
		    .input = "\t			null   ",
		},
		JsonParseTestCaseCompat{
		    .input = "true",
		},
		JsonParseTestCaseCompat{
		    .input = "false",
		},
		JsonParseTestCaseCompat{
		    .input = "100",
		},
		JsonParseTestCaseCompat{
		    .input = "-100",
		},
		JsonParseTestCaseCompat{
		    .input = "-100.01",
		},
		JsonParseTestCaseCompat{
		    .input = "100.43",
		},
		JsonParseTestCaseCompat{
		    .input = "1e2",
		},
		JsonParseTestCaseCompat{
		    .input = "-1e2",
		},
		JsonParseTestCaseCompat{
		    .input = "1e20",
		},
		JsonParseTestCaseCompat{
		    .input = "1E20",
		},
		JsonParseTestCaseCompat{
		    .input = "1.2e3",
		},
		JsonParseTestCaseCompat{
		    .input = "0",
		},
		JsonParseTestCaseCompat{
		    .input = "1.3E+3",
		},
		JsonParseTestCaseCompat{
		    .input = "1.5E-2",
		},
		JsonParseTestCaseCompat{
		    .input = "1.5E10",
		},
		JsonParseTestCaseCompat{
		    .input = "8.98846567431158e307", // 2^1023 exactly
		},
		JsonParseTestCaseCompat{
		    .input = "1e0",
		},
		JsonParseTestCaseCompat{
		    .input = R"("hello world")",
		},
		JsonParseTestCaseCompat{
		    .input = R"("hello world\n\"\f\t")",
		},
		JsonParseTestCaseCompat{
		    .input = R"("escape chars \\\/\b\r::\u0010\u000A\u000a")",
		},
		JsonParseTestCaseCompat{
		    .input = R"({})",
		},
		JsonParseTestCaseCompat{
		    .input = R"([null,  	1,-2,   true ])",
		},
		JsonParseTestCaseCompat{
		    .input = R"([1e10, -2e10, 1e-10, -2e-10, -1.0, 1.0, 1.25e-10, -2.25e-10])",
		},
		JsonParseTestCaseCompat{
		    .input =
		        R"({"key1": "hello", "key2": null, "nested": { "nested_key"   : {"nested_key2":
		   true, "array": []}}})",
		},
	};

	for(const auto& test_case : json_compat_test_cases) {

		INFO("Test case: ", test_case.input);

		const tstr_view str_view = helpers::tstr_view_from_str(test_case.input);

		const auto parse_result = json_value_parse_from_str(str_view);

		REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

		JsonValue result = json_parse_result_get_as_ok(parse_result);
		CAutoFreePtr<JsonValue> defer = { &result, free_json_value };

		const auto& my_json_result = result;

		const auto& compat_json_result = nlohmann::json::parse(test_case.input);

		REQUIRE_EQ(my_json_result, compat_json_result);
	}
}

using MockFileTest = std::tuple<JsonParseResultCpp, std::unique_ptr<MockFile>, std::string>;

// just here as a dummy tstr_view
const static tstr dummy_file = TSTR_LIT_CONST("__dummy_file__impl__");

[[nodiscard]] static std::vector<MockFileTest> get_mock_file_tests(bool debug) {
	std::vector<MockFileTest> tests = {};

	{
		const auto file = "test_file_allow";

		tests.emplace_back(
		    JsonParseResultCpp{ json::array({ json::null() }) },
		    std::make_unique<MockFileFuse>(
		        std::initializer_list<
		            std::tuple<std::string, MockFileFuse::FileData, MockFlagsCpp>>{
		            { file, "[null]", MockFlagsCpp{ .scenario = FailScenarioNone } } },
		        debug),
		    file);
	}

	{
		const auto file = "test_file_deny";

		tests.emplace_back(
		    JsonParseResultCpp::unexpected_type{
		        JsonErrorCpp::with_file_loc("Couldn't read from the file", &dummy_file,
		                                    JsonSourcePosition{ .line = 0, .col = 0 }) },
		    std::make_unique<MockFileFuse>(
		        std::initializer_list<
		            std::tuple<std::string, MockFileFuse::FileData, MockFlagsCpp>>{
		            { file, "[null]", MockFlagsCpp{ .scenario = FailScenarioReadFailsGeneric } } },
		        debug),
		    file);
	}

	{
		const auto file = "test_file_deny";

		tests.emplace_back(
		    JsonParseResultCpp::unexpected_type{
		        JsonErrorCpp::with_file_loc("Couldn't open file for reading", &dummy_file,
		                                    JsonSourcePosition{ .line = 0, .col = 0 }) },
		    std::make_unique<MockFileFuse>(
		        std::initializer_list<
		            std::tuple<std::string, MockFileFuse::FileData, MockFlagsCpp>>{
		            { file, "[null]",
		              MockFlagsCpp{ .scenario = FailScenarioStatNegativeFileSize } } },
		        debug),
		    file);
	}

	{
		const auto file = "test_file_deny";

		tests.emplace_back(
		    JsonParseResultCpp::unexpected_type{ JsonErrorCpp::with_file_loc(
		        "Read to few data", &dummy_file, JsonSourcePosition{ .line = 0, .col = 0 }) },
		    std::make_unique<MockFileFuse>(
		        std::initializer_list<
		            std::tuple<std::string, MockFileFuse::FileData, MockFlagsCpp>>{
		            { file, "[null]", MockFlagsCpp{ .scenario = FailScenarioReadFailsLessData } } },
		        debug),
		    file);
	}

	return tests;
}

[[nodiscard]] static std::unique_ptr<MockFileLock> get_file_lock(MockFile* raw_ptr) {
	if(raw_ptr == nullptr) {
		return std::make_unique<DummyFileLock>();
	}
	return raw_ptr->lock();
}

TEST_CASE("testing json file parsing <json_file_parse>" * doctest::timeout(60.0)) {

	std::filesystem::path test_file_root = helpers::get_test_file_root();

	std::vector<JsonFileParseTestCase> json_file_test_cases = {
		JsonFileParseTestCase{ .file = test_file_root / "inputs" / "NOT_PRESENT.json",
		                       .expected =
		                           JsonParseResultCpp::unexpected_type{ JsonErrorCpp::with_file_loc(
		                               "Couldn't open file for reading", &dummy_file,
		                               JsonSourcePosition{ .line = 0, .col = 0 }) },
		                       .raw_lock_ptr = nullptr },
		JsonFileParseTestCase{
		    .file = test_file_root / "inputs" / "test.json",
		    .expected = JsonParseResultCpp{ json::object({
		        { "my object key", json::array({ json::number((int64_t)1), json::number((int64_t)2),
		                                         json::null(), json::boolean(true) }) },
		    }) },
		    .raw_lock_ptr = nullptr },

	};

	const bool debug = false;

	std::vector<MockFileTest> mock_file_tests = get_mock_file_tests(debug);

	for(const auto& mock_file_test : mock_file_tests) {

		std::filesystem::path file_path =
		    std::get<1>(mock_file_test)->root() / std::get<2>(mock_file_test);

		json_file_test_cases.push_back(
		    JsonFileParseTestCase{ .file = file_path,
		                           .expected = std::get<0>(mock_file_test),
		                           .raw_lock_ptr = std::get<1>(mock_file_test).get() });
	}

	CAutoFreePtr<std::vector<JsonFileParseTestCase>> defer_tests = {
		&json_file_test_cases,
		[](std::vector<JsonFileParseTestCase>* const values) -> void {
		    for(size_t i = 0; i < values->size(); ++i) {
			    auto* const value = &(values->at(i));

			    if(value->expected.has_value()) {
				    JsonValue* const ok_value = &(value->expected.value());
				    free_json_value(ok_value);
			    }
		    }
		}
	};

	for(const auto& test_case : json_file_test_cases) {
		INFO("Test case: ", test_case.file);

		std::unique_ptr<MockFileLock> fuse_lock = get_file_lock(test_case.raw_lock_ptr);

		std::string defer_str = test_case.file.string();

		const tstr_view file_view = helpers::tstr_view_from_str(defer_str);

		const tstr file_path = tstr_from_static_cstr_with_len(file_view.data, file_view.len);

		const auto parse_result = json_value_parse_from_file(&file_path);

		if(!test_case.expected.has_value()) {

			const auto& expected_error = test_case.expected.error();

			REQUIRE_EQ(parse_result, JsonParseResultTypeError);

			JsonError result = json_parse_result_get_as_error(parse_result);

			const auto actual_error = JsonErrorCpp{ result };

			REQUIRE_EQ(actual_error, expected_error);
		} else {

			REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

			JsonValue result = json_parse_result_get_as_ok(parse_result);
			CAutoFreePtr<JsonValue> defer = { &result, free_json_value };

			const auto& expected_result = test_case.expected.value();

			REQUIRE_EQ(result, expected_result);
		}
	}
}

TEST_SUITE_END();
