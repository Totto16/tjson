
#pragma once

#include <filesystem>

#include "./fuse_impl.h"

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

struct MockFile {
  public:
	using Data = std::string;

  private:
	FUSEHandle* m_handle;
	TempDir m_temp_dir;
	std::string m_temp_file;
	Data m_data;

  public:
	explicit MockFile(Data&& data);

	[[nodiscard]] std::filesystem::path file_path() const;

	MockFile(MockFile const&) = delete;
	MockFile& operator=(MockFile const&) = delete;

	MockFile(MockFile&&) noexcept = delete;
	MockFile& operator=(MockFile&&) noexcept = delete;

	~MockFile() noexcept(false);
};
