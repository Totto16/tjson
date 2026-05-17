

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

[[nodiscard]] bool operator==(const IsNotError& /* error */, const JsonIterateResult& result) {
	IF_JSON_ITERATE_RESULT_IS_ERROR_IGN(result) {
		return false;
	}

	return true;
}

std::ostream& operator<<(std::ostream& os, const JsonIterateResult& result) {

	IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(result) {

		os << "JsonIterateResult::error -> " << error.error.err;

		return os;
	}

	IF_JSON_ITERATE_RESULT_IS_OK_CONST(result) {
		os << "JsonIterateResult::ok -> " << ok;

		return os;
	}

	return os;
}
