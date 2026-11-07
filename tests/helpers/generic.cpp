

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

[[nodiscard]] std::filesystem::path helpers::get_test_file_root() {
	std::filesystem::path test_file_root = std::filesystem::current_path();

	if(!std::filesystem::exists(test_file_root)) {
		throw std::runtime_error{ std::string{ "Path for the test files doesn't exist: " } +
			                      test_file_root.string() };
	}

	if(!std::filesystem::exists((test_file_root / "inputs"))) {
		test_file_root = test_file_root / "tests" / "files";
	}

	if(!std::filesystem::exists(test_file_root)) {
		throw std::runtime_error{ std::string{ "Path for the test files doesn't exist: " } +
			                      test_file_root.string() };
	}

	return test_file_root;
}
