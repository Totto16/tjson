
#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "./fuse_impl.h"

struct TempDir {
  private:
	std::string m_dir;

  public:
	explicit TempDir();

	[[nodiscard]] const std::string& dir() const;

	TempDir(TempDir const&) = delete;
	TempDir& operator=(TempDir const&) = delete;

	TempDir(TempDir&&) noexcept;
	TempDir& operator=(TempDir&&) noexcept;

	~TempDir() noexcept(false);
};

struct MockFileSystem {
  public:
	using FileData = std::string;
	using Data = std::vector<FuseFile>;

  private:
	FUSEHandle* m_handle;
	TempDir m_temp_dir;
	std::vector<std::pair<std::string, FileData>> m_data;
	Data m_data_view_c;

  public:
	MockFileSystem(std::initializer_list<std::pair<std::string, FileData>>&&);

	[[nodiscard]] std::filesystem::path root() const;

	MockFileSystem(MockFileSystem const&) = delete;
	MockFileSystem& operator=(MockFileSystem const&) = delete;

	MockFileSystem(MockFileSystem&&) noexcept;
	MockFileSystem& operator=(MockFileSystem&&) noexcept;

	~MockFileSystem() noexcept(false);
};
