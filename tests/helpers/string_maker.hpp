

#pragma once

#include <doctest.h>

#include <sstream>
#include <vector>

#include "./helpers.hpp"
#include "./json.hpp"
#include "./json_schema.hpp"

template <typename T>
concept is_cpp_stream_printable = requires(T val, std::ostream& os) {
	{ os << val } -> std::convertible_to<std::ostream&>;
};

template <typename T>
doctest::String os_stream_formattable_to_doctest(const T& value)
    requires(is_cpp_stream_printable<T>)
{
	std::stringstream str{};
	str << value;
	std::string string = str.str();
	return doctest::String{ string.c_str(),
		                    static_cast<doctest::String::size_type>(string.size()) };
}

// #include <support/helpers.hpp>

namespace doctest {
template <> struct StringMaker<std::vector<std::pair<std::string, std::string>>> {
	static String convert(const std::vector<std::pair<std::string, std::string>>& string_map) {
		return ::os_stream_formattable_to_doctest(string_map);
	}
};

template <typename T> struct StringMaker<std::optional<T>> {
	static String convert(const std::optional<T>& val) {
		return ::os_stream_formattable_to_doctest(val);
	}
};

template <typename T> struct StringMaker<std::vector<T>> {
	static String convert(const std::vector<T>& vec) {
		return ::os_stream_formattable_to_doctest(vec);
	}
};

template <typename A, typename B> struct StringMaker<std::pair<A, B>> {
	static String convert(const std::pair<A, B>& pair) {
		return ::os_stream_formattable_to_doctest(pair);
	}
};

template <> struct StringMaker<IsNotError> {
	static String convert(const IsNotError& error) {
		return ::os_stream_formattable_to_doctest(error);
	}
};

template <> struct StringMaker<TstrStaticIsNull> {
	static String convert(const TstrStaticIsNull& is_null) {
		return ::os_stream_formattable_to_doctest(is_null);
	}
};

template <typename T>
    requires(IsCErrorVariant<T>)
struct StringMaker<T> {
	static String convert(const T& val) { return ::os_stream_formattable_to_doctest(val); }
};

template <> struct StringMaker<tstr> {
	static String convert(const tstr& str) { return ::os_stream_formattable_to_doctest(str); }
};

template <> struct StringMaker<tstr_static> {
	static String convert(const tstr_static& str) {
		return ::os_stream_formattable_to_doctest(str);
	}
};

template <> struct StringMaker<JsonValue> {
	static String convert(const JsonValue& json_value) {
		return ::os_stream_formattable_to_doctest(json_value);
	}
};

template <> struct StringMaker<JsonErrorCpp> {
	static String convert(const JsonErrorCpp& json_error) {
		return ::os_stream_formattable_to_doctest(json_error);
	}
};

template <> struct StringMaker<JsonError> {
	static String convert(const JsonError& json_error) {
		return ::os_stream_formattable_to_doctest(json_error);
	}
};

template <> struct StringMaker<JsonSchema> {
	static String convert(const JsonSchema& json_value) {
		return ::os_stream_formattable_to_doctest(json_value);
	}
};

} // namespace doctest
