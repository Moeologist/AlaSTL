#ifndef _ALA_DETAIL_UTILITY_BASE_H
#define _ALA_DETAIL_UTILITY_BASE_H

#ifndef _ALA_DETAIL_TRAITS_DECLARE_H
#error "never use this directly"
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
        "template argument substituting T can not be lvalue reference type");
    return static_cast<T &&>(t);
}

template<class T>
constexpr enable_if_t<is_move_constructible<T>::value && is_move_assignable<T>::value>
swap(T &lhs, T &rhs) noexcept(is_nothrow_move_constructible<T>::value
                                  &&is_nothrow_move_assignable<T>::value) {
    T tmp = move(lhs);
    lhs = move(rhs);
    rhs = move(tmp);
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

struct _invoker_mf {
    template<class Mf, class Class, class... Args>
    static decltype(auto) _call(Mf mf, Class &&ins, Args &&... args) noexcept(
        noexcept((ala::forward<Class>(ins).*mf)(ala::forward<Args>(args)...))) {
        return (ala::forward<Class>(ins).*mf)(ala::forward<Args>(args)...);
    }
};

struct _invoker_mf_refw {
    template<class Mf, class Class, class... Args>
    static decltype(auto)
    _call(Mf mf, Class &&ins, Args &&... args) noexcept(noexcept(
        (ala::forward<Class>(ins).get().*mf)(ala::forward<Args>(args)...))) {
        return (ala::forward<Class>(ins).get().*mf)(ala::forward<Args>(args)...);
    }
};

struct _invoker_mf_deref {
    template<class Mf, class Class, class... Args>
    static decltype(auto) _call(Mf mf, Class &&ins, Args &&... args) noexcept(
        noexcept(((*ala::forward<Class>(ins)).*mf)(ala::forward<Args>(args)...))) {
        return ((*ala::forward<Class>(ins)).*mf)(ala::forward<Args>(args)...);
    }
};

struct _invoker_mo {
    template<class Mo, class Class>
    static decltype(auto)
    _call(Mo mo, Class &&ins) noexcept(noexcept(ala::forward<Class>(ins).*mo)) {
        return ala::forward<Class>(ins).*mo;
    }
};

struct _invoker_mo_refw {
    template<class Mo, class Class>
    static decltype(auto)
    _call(Mo mo,
          Class &&ins) noexcept(noexcept(ala::forward<Class>(ins).get().*mo)) {
        return ala::forward<Class>(ins).get().*mo;
    }
};

struct _invoker_mo_deref {
    template<class Mo, class Class>
    static decltype(auto)
    _call(Mo mo, Class &&ins) noexcept(noexcept((*ala::forward<Class>(ins)).*mo)) {
        return (*ala::forward<Class>(ins)).*mo;
    }
};

struct _invoker_functor {
    template<class Call, class... Args>
    static decltype(auto) _call(Call &&call, Args &&... args) noexcept(
        noexcept(ala::forward<Call>(call)(ala::forward<Args>(args)...))) {
        return ala::forward<Call>(call)(ala::forward<Args>(args)...);
    }
};

template<class Call, class T, class RMcvref = remove_cvref_t<Call>,
         bool IsMfp = is_member_function_pointer<RMcvref>::value,
         bool IsMop = is_member_object_pointer<RMcvref>::value>
struct _invoker_helper;

template<class Mf, class Class, class RMcvref>
struct _invoker_helper<Mf, Class, RMcvref, true, false>
    : conditional_t<is_base_of<_class_of_memptr_t<Mf>, remove_reference_t<Class>>::value,
                    _invoker_mf,
                    conditional_t<is_specification<Class, reference_wrapper>::value,
                                  _invoker_mf_refw, _invoker_mf_deref>> {};

template<class Mo, class Class, class RMcvref>
struct _invoker_helper<Mo, Class, RMcvref, false, true>
    : conditional_t<is_base_of<_class_of_memptr_t<Mo>, remove_reference_t<Class>>::value,
                    _invoker_mo,
                    conditional_t<is_specification<Class, reference_wrapper>::value,
                                  _invoker_mo_refw, _invoker_mo_deref>> {};

template<class Call, class T, class RMcvref>
struct _invoker_helper<Call, T, RMcvref, false, false>: _invoker_functor {};

template<class Call, class... Args>
struct _invoker;

template<class Call>
struct _invoker<Call>: _invoker_functor {};

template<class Call, class T, class... Args>
struct _invoker<Call, T, Args...>: _invoker_helper<Call, T> {};

// use decltype(auto) trigger msvc bug
template<class Call, class... Args>
auto invoke(Call &&call, Args &&... args) noexcept(
    noexcept(_invoker<Call, Args...>::_call(ala::forward<Call>(call),
                                            ala::forward<Args>(args)...)))
    -> decltype(_invoker<Call, Args...>::_call(ala::forward<Call>(call),
                                               ala::forward<Args>(args)...)) {
    return (_invoker<Call, Args...>::_call(ala::forward<Call>(call),
                                           ala::forward<Args>(args)...));
}

} // namespace ala

#endif