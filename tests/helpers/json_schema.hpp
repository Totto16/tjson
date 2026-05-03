

#pragma once

#include <tjson_schema.h>

#include <_impl/utils.h>

#include <ostream>

template <typename T> struct JsonSchemaBuilderGeneric {
  private:
	T* m_value;

	// virtual functions

	[[nodiscard]] virtual JsonSchema to_schema(T* const value) const = 0;

	virtual void free_value(T* const value) const = 0;

  protected:
	JsonSchemaBuilderGeneric(T* raw_value) : m_value{ raw_value } {}

  public:
	[[nodiscard]] JsonSchema get() {
		JsonSchema schema = this->to_schema();
		this->~JsonSchemaBuilderGeneric();
		return schema;
	}

	JsonSchemaBuilderGeneric(JsonSchemaBuilderGeneric const&) = delete;
	JsonSchemaBuilderGeneric& operator=(JsonSchemaBuilderGeneric const&) = delete;

	JsonSchemaBuilderGeneric(JsonSchemaBuilderGeneric&&) noexcept = delete;
	JsonSchemaBuilderGeneric& operator=(JsonSchemaBuilderGeneric&&) noexcept = delete;

	~JsonSchemaBuilderGeneric() {
		if(this->m_value == nullptr) {
			return;
		}

		this->free_value();

		this->m_value = nullptr;
	}
};

struct JsonSchemaStringBuilder final : JsonSchemaBuilderGeneric<JsonSchemaString> {
  private:
	// virtual functions

	[[nodiscard]] JsonSchema to_schema(JsonSchemaString* const value) const override;

	void free_value(JsonSchemaString* const value) const override;

  public:
	JsonSchemaStringBuilder();

	[[nodiscard]] JsonSchemaStringBuilder& nonempty();

	[[nodiscard]] JsonSchemaStringBuilder& min(size_t value);

	[[nodiscard]] JsonSchemaStringBuilder& max(size_t value);

	[[nodiscard]] JsonSchemaStringBuilder& regex(const std::string& str);
};

struct JsonSchemaArrayBuilder final : JsonSchemaBuilderGeneric<JsonSchemaArray> {
  private:
	// virtual functions

	[[nodiscard]] JsonSchema to_schema(JsonSchemaArray* const value) const override;

	void free_value(JsonSchemaArray* const value) const override;

  public:
	JsonSchemaArrayBuilder(JsonSchema items, bool require_unique_items);

	[[nodiscard]] JsonSchemaArrayBuilder& min(size_t value);

	[[nodiscard]] JsonSchemaArrayBuilder& max(size_t value);
};

struct JsonSchemaCpp {
	// static helper fn's

	[[nodiscard]] static JsonSchemaStringBuilder string();

	[[nodiscard]] static JsonSchemaArrayBuilder array(JsonSchema items, bool require_unique_items);

	[[nodiscard]] static JsonSchema literal(std::string&& str);

	[[nodiscard]] static JsonSchema number();

	[[nodiscard]] static JsonSchema boolean();

	[[nodiscard]] static JsonSchema null();

	[[nodiscard]] static JsonSchema one_of(std::initializer_list<JsonSchema>&& values);

	[[nodiscard]] static JsonSchema
	object(bool allow_additional_properties,
	       std::initializer_list<std::pair<std::string, JsonSchema>>&& values);

	friend std::ostream& operator<<(std::ostream& os, const JsonSchemaCpp& json_error);
};

[[nodiscard]] bool operator==(const JsonSchema& json_schema1, const JsonSchema& json_schema2);

std::ostream& operator<<(std::ostream& os, const JsonSchema& json_schema);
