
#pragma once

#include <filesystem>

struct TempDir {
  private:
	std::filesystem::path m_dir;

  public:
	explicit TempDir();

	[[nodiscard]] std::filesystem::path dir() const;

	TempDir(TempDir const&) = delete;
	TempDir& operator=(TempDir const&) = delete;

	TempDir(TempDir&&) noexcept = delete;
	TempDir& operator=(TempDir&&) noexcept = delete;

	~TempDir() noexcept(false);
};

using FUSEHandle = struct FUSEHandle;

struct MockFile {
  private:
	FUSEHandle* m_handle;
	TempDir m_temp_dir;
    std::string m_temp_file;

    public:
	explicit MockFile();

	[[nodiscard]] std::filesystem::path file_path() const;

	MockFile(MockFile const&) = delete;
	MockFile& operator=(MockFile const&) = delete;

	MockFile(MockFile&&) noexcept = delete;
	MockFile& operator=(MockFile&&) noexcept = delete;

	~MockFile() noexcept(false);
};
