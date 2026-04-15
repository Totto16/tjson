
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <tstr.h>

namespace helpers {

[[nodiscard]] tstr tstr_from_utf8_string(const std::vector<std::uint8_t>& val);

[[nodiscard]] std::vector<std::uint8_t> vector_from_string(const std::string& data);

[[nodiscard]] tstr_view tstr_view_from_str(const std::string& value);

} // namespace helpers
