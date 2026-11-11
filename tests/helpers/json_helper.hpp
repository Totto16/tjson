

#pragma once

#include <tjson_helper.h>

#include <_impl/utils.h>

#include <expected>
#include <filesystem>
#include <functional>
#include <ostream>

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
	object(tstr value);

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

	path operator/(array&& arr);

	path operator/(object&& obj);

	path operator/(dotdot);

	[[nodiscard]] bool operator==(const path& other) const;

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
