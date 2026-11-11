#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include "helpers/allocators.hpp"
#include "helpers/mock.hpp"
#include "helpers/string_maker.hpp"

#ifndef TJSON_HAVE_FILE_RELATED_64_FUNCTIONS
	#error "TODO: remove the ue of file related 64 functions when this is not defined"
#endif

static void copy_statbuf64_to_statbuf(struct stat64* statbuf64, struct stat* statbuf) {

	statbuf->st_dev = statbuf64->st_dev;
	statbuf->st_ino = statbuf64->st_ino;
	statbuf->st_mode = statbuf64->st_mode;
	statbuf->st_nlink = statbuf64->st_nlink;
	statbuf->st_uid = statbuf64->st_uid;
	statbuf->st_gid = statbuf64->st_gid;
	statbuf->st_rdev = statbuf64->st_rdev;
	statbuf->st_size = statbuf64->st_size;
	statbuf->st_blksize = statbuf64->st_blksize;
	statbuf->st_blocks = statbuf64->st_blocks;
	statbuf->st_atime = statbuf64->st_atime;
	statbuf->st_mtime = statbuf64->st_mtime;
	statbuf->st_ctime = statbuf64->st_ctime;
}

TEST_SUITE_BEGIN("json_mock" * doctest::description("json mock tests") * doctest::timeout(2.0));

TEST_CASE("testing mock behaviour of json functions <json_mock_tester>") {

	SUBCASE("testing custom open wrap (variadic)") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.open();

			auto used_handle = handle.use([](const char* path, int flags, mode_t mode) -> int {
				UNUSED(path);
				if(flags & O_CREAT) {
					return -((int)mode);
				}

				return -1 + (int)mode;
			});

			int res = wrapped_fn_open("test", O_WRONLY);

			REQUIRE_EQ(res, -1);

			int res2 = wrapped_fn_open("test", O_CREAT, 17);

			REQUIRE_EQ(res2, -17);

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}

	SUBCASE("testing path reading: fstat fails") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [](int fd) -> int {
				UNUSED(fd);

				errno = EBADF;
				return -1;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd);
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd);
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonErrorCpp expected_error =
				    JsonErrorCpp::with_file_loc("Couldn't get stats of the file", &file_path,
				                                JsonSourcePosition{ .line = 0, .col = 0 });

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeError);

				JsonError result = json_parse_result_get_as_error(parse_result);

				const auto actual_error = JsonErrorCpp{ result };

				REQUIRE_EQ(actual_error, expected_error);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);
		}();
	}

	SUBCASE("testing path reading: fstat returns wrong size") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = -1;

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonErrorCpp expected_error =
				    JsonErrorCpp::with_file_loc("Couldn't get the file size", &file_path,
				                                JsonSourcePosition{ .line = 0, .col = 0 });

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeError);

				JsonError result = json_parse_result_get_as_error(parse_result);

				const auto actual_error = JsonErrorCpp{ result };

				REQUIRE_EQ(actual_error, expected_error);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);
		}();
	}

	SUBCASE("testing path reading: alloc fails") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto mock_allocator = mock::CMockAllocator::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [&mock_allocator](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = 42;
				const bool mock_res = mock_allocator.malloc().always_fail();
				REQUIRE_TRUE(mock_res);

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonErrorCpp expected_error =
				    JsonErrorCpp::with_file_loc("Couldn't allocate file data", &file_path,
				                                JsonSourcePosition{ .line = 0, .col = 0 });

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeError);

				JsonError result = json_parse_result_get_as_error(parse_result);

				const auto actual_error = JsonErrorCpp{ result };

				REQUIRE_EQ(actual_error, expected_error);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);
		}();
	}

	SUBCASE("testing path reading: read returns to few data") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			std::string file_data = R"({"hello": null})";

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [&file_data](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = static_cast<off64_t>(file_data.size());

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			auto& read_handle = mock_general.read();

			size_t data_to_read = file_data.size();

			auto used_read_handle = read_handle.use(
			    [&file_data, &data_to_read](int fd, void* buf, size_t count) -> ssize_t {
				    UNUSED(fd);

				    if(buf == NULL) {
					    errno = EINVAL;
					    return -1;
				    }

				    size_t copy_count = count;

				    if(copy_count > (data_to_read - 1)) {
					    copy_count = data_to_read - 1;
				    }

				    const size_t offset = file_data.size() - data_to_read;

				    memcpy(buf, file_data.data() + offset, copy_count);
				    data_to_read -= copy_count;

				    return (ssize_t)copy_count;
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonErrorCpp expected_error = JsonErrorCpp::with_file_loc(
				    "Read to few data", &file_path, JsonSourcePosition{ .line = 0, .col = 0 });

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeError);

				JsonError result = json_parse_result_get_as_error(parse_result);

				const auto actual_error = JsonErrorCpp{ result };

				REQUIRE_EQ(actual_error, expected_error);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);

			const size_t read_count = used_read_handle.count();

			REQUIRE_EQ(read_count, 2);

			REQUIRE_EQ(data_to_read, 1);
		}();
	}

	SUBCASE("testing path reading: read returns to much data") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			std::string file_data = R"({"hello": null})";

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [&file_data](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = static_cast<off64_t>(file_data.size());

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			auto& read_handle = mock_general.read();

			size_t data_to_read = file_data.size();

			auto used_read_handle = read_handle.use(
			    [&file_data, &data_to_read](int fd, void* buf, size_t count) -> ssize_t {
				    UNUSED(fd);

				    if(buf == NULL) {
					    errno = EINVAL;
					    return -1;
				    }

				    size_t copy_count = count;

				    if(copy_count > data_to_read) {
					    copy_count = data_to_read;
				    }

				    if(copy_count == 0) {
					    // we are EOF, but we have more data, LOL

					    memcpy(buf, "oh", 2);

					    return (ssize_t)2;
				    }

				    const size_t offset = file_data.size() - data_to_read;

				    memcpy(buf, file_data.data() + offset, copy_count);
				    data_to_read -= copy_count;

				    return (ssize_t)copy_count;
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonErrorCpp expected_error = JsonErrorCpp::with_file_loc(
				    "File size was wrong, we expected EOF not more data", &file_path,
				    JsonSourcePosition{ .line = 0, .col = 0 });

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeError);

				JsonError result = json_parse_result_get_as_error(parse_result);

				const auto actual_error = JsonErrorCpp{ result };

				REQUIRE_EQ(actual_error, expected_error);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);

			const size_t read_count = used_read_handle.count();

			REQUIRE_EQ(read_count, 2);

			REQUIRE_EQ(data_to_read, 0);
		}();
	}

	SUBCASE("testing path reading: read fails (EINTR)") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			std::string file_data = R"({"hello": null})";

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [&file_data](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = static_cast<off64_t>(file_data.size());

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			bool read_failed = false;

			auto& read_handle = mock_general.read();

			size_t data_to_read = file_data.size();

			auto used_read_handle =
			    read_handle.use([&read_failed, &file_data, &data_to_read](int fd, void* buf,
			                                                              size_t count) -> ssize_t {
				    UNUSED(fd);

				    if(buf == NULL) {
					    errno = EINVAL;
					    return -1;
				    }

				    if(!read_failed) {
					    read_failed = !read_failed;
					    errno = EINTR;
					    return -1;
				    }

				    size_t copy_count = count;

				    if(copy_count > data_to_read) {
					    copy_count = data_to_read;
				    }

				    const size_t offset = file_data.size() - data_to_read;

				    memcpy(buf, file_data.data() + offset, copy_count);
				    data_to_read -= copy_count;

				    return (ssize_t)copy_count;
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonValue expected_value = json::object({ { "hello", json::null() } });

				CAutoFreePtr<JsonValue> defer_expected_value = { &expected_value, free_json_value };

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

				JsonValue result = json_parse_result_get_as_ok(parse_result);

				CAutoFreePtr<JsonValue> defer_result = { &result, free_json_value };

				REQUIRE_EQ(result, expected_value);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);

			const size_t read_count = used_read_handle.count();

			REQUIRE_EQ(read_count, 3);

			REQUIRE_EQ(data_to_read, 0);
		}();
	}

	SUBCASE("testing path reading: read fails (takes two iterations)") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			std::string file_data = R"({"hello": null})";

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [&file_data](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = static_cast<off64_t>(file_data.size());

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			bool first_read = true;

			auto& read_handle = mock_general.read();

			size_t data_to_read = file_data.size();

			auto used_read_handle =
			    read_handle.use([&first_read, &file_data, &data_to_read](int fd, void* buf,
			                                                             size_t count) -> ssize_t {
				    UNUSED(fd);

				    if(buf == NULL) {
					    errno = EINVAL;
					    return -1;
				    }

				    size_t copy_count = count;

				    if(copy_count > data_to_read) {
					    copy_count = data_to_read;
				    }

				    if(first_read) {
					    first_read = !first_read;
					    copy_count = copy_count / 2;
				    }

				    const size_t offset = file_data.size() - data_to_read;

				    memcpy(buf, file_data.data() + offset, copy_count);
				    data_to_read -= copy_count;

				    return (ssize_t)copy_count;
			    });

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonValue expected_value = json::object({ { "hello", json::null() } });

				CAutoFreePtr<JsonValue> defer_expected_value = { &expected_value, free_json_value };

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeOk);

				JsonValue result = json_parse_result_get_as_ok(parse_result);

				CAutoFreePtr<JsonValue> defer_result = { &result, free_json_value };

				REQUIRE_EQ(result, expected_value);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);

			const size_t read_count = used_read_handle.count();

			REQUIRE_EQ(read_count, 3);

			REQUIRE_EQ(data_to_read, 0);
		}();
	}

	SUBCASE("testing path reading: close fails") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			const auto open_wrapper = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(flags);
				UNUSED(mode);

				if(strcmp(path, "test.json") == 0) {
					return open("/dev/null", flags, mode);
				}

				errno = ENOENT;
				return -1;
			};

			std::string file_data = R"({"hello": null})";

			auto& open_handle = mock_general.open();

			auto used_open_handle = open_handle.use(open_wrapper);

			auto& open64_handle = mock_general.open64();

			auto used_open64_handle = open64_handle.use(open_wrapper);

			auto& fstat_handle = mock_general.fstat();

			const auto stat_fun_impl = [&file_data](int fd, struct stat64* statbuf) -> int {
				int result = fstat64(fd, statbuf);

				if(result != 0) {
					return result;
				}

				statbuf->st_size = static_cast<off64_t>(file_data.size());

				return result;
			};

			auto used_fstat_handle =
			    fstat_handle.use([&stat_fun_impl](int fd, struct stat* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    struct stat64 statbuf64 = {};

				    int result = stat_fun_impl(fd, &statbuf64);
				    if(result != 0) {
					    return result;
				    }

				    copy_statbuf64_to_statbuf(&statbuf64, statbuf);

				    return result;
			    });

			auto& fstat64_handle = mock_general.fstat64();

			auto used_fstat64_handle =
			    fstat64_handle.use([&stat_fun_impl](int fd, struct stat64* statbuf) -> int {
				    if(statbuf == NULL) {
					    errno = EFAULT;
					    return -1;
				    }

				    return stat_fun_impl(fd, statbuf);
			    });

			auto& read_handle = mock_general.read();

			auto used_read_handle =
			    read_handle.use([&file_data](int fd, void* buf, size_t count) -> ssize_t {
				    UNUSED(fd);

				    if(buf == NULL) {
					    errno = EINVAL;
					    return -1;
				    }

				    size_t copy_count = count;

				    if(copy_count > file_data.size()) {
					    copy_count = file_data.size();
				    }

				    memcpy(buf, file_data.data(), copy_count);

				    return (ssize_t)copy_count;
			    });

			auto& close_handle = mock_general.close();

			auto used_close_handle = close_handle.use([](int fd) -> int {
				int result = close(fd);
				if(result != 0) {
					return result;
				}

				errno = EBADF;
				return -1;
			});

			{ // actual test

				tstr file_path = TSTR_LIT("test.json");

				JsonErrorCpp expected_error = JsonErrorCpp::with_file_loc(
				    "Couldn't close file", &file_path, JsonSourcePosition{ .line = 0, .col = 0 });

				CAutoFreePtr<tstr> defer_path = { &file_path, tstr_free };

				const auto parse_result = json_value_parse_from_file(&file_path);

				REQUIRE_EQ(parse_result, JsonParseResultTypeError);

				JsonError result = json_parse_result_get_as_error(parse_result);

				const auto actual_error = JsonErrorCpp{ result };

				REQUIRE_EQ(actual_error, expected_error);
			}

			const size_t open_count = used_open_handle.count();
			const size_t open64_count = used_open64_handle.count();

			REQUIRE_EQ(open_count + open64_count, 1);

			const size_t fstat_count = used_fstat_handle.count();
			const size_t fstat64_count = used_fstat64_handle.count();

			REQUIRE_EQ(fstat_count + fstat64_count, 1);

			const size_t read_count = used_read_handle.count();

			REQUIRE_EQ(read_count, 2);

			const size_t close_count = used_close_handle.count();

			REQUIRE_EQ(close_count, 1);
		}();
	}
}

TEST_CASE("testing general mock implementation <general_mock_impl>") {

	SUBCASE("testing duplicate setting of wrap fn: fstat") {
		[]() -> void {
			fstat_function_type wrap_fn_fstat = [](int fd, struct stat* statbuf) -> int {
				UNUSED(fd);
				UNUSED(statbuf);
				throw std::runtime_error(
				    std::string{ "expecting this to never be called: fstat -> " } +
				    std::to_string(fd));
			};

			CAutoFreePtr<void> defer_unwrap = auto_free_ign([]() -> void { unwrap_fstat(); });

			bool res0 = wrap_fstat(nullptr);

			REQUIRE_EQ(res0, false);
			REQUIRE_EQ(fstat_is_wrapped(), false);

			bool res1 = wrap_fstat(wrap_fn_fstat);

			REQUIRE_EQ(res1, true);
			REQUIRE_EQ(fstat_is_wrapped(), true);

			bool res2 = wrap_fstat(wrap_fn_fstat);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: read") {
		[]() -> void {
			read_function_type wrap_fn_read = [](int fd, void* buf, size_t count) -> ssize_t {
				UNUSED(fd);
				UNUSED(buf);
				UNUSED(count);
				throw std::runtime_error(
				    std::string{ "expecting this to never be called: read -> " } +
				    std::to_string(fd));
			};

			CAutoFreePtr<void> defer_unwrap = auto_free_ign([]() -> void { unwrap_read(); });

			bool res0 = wrap_read(nullptr);

			REQUIRE_EQ(res0, false);
			REQUIRE_EQ(read_is_wrapped(), false);

			bool res1 = wrap_read(wrap_fn_read);

			REQUIRE_EQ(res1, true);
			REQUIRE_EQ(read_is_wrapped(), true);

			bool res2 = wrap_read(wrap_fn_read);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: close") {
		[]() -> void {
			close_function_type wrap_fn_close = [](int fd) -> int {
				UNUSED(fd);
				throw std::runtime_error(
				    std::string{ "expecting this to never be called: close -> " } +
				    std::to_string(fd));
			};

			CAutoFreePtr<void> defer_unwrap = auto_free_ign([]() -> void { unwrap_close(); });

			bool res0 = wrap_close(nullptr);

			REQUIRE_EQ(res0, false);
			REQUIRE_EQ(close_is_wrapped(), false);

			bool res1 = wrap_close(wrap_fn_close);

			REQUIRE_EQ(res1, true);
			REQUIRE_EQ(close_is_wrapped(), true);

			bool res2 = wrap_close(wrap_fn_close);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: open") {
		[]() -> void {
			open_function_type wrap_fn_open = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(path);
				UNUSED(flags);
				UNUSED(mode);
				throw std::runtime_error(
				    std::string{ "expecting this to never be called: open -> " } + path + " -> " +
				    std::to_string(flags));
			};

			CAutoFreePtr<void> defer_unwrap = auto_free_ign([]() -> void { unwrap_open(); });

			bool res0 = wrap_open(nullptr);

			REQUIRE_EQ(res0, false);
			REQUIRE_EQ(open_is_wrapped(), false);

			bool res1 = wrap_open(wrap_fn_open);

			REQUIRE_EQ(res1, true);
			REQUIRE_EQ(open_is_wrapped(), true);

			bool res2 = wrap_open(wrap_fn_open);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: open64") {
		[]() -> void {
			open64_function_type wrap_fn_open64 = [](const char* path, int flags,
			                                         mode_t mode) -> int {
				UNUSED(path);
				UNUSED(flags);
				UNUSED(mode);
				throw std::runtime_error(
				    std::string{ "expecting this to never be called: open64 -> " } + path + " -> " +
				    std::to_string(flags));
			};

			CAutoFreePtr<void> defer_unwrap = auto_free_ign([]() -> void { unwrap_open64(); });

			bool res0 = wrap_open64(nullptr);

			REQUIRE_EQ(res0, false);
			REQUIRE_EQ(open64_is_wrapped(), false);

			bool res1 = wrap_open64(wrap_fn_open64);

			REQUIRE_EQ(res1, true);
			REQUIRE_EQ(open64_is_wrapped(), true);

			bool res2 = wrap_open64(wrap_fn_open64);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: fstat64") {
		[]() -> void {
			fstat64_function_type wrap_fn_fstat64 = [](int fd, struct stat64* statbuf) -> int {
				UNUSED(fd);
				UNUSED(statbuf);
				throw std::runtime_error(
				    std::string{ "expecting this to never be called: fstat64 -> " } +
				    std::to_string(fd));
			};

			CAutoFreePtr<void> defer_unwrap = auto_free_ign([]() -> void { unwrap_fstat64(); });

			bool res0 = wrap_fstat64(nullptr);

			REQUIRE_EQ(res0, false);
			REQUIRE_EQ(fstat64_is_wrapped(), false);

			bool res1 = wrap_fstat64(wrap_fn_fstat64);

			REQUIRE_EQ(res1, true);
			REQUIRE_EQ(fstat64_is_wrapped(), true);

			bool res2 = wrap_fstat64(wrap_fn_fstat64);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing if wrapping works: fstat") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.fstat();

			auto used_handle = handle.use([](int fd, struct stat* statbuf) -> int {
				UNUSED(statbuf);
				return -(fd + 1);
			});

			int res = wrapped_fn_fstat(1, nullptr);

			REQUIRE_EQ(res, -2);

			int res2 = wrapped_fn_fstat(11, nullptr);

			REQUIRE_EQ(res2, -12);

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}

	SUBCASE("testing if wrapping works: read") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.read();

			auto used_handle = handle.use([](int fd, void* buf, size_t count) -> ssize_t {
				UNUSED(buf);
				UNUSED(count);
				return -(fd + 1);
			});

			ssize_t res = wrapped_fn_read(1, nullptr, 0);

			REQUIRE_EQ(res, -2);

			ssize_t res2 = wrapped_fn_read(11, nullptr, 0);

			REQUIRE_EQ(res2, -12);

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}

	SUBCASE("testing if wrapping works: close") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.close();

			auto used_handle = handle.use([](int fd) -> int { return -(fd + 1); });

			int res = wrapped_fn_close(1);

			REQUIRE_EQ(res, -2);

			int res2 = wrapped_fn_close(11);

			REQUIRE_EQ(res2, -12);

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}

	SUBCASE("testing if wrapping works: open") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.open();

			auto used_handle = handle.use([](const char* path, int flags, mode_t mode) -> int {
				UNUSED(path);
				UNUSED(flags);
				UNUSED(mode);
				return -(path[0]);
			});

			int res = wrapped_fn_open("\x02 test", 0, 0);

			REQUIRE_EQ(res, -2);

			int res2 = wrapped_fn_open("\x12 test", 0, 0);

			REQUIRE_EQ(res2, -(0x12));

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}

	SUBCASE("testing if wrapping works: open64") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.open64();

			auto used_handle = handle.use([](const char* path, int flags, mode_t mode) -> int {
				UNUSED(path);
				UNUSED(flags);
				UNUSED(mode);
				return -(path[0]);
			});

			int res = wrapped_fn_open64("\x02 test", 0, 0);

			REQUIRE_EQ(res, -2);

			int res2 = wrapped_fn_open64("\x12 test", 0, 0);

			REQUIRE_EQ(res2, -(0x12));

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}

	SUBCASE("testing if wrapping works: fstat64") {
		[]() -> void {
			auto mock_general = mock::CMockGeneral::get_instance();

			auto& handle = mock_general.fstat64();

			auto used_handle = handle.use([](int fd, struct stat64* statbuf) -> int {
				UNUSED(statbuf);
				return -(fd + 1);
			});

			int res = wrapped_fn_fstat64(1, nullptr);

			REQUIRE_EQ(res, -2);

			int res2 = wrapped_fn_fstat64(11, nullptr);

			REQUIRE_EQ(res2, -12);

			const size_t count = used_handle.count();

			REQUIRE_EQ(count, 2);
		}();
	}
}

TEST_SUITE_END();
