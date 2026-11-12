

#include "./rtti.hpp"

[[nodiscard]] bool operator==(const RTTIAnnotatedValue& rtti_value1,
                              const RTTIAnnotatedValue& rtti_value2) {

	return TRTTI_TYPE_MATCHES_OTHER_TYPE(rtti_value1.type, rtti_value2.type);
}

std::ostream& operator<<(std::ostream& os, const RTTIAnnotatedValue& rtti_value) {

	os << "RTTIAnnotatedValue{ "
	   << string_from_tstr_static(
	          tstr_static{ .ptr = rtti_value.type.name.ptr, .len = rtti_value.type.name.len })
	   << " }";

	return os;
}
