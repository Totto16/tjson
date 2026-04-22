
#include "./allocators.hpp"

#include <tjson_coverage_allocators.h>

struct FunctionMockMemberImpl {
	AllocatorFunctionHandle* handle;
};

mock::FunctionMock::FunctionMock() : m_impl{ nullptr } {}

mock::FunctionMock::FunctionMock(Ptr&& member) : m_impl{ std::move(member) } {}

template <typename T>
T call_c_fn(mock::FunctionMockMember* orig_handle,
            const std::function<T(AllocatorFunctionHandle*)>& fn) {
	if(orig_handle == nullptr) {
		throw std::runtime_error("Wrong initialized FunctionMockMember");
	}
	AllocatorFunctionHandle* handle = (AllocatorFunctionHandle*)orig_handle;

	return fn(handle);
}

bool mock::FunctionMock::always_fail() const {
	return call_c_fn<bool>(this->m_impl.get(), [](AllocatorFunctionHandle* handle) -> bool {
		return tjson_coverage_allocator_handle_fail_always(handle);
	});
}

bool mock::FunctionMock::fail_after(size_t count) const {
	return call_c_fn<bool>(this->m_impl.get(), [count](AllocatorFunctionHandle* handle) -> bool {
		return tjson_coverage_allocator_handle_fail_after(handle, count);
	});
}

bool mock::FunctionMock::never_fail() const {
	return call_c_fn<bool>(this->m_impl.get(), [](AllocatorFunctionHandle* handle) -> bool {
		return tjson_coverage_allocator_handle_fail_never(handle);
	});
}

std::unique_ptr<mock::CMockAllocator> mock::CMockAllocator::s_instance = nullptr;

static mock::FunctionMock new_function_mock(AllocatorFunctionType type) {
	auto* handle = tjson_coverage_allocator_get_handle(type);
	if(handle == nullptr) {
		throw std::runtime_error("Error on getting C handle for FunctionMock");
	}

	mock::FunctionMockMember* cast_handle = (mock::FunctionMockMember*)handle;

	std::unique_ptr<mock::FunctionMockMember, std::function<void(mock::FunctionMockMember*)>>
	    ptr_handle = std::unique_ptr<mock::FunctionMockMember,
	                                 std::function<void(mock::FunctionMockMember*)>>{
		    cast_handle,
		    [](mock::FunctionMockMember* del_handle) -> void {
		        tjson_coverage_allocator_free_handle((AllocatorFunctionHandle*)del_handle);
		    }
	    };

	return mock::FunctionMock(std::move(ptr_handle));
}

mock::CMockAllocator::CMockAllocator() {
	this->m_malloc = new_function_mock(AllocatorFunctionTypeMalloc);
	this->m_calloc = new_function_mock(AllocatorFunctionTypeCalloc);
	this->m_realloc = new_function_mock(AllocatorFunctionTypeRealloc);
}

const std::unique_ptr<mock::CMockAllocator>& mock::CMockAllocator::instantiate() {
	if(CMockAllocator::s_instance.get() == nullptr) {
		CMockAllocator::s_instance = std::unique_ptr<CMockAllocator>(new CMockAllocator());
	} else {
		throw std::runtime_error("Only one instance of the mock allocator cane be used at a time");
	}

	return CMockAllocator::s_instance;
}

[[nodiscard]] const mock::FunctionMock& mock::CMockAllocator::malloc() const {
	return this->m_malloc;
}

[[nodiscard]] const mock::FunctionMock& mock::CMockAllocator::calloc() const {
	return this->m_calloc;
}

[[nodiscard]] const mock::FunctionMock& mock::CMockAllocator::realloc() const {
	return this->m_realloc;
}

mock::CMockAllocator::~CMockAllocator() {
	auto* ptr = CMockAllocator::s_instance.release();
	delete ptr;
}
