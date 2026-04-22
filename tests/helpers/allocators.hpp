

#pragma once

#include <functional>
#include <memory>

namespace mock {

using FunctionMockMember = struct FunctionMockMemberImpl;

class FunctionMock {
	using Ptr = std::unique_ptr<FunctionMockMember, std::function<void(FunctionMockMember*)>>;

	Ptr m_impl;

  public:
	FunctionMock();

	FunctionMock(Ptr&& member);

	[[nodiscard]] bool always_fail() const;
	[[nodiscard]] bool fail_after(size_t count) const;
	[[nodiscard]] bool never_fail() const;
};

class CMockAllocator {
	// this is a singleton!
	static std::unique_ptr<CMockAllocator> s_instance;
	// members
	FunctionMock m_malloc;
	FunctionMock m_calloc;
	FunctionMock m_realloc;

	CMockAllocator();

  public:
	[[nodiscard]] static const std::unique_ptr<CMockAllocator>& instantiate();

	[[nodiscard]] const FunctionMock& malloc() const;

	[[nodiscard]] const FunctionMock& calloc() const;

	[[nodiscard]] const FunctionMock& realloc() const;

	CMockAllocator(CMockAllocator const&) = delete;
	CMockAllocator& operator=(CMockAllocator const&) = delete;

	~CMockAllocator();
};

} // namespace mock
