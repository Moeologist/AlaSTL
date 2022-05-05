#ifndef _ALA_DETAIL_UTILITY_BASE_H
#define _ALA_DETAIL_UTILITY_BASE_H

#ifndef _ALA_DETAIL_TRAITS_DECLARE_H
    #error Never include this head directly, use ala/utility.h instead
#endif

namespace ala {

template<typename T>
add_rvalue_reference_t<T> declval() noexcept;

template<typename T>
constexpr typename remove_reference<T>::type &&move(T &&x) noexcept {
    return static_cast<typename remove_reference<T>::type &&>(x);
}

template<typename T>
constexpr T &&forward(remove_reference_t<T> &t) noexcept {
    return static_cast<T &&>(t);
}

template<typename T>
constexpr T &&forward(remove_reference_t<T> &&t) noexcept {
    static_assert(!is_lvalue_reference<T>::value,
                  "T can not be lvalue reference type");
    return static_cast<T &&>(t);
}

template<class T>
constexpr enable_if_t<is_move_constructible<T>::value && is_move_assignable<T>::value>
swap(T &lhs, T &rhs) noexcept(
    is_nothrow_move_constructible<T>::value &&is_nothrow_move_assignable<T>::value);

template<class T, size_t N>
constexpr enable_if_t<is_swappable<T>::value>
    swap(T (&lhs)[N], T (&rhs)[N]) noexcept(is_nothrow_swappable<T>::value);

template<class T>
constexpr void _swap_adl(T &lhs, T &rhs) noexcept(is_nothrow_swappable<T>::value) {
    using ala::swap;
    swap(lhs, rhs);
}

template<class T>
constexpr enable_if_t<is_move_constructible<T>::value && is_move_assignable<T>::value>
swap(T &lhs, T &rhs) noexcept(is_nothrow_move_constructible<T>::value
                                  &&is_nothrow_move_assignable<T>::value) {
    T tmp = ala::move(lhs);
    lhs = ala::move(rhs);
    rhs = ala::move(tmp);
}

template<class T, size_t N>
constexpr enable_if_t<is_swappable<T>::value>
swap(T (&lhs)[N], T (&rhs)[N]) noexcept(is_nothrow_swappable<T>::value) {
    if (&lhs != &rhs) {
        T *il = lhs;
        T *last = lhs + N;
        T *ir = rhs;
        for (; il != last; ++il, ++ir)
            ala::_swap_adl(*il, *ir);
    }
}

// clang-format off

template<class Base, class T, class Derived, class... Args>
constexpr auto _invoke_helper(T Base::*pmf, Derived &&ref, Args &&...args) noexcept(
  noexcept((static_cast<Derived&&>(ref).*pmf)(static_cast<Args&&>(args)...))) -> enable_if_t<is_function<T>::value && !is_reference_wrapper<decay_t<Derived>>::value && is_base_of<Base, decay_t<Derived>>::value,
  decltype((static_cast<Derived&&>(ref).*pmf)(static_cast<Args&&>(args)...))> {
    return (static_cast<Derived&&>(ref).*pmf)(static_cast<Args&&>(args)...);
}

template<class Base, class T, class Ptr, class... Args>
constexpr auto _invoke_helper(T Base::*pmf, Ptr &&ptr, Args &&...args) noexcept(
  noexcept(((*static_cast<Ptr&&>(ptr)).*pmf)(static_cast<Args&&>(args)...))) -> enable_if_t<is_function<T>::value && !is_reference_wrapper<decay_t<Ptr>>::value && !is_base_of<Base, decay_t<Ptr>>::value,
  decltype(((*static_cast<Ptr&&>(ptr)).*pmf)(static_cast<Args&&>(args)...))> {
    return ((*static_cast<Ptr&&>(ptr)).*pmf)(static_cast<Args&&>(args)...);
}

template<class Base, class T, class RefWrap, class... Args>
constexpr auto _invoke_helper(T Base::*pmf, RefWrap &&ref, Args &&...args) noexcept(
  noexcept((ref.get().*pmf)(static_cast<Args&&>(args)...))) -> enable_if_t<is_function<T>::value & is_reference_wrapper<decay_t<RefWrap>>::value,
  decltype((ref.get().*pmf)(static_cast<Args&&>(args)...))> {
    return (ref.get().*pmf)(static_cast<Args&&>(args)...);
}

template<class Base, class T, class Derived>
constexpr auto
_invoke_helper(T Base::*pmd, Derived &&ref) noexcept(
  noexcept(static_cast<Derived&&>(ref).*pmd)) -> enable_if_t<!is_function<T>::value && !is_reference_wrapper<decay_t<Derived>>::value && is_base_of<Base, decay_t<Derived>>::value,
  decltype(static_cast<Derived&&>(ref).*pmd)> {
    return static_cast<Derived&&>(ref).*pmd;
}

template<class Base, class T, class Ptr>
constexpr auto
_invoke_helper(T Base::*pmd, Ptr &&ptr) noexcept(
  noexcept((*static_cast<Ptr&&>(ptr)).*pmd)) -> enable_if_t<!is_function<T>::value && !is_reference_wrapper<decay_t<Ptr>>::value && !is_base_of<Base, decay_t<Ptr>>::value,
  decltype((*static_cast<Ptr&&>(ptr)).*pmd)> {
    return (*static_cast<Ptr&&>(ptr)).*pmd;
}

template<class Base, class T, class RefWrap>
constexpr auto _invoke_helper(T Base::*pmd, RefWrap &&ref) noexcept(
  noexcept(ref.get().*pmd)) -> enable_if_t<!is_function<T>::value && is_reference_wrapper<decay_t<RefWrap>>::value,
  decltype(ref.get().*pmd)> {
    return ref.get().*pmd;
}

template<class Fn, class... Args>
constexpr auto _invoke_helper(Fn &&f, Args &&...args) noexcept(
  noexcept(ala::forward<Fn>(f)(static_cast<Args&&>(args)...))) -> enable_if_t<!is_member_pointer<decay_t<Fn>>::value,
  decltype(ala::forward<Fn>(f)(static_cast<Args&&>(args)...))> {
    return ala::forward<Fn>(f)(static_cast<Args&&>(args)...);
}

template<class Fn, class... Args>
constexpr auto invoke(Fn &&f, Args &&...args) noexcept(
  noexcept(ala::_invoke_helper(ala::forward<Fn>(f), static_cast<Args&&>(args)...))) ->
  decltype(ala::_invoke_helper(ala::forward<Fn>(f), static_cast<Args&&>(args)...)) {
    return ala::_invoke_helper(ala::forward<Fn>(f), static_cast<Args&&>(args)...);
}

// clang-format on

// template<class Fn, class... Args>
// constexpr invoke_result_t<Fn, Args...>
// invoke(Fn &&f, Args &&...args) noexcept(is_nothrow_invocable<Fn, Args...>::value) {
//     return ala::_invoke(ala::forward<Fn>(f), static_cast<Args&&>(args)...);
// }

template<class T>
const T *addressof(const T &&) = delete;

template<class T>
constexpr T *addressof(T &arg) noexcept {
    return __builtin_addressof(arg);
}

template<class T>
struct reference_wrapper {
    using type = T;
    T *_ptr;

    static constexpr T &_test(T &t) noexcept {
        return t;
    }
    static constexpr void _test(T &&) = delete;

    template<class U, class = decltype(_test(declval<U>())),
             class = enable_if_t<!is_same<remove_cvref_t<U>, reference_wrapper>::value>>
    constexpr reference_wrapper(U &&u) noexcept(noexcept(_test(declval<U>())))
        : _ptr(ala::addressof(_test(ala::forward<U>(u)))) {}
    constexpr reference_wrapper(const reference_wrapper &) noexcept = default;
    constexpr reference_wrapper &
    operator=(const reference_wrapper &) noexcept = default;
    constexpr operator T &() const noexcept {
        return *_ptr;
    }

    constexpr T &get() const noexcept {
        return *_ptr;
    }

    template<class... Args>
    constexpr enable_if_t<is_invocable<T &, Args...>::value,
                          invoke_result_t<T &, Args...>>
    operator()(Args &&...args) const {
        return ala::invoke(get(), static_cast<Args &&>(args)...);
    }
};

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class T>
reference_wrapper(T &) -> reference_wrapper<T>;
#endif

template<class T>
reference_wrapper<T> ref(T &t) noexcept {
    return reference_wrapper<T>(t);
}
auto a = (unsigned)(-1);
template<class T>
reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
    return ala::ref(t.get());
}

template<class T>
void ref(const T &&) = delete;

template<class T>
reference_wrapper<const T> cref(const T &t) noexcept {
    return reference_wrapper<const T>(t);
}

template<class T>
reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept {
    return ala::cref(t.get());
}

template<class T>
void cref(const T &&) = delete;

} // namespace ala

#endif