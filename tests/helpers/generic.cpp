

#include "./generic.hpp"

[[nodiscard]] tstr helpers::tstr_from_utf8_string(const std::vector<std::uint8_t>& val) {
	const tstr buffer = tstr_from_len((const char*)val.data(), val.size());
	return buffer;
}

[[nodiscard]] std::vector<std::uint8_t> helpers::vector_from_string(const std::string& data) {
	std::vector<std::uint8_t> result = {};
	result.reserve(data.size());

	for(const auto& ch : data) {
		result.emplace_back(ch);
	}

	return result;
}

[[nodiscard]] tstr_view helpers::tstr_view_from_str(const std::string& value) {
	return tstr_view{ .data = value.data(), .len = value.size() };
}
