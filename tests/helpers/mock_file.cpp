#include "./cpp_types.hpp"

#include "./mock_file.hpp"

#include <cstdlib>
#include <string.h>
#include <unistd.h>

TempDir::TempDir() {

	char temp_template[] = "/tmp/tmpdir.XXXXXX";
	char* dir_name = mkdtemp(temp_template);

	if(dir_name == NULL) {
		throw std::runtime_error{ std::string{ "mkdtemp failed: " } + strerror(errno) };
	}

	this->m_dir = std::string{ dir_name };
}

[[nodiscard]] std::filesystem::path TempDir::dir() const {
	return this->m_dir;
}

TempDir::~TempDir() noexcept(false) {
	if(this->m_dir.empty()) {
		return;
	}

	const auto res = rmdir(this->m_dir.string().c_str());

	if(res != 0) {
		throw std::runtime_error{ std::string{ "rmdir failed: " } + strerror(errno) };
	}
	this->m_dir = "";
}

TempDir::TempDir(TempDir&& other) noexcept : m_dir{ other.m_dir } {
	other.m_dir = "";
}

TempDir& TempDir::TempDir::operator=(TempDir&& other) noexcept {

	this->m_dir = other.m_dir;
	other.m_dir = "";

	return *this;
}

MockFile::MockFile(MockFile::Data&& data)
    : m_handle{ nullptr }, m_temp_dir{}, m_temp_file{}, m_data{ std::move(data) } {

	this->m_temp_file = (this->m_temp_dir.dir() / "fuse_file").string();

	auto result = create_new_fuse_file(this->m_temp_file.c_str(), m_data.c_str(), m_data.size());

	if(result.is_error) {
		throw std::runtime_error(std::string{ "Couldn't create fuse file: " } +
		                         string_from_tstr_static(result.data.error));
	}

	this->m_handle = result.data.ok;

	if(this->m_handle == nullptr) {
		throw std::runtime_error("Couldn't create fuse file: ok returned nullptr");
	}
}

[[nodiscard]] std::filesystem::path MockFile::file_path() const {
	return std::filesystem::path{ this->m_temp_file };
}

MockFile::~MockFile() noexcept(false) {
	if(this->m_handle == nullptr) {
		return;
	}
	if(!clear_fuse_file(this->m_handle)) {
		throw std::runtime_error("Fuse destruction failed");
	}

	this->m_handle = nullptr;

	this->m_temp_dir.~TempDir();
}

MockFile::MockFile(MockFile&& other) noexcept
    : m_handle{ other.m_handle }, m_temp_dir{ std::move(other.m_temp_dir) },
      m_temp_file{ std::move(other.m_temp_file) }, m_data{ std::move(other.m_data) } {
	other.m_handle = nullptr;
}

MockFile& MockFile::MockFile::operator=(MockFile&& other) noexcept {

	this->m_handle = other.m_handle;
	other.m_handle = nullptr;

	this->m_temp_dir = std::move(other.m_temp_dir);
	this->m_temp_file = std::move(other.m_temp_file);
	this->m_data = std::move(other.m_data);

	return *this;
}
