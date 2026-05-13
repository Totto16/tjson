#include "./compat.hpp"

#include "./cpp_types.hpp"
#include "./generic.hpp"

#include "./json.hpp"

[[nodiscard]] static bool operator==(const JsonNumber& json_number, const nlohmann::json& compat) {

	// NOTE: here it is fine for us, to compare float values, as this is a strict equal!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

	if(compat.is_number_float()) {
		return json_number.value == (double)compat.get<nlohmann::json::number_float_t>();
	}

	if(compat.is_number_integer()) {
		return json_number.value == (double)compat.get<nlohmann::json::number_integer_t>();
	}

	if(compat.is_number_unsigned()) {
		return json_number.value == (double)compat.get<nlohmann::json::number_unsigned_t>();
	}

#pragma GCC diagnostic pop

	throw std::runtime_error("comapt json type is not a number!");
}

[[nodiscard]] static bool operator==(const JsonString* const json_string,
                                     const nlohmann::json::string_t& compat) {

	JsonString* const compat_string =
	    json_get_string_from_tstr_view(helpers::tstr_view_from_str(compat));

	if(compat_string == nullptr) {
		throw std::runtime_error("JSON string initialization failed");
	}

	CAutoFreePtr<JsonString> defer_str = { compat_string, free_json_string };

	return json_string == JsonStringCpp{ compat_string };
}

[[nodiscard]] static bool operator==(const JsonArray* const json_array,
                                     const nlohmann::json::array_t& compat) {

	const size_t size1 = json_array_get_size(json_array);
	const size_t size2 = compat.size();

	if(size1 != size2) {
		return false;
	}

	for(size_t i = 0; i < size1; ++i) {

		const JsonValue* val1 = json_array_get_at(json_array, i);
		const auto& val2 = compat.at(i);

		if(*val1 != val2) {
			return false;
		}
	}

	return true;
}

[[nodiscard]] static bool operator==(const JsonObject* const json_object,
                                     const nlohmann::json::object_t& compat) {

	const size_t size1 = json_object_get_count(json_object);
	const size_t size2 = compat.size();

	if(size1 != size2) {
		return false;
	}

	// as the iteration is not stable, we need to iterate over the one, and assert that the other
	// one has also the same entry!
	JsonObjectIter* iter1 = json_object_get_iterator(json_object);

	assert(iter1 != nullptr);

	CAutoFreePtr<JsonObjectIter> defer = { iter1, json_object_free_iterator };

	while(true) {

		const JsonObjectEntry* entry = json_object_iterator_next(iter1);

		if(entry == nullptr) {
			break;
		}

		const JsonString* const key = json_object_entry_get_key(entry);

		assert(key != nullptr);

		auto key_tstr = json_string_get_as_str(key);

		assert(!tstr_is_null(&key_tstr));

		CAutoFreePtr<tstr> defer_key_str = { &key_tstr, tstr_free };

		std::string key_str = string_from_tstr(key_tstr);

		const bool compat_has_kes = compat.contains(key_str);

		// the same key was not found
		if(!compat_has_kes) {
			return false;
		}

		const auto& value2 = compat.at(key_str);

		const JsonValue value1 = json_object_entry_get_value(entry);

		if(value1 != value2) {
			return false;
		}
	}

	return true;
}

[[nodiscard]] static JsonValueType
get_current_tag_type_for_json_value_compat(const nlohmann::json& compat) {

	const auto type_n = compat.type();

	switch(type_n) {
		case nlohmann::json::value_t::null: {
			return JsonValueTypeNull;
		}
		case nlohmann::json::value_t::object: {
			return JsonValueTypeObject;
		}
		case nlohmann::json::value_t::array: {
			return JsonValueTypeArray;
		}
		case nlohmann::json::value_t::string: {
			return JsonValueTypeString;
		}
		case nlohmann::json::value_t::boolean: {
			return JsonValueTypeBoolean;
		}
		case nlohmann::json::value_t::number_integer: {
			return JsonValueTypeNumber;
		}
		case nlohmann::json::value_t::number_unsigned: {
			return JsonValueTypeNumber;
		}
		case nlohmann::json::value_t::number_float: {
			return JsonValueTypeNumber;
		}
		case nlohmann::json::value_t::binary:
		case nlohmann::json::value_t::discarded:
		default: {
			throw std::runtime_error(std::string{ "invalid type: " } +
			                         std::to_string(std::to_underlying(type_n)) +
			                         "for json object");
		}
	}
}

[[nodiscard]] static bool json_value_compat_eq_impl(const JsonValue* const json_value,
                                                    const nlohmann::json& compat) {
	const auto tag1 = get_current_tag_type_for_json_value(*json_value);
	const auto tag2 = get_current_tag_type_for_json_value_compat(compat);

	if(tag1 != tag2) {
		return false;
	}

	SWITCH_JSON_VALUE(*json_value) {
		CASE_JSON_VALUE_IS_NULL() {

			if(compat.is_null()) {
				return true;
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_BOOLEAN_CONST(*json_value, boolean_1) {

			if(compat.is_boolean()) {

				return boolean_1.value == compat.get<nlohmann::json::boolean_t>();
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_NUMBER_CONST(*json_value, number_1) {

			if(compat.is_number()) {

				return number_1 == compat;
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_STRING_CONST(*json_value, string_1) {

			if(compat.is_string()) {

				return string_1 == compat.get<nlohmann::json::string_t>();
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_ARRAY_CONST(*json_value, array_1) {

			if(compat.is_array()) {

				return array_1.arr == compat.get<nlohmann::json::array_t>();
			}

			return false;
		}
		VARIANT_CASE_END();
		CASE_JSON_VALUE_IS_OBJECT_CONST(*json_value, object_1) {

			if(compat.is_object()) {

				return object_1.obj == compat.get<nlohmann::json::object_t>();
			}

			return false;
		}
		VARIANT_CASE_END();
		default: {
			return false;
		}
	}
}

[[nodiscard]] bool operator==(const JsonValue& json_value, const nlohmann::json& compat) {
	return json_value_compat_eq_impl(&json_value, compat);
}
