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
            ala::swap(*il, *ir);
    }
}

template<class T>
void _swap_adl(T &lhs, T &rhs) noexcept(is_nothrow_swappable<T>::value) {
    swap(lhs, rhs);
}

template<class Base, class T, class Derived, class... Args>
constexpr auto
_invoke_helper(T Base::*pmf, Derived &&ref, Args &&... args) -> enable_if_t<
    is_function<T>::value && !is_reference_wrapper<decay_t<Derived>>::value &&
        is_base_of<Base, decay_t<Derived>>::value,
    decltype((ala::forward<Derived>(ref).*pmf)(ala::forward<Args>(args)...))> {
    return (ala::forward<Derived>(ref).*pmf)(ala::forward<Args>(args)...);
}

template<class Base, class T, class Ptr, class... Args>
constexpr auto _invoke_helper(T Base::*pmf, Ptr &&ptr, Args &&... args)
    -> enable_if_t<
        is_function<T>::value && !is_reference_wrapper<decay_t<Ptr>>::value &&
            !is_base_of<Base, decay_t<Ptr>>::value,
        decltype(((*ala::forward<Ptr>(ptr)).*pmf)(ala::forward<Args>(args)...))> {
    return ((*ala::forward<Ptr>(ptr)).*pmf)(ala::forward<Args>(args)...);
}

template<class Base, class T, class RefWrap, class... Args>
constexpr auto _invoke_helper(T Base::*pmf, RefWrap &&ref, Args &&... args)
    -> enable_if_t<is_function<T>::value && is_reference_wrapper<decay_t<RefWrap>>::value,
                   decltype((ref.get().*pmf)(ala::forward<Args>(args)...))> {
    return (ref.get().*pmf)(ala::forward<Args>(args)...);
}

template<class Base, class T, class Derived>
constexpr auto _invoke_helper(T Base::*pmd, Derived &&ref)
    -> enable_if_t<!is_function<T>::value &&
                       !is_reference_wrapper<decay_t<Derived>>::value &&
                       is_base_of<Base, decay_t<Derived>>::value,
                   decltype(ala::forward<Derived>(ref).*pmd)> {
    return ala::forward<Derived>(ref).*pmd;
}

template<class Base, class T, class Ptr>
constexpr auto _invoke_helper(T Base::*pmd, Ptr &&ptr)
    -> enable_if_t<!is_function<T>::value && !is_reference_wrapper<decay_t<Ptr>>::value &&
                       !is_base_of<Base, decay_t<Ptr>>::value,
                   decltype((*ala::forward<Ptr>(ptr)).*pmd)> {
    return (*ala::forward<Ptr>(ptr)).*pmd;
}

template<class Base, class T, class RefWrap>
constexpr auto _invoke_helper(T Base::*pmd, RefWrap &&ref)
    -> enable_if_t<!is_function<T>::value &&
                       is_reference_wrapper<decay_t<RefWrap>>::value,
                   decltype(ref.get().*pmd)> {
    return ref.get().*pmd;
}

template<class Fn, class... Args>
constexpr auto _invoke_helper(Fn &&f, Args &&... args)
    -> enable_if_t<!is_member_pointer<decay_t<Fn>>::value,
                   decltype(ala::forward<Fn>(f)(ala::forward<Args>(args)...))> {
    return ala::forward<Fn>(f)(ala::forward<Args>(args)...);
}

template<class Fn, class... Args>
constexpr invoke_result_t<Fn, Args...>
invoke(Fn &&f,
       Args &&... args) noexcept(is_nothrow_invocable<Fn, Args...>::value) {
    static_assert(
        is_same<invoke_result_t<Fn, Args...>,
                decltype(_invoke_helper(ala::forward<Fn>(f),
                                        ala::forward<Args>(args)...))>::value,
        "invoke_result not compatible with invoke");
    return _invoke_helper(ala::forward<Fn>(f), ala::forward<Args>(args)...);
}


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

    static void _test(T &t) noexcept;
    static void _test(T &&) = delete;

    template<class U, class = decltype(_test(declval<U>())),
             class = enable_if_t<!is_same<remove_cvref_t<U>, reference_wrapper>::value>>
    reference_wrapper(U &&u) noexcept(noexcept(_test(declval<U>())))
        : _ptr(ala::addressof(ala::forward<U>(u))) {}
    reference_wrapper(const reference_wrapper &) noexcept = default;
    reference_wrapper &operator=(const reference_wrapper &) noexcept = default;
    operator T &() const noexcept {
        return *_ptr;
    }

    T &get() const noexcept {
        return *_ptr;
    }

    template<class... Args>
    enable_if_t<is_invocable<T &, Args...>::value, invoke_result_t<T &, Args...>>
    operator()(Args &&... args) const {
        return ala::invoke(get(), ala::forward<Args>(args)...);
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

template<class T>
reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
    return ref(t.get());
}

template<class T>
void ref(const T &&) = delete;

template<class T>
reference_wrapper<const T> cref(const T &t) noexcept {
    return reference_wrapper<const T>(t);
}

template<class T>
reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept {
    return cref(t.get());
}

template<class T>
void cref(const T &&) = delete;

} // namespace ala

#endif