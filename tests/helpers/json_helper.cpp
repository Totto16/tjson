
#include "./json_helper.hpp"

#include "./cpp_types.hpp"
#include "./generic.hpp"
#include "./rtti.hpp"

json::path::array::array(size_t index) : m_index{ index } {}

[[nodiscard]] size_t json::path::array::index() const {
	return this->m_index;
}

json::path::array::array(array&& other) noexcept {
	this->m_index = other.m_index;
}

json::path::array& json::path::array::operator=(array&& other) noexcept {
	this->m_index = other.m_index;
	return *this;
}

json::path::array::~array() {
	// noop
}

json::path::object::object(std::string value)
    : m_value{ tstr_from_len(value.c_str(), value.size()) } {}

json::path::object::object(const char* value) : m_value{ tstr_from(value) } {}

[[nodiscard]] const tstr* json::path::object::value() const {
	return &(this->m_value);
}

[[nodiscard]] tstr json::path::object::value_moved() {
	tstr value = this->m_value;
	this->m_value = tstr_null();

	return value;
}

json::path::object::object(object&& other) noexcept {
	this->m_value = other.m_value;
	other.m_value = tstr_null();
}

json::path::object& json::path::object::operator=(object&& other) noexcept {
	this->m_value = other.m_value;
	other.m_value = tstr_null();

	return *this;
}

json::path::object::~object() {
	if(tstr_is_null(&(this->m_value))) {
		return;
	}

	tstr_free(&(this->m_value));
	this->m_value = tstr_null();
}

json::path::dotdot::dotdot() {}

json::path::dotdot::dotdot(dotdot&&) noexcept = default;
json::path::dotdot& json::path::dotdot::operator=(dotdot&&) noexcept = default;

json::path::dotdot::~dotdot() = default;

json::path::path::path() : m_holder{ json_path_get_root() } {
	if(this->m_holder == nullptr) {
		throw std::runtime_error("OOM");
	}
}

json::path::path json::path::path::operator/(array&& arr) {
	bool ok = json_path_add_array_index(this->m_holder, arr.index());

	if(!ok) {
		throw std::runtime_error("array add error");
	}

	return std::move(*this);
}

json::path::path json::path::path::operator/(object&& obj) {

	tstr val = obj.value_moved();

	bool ok = json_path_add_object_key_moved(this->m_holder, TJSON_MOVE(tstr, val));

	if(!ok) {
		throw std::runtime_error("object add error");
	}

	return std::move(*this);
}

json::path::path json::path::path::operator/(dotdot) {
	bool ok = json_path_remove_last_segment(this->m_holder);

	if(!ok) {
		throw std::runtime_error("remove last segment error");
	}

	return std::move(*this);
}

[[nodiscard]] bool json::path::path::is_parent_of(const path& other) const {
	return json_path_is_parent_of(this->m_holder, other.m_holder);
}

[[nodiscard]] json::path::path::operator std::string() const {
	return this->string();
}

[[nodiscard]] std::string json::path::path::string() const {
	tstr path_str = json_path_to_str(this->m_holder);

	if(tstr_is_null(&path_str)) {
		throw std::runtime_error("error in formating path");
	}

	std::string str = string_from_tstr(path_str);

	tstr_free(&path_str);

	return str;
}

[[nodiscard]] std::string json::path::path::string(JsonPathStringOptions options) const {
	tstr path_str = json_path_to_str_advanced(this->m_holder, options);

	if(tstr_is_null(&path_str)) {
		throw std::runtime_error("error in formating path");
	}

	std::string str = string_from_tstr(path_str);

	tstr_free(&path_str);

	return str;
}

[[nodiscard]] bool json::path::path::operator==(const path& other) const {
	return json_path_is_eq(this->m_holder, other.m_holder);
}

[[nodiscard]] bool json::path::path::operator==(const JsonPath* other) const {
	return json_path_is_eq(this->m_holder, other);
}

json::path::path::path(path&& other) noexcept {
	this->m_holder = other.m_holder;
	other.m_holder = nullptr;
}

json::path::path& json::path::path::operator=(path&& other) noexcept {
	this->m_holder = other.m_holder;
	other.m_holder = nullptr;
	return *this;
}

json::path::path::~path() {
	if(this->m_holder == nullptr) {
		return;
	}

	free_json_path(this->m_holder);
	this->m_holder = nullptr;
}

std::ostream& json::path::path::operator<<(std::ostream& os) const {

	tstr path_str = json_path_to_str(this->m_holder);

	if(tstr_is_null(&path_str)) {
		throw std::runtime_error("error in formating path");
	}

	os << path_str;

	tstr_free(&path_str);

	return os;
}

std::ostream& json::path::operator<<(std::ostream& os, const json::path::path& path) {

	return path.operator<<(os);
}

[[nodiscard]] bool operator==(const IsNotError& /* error */, const JsonIterateResult& result) {
	IF_JSON_ITERATE_RESULT_IS_ERROR_IGN(result) {
		return false;
	}

	return true;
}

std::ostream& operator<<(std::ostream& os, const JsonIterateResult& result) {

	IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(result) {

		os << "JsonIterateResult::error -> " << error.error;

		return os;
	}

	IF_JSON_ITERATE_RESULT_IS_OK_CONST(result) {
		os << "JsonIterateResult::ok -> " << ok;

		return os;
	}

	return os;
}

JsonIterateErrorCpp::JsonIterateErrorCpp(const char* err, json::path::path&& path)
    : m_err{ err }, m_path(std::move(path)) {}

JsonIterateErrorCpp::JsonIterateErrorCpp(std::string err, json::path::path&& path)
    : m_err{ std::move(err) }, m_path(std::move(path)) {}

JsonIterateErrorCpp::JsonIterateErrorCpp(JsonIterateErrorCpp&& other) noexcept = default;

JsonIterateErrorCpp& JsonIterateErrorCpp::operator=(JsonIterateErrorCpp&& other) noexcept = default;

JsonIterateErrorCpp::~JsonIterateErrorCpp() = default;

[[nodiscard]] const std::string& JsonIterateErrorCpp::err() const {
	return this->m_err;
}

[[nodiscard]] const json::path::path& JsonIterateErrorCpp::path() const {
	return this->m_path;
}

std::ostream& JsonIterateErrorCpp::operator<<(std::ostream& os) const {
	os << this->m_err << " at " << this->m_path;

	return os;
}

std::ostream& operator<<(std::ostream& os, const JsonIterateErrorCpp& error) {

	return error.operator<<(os);
}

std::ostream& operator<<(std::ostream& os, const JsonIterateError& error) {

	tstr path_str = json_path_to_str(error.path);

	if(tstr_is_null(&path_str)) {
		throw std::runtime_error("error in formating path");
	}

	os << error.err << " at " << path_str;

	tstr_free(&path_str);

	return os;
}

[[nodiscard]] bool operator==(const JsonIterateError& err1, const JsonIterateErrorCpp& err2) {

	if(err1.err != err2.err()) {
		return false;
	}

	return (err1.path == err2.path());
}
