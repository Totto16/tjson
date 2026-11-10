#pragma once

#include <cstddef>

#include <custom_mock.h>

#include <functional>

#include "./cpp_types.hpp"

#define CUSTOM_MOCK_HANDLER_AMOUNT 4

#define CUSTOM_MOCK_DEFINE_MEMBER(name, fn_type, number) general::Handle<fn_type, number> m_##name;

#define CUSTOM_MOCK_DEFINE_ENTRY(name, fn_type, number) \
	[[nodiscard]] mock::general::Handle<fn_type, number>& name();

namespace mock {

namespace helper {
template <typename T> struct to_std_function;

template <typename Ret, typename... Args> struct to_std_function<Ret (*)(Args...)> {
	using fn_type = std::function<Ret(Args...)>;

	using ret_type = Ret;
	using args_tuple = std::tuple<Args...>;
	using signature = Ret(Args...);
};

template <typename T> using to_std_function_t = to_std_function<T>::fn_type;

} // namespace helper

namespace general {

template <typename fn_type> struct FnHolder {
	bool (*is_wrapped)(void);
	bool (*wrap)(fn_type fun);
	void (*unwrap)(void);
	size_t (*count)(void);

	FnHolder() : is_wrapped{ nullptr }, wrap{ nullptr }, unwrap{ nullptr }, count{ nullptr } {}

	FnHolder(bool (*is_wrapped_a)(void), bool (*wrap_a)(fn_type fun), void (*unwrap_a)(void),
	         size_t (*count_a)(void))
	    : is_wrapped{ is_wrapped_a }, wrap{ wrap_a }, unwrap{ unwrap_a }, count{ count_a } {}

	FnHolder(FnHolder const&) = delete;
	FnHolder& operator=(FnHolder const&) = delete;

	FnHolder(FnHolder&& other) noexcept {
		this->is_wrapped = other.is_wrapped;
		other.is_wrapped = nullptr;

		this->wrap = other.wrap;
		other.wrap = nullptr;

		this->unwrap = other.unwrap;
		other.unwrap = nullptr;

		this->count = other.count;
		other.count = nullptr;
	}

	FnHolder& operator=(FnHolder&& other) noexcept {
		this->is_wrapped = other.is_wrapped;
		other.is_wrapped = nullptr;

		this->wrap = other.wrap;
		other.wrap = nullptr;

		this->unwrap = other.unwrap;
		other.unwrap = nullptr;

		this->count = other.count;
		other.count = nullptr;
		return *this;
	}

	~FnHolder() {
		if(this->wrap == nullptr) {
			return;
		}

		this->is_wrapped = nullptr;
		this->wrap = nullptr;
		this->unwrap = nullptr;
		this->count = nullptr;
	}
};

template <size_t ID, typename Signature> class TrampolineImpl;

template <std::size_t ID, typename Ret, typename... Args> class TrampolineImpl<ID, Ret(Args...)> {
  private:
	using Fn = std::function<Ret(Args...)>;
	using WrapperFn = std::function<Ret(Fn*, Args...)>;
	using CFun = Ret (*)(Args...);

	struct InstanceData {
		CFun m_fun;
		Fn* m_data;

		InstanceData() : m_fun{ nullptr }, m_data{ nullptr } {}
		InstanceData(CFun fun, Fn* data) : m_fun{ fun }, m_data{ data } {}
	};

	inline static std::array<InstanceData, CUSTOM_MOCK_HANDLER_AMOUNT> s_instances{};

  public:
	TrampolineImpl() {
		static_assert(ID < CUSTOM_MOCK_HANDLER_AMOUNT);
		s_instances[ID] = {};
	}

	TrampolineImpl(Fn* fun) { this->set(fun); }

	void set(Fn* fun) {
		InstanceData data = {
			[](Args... args) -> Ret {
			    Fn* ctx = s_instances[ID].m_data;
			    return (*ctx)(std::forward<Args>(args)...);
			},
			fun,
		};

		s_instances[ID] = data;
	}

	[[nodiscard]] static CFun raw() { return s_instances[ID].m_fun; }

	TrampolineImpl(TrampolineImpl const&) = delete;
	TrampolineImpl& operator=(TrampolineImpl const&) = delete;

	TrampolineImpl(TrampolineImpl&&) noexcept {
		// noop
	}
	TrampolineImpl& operator=(TrampolineImpl&&) noexcept {
		// noop
		return *this;
	}

	~TrampolineImpl() {
		if(s_instances[ID].m_data == nullptr) {
			return;
		}

		s_instances[ID] = {};
	}
};

template <typename fn_type, size_t ID>
class Trampoline final
    : public TrampolineImpl<ID, typename helper::to_std_function<fn_type>::signature> {};

template <typename fn_type, size_t ID> class UsedHandle {
  private:
	helper::to_std_function_t<fn_type> m_fun;
	FnHolder<fn_type>* m_holder;
	Trampoline<fn_type, ID>* m_trampoline;

	void use() {
		this->m_trampoline->set(&(this->m_fun));

		this->m_holder->wrap(this->m_trampoline->raw());
	}

	void unset() {
		this->m_trampoline = {};

		this->m_holder->unwrap();
	}

  public:
	template <typename F>
	UsedHandle(F&& fun, FnHolder<fn_type>* holder, Trampoline<fn_type, ID>* trampoline)
	    : m_fun{ std::forward<F>(fun) }, m_holder{ holder }, m_trampoline{ trampoline } {
		this->use();
	}

	[[nodiscard]] size_t count() const { return this->m_holder->count(); }

	UsedHandle(UsedHandle const&) = delete;
	UsedHandle& operator=(UsedHandle const&) = delete;

	UsedHandle(UsedHandle&& other) noexcept {
		this->m_holder = other.m_holder;
		other.m_holder = nullptr;

		this->m_holder = other.m_trampoline;
		other.m_trampoline = nullptr;

		this->m_fun = std::move(other.m_fun);
	}

	UsedHandle& operator=(UsedHandle&& other) noexcept {
		this->m_holder = other.m_holder;
		other.m_holder = nullptr;

		this->m_holder = other.m_trampoline;
		other.m_trampoline = nullptr;

		this->m_fun = std::move(other.m_fun);

		return *this;
	}

	~UsedHandle() {
		if(this->m_holder == nullptr) {
			return;
		}

		this->unset();
		this->m_holder = nullptr;
	}
};

template <typename fn_type, size_t ID> class Handle {
  private:
	FnHolder<fn_type> m_holder;
	Trampoline<fn_type, ID> m_trampoline;

  public:
	Handle() : m_holder{}, m_trampoline{} {}

	explicit Handle(FnHolder<fn_type>&& holder) : m_holder{ std::move(holder) }, m_trampoline{} {}

	template <typename F> UsedHandle<fn_type, ID> use(F&& fun) {
		return UsedHandle<fn_type, ID>{
			std::forward<F>(fun),
			&(this->m_holder),
			&(this->m_trampoline),
		};
	}

	[[nodiscard]] operator bool() const { return this->is_used(); }

	[[nodiscard]] bool is_used() const { return this->m_holder.is_wrapped(); }

	Handle(Handle const&) = delete;
	Handle& operator=(Handle const&) = delete;

	Handle(Handle&& other) noexcept {
		this->m_holder = std::move(other.m_holder);
		this->m_trampoline = std::move(other.m_trampoline);
	}

	Handle& operator=(Handle&& other) noexcept {
		this->m_holder = std::move(other.m_holder);
		this->m_trampoline = std::move(other.m_trampoline);
		return *this;
	}

	~Handle() = default;
};

} // namespace general

class CMockGeneral {
	// members
	CUSTOM_MOCK_DEFINE_MEMBER(open, open_function_type, 0)
	CUSTOM_MOCK_DEFINE_MEMBER(fstat, fstat_function_type, 1)
	CUSTOM_MOCK_DEFINE_MEMBER(read, read_function_type, 2)
	CUSTOM_MOCK_DEFINE_MEMBER(close, close_function_type, 3)
	void* m_associated_instance;

	explicit CMockGeneral(void* associated_instance);

  public:
	[[nodiscard]] static CMockGeneral get_instance();

	CUSTOM_MOCK_DEFINE_ENTRY(open, open_function_type, 0)
	CUSTOM_MOCK_DEFINE_ENTRY(fstat, fstat_function_type, 1)
	CUSTOM_MOCK_DEFINE_ENTRY(read, read_function_type, 2)
	CUSTOM_MOCK_DEFINE_ENTRY(close, close_function_type, 3)

	CMockGeneral(CMockGeneral const&) = delete;
	CMockGeneral& operator=(CMockGeneral const&) = delete;

	CMockGeneral(CMockGeneral&&) noexcept;
	CMockGeneral& operator=(CMockGeneral&&) noexcept;

	~CMockGeneral() noexcept(false);
};

} // namespace mock
