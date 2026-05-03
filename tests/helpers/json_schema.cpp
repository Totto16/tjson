
#include "./json_schema.hpp"

#include "./cpp_types.hpp"
#include "./generic.hpp"

JsonSchemaStringBuilder::JsonSchemaStringBuilder()
    : JsonSchemaBuilderGeneric<JsonSchemaString>{ json_schema_string_get() } {}

[[nodiscard]] JsonSchema JsonSchemaStringBuilder::to_schema(JsonSchemaString* const value) const {
	return new_json_schema_string_rc(value);
}

void JsonSchemaStringBuilder::free_value(JsonSchemaString* const value) const {
	free_json_schema_string(value);
}

JsonSchemaArrayBuilder::JsonSchemaArrayBuilder(const JsonSchema items,
                                               const bool require_unique_items)
    : JsonSchemaBuilderGeneric<JsonSchemaArray>{ json_schema_array_get(items,
	                                                                   require_unique_items) } {}

[[nodiscard]] JsonSchema JsonSchemaArrayBuilder::to_schema(JsonSchemaArray* const value) const {
	return new_json_schema_array_rc(value);
}

void JsonSchemaArrayBuilder::free_value(JsonSchemaArray* const value) const {
	free_json_schema_array(value);
}

[[nodiscard]] JsonSchemaStringBuilder JsonSchemaCpp::string() {
	return JsonSchemaStringBuilder{};
}

[[nodiscard]] JsonSchemaArrayBuilder JsonSchemaCpp::array(JsonSchema items,
                                                          bool require_unique_items) {
	return JsonSchemaArrayBuilder{ items, require_unique_items };
}

[[nodiscard]] JsonSchema JsonSchemaCpp::literal(std::string&& str) {

	return new_json_schema_literal_rc(json_schema_literal_get_cstr(str.c_str()));
}

[[nodiscard]] JsonSchema JsonSchemaCpp::number() {
	return new_json_schema_number();
}

[[nodiscard]] JsonSchema JsonSchemaCpp::boolean() {
	return new_json_schema_boolean();
}

[[nodiscard]] JsonSchema JsonSchemaCpp::null() {
	return new_json_schema_null();
}

[[nodiscard]] JsonSchema JsonSchemaCpp::one_of(std::initializer_list<JsonSchema>&& values) {
	JsonSchemaOneOf* const one_of = json_schema_one_of_get_empty();

	if(one_of == nullptr) {
		throw std::runtime_error("JSON Schema OneOf initialization failed");
	}

	for(auto&& value : values) {
		const auto add_result = json_schema_one_of_add_entry(one_of, value);
		if(!tstr_static_is_null(add_result)) {
			throw std::runtime_error(std::string{ "JSON Schema OneOf initialization failed:" } +
			                         string_from_tstr_static(add_result));
		}
	}

	return new_json_schema_one_of_rc(one_of);
}

[[nodiscard]] JsonSchema
JsonSchemaCpp::object(const bool allow_additional_properties,
                      std::initializer_list<std::pair<std::string, JsonSchema>>&& values) {
	JsonSchemaObject* const object = json_schema_object_get(allow_additional_properties);

	if(object == nullptr) {
		throw std::runtime_error("JSON Schema object initialization failed");
	}

	for(auto&& value : values) {
		JsonString* key_moved =
		    json_get_string_from_tstr_view(helpers::tstr_view_from_str(value.first));
		if(key_moved == nullptr) {
			throw std::runtime_error(
			    "JSON Schema object initialization failed: key initialization failed");
		}

		const JsonValue final_value = value.second;
		const auto add_result = json_object_add_entry(object, &key_moved, final_value);
		if(!tstr_static_is_null(add_result)) {
			throw std::runtime_error(
			    std::string{ "JSON Schema object entry addition failed for key: " } +
			    string_from_tstr_static(add_result));
		}
	}

	return new_json_schema_object_rc(object);
}

std::ostream& operator<<(std::ostream& os, const JsonSchema& json_schema) {
	auto str = json_schema_to_string(&json_schema);

	os << str;

	return os;
}
