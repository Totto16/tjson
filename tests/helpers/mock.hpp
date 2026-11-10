

#pragma once

#include <cstddef>

#include <custom_mock.h>

#define CUSTOM_MOCK_DEFINE_ENTRY(name, fn_type) [[nodiscard]] general::Handle<fn_type> name() const;

namespace mock {

namespace general {

template <typename FNType> class Handle {};

} // namespace general

class CMockGeneral {
	void* m_associated_instance;

	explicit CMockGeneral(void* associated_instance);

  public:
	[[nodiscard]] static CMockGeneral get_instance();

	CUSTOM_MOCK_DEFINE_ENTRY(fstat, fstat_function_type)
	CUSTOM_MOCK_DEFINE_ENTRY(read, read_function_type)
	CUSTOM_MOCK_DEFINE_ENTRY(close, close_function_type)

	CMockGeneral(CMockGeneral const&) = delete;
	CMockGeneral& operator=(CMockGeneral const&) = delete;

	CMockGeneral(CMockGeneral&&) noexcept;
	CMockGeneral& operator=(CMockGeneral&&) noexcept;

	~CMockGeneral() noexcept(false);
};

} // namespace mock
