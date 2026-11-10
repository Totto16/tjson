
#include "./mock.hpp"

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <stdexcept>

mock::CMockGeneral::CMockGeneral(void* associated_instance)
    : m_associated_instance{ associated_instance } {}

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

mock::CMockGeneral::CMockGeneral(CMockGeneral&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;
}

mock::CMockGeneral& mock::CMockGeneral::operator=(CMockGeneral&& other) noexcept {

	this->m_associated_instance = other.m_associated_instance;
	other.m_associated_instance = nullptr;

	return *this;
}

static void impl_unwrap_all() {
	CUSTOM_MOCK_WRAP_NAME_UNWRAP(open)();
	CUSTOM_MOCK_WRAP_NAME_UNWRAP(fstat)();
	CUSTOM_MOCK_WRAP_NAME_UNWRAP(read)();
	CUSTOM_MOCK_WRAP_NAME_UNWRAP(close)();
}

mock::CMockGeneral::~CMockGeneral() noexcept(false) {
	if(m_associated_instance == nullptr) {
		return;
	}

	if(m_associated_instance != mock_general_instance) {
		throw std::runtime_error("Only one instance of the general mock can be used at a time, "
		                         "but this one has another pointer?? Was it copied somehow?");
	}

	this->m_associated_instance = nullptr;

	impl_unwrap_all();

	if(mock_general_instance == nullptr) {
		throw std::runtime_error(
		    "Only one instance of the general mock can be used at a time, but this one was "
		    "release with the global instance being NULL?? Was it copied somehow?");
	}

	free(mock_general_instance);
	mock_general_instance = nullptr;
}
