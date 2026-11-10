
#include "./mock.hpp"

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <stdexcept>

#define CUSTOM_MOCK_INIT_ENTRY(name, fn_type, number) \
	this->m_##name = general::Handle<fn_type, number>{ general::FnHolder<fn_type>{ \
		CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(name), \
		CUSTOM_MOCK_WRAP_NAME_WRAP(name), \
		CUSTOM_MOCK_WRAP_NAME_UNWRAP(name), \
		CUSTOM_MOCK_WRAP_COUNT_NAME(name), \
	} };

mock::CMockGeneral::CMockGeneral(void* associated_instance)
    : m_associated_instance{ associated_instance } {

	CUSTOM_MOCK_INIT_ENTRY(open, open_function_type, 0)
	CUSTOM_MOCK_INIT_ENTRY(fstat, fstat_function_type, 1)
	CUSTOM_MOCK_INIT_ENTRY(read, read_function_type, 2)
	CUSTOM_MOCK_INIT_ENTRY(close, close_function_type, 3)
}

// this is a singleton!
static mock::CMockGeneral* mock_general_instance = nullptr;

mock::CMockGeneral mock::CMockGeneral::get_instance() {
	if(mock_general_instance == nullptr) {
		CMockGeneral* result = (CMockGeneral*)::malloc(sizeof(CMockGeneral));
		*result = CMockGeneral(result);

		mock_general_instance = result;

		return std::move(*mock_general_instance);
	} else {
		throw std::runtime_error("Only one instance of the general mock can be used at a time");
	}
}

#define CUSTOM_MOCK_IMPLEMENT_ENTRY(name, fn_type, number) \
	[[nodiscard]] mock::general::Handle<fn_type, number>& mock::CMockGeneral::name() { \
		return this->m_##name; \
	}

CUSTOM_MOCK_IMPLEMENT_ENTRY(open, open_function_type, 0)
CUSTOM_MOCK_IMPLEMENT_ENTRY(fstat, fstat_function_type, 1)
CUSTOM_MOCK_IMPLEMENT_ENTRY(read, read_function_type, 2)
CUSTOM_MOCK_IMPLEMENT_ENTRY(close, close_function_type, 3)

#define CUSTOM_MOCK_IMPLEMENT_MOVE(name) this->m_##name = std::move(other.m_##name);

mock::CMockGeneral::CMockGeneral(CMockGeneral&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

	CUSTOM_MOCK_IMPLEMENT_MOVE(open)
	CUSTOM_MOCK_IMPLEMENT_MOVE(fstat)
	CUSTOM_MOCK_IMPLEMENT_MOVE(read)
	CUSTOM_MOCK_IMPLEMENT_MOVE(close)
}

mock::CMockGeneral& mock::CMockGeneral::operator=(CMockGeneral&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

	CUSTOM_MOCK_IMPLEMENT_MOVE(open)
	CUSTOM_MOCK_IMPLEMENT_MOVE(fstat)
	CUSTOM_MOCK_IMPLEMENT_MOVE(read)
	CUSTOM_MOCK_IMPLEMENT_MOVE(close)

	return *this;
}

#define CUSTOM_MOCK_IMPLEMENT_DELETE(name) this->m_##name.~Handle();

mock::CMockGeneral::~CMockGeneral() noexcept(false) {
	if(m_associated_instance == nullptr) {
		return;
	}

	if(m_associated_instance != mock_general_instance) {
		throw std::runtime_error("Only one instance of the general mock can be used at a time, "
		                         "but this one has another pointer?? Was it copied somehow?");
	}

	this->m_associated_instance = nullptr;

	CUSTOM_MOCK_IMPLEMENT_DELETE(open)
	CUSTOM_MOCK_IMPLEMENT_DELETE(fstat)
	CUSTOM_MOCK_IMPLEMENT_DELETE(read)
	CUSTOM_MOCK_IMPLEMENT_DELETE(close)

	if(mock_general_instance == nullptr) {
		throw std::runtime_error(
		    "Only one instance of the general mock can be used at a time, but this one was "
		    "release with the global instance being NULL?? Was it copied somehow?");
	}

	free(mock_general_instance);
	mock_general_instance = nullptr;
}
