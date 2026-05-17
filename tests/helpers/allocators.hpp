

#pragma once

#include <cstddef>

namespace mock {

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

class CMockAllocator {
	// members
	FunctionMock m_malloc;
	FunctionMock m_calloc;
	FunctionMock m_realloc;
	void* m_associated_instance;

	explicit CMockAllocator(void* associated_instance);

  public:
	[[nodiscard]] static CMockAllocator get_instance();

	[[nodiscard]] const FunctionMock& malloc() const;

	[[nodiscard]] const FunctionMock& calloc() const;

	[[nodiscard]] const FunctionMock& realloc() const;

	CMockAllocator(CMockAllocator const&) = delete;
	CMockAllocator& operator=(CMockAllocator const&) = delete;

	CMockAllocator(CMockAllocator&&) noexcept;
	CMockAllocator& operator=(CMockAllocator&&) noexcept;

	~CMockAllocator() noexcept(false);
};

} // namespace mock
