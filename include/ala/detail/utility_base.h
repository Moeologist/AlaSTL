#ifndef _ALA_DETAIL_UTILITY_BASE_H
#define _ALA_DETAIL_UTILITY_BASE_H

#ifndef _ALA_DETAIL_TRAITS_DECLARE_H
#error Never use this directly
#endif

namespace ala {

template<typename T>
struct reference_wrapper;

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
    static_assert(
        !is_lvalue_reference<T>::value,
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

template<class Base, class T, class Derived, class... Args>
auto _invoke_helper(T Base::*pmf, Derived &&ref, Args &&... args) -> enable_if_t<
    is_function<T>::value && !is_reference_wrapper<decay_t<Derived>>::value &&
        is_base_of<Base, decay_t<Derived>>::value,
    decltype((ala::forward<Derived>(ref).*pmf)(ala::forward<Args>(args)...))> {
    return (ala::forward<Derived>(ref).*pmf)(ala::forward<Args>(args)...);
}

template<class Base, class T, class Ptr, class... Args>
auto _invoke_helper(T Base::*pmf, Ptr &&ptr, Args &&... args) -> enable_if_t<
    is_function<T>::value && !is_reference_wrapper<decay_t<Ptr>>::value &&
        !is_base_of<Base, decay_t<Ptr>>::value,
    decltype(((*ala::forward<Ptr>(ptr)).*pmf)(ala::forward<Args>(args)...))> {
    return ((*ala::forward<Ptr>(ptr)).*pmf)(ala::forward<Args>(args)...);
}

template<class Base, class T, class RefWrap, class... Args>
auto _invoke_helper(T Base::*pmf, RefWrap &&ref, Args &&... args)
    -> enable_if_t<is_function<T>::value && is_reference_wrapper<decay_t<RefWrap>>::value,
                   decltype((ref.get().*pmf)(ala::forward<Args>(args)...))> {
    return (ref.get().*pmf)(ala::forward<Args>(args)...);
}

template<class Base, class T, class Derived>
auto _invoke_helper(T Base::*pmd, Derived &&ref)
    -> enable_if_t<!is_function<T>::value &&
                       !is_reference_wrapper<decay_t<Derived>>::value &&
                       is_base_of<Base, decay_t<Derived>>::value,
                   decltype(ala::forward<Derived>(ref).*pmd)> {
    return ala::forward<Derived>(ref).*pmd;
}

template<class Base, class T, class Ptr>
auto _invoke_helper(T Base::*pmd, Ptr &&ptr)
    -> enable_if_t<!is_function<T>::value && !is_reference_wrapper<decay_t<Ptr>>::value &&
                       !is_base_of<Base, decay_t<Ptr>>::value,
                   decltype((*ala::forward<Ptr>(ptr)).*pmd)> {
    return (*ala::forward<Ptr>(ptr)).*pmd;
}

template<class Base, class T, class RefWrap>
auto _invoke_helper(T Base::*pmd, RefWrap &&ref)
    -> enable_if_t<!is_function<T>::value &&
                       is_reference_wrapper<decay_t<RefWrap>>::value,
                   decltype(ref.get().*pmd)> {
    return ref.get().*pmd;
}

template<class Fn, class... Args>
auto _invoke_helper(Fn &&f, Args &&... args)
    -> enable_if_t<!is_member_pointer<decay_t<Fn>>::value,
                   decltype(ala::forward<Fn>(f)(ala::forward<Args>(args)...))> {
    return ala::forward<Fn>(f)(ala::forward<Args>(args)...);
}

template<class Fn, class... Args>
invoke_result_t<Fn, Args...>
invoke(Fn &&f,
       Args &&... args) noexcept(is_nothrow_invocable<Fn, Args...>::value) {
    static_assert(
        is_same<invoke_result_t<Fn, Args...>,
                decltype(_invoke_helper(ala::forward<Fn>(f),
                                        ala::forward<Args>(args)...))>::value,
        "invoke_result not compatible with invoke");
    return _invoke_helper(ala::forward<Fn>(f), ala::forward<Args>(args)...);
}

} // namespace ala

#endif