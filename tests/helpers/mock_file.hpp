
#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "./fuse/fuse_impl.h"

struct TempDir final {
  private:
	std::unique_ptr<std::string> m_dir;

  public:
	explicit TempDir();

	[[nodiscard]] const std::string& dir() const;

	TempDir(TempDir const&) = delete;
	TempDir& operator=(TempDir const&) = delete;

	TempDir(TempDir&&) noexcept;
	TempDir& operator=(TempDir&&) noexcept;

	~TempDir() noexcept(false);
};

struct MockFlagsCpp final {
	bool allow_stat;
	bool allow_read;

	[[nodiscard]] static MockFlagsCpp allow_everything();

	[[nodiscard]] static MockFlagsCpp allow_nothing();
};

struct FuseFilesArrayC final {
  public:
	using FileData = std::string;

  private:
	FuseFile* m_files;
	size_t m_size;

  public:
	FuseFilesArrayC(const std::vector<std::tuple<std::string, FileData, MockFlagsCpp>>&);

	[[nodiscard]] FuseFile* data() const;

	[[nodiscard]] size_t size() const;

	FuseFilesArrayC(FuseFilesArrayC const&) = delete;
	FuseFilesArrayC& operator=(FuseFilesArrayC const&) = delete;

	FuseFilesArrayC(FuseFilesArrayC&&) noexcept;
	FuseFilesArrayC& operator=(FuseFilesArrayC&&) noexcept;

	~FuseFilesArrayC();
};

struct MockFileLock {
  private:
  public:
	explicit MockFileLock();

	MockFileLock(MockFileLock const&) = delete;
	MockFileLock& operator=(MockFileLock const&) = delete;

	MockFileLock(MockFileLock&&) noexcept;
	MockFileLock& operator=(MockFileLock&&) noexcept;

	virtual ~MockFileLock() noexcept(false);
};

struct DummyFileLock final : MockFileLock {
  private:
  public:
	explicit DummyFileLock();

	DummyFileLock(DummyFileLock const&) = delete;
	DummyFileLock& operator=(DummyFileLock const&) = delete;

	DummyFileLock(DummyFileLock&&) noexcept;
	DummyFileLock& operator=(DummyFileLock&&) noexcept;

	virtual ~DummyFileLock() noexcept(false) override;
};

struct MockFile {
  private:
	TempDir m_temp_dir;

  public:
	MockFile();

	[[nodiscard]] std::filesystem::path root() const;

	[[nodiscard]] virtual std::unique_ptr<MockFileLock> lock() const = 0;

	MockFile(MockFile const&) = delete;
	MockFile& operator=(MockFile const&) = delete;

	MockFile(MockFile&&) noexcept;
	MockFile& operator=(MockFile&&) noexcept;

	virtual ~MockFile() noexcept(false);
};

struct ActiveFUSE final : MockFileLock {

  private:
	FUSEHandle* m_handle;

  public:
	explicit ActiveFUSE(FUSEHandle* handle);

	ActiveFUSE(ActiveFUSE const&) = delete;
	ActiveFUSE& operator=(ActiveFUSE const&) = delete;

	ActiveFUSE(ActiveFUSE&&) noexcept;
	ActiveFUSE& operator=(ActiveFUSE&&) noexcept;

	virtual ~ActiveFUSE() noexcept(false) override;
};

struct MockFileFuse final : MockFile {
  public:
	using FileData = std::string;
	using Data = FuseFilesArrayC;

  private:
	std::unique_ptr<Data> m_data_c;
	bool m_debug;

	[[nodiscard]] std::unique_ptr<ActiveFUSE> get_fuse() const;

  public:
	MockFileFuse(std::initializer_list<std::tuple<std::string, FileData, MockFlagsCpp>>&&,
	             bool debug);

	[[nodiscard]] std::unique_ptr<MockFileLock> lock() const override;

	MockFileFuse(MockFileFuse const&) = delete;
	MockFileFuse& operator=(MockFileFuse const&) = delete;

	MockFileFuse(MockFileFuse&&) noexcept;
	MockFileFuse& operator=(MockFileFuse&&) noexcept;

	~MockFileFuse() noexcept(false) override;
};
