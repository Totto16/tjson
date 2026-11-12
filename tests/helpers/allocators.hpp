

#pragma once

#include <cstddef>

#define ALLOCATOR_MOCK_MEMBER_NAME(name) m_##name

#define ALLOCATOR_MOCK_DEFINE_MEMBER(name) alloc::FunctionMock ALLOCATOR_MOCK_MEMBER_NAME(name);

#define ALLOCATOR_MOCK_DEFINE_ENTRY(name) [[nodiscard]] const alloc::FunctionMock& name() const;

namespace mock {

namespace alloc {

using FunctionMockMember = struct FunctionMockMemberImpl;

class FunctionMock {
	FunctionMockMember* m_impl;

  public:
	FunctionMock();

	explicit FunctionMock(FunctionMockMember*&& member);

	[[nodiscard]] bool always_fail() const;
	[[nodiscard]] bool fail_after(size_t count) const;
	[[nodiscard]] bool never_fail() const;

	FunctionMock(FunctionMock const&) = delete;
	FunctionMock& operator=(FunctionMock const&) = delete;

	FunctionMock(FunctionMock&&) noexcept;
	FunctionMock& operator=(FunctionMock&&) noexcept;

	~FunctionMock();
};

} // namespace alloc

#define ALLOCATOR_MOCK_DEFINE_FUNCTIONS_LIST \
	ALLOCATOR_MOCK_FUNCTION_X_MACRO(malloc, Malloc) \
	ALLOCATOR_MOCK_FUNCTION_X_MACRO(calloc, Calloc) \
	ALLOCATOR_MOCK_FUNCTION_X_MACRO(realloc, Realloc)

class CMockAllocator {
	// members

#define ALLOCATOR_MOCK_FUNCTION_X_MACRO(name, name_upper) ALLOCATOR_MOCK_DEFINE_MEMBER(name)
	ALLOCATOR_MOCK_DEFINE_FUNCTIONS_LIST
#undef ALLOCATOR_MOCK_FUNCTION_X_MACRO

	void* m_associated_instance;

	explicit CMockAllocator(void* associated_instance);

  public:
	[[nodiscard]] static CMockAllocator get_instance();

#define ALLOCATOR_MOCK_FUNCTION_X_MACRO(name, name_upper) ALLOCATOR_MOCK_DEFINE_ENTRY(name)
	ALLOCATOR_MOCK_DEFINE_FUNCTIONS_LIST
#undef ALLOCATOR_MOCK_FUNCTION_X_MACRO

	CMockAllocator(CMockAllocator const&) = delete;
	CMockAllocator& operator=(CMockAllocator const&) = delete;

	CMockAllocator(CMockAllocator&&) noexcept;
	CMockAllocator& operator=(CMockAllocator&&) noexcept;

	~CMockAllocator() noexcept(false);
};

} // namespace mock
