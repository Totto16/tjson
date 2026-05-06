
#include "./mock_file.hpp"

#include <cstdlib>
#include <string.h>
#include <unistd.h>

#include <allocator.h>

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
	const auto res = rmdir(this->m_dir.string().c_str());

	if(res != 0) {
		throw std::runtime_error{ std::string{ "rmdir failed: " } + strerror(errno) };
	}
}

[[nodiscard]] static FUSEHandle* create_new_fuse_file(const char* file);

static void clear_fuse_file(FUSEHandle* handle);

MockFile::MockFile() : m_handle{ nullptr }, m_temp_dir{}, m_temp_file{} {

	this->m_temp_file = (this->m_temp_dir.dir() / "fuse_file").string();

	this->m_handle = create_new_fuse_file(this->m_temp_file.c_str());

	if(this->m_handle == nullptr) {
		throw std::runtime_error("Couldn't create fuse file");
	}
}

[[nodiscard]] std::filesystem::path MockFile::file_path() const {
	return std::filesystem::path{ this->m_temp_file };
}

MockFile::~MockFile() noexcept(false) {
	if(this->m_handle == nullptr) {
		return;
	}
	clear_fuse_file(this->m_handle);
	this->m_handle = nullptr;

	this->m_temp_dir.~TempDir();
}

#define FUSE_USE_VERSION 31

#include <fuse.h>

struct FUSEHandle {
	const char* file_path;
};

[[nodiscard]] static FUSEHandle* create_new_fuse_file(const char* const file) {

	FUSEHandle* handle = (FUSEHandle*)TJSON_MALLOC(sizeof(FUSEHandle));

	handle->file_path = file;

	// TODO

	return handle;
}

static void clear_fuse_file(FUSEHandle* const handle) {

	// TODO

	TJSON_FREE(handle);
}
