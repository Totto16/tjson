#include "./c_types.hpp"

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

[[nodiscard]] std::string get_hex_value_for_u8(std::uint8_t value) {
	std::ostringstream oss;
	oss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
	    << static_cast<int>(value);

	return oss.str();
}
