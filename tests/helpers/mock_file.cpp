#include "./cpp_types.hpp"

#include "./mock_file.hpp"

#include <cstdlib>
#include <string.h>
#include <unistd.h>

// don't use those here
#undef TJSON_MALLOC
#undef TJSON_CALLOC
#undef TJSON_REALLOC
#undef TJSON_FREE

TempDir::TempDir() {

	char temp_template[] = "/tmp/tmpdir.XXXXXX";
	char* dir_name = mkdtemp(temp_template);

	if(dir_name == NULL) {
		throw std::runtime_error{ std::string{ "mkdtemp failed: " } + strerror(errno) };
	}

	this->m_dir = std::make_unique<std::string>(std::string{ dir_name });
}

[[nodiscard]] const std::string& TempDir::dir() const {
	return *(this->m_dir);
}

TempDir::~TempDir() noexcept(false) {
	if(this->m_dir == nullptr) {
		return;
	}

	const auto res = rmdir(this->m_dir->c_str());

	if(res != 0) {
		throw std::runtime_error{ std::string{ "rmdir failed: " } + strerror(errno) };
	}
	this->m_dir = nullptr;
}

TempDir::TempDir(TempDir&& other) noexcept : m_dir{ std::move(other.m_dir) } {
	other.m_dir = nullptr;
}

TempDir& TempDir::TempDir::operator=(TempDir&& other) noexcept {

	this->m_dir = std::move(other.m_dir);
	other.m_dir = nullptr;

	return *this;
}

MockFile::MockFile() : m_temp_dir{} {}

[[nodiscard]] std::filesystem::path MockFile::root() const {
	return this->m_temp_dir.dir();
}

MockFile::MockFile(MockFile&& other) noexcept : m_temp_dir{ std::move(other.m_temp_dir) } {
	//
}

MockFile& MockFile::operator=(MockFile&& other) noexcept {
	this->m_temp_dir = std::move(other.m_temp_dir);

	return *this;
}

MockFile::~MockFile() noexcept(false) {
	this->m_temp_dir.~TempDir();
}

MockFileFuse::MockFileFuse(
    std::initializer_list<std::tuple<std::string, FileData, MockFlagsCpp>>&& data, bool debug)
    : m_data_c{ std::make_unique<Data>(std::move(data)) }, m_debug{ debug } {}

[[nodiscard]] std::unique_ptr<ActiveFUSE> MockFileFuse::get_fuse() const {

	auto result = create_new_fuse_file(this->root().c_str(), this->m_data_c->data(),
	                                   this->m_data_c->size(), this->m_debug);

	if(result.is_error) {
		throw std::runtime_error(std::string{ "Couldn't create fuse file: " } +
		                         string_from_tstr_static(result.data.error));
	}
	FUSEHandle* handle = result.data.ok;

	if(handle == nullptr) {
		throw std::runtime_error("Couldn't create fuse file: ok returned nullptr");
	}

	return std::make_unique<ActiveFUSE>(handle);
}

[[nodiscard]] std::unique_ptr<MockFileLock> MockFileFuse::lock() const {
	return this->get_fuse();
}

MockFileFuse::~MockFileFuse() noexcept(false) {
	if(this->m_data_c == nullptr) {
		return;
	}

	// this calls the destructor on the data
	this->m_data_c = nullptr;
}

MockFileFuse::MockFileFuse(MockFileFuse&& other) noexcept
    : m_data_c{ std::move(other.m_data_c) }, m_debug{ other.m_debug } {
	other.m_data_c = nullptr;
}

MockFileFuse& MockFileFuse::MockFileFuse::operator=(MockFileFuse&& other) noexcept {

	this->m_data_c = std::move(other.m_data_c);
	other.m_data_c = nullptr;

	this->m_debug = other.m_debug;

	return *this;
}

[[nodiscard]] static FuseFileMockFlags c_flags_from(const MockFlagsCpp& flags) {
	return FuseFileMockFlags{
		.allow_stat = flags.allow_stat,
		.allow_read = flags.allow_read,
	};
}

FuseFilesArrayC::FuseFilesArrayC(
    const std::vector<std::tuple<std::string, FileData, MockFlagsCpp>>& data)
    : m_size{ data.size() } {

	this->m_files = (FuseFile*)malloc(sizeof(FuseFile) * data.size());

	for(size_t i = 0; i < data.size(); ++i) {
		const auto& d = data.at(i);
		this->m_files[i] = FuseFile{ .name = strdup(std::get<0>(d).c_str()),
			                         .content = { .data = strdup(std::get<1>(d).c_str()),
			                                      .size = std::get<1>(d).size() },
			                         .flags = c_flags_from(std::get<2>(d)) };
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

		free((void*)f.name);
		free((void*)f.content.data);
	}

	free(this->m_files);
	this->m_files = nullptr;
}

[[nodiscard]] MockFlagsCpp MockFlagsCpp::allow_everything() {
	return MockFlagsCpp{ .allow_stat = true, .allow_read = true };
}

[[nodiscard]] MockFlagsCpp MockFlagsCpp::allow_nothing() {
	return MockFlagsCpp{ .allow_stat = false, .allow_read = false };
}

ActiveFUSE::ActiveFUSE(FUSEHandle* handle) : m_handle{ handle } {}

ActiveFUSE::ActiveFUSE(ActiveFUSE&& other) noexcept : m_handle{ std::move(other.m_handle) } {
	other.m_handle = nullptr;
}

ActiveFUSE& ActiveFUSE::operator=(ActiveFUSE&& other) noexcept {

	this->m_handle = std::move(other.m_handle);
	other.m_handle = nullptr;

	return *this;
}

ActiveFUSE::~ActiveFUSE() noexcept(false) {
	if(this->m_handle == nullptr) {
		return;
	}

	if(!clear_fuse_file(this->m_handle)) {
		throw std::runtime_error("Fuse destruction failed");
	}
	this->m_handle = nullptr;
}

MockFileLock::MockFileLock() = default;

MockFileLock::MockFileLock(MockFileLock&&) noexcept = default;
MockFileLock& MockFileLock::operator=(MockFileLock&&) noexcept = default;

MockFileLock::~MockFileLock() noexcept(false) = default;

DummyFileLock::DummyFileLock() = default;

DummyFileLock::DummyFileLock(DummyFileLock&&) noexcept = default;
DummyFileLock& DummyFileLock::operator=(DummyFileLock&&) noexcept = default;

DummyFileLock::~DummyFileLock() noexcept(false) = default;
