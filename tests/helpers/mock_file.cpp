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

MockFileSystem::MockFileSystem(std::initializer_list<std::pair<std::string, FileData>>&& data)
    : m_handle{ nullptr }, m_temp_dir{}, m_data_c{ std::move(data) } {

	auto result = create_new_fuse_file(this->m_temp_dir.dir().c_str(), this->m_data_c.data(),
	                                   this->m_data_c.size());

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

	this->m_data_c.~FuseFilesArrayC();

	this->m_temp_dir.~TempDir();
}

MockFileSystem::MockFileSystem(MockFileSystem&& other) noexcept
    : m_handle{ other.m_handle }, m_temp_dir{ std::move(other.m_temp_dir) },
      m_data_c{ std::move(other.m_data_c) } {
	other.m_handle = nullptr;
}

MockFileSystem& MockFileSystem::MockFileSystem::operator=(MockFileSystem&& other) noexcept {

	this->m_handle = other.m_handle;
	other.m_handle = nullptr;

	this->m_temp_dir = std::move(other.m_temp_dir);
	this->m_data_c = std::move(other.m_data_c);

	return *this;
}

FuseFilesArrayC::FuseFilesArrayC(const std::vector<std::pair<std::string, FileData>>& data)
    : m_size{ data.size() } {

	this->m_files = (FuseFile*)TJSON_MALLOC(sizeof(FuseFile) * data.size());

	for(size_t i = 0; i < data.size(); ++i) {
		const auto& d = data.at(i);
		this->m_files[i] =
		    FuseFile{ .name = strdup(d.first.c_str()),
			          .content = { .data = strdup(d.second.c_str()), .size = d.second.size() } };
	}
}

[[nodiscard]] FuseFile* FuseFilesArrayC::data() const {
	return this->m_files;
}

[[nodiscard]] size_t FuseFilesArrayC::size() const {
	return this->m_size;
}

FuseFilesArrayC::FuseFilesArrayC(FuseFilesArrayC&& other) noexcept
    : m_files{ other.m_files }, m_size{ std::move(other.m_size) } {
	other.m_files = nullptr;
}
FuseFilesArrayC& FuseFilesArrayC::operator=(FuseFilesArrayC&& other) noexcept {
	this->m_files = other.m_files;
	other.m_files = nullptr;

	this->m_size = std::move(other.m_size);

	return *this;
}

FuseFilesArrayC::~FuseFilesArrayC() {
	if(this->m_files == nullptr) {
		return;
	}

	for(size_t i = 0; i < this->m_size; ++i) {
		const auto& f = this->m_files[i];

		TJSON_FREE((void*)f.name);
		TJSON_FREE((void*)f.content.data);
	}

	TJSON_FREE(this->m_files);
	this->m_files = nullptr;
}
