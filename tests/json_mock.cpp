#include <doctest.h>

#include "./helpers/helpers.hpp"

#include "./helpers/generic.hpp"
#include "./helpers/json.hpp"

#include <tjson.h>

#include "helpers/mock.hpp"
#include "helpers/string_maker.hpp"

TEST_SUITE_BEGIN("json_mock" * doctest::description("json mock tests") * doctest::timeout(2.0));

TEST_CASE("testing mock behaviour of json functions <json_mock_tester>") {

	// TODO
}

TEST_CASE("testing general mock implementation <general_mock_impl>") {

	SUBCASE("testing duplicate setting of wrap fn: open") {
		[]() -> void {
			open_function_type wrap_fn_open = [](const char* path, int flags, mode_t mode) -> int {
				UNUSED(path);
				UNUSED(flags);
				UNUSED(mode);
				return -1;
			};

			bool res1 = wrap_open(wrap_fn_open);

			REQUIRE_EQ(res1, true);

			bool res2 = wrap_open(wrap_fn_open);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: fstat") {
		[]() -> void {
			fstat_function_type wrap_fn_fstat = [](int fd, struct stat* statbuf) -> int {
				UNUSED(fd);
				UNUSED(statbuf);
				return -1;
			};

			bool res1 = wrap_fstat(wrap_fn_fstat);

			REQUIRE_EQ(res1, true);

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
				return -1;
			};

			bool res1 = wrap_read(wrap_fn_read);

			REQUIRE_EQ(res1, true);

			bool res2 = wrap_read(wrap_fn_read);

			REQUIRE_EQ(res2, false);
		}();
	}

	SUBCASE("testing duplicate setting of wrap fn: close") {
		[]() -> void {
			close_function_type wrap_fn_close = [](int fd) -> int {
				UNUSED(fd);
				return -1;
			};

			bool res1 = wrap_close(wrap_fn_close);

			REQUIRE_EQ(res1, true);

			bool res2 = wrap_close(wrap_fn_close);

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
