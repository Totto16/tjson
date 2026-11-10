
#include "./allocators.hpp"

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <stdexcept>

#include <tjson_coverage_allocators.h>

struct FunctionMockMemberImpl {
	// note: this is only a placeholder, we cast this type arounf, but we need a incomplete type for
	// the header file!
	AllocatorFunctionHandle* handle;
};

mock::alloc::FunctionMock::FunctionMock() : m_impl{ nullptr } {}

mock::alloc::FunctionMock::FunctionMock(FunctionMockMember*&& member) : m_impl{ std::move(member) } {}

template <typename T>
T call_c_fn(mock::alloc::FunctionMockMember* orig_handle,
            const std::function<T(AllocatorFunctionHandle*)>& fn) {
	if(orig_handle == nullptr) {
		throw std::runtime_error("Wrong initialized FunctionMockMember");
	}
	AllocatorFunctionHandle* handle = (AllocatorFunctionHandle*)orig_handle;

	return fn(handle);
}

bool mock::alloc::FunctionMock::always_fail() const {
	return call_c_fn<bool>(this->m_impl, [](AllocatorFunctionHandle* handle) -> bool {
		return tjson_coverage_allocator_handle_fail_always(handle);
	});
}

bool mock::alloc::FunctionMock::fail_after(size_t count) const {
	return call_c_fn<bool>(this->m_impl, [count](AllocatorFunctionHandle* handle) -> bool {
		return tjson_coverage_allocator_handle_fail_after(handle, count);
	});
}

bool mock::alloc::FunctionMock::never_fail() const {
	return call_c_fn<bool>(this->m_impl, [](AllocatorFunctionHandle* handle) -> bool {
		return tjson_coverage_allocator_handle_fail_never(handle);
	});
}

mock::alloc::FunctionMock::FunctionMock(FunctionMock&& other) noexcept {
	this->m_impl = other.m_impl;
	other.m_impl = nullptr;
}

mock::alloc::FunctionMock& mock::alloc::FunctionMock::operator=(FunctionMock&& other) noexcept {

	this->m_impl = other.m_impl;
	other.m_impl = nullptr;

	return *this;
}

mock::alloc::FunctionMock::~FunctionMock() {
	if(this->m_impl == NULL) {
		return;
	}

	tjson_coverage_allocator_free_handle((AllocatorFunctionHandle*)this->m_impl);
	this->m_impl = nullptr;
}

static mock::alloc::FunctionMock new_function_mock(AllocatorFunctionType type) {
	auto* handle = tjson_coverage_allocator_get_handle(type);
	if(handle == nullptr) {
		throw std::runtime_error("Error on getting C handle for FunctionMock");
	}

	mock::alloc::FunctionMockMember* cast_handle = (mock::alloc::FunctionMockMember*)handle;

	return mock::alloc::FunctionMock(std::move(cast_handle));
}

mock::CMockAllocator::CMockAllocator(void* associated_instance)
    : m_associated_instance{ associated_instance } {
	this->m_malloc = new_function_mock(AllocatorFunctionTypeMalloc);
	this->m_calloc = new_function_mock(AllocatorFunctionTypeCalloc);
	this->m_realloc = new_function_mock(AllocatorFunctionTypeRealloc);
}

// this is a singleton!
static mock::CMockAllocator* mock_allocator_instance = nullptr;

mock::CMockAllocator mock::CMockAllocator::get_instance() {
	if(mock_allocator_instance == nullptr) {
		CMockAllocator* result = (CMockAllocator*)::malloc(sizeof(CMockAllocator));
		*result = CMockAllocator(result);

		mock_allocator_instance = result;

		return std::move(*mock_allocator_instance);
	} else {
		throw std::runtime_error("Only one instance of the mock allocator can be used at a time");
	}
}

[[nodiscard]] const mock::alloc::FunctionMock& mock::CMockAllocator::malloc() const {
	return this->m_malloc;
}

[[nodiscard]] const mock::alloc::FunctionMock& mock::CMockAllocator::calloc() const {
	return this->m_calloc;
}

[[nodiscard]] const mock::alloc::FunctionMock& mock::CMockAllocator::realloc() const {
	return this->m_realloc;
}

mock::CMockAllocator::CMockAllocator(CMockAllocator&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

	this->m_malloc = std::move(other.m_malloc);
	this->m_calloc = std::move(other.m_calloc);
	this->m_realloc = std::move(other.m_realloc);
}

mock::CMockAllocator& mock::CMockAllocator::operator=(CMockAllocator&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

	this->m_malloc = std::move(other.m_malloc);
	this->m_calloc = std::move(other.m_calloc);
	this->m_realloc = std::move(other.m_realloc);

	return *this;
}

mock::CMockAllocator::~CMockAllocator() noexcept(false) {
	if(m_associated_instance == nullptr) {
		return;
	}

	if(m_associated_instance != mock_allocator_instance) {
		throw std::runtime_error("Only one instance of the mock allocator can be used at a time, "
		                         "but this one has another pointer?? Was it copied somehow?");
	}

	this->m_associated_instance = nullptr;

	this->m_calloc.~FunctionMock();
	this->m_calloc.~FunctionMock();
	this->m_calloc.~FunctionMock();

	if(mock_allocator_instance == nullptr) {
		throw std::runtime_error(
		    "Only one instance of the mock allocator can be used at a time, but this one was "
		    "release with the global instance being NULL?? Was it copied somehow?");
	}

	free(mock_allocator_instance);
	mock_allocator_instance = nullptr;
}
