#pragma once

#include <cstdint>
#include <string>
#include <vector>

[[nodiscard]] std::string get_hex_value_for_u8(std::uint8_t value);

#define REQUIRE_TRUE(val) REQUIRE(val)
