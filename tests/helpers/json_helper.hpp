

#pragma once

#include <tjson_helper.h>

#include <_impl/utils.h>

#include <expected>
#include <filesystem>
#include <functional>
#include <ostream>

#include "./cpp_types.hpp"

namespace json::path {

struct array {
  private:
	size_t m_index;

  public:
	array(size_t index);

	[[nodiscard]] size_t index() const;

	array(array const&) = delete;
	array& operator=(array const&) = delete;

	array(array&&) noexcept;
	array& operator=(array&&) noexcept;

	~array();
};

struct object {
  private:
	tstr m_value;

  public:
	object(std::string value);
	object(const char* value);

	[[nodiscard]] const tstr* value() const;

	[[nodiscard]] tstr value_moved();

	object(object const&) = delete;
	object& operator=(object const&) = delete;

	object(object&&) noexcept;
	object& operator=(object&&) noexcept;

	~object();
};

struct dotdot {
  private:
  public:
	dotdot();

	dotdot(dotdot const&) = delete;
	dotdot& operator=(dotdot const&) = delete;

	dotdot(dotdot&&) noexcept;
	dotdot& operator=(dotdot&&) noexcept;

	~dotdot();
};

struct path {
  private:
	JsonPath* m_holder;

  public:
	path();

	[[nodiscard]] path operator/(array&& arr);

	[[nodiscard]] path operator/(object&& obj);

	[[nodiscard]] path operator/(dotdot);

	[[nodiscard]] bool is_parent_of(const path& other) const;

	[[nodiscard]] operator std::string() const;

	[[nodiscard]] std::string string() const;

	[[nodiscard]] std::string string(JsonPathStringOptions options) const;

	[[nodiscard]] bool operator==(const path& other) const;

	[[nodiscard]] bool operator==(const JsonPath* other) const;

	std::ostream& operator<<(std::ostream& os) const;

	friend std::ostream& operator<<(std::ostream& os, const path& path);

	path(path const&) = delete;
	path& operator=(path const&) = delete;

	path(path&&) noexcept;
	path& operator=(path&&) noexcept;

	~path();
};

std::ostream& operator<<(std::ostream& os, const path& path);

} // namespace json::path

struct JsonIterateErrorCpp {
  private:
	std::string m_err;
	json::path::path m_path;

  public:
	JsonIterateErrorCpp(const char* err, json::path::path&& path);
	JsonIterateErrorCpp(std::string err, json::path::path&& path);

	JsonIterateErrorCpp(JsonIterateErrorCpp const&) = delete;
	JsonIterateErrorCpp& operator=(JsonIterateErrorCpp const&) = delete;

	JsonIterateErrorCpp(JsonIterateErrorCpp&&) noexcept;
	JsonIterateErrorCpp& operator=(JsonIterateErrorCpp&&) noexcept;

	~JsonIterateErrorCpp();

	[[nodiscard]] const std::string& err() const;

	[[nodiscard]] const json::path::path& path() const;
	

	std::ostream& operator<<(std::ostream& os) const;

	friend std::ostream& operator<<(std::ostream& os, const JsonIterateErrorCpp& error);
};

std::ostream& operator<<(std::ostream& os, const JsonIterateErrorCpp& error);

std::ostream& operator<<(std::ostream& os, const JsonIterateError& error);

[[nodiscard]] bool operator==(const JsonIterateError& err1, const JsonIterateErrorCpp& err2);

[[nodiscard]] bool operator==(const IsNotError& error, const JsonIterateResult& result);

std::ostream& operator<<(std::ostream& os, const JsonIterateResult& result);
