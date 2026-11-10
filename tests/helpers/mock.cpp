
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

#define CUSTOM_MOCK_FUNCTION_X_MACRO(name, fn_type, number) \
	CUSTOM_MOCK_INIT_ENTRY(name, fn_type, number)
	CUSTOM_MOCK_DEFINE_FUNCTIONS_LIST
#undef CUSTOM_MOCK_FUNCTION_X_MACRO
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

#define CUSTOM_MOCK_FUNCTION_X_MACRO(name, fn_type, number) \
	CUSTOM_MOCK_IMPLEMENT_ENTRY(name, fn_type, number)
CUSTOM_MOCK_DEFINE_FUNCTIONS_LIST
#undef CUSTOM_MOCK_FUNCTION_X_MACRO

#define CUSTOM_MOCK_IMPLEMENT_MOVE(name) this->m_##name = std::move(other.m_##name);

mock::CMockGeneral::CMockGeneral(CMockGeneral&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

#define CUSTOM_MOCK_FUNCTION_X_MACRO(name, fn_type, number) CUSTOM_MOCK_IMPLEMENT_MOVE(name)
	CUSTOM_MOCK_DEFINE_FUNCTIONS_LIST
#undef CUSTOM_MOCK_FUNCTION_X_MACRO
}

mock::CMockGeneral& mock::CMockGeneral::operator=(CMockGeneral&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

#define CUSTOM_MOCK_FUNCTION_X_MACRO(name, fn_type, number) CUSTOM_MOCK_IMPLEMENT_MOVE(name)
	CUSTOM_MOCK_DEFINE_FUNCTIONS_LIST
#undef CUSTOM_MOCK_FUNCTION_X_MACRO

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

#define CUSTOM_MOCK_FUNCTION_X_MACRO(name, fn_type, number) CUSTOM_MOCK_IMPLEMENT_DELETE(name)
	CUSTOM_MOCK_DEFINE_FUNCTIONS_LIST
#undef CUSTOM_MOCK_FUNCTION_X_MACRO

	if(mock_general_instance == nullptr) {
		throw std::runtime_error(
		    "Only one instance of the general mock can be used at a time, but this one was "
		    "release with the global instance being NULL?? Was it copied somehow?");
	}

	free(mock_general_instance);
	mock_general_instance = nullptr;
}
