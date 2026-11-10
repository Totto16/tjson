#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include "helpers/mock.hpp"
#include "helpers/string_maker.hpp"

TEST_SUITE_BEGIN("json_mock" * doctest::description("json mock tests") * doctest::timeout(2.0));

TEST_CASE("testing mock behaviour of json functions <json_mock_tester>") {

	SUBCASE("testing custom open wrap (variadic)") {
		[]() -> void {
			const auto mock_general = mock::CMockGeneral::get_instance();

			auto i = 0;

			auto open_handle =
			    mock_general.open([&i](const char* path, int flags, mode_t mode) -> int {
				    UNUSED(path);
				    if(flags & O_CREAT) {
					    return -((int)mode);
				    }

				    return -1 + i;
			    });

			auto used_handle = open_handle.use_local();

			int res = wrapped_fn_open("test", O_WRONLY);

			REQUIRE_EQ(res, -1);

			int res2 = wrapped_fn_open("test", O_CREAT, 17);

			REQUIRE_EQ(res2, -17);

			const size_t open_count = used_handle.count();

			REQUIRE_EQ(open_count, 2);
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

	/* 	SUBCASE("testing if wrapping works") {
	        []() -> void {
	            const auto mock_allocator = mock::CMockGeneral::get_instance();

	            const std::vector<size_t> fail_amounts = { 1, 30, 200 };

	            for(const auto& fail_amount : fail_amounts) {

	                const bool mock_res = mock_allocator.calloc().fail_after(fail_amount);
	                REQUIRE_TRUE(mock_res);

	                for(size_t i = 0; i < fail_amount; ++i) {
	                    AllocTest* allocated = (AllocTest*)TJSON_CALLOC(2, sizeof(AllocTest));

	                    REQUIRE_NE(allocated, nullptr);
	                    TJSON_FREE(allocated);
	                }

	                AllocTest* allocated = (AllocTest*)TJSON_CALLOC(2, sizeof(AllocTest));

	                REQUIRE_EQ(allocated, nullptr);
	            }
	        }();
	    }
	 */

	/* 	SUBCASE("testing recovery with fail never") {
	        []() -> void {
	            const auto mock_allocator = mock::CMockGeneral::get_instance();

	            const bool mock_res1 = mock_allocator.malloc().always_fail();
	            REQUIRE_TRUE(mock_res1);

	            AllocTest* allocated1 = TJSON_NEW(AllocTest);

	            REQUIRE_EQ(allocated1, nullptr);

	            const bool mock_res2 = mock_allocator.malloc().never_fail();
	            REQUIRE_TRUE(mock_res2);

	            AllocTest* allocated2 = TJSON_NEW(AllocTest);

	            REQUIRE_NE(allocated2, nullptr);

	            TJSON_FREE(allocated2);
	        }();
	    } */
}

TEST_SUITE_END();
