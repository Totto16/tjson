
#include "./json_schema.hpp"

#include "./cpp_types.hpp"
#include "./generic.hpp"

JsonSchemaStringBuilder::JsonSchemaStringBuilder()
    : JsonSchemaBuilderGeneric<JsonSchemaString>{ json_schema_string_get(),
	                                              free_json_schema_string } {}

[[nodiscard]] JsonSchema JsonSchemaStringBuilder::to_schema(JsonSchemaString* const value) const {
	return new_json_schema_string_rc(value);
}

[[nodiscard]] JsonSchemaStringBuilder& JsonSchemaStringBuilder::nonempty() {

	const auto result = json_schema_string_set_nonempty(this->value());

	if(!tstr_static_is_null(result)) {
		throw std::runtime_error(string_from_tstr_static(result));
	}

	return *this;
}

[[nodiscard]] JsonSchemaStringBuilder& JsonSchemaStringBuilder::min(size_t value) {

	const auto result = json_schema_string_set_min(this->value(), value);

	if(!tstr_static_is_null(result)) {
		throw std::runtime_error(string_from_tstr_static(result));
	}

	return *this;
}

[[nodiscard]] JsonSchemaStringBuilder& JsonSchemaStringBuilder::max(size_t value) {

	const auto result = json_schema_string_set_max(this->value(), value);

	if(!tstr_static_is_null(result)) {
		throw std::runtime_error(string_from_tstr_static(result));
	}

	return *this;
}

[[nodiscard]] JsonSchemaStringBuilder& JsonSchemaStringBuilder::regex(const std::string& str) {

	auto* regex = json_schema_regex_get(str.c_str());

	if(regex == nullptr) {
		throw std::runtime_error("Regex was invalid!");
	}

	const auto result = json_schema_string_set_regex(this->value(), regex);

	if(!tstr_static_is_null(result)) {
		free_json_schema_regex(regex);
		throw std::runtime_error(string_from_tstr_static(result));
	}

	return *this;
}

JsonSchemaArrayBuilder::JsonSchemaArrayBuilder(const JsonSchemaCpp& items,
                                               const bool require_unique_items)
    : JsonSchemaBuilderGeneric<JsonSchemaArray>{
	      json_schema_array_get(items.get(), require_unique_items), free_json_schema_array
      } {}

[[nodiscard]] JsonSchema JsonSchemaArrayBuilder::to_schema(JsonSchemaArray* const value) const {
	return new_json_schema_array_rc(value);
}

[[nodiscard]] JsonSchemaStringBuilder json_schema::string() {
	return JsonSchemaStringBuilder{};
}

[[nodiscard]] JsonSchemaArrayBuilder json_schema::array(const JsonSchemaCpp& items,
                                                        bool require_unique_items) {
	return JsonSchemaArrayBuilder{ items, require_unique_items };
}

[[nodiscard]] JsonSchemaCpp json_schema::literal(std::string&& str) {

	return JsonSchemaCpp{ new_json_schema_literal_rc(json_schema_literal_get_cstr(str.c_str())) };
}

[[nodiscard]] JsonSchemaCpp json_schema::number() {
	return JsonSchemaCpp{ new_json_schema_number() };
}

[[nodiscard]] JsonSchemaCpp json_schema::boolean() {
	return JsonSchemaCpp{ new_json_schema_boolean() };
}

[[nodiscard]] JsonSchemaCpp json_schema::null() {
	return JsonSchemaCpp{ new_json_schema_null() };
}

[[nodiscard]] JsonSchemaCpp json_schema::one_of(std::initializer_list<JsonSchemaCpp>&& values) {
	JsonSchemaOneOf* const one_of = json_schema_one_of_get_empty();

	if(one_of == nullptr) {
		throw std::runtime_error("JSON Schema OneOf initialization failed");
	}

	for(auto&& value : values) {
		const auto add_result = json_schema_one_of_add_entry(one_of, value.get());
		if(!tstr_static_is_null(add_result)) {
			throw std::runtime_error(std::string{ "JSON Schema OneOf initialization failed:" } +
			                         string_from_tstr_static(add_result));
		}
	}

	return JsonSchemaCpp{ new_json_schema_one_of_rc(one_of) };
}

[[nodiscard]] JsonSchemaCpp
json_schema::object(const bool allow_additional_properties,
                    std::initializer_list<json_schema::JsonSchemaObjectEntryCpp>&& values) {
	JsonSchemaObject* const object = json_schema_object_get(allow_additional_properties);

	if(object == nullptr) {
		throw std::runtime_error("JSON Schema object initialization failed");
	}

	for(auto&& value : values) {
		tstr key_moved = tstr_from_view(helpers::tstr_view_from_str(value.key));

		const auto add_result =
		    json_schema_object_add_entry(object, &key_moved, value.value.get(), value.required);
		if(!tstr_static_is_null(add_result)) {
			throw std::runtime_error(
			    std::string{ "JSON Schema object entry addition failed for key: " } +
			    string_from_tstr_static(add_result));
		}
	}

	return JsonSchemaCpp{ new_json_schema_object_rc(object) };
}

JsonSchemaCpp::JsonSchemaCpp(JsonSchema&& schema) : m_schema{ std::move(schema) } {}

// NOTE: this doesn't ref recursively, as we don't unref recursively either, so if we free a
// subvalue of this json_schema, it is prone do errors!
NODISCARD static JsonSchema json_schema_ref_increment(const JsonSchema json_schema) {
	SWITCH_JSON_SCHEMA(json_schema) {
		CASE_JSON_SCHEMA_IS_OBJECT_CONST(json_schema) {
			return new_json_schema_object_rc(object.obj);
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_ARRAY_CONST(json_schema) {
			return new_json_schema_array_rc(array.arr);
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_NUMBER() {
			return json_schema;
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_STRING_CONST(json_schema) {
			return new_json_schema_string_rc(string.str);
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_BOOLEAN() {
			return json_schema;
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_NULL() {
			return json_schema;
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_ONE_OF_CONST(json_schema) {
			return new_json_schema_one_of_rc(one_of.one_of);
		}
		VARIANT_CASE_END();
		CASE_JSON_SCHEMA_IS_LITERAL_MUT(json_schema) {
			return new_json_schema_literal_rc(literal.lit);
		}
		VARIANT_CASE_END();
		default: {
			UNREACHABLE();
		}
	}
}

[[nodiscard]] JsonSchema JsonSchemaCpp::get() const {
	// copy of this
	return json_schema_ref_increment(this->m_schema);
}

[[nodiscard]] const JsonSchema* JsonSchemaCpp::ptr() const {
	return &(this->m_schema);
}

[[nodiscard]] static JsonSchema empty_json_schema() {
	return new_json_schema_null();
}

JsonSchemaCpp::JsonSchemaCpp(JsonSchemaCpp const& json_schema) {
	this->m_schema = json_schema_ref_increment(json_schema.m_schema);
}
JsonSchemaCpp& JsonSchemaCpp::operator=(JsonSchemaCpp const& json_schema) {
	this->m_schema = json_schema_ref_increment(json_schema.m_schema);

	return *this;
}

JsonSchemaCpp::JsonSchemaCpp(JsonSchemaCpp&& json_schema) noexcept {
	this->m_schema = json_schema.m_schema;
	json_schema.m_schema = empty_json_schema();
}
JsonSchemaCpp& JsonSchemaCpp::operator=(JsonSchemaCpp&& json_schema) noexcept {
	this->m_schema = json_schema.m_schema;
	json_schema.m_schema = empty_json_schema();

	return *this;
}

JsonSchemaCpp::~JsonSchemaCpp() {
	free_json_schema(&this->m_schema);
	this->m_schema = empty_json_schema();
}

std::ostream& operator<<(std::ostream& os, const JsonSchema& json_schema) {
	auto str = json_schema_to_string(&json_schema);

	os << str;

	return os;
}

std::ostream& operator<<(std::ostream& os, const JsonSchemaCpp& json_schema) {
	auto str = json_schema_to_string(&json_schema.m_schema);

	os << str;

	return os;
}
