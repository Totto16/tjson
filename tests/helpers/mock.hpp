

#pragma once

#include <cstddef>

#include <custom_mock.h>

#include <functional>

#include "./cpp_types.hpp"

#define CUSTOM_MOCK_DEFINE_ENTRY(name, fn_type) \
	template <typename F> [[nodiscard]] general::Handle<fn_type> name(F&& fn) const { \
		return general::Handle<fn_type>{ std::forward<F>(fn), \
			                             general::FnHolder<fn_type>{ \
			                                 .is_wrapped = CUSTOM_MOCK_WRAP_NAME_IS_WRAPPED(name), \
			                                 .wrap = CUSTOM_MOCK_WRAP_NAME_WRAP(name), \
			                                 .unwrap = CUSTOM_MOCK_WRAP_NAME_UNWRAP(name), \
			                             } }; \
	}

namespace mock {

namespace helper {
template <typename T> struct to_std_function;

template <typename Ret, typename... Args> struct to_std_function<Ret (*)(Args...)> {
	using type = std::function<Ret(Args...)>;
};

template <typename T> using to_std_function_t = to_std_function<T>::type;

} // namespace helper

namespace general {

template <typename fn_type> struct FnHolder {
	bool (*is_wrapped)(void);
	bool (*wrap)(fn_type fun);
	void (*unwrap)(void);
};

template <typename fn_type> class Handle {
  private:
	helper::to_std_function_t<fn_type> m_fun;
	FnHolder<fn_type> m_holder;

  public:
	template <typename F>
	Handle(F&& fun, FnHolder<fn_type>&& holder)
	    : m_fun{ std::forward<F>(fun) }, m_holder{ std::move(holder) } {}

	void use() { this->m_holder.wrap(this->m_fun); }

	CAutoFreePtr<void> use_local() {
		this->use();
		return auto_free_ign([this]() -> void { this->unset(); });
	}

	[[nodiscard]] operator bool() const { return this->is_used(); }

	[[nodiscard]] bool is_used() const { return this->m_holder.is_wrapped(); }

	void unset() const { this->m_holder.unwrap(); }
};

} // namespace general

class CMockGeneral {
	void* m_associated_instance;

	explicit CMockGeneral(void* associated_instance);

  public:
	[[nodiscard]] static CMockGeneral get_instance();

	CUSTOM_MOCK_DEFINE_ENTRY(open, open_function_type)
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
