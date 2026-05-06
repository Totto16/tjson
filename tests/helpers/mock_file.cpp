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

[[nodiscard]] const std::string& TempDir::dir() const {
	return this->m_dir;
}

TempDir::~TempDir() noexcept(false) {
	if(this->m_dir.empty()) {
		return;
	}

	const auto res = rmdir(this->m_dir.c_str());

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

MockFileSystem::MockFileSystem(std::initializer_list<std::pair<std::string, FileData>>&& data_cpp)
    : m_handle{ nullptr }, m_temp_dir{}, m_data{ std::move(data_cpp) }, m_data_view_c{} {

	this->m_data_view_c = std::vector<FuseFile>{};
	this->m_data_view_c.reserve(data_cpp.size());

	for(const auto& data : this->m_data) {
		this->m_data_view_c.emplace_back(
		    FuseFile{ .name = data.first.c_str(),
		              .content = { .data = data.second.c_str(), .size = data.second.size() } });
	}

	auto result = create_new_fuse_file(this->m_temp_dir.dir().c_str(), this->m_data_view_c.data(),
	                                   this->m_data_view_c.size());

	if(result.is_error) {
		throw std::runtime_error(std::string{ "Couldn't create fuse file: " } +
		                         string_from_tstr_static(result.data.error));
	}

	this->m_handle = result.data.ok;

	if(this->m_handle == nullptr) {
		throw std::runtime_error("Couldn't create fuse file: ok returned nullptr");
	}
}

[[nodiscard]] std::filesystem::path MockFileSystem::root() const {
	return this->m_temp_dir.dir();
}

MockFileSystem::~MockFileSystem() noexcept(false) {
	if(this->m_handle == nullptr) {
		return;
	}
	if(!clear_fuse_file(this->m_handle)) {
		throw std::runtime_error("Fuse destruction failed");
	}

	this->m_handle = nullptr;

	this->m_temp_dir.~TempDir();
}

MockFileSystem::MockFileSystem(MockFileSystem&& other) noexcept
    : m_handle{ other.m_handle }, m_temp_dir{ std::move(other.m_temp_dir) },
      m_data{ std::move(other.m_data) }, m_data_view_c{ std::move(other.m_data_view_c) } {
	other.m_handle = nullptr;
}

MockFileSystem& MockFileSystem::MockFileSystem::operator=(MockFileSystem&& other) noexcept {

	this->m_handle = other.m_handle;
	other.m_handle = nullptr;

	this->m_temp_dir = std::move(other.m_temp_dir);
	this->m_data = std::move(other.m_data);
	this->m_data_view_c = std::move(other.m_data_view_c);

	return *this;
}
