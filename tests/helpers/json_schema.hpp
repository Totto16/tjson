

#pragma once

#include <tjson_schema.h>

#include <_impl/utils.h>

#include <functional>
#include <ostream>

struct JsonSchemaCpp final {
  private:
	JsonSchema m_schema;

  public:
	explicit JsonSchemaCpp(JsonSchema&& schema);

	[[nodiscard]] JsonSchema get() const;

	[[nodiscard]] const JsonSchema* ptr() const;

	JsonSchemaCpp(JsonSchemaCpp const&);
	JsonSchemaCpp& operator=(JsonSchemaCpp const&);

	JsonSchemaCpp(JsonSchemaCpp&&) noexcept;
	JsonSchemaCpp& operator=(JsonSchemaCpp&&) noexcept;

	~JsonSchemaCpp();

	friend std::ostream& operator<<(std::ostream& os, const JsonSchemaCpp& json_schema);
};

std::ostream& operator<<(std::ostream& os, const JsonSchemaCpp& json_schema);

template <typename T> struct JsonSchemaBuilderGeneric {
  private:
	T* m_value;
	std::function<void(T*)> m_free_fn;

	// virtual functions

	[[nodiscard]] virtual JsonSchema to_schema(T* const value) const = 0;

  protected:
	JsonSchemaBuilderGeneric(T* raw_value, std::function<void(T*)> free_fn)
	    : m_value{ raw_value }, m_free_fn{ free_fn } {
		if(raw_value == nullptr) {
			throw std::runtime_error("Invalid nullptr received");
		}
	}

	[[nodiscard]] T* value() const {
		if(this->m_value == nullptr) {
			throw std::runtime_error("Invalid get of empty schema builder");
		}

		return this->m_value;
	}

  public:
	[[nodiscard]] JsonSchemaCpp get() {
		JsonSchema schema = this->to_schema(this->m_value);
		this->m_value = nullptr;
		// call eventual parent destructors, as they might have some things to free too, the
		// deconstructor for this class doesn't do anything, as m_value is NULL
		this->~JsonSchemaBuilderGeneric();
		return JsonSchemaCpp{ std::move(schema) };
	}

	JsonSchemaBuilderGeneric(JsonSchemaBuilderGeneric const&) = delete;
	JsonSchemaBuilderGeneric& operator=(JsonSchemaBuilderGeneric const&) = delete;

	JsonSchemaBuilderGeneric(JsonSchemaBuilderGeneric&&) noexcept = delete;
	JsonSchemaBuilderGeneric& operator=(JsonSchemaBuilderGeneric&&) noexcept = delete;

	virtual ~JsonSchemaBuilderGeneric() {
		if(this->m_value == nullptr) {
			return;
		}

		this->m_free_fn(this->m_value);

		this->m_value = nullptr;
	}
};

struct JsonSchemaStringBuilder final : JsonSchemaBuilderGeneric<JsonSchemaString> {
  private:
	// virtual functions

	[[nodiscard]] JsonSchema to_schema(JsonSchemaString* const value) const override;

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

  public:
	JsonSchemaArrayBuilder(const JsonSchemaCpp& items, bool require_unique_items);

	[[nodiscard]] JsonSchemaArrayBuilder& min(size_t value);

	[[nodiscard]] JsonSchemaArrayBuilder& max(size_t value);
};

namespace json_schema {

struct JsonSchemaObjectEntryCpp {
	std::string key;
	JsonSchemaCpp value;
	bool required;
};

[[nodiscard]] JsonSchemaStringBuilder string();

[[nodiscard]] JsonSchemaArrayBuilder array(const JsonSchemaCpp& items, bool require_unique_items);

[[nodiscard]] JsonSchemaCpp literal(std::string&& str);

[[nodiscard]] JsonSchemaCpp number();

[[nodiscard]] JsonSchemaCpp boolean();

[[nodiscard]] JsonSchemaCpp null();

[[nodiscard]] JsonSchemaCpp one_of(std::initializer_list<JsonSchemaCpp>&& values);

[[nodiscard]] JsonSchemaCpp object(bool allow_additional_properties,
                                   std::initializer_list<JsonSchemaObjectEntryCpp>&& values);

}; // namespace json_schema

[[nodiscard]] bool operator==(const JsonSchema& json_schema1, const JsonSchema& json_schema2);

std::ostream& operator<<(std::ostream& os, const JsonSchema& json_schema);
