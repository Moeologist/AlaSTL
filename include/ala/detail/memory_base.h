#ifndef _ALA_DETAIL_MEMORY_BASE_H
#define _ALA_DETAIL_MEMORY_BASE_H

#include <new>
#include <ala/type_traits.h>
#include <ala/detail/macro.h>

namespace ala {

template<class T>
constexpr void *_voidify(T &obj) noexcept {
    return const_cast<void *>(
        static_cast<const volatile void *>(::ala::addressof(obj)));
}

} // namespace ala

ALA_BEGIN_NAMESPACE_STD

template<class T, class... Args>
constexpr T *_ala_construct_at(T *p, Args &&...args) {
    return ::new (::ala::_voidify(*p)) T(::ala::forward<Args>(args)...);
}

ALA_END_NAMESPACE_STD

namespace ala {

template<typename Ptr>
struct pointer_traits {
    ALA_HAS_MEM_TYPE(element_type)
    template<typename T, bool = _has_element_type<T>::value>
    struct _get_element_type;

    template<typename T>
    struct _get_element_type<T, true> {
        using type = typename T::element_type;
    };

    template<template<typename, typename...> class Templt, typename T, typename... Args>
    struct _get_element_type<Templt<T, Args...>, false> {
        using type = T;
    };

    template<typename T, typename U, typename = void>
    struct _has_rebind_template: false_type {};

    template<typename T, typename U>
    struct _has_rebind_template<T, U, void_t<typename T::template rebind<U>>>
        : true_type {};

    template<typename T, typename U, bool = _has_rebind_template<T, U>::value>
    struct _get_rebind;

    template<typename T, typename U>
    struct _get_rebind<T, U, true> {
        using type = typename T::template rebind<U>;
    };

    template<template<typename, typename...> class Templt, typename U,
             typename T, typename... Args>
    struct _get_rebind<Templt<T, Args...>, U, false> {
        using type = Templt<U, Args...>;
    };

    using pointer = Ptr;
    using element_type = typename _get_element_type<pointer>::type;
    ALA_HAS_MEM_TYPEDEF(pointer, difference_type, ptrdiff_t)

    template<typename U>
    using rebind = typename _get_rebind<pointer, U>::type;

    struct _dummy {};

    constexpr static pointer pointer_to(
        conditional_t<is_void<element_type>::value, _dummy, element_type> &r) noexcept {
        return pointer::pointer_to(r);
    }
};

template<typename T>
struct pointer_traits<T *> {
    using pointer = T *;
    using element_type = T;
    using difference_type = ptrdiff_t;

    template<typename U>
    using rebind = U *;

    struct _dummy {};
    constexpr static pointer pointer_to(
        conditional_t<is_void<element_type>::value, _dummy, element_type> &r) noexcept {
        return ala::addressof(r);
    }
};

template<class Ptr>
constexpr auto to_address(const Ptr &p) noexcept;

template<class T>
constexpr T *to_address(T *p) noexcept;

template<class Ptr, class = void>
struct _to_address_helper {
    static auto impl(const Ptr &p) noexcept {
        return ala::to_address(p.operator->());
    }
};

template<class Ptr>
struct _to_address_helper<
    Ptr, void_t<decltype(pointer_traits<Ptr>::to_address(declval<const Ptr &>()))>> {
    static auto impl(const Ptr &p) noexcept {
        return pointer_traits<Ptr>::to_address(p);
    }
};

template<class Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    return ala::_to_address_helper<Ptr>::impl(p);
}

template<class T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!is_function<T>::value, "Ill-formed, T is a function type");
    return p;
}

template<class T>
constexpr enable_if_t<!is_array<T>::value> destroy_at(T *p) {
    p->~T();
}

template<class T>
constexpr enable_if_t<is_array<T>::value> destroy_at(T *p) {
    for (auto &elem : *p)
        ala::destroy_at(ala::addressof(elem));
}

template<class T, class... Args>
constexpr T *construct_at(T *p, Args &&...args) {
    // std namespace make constexpr new happy
    return ::std::_ala_construct_at(p, ala::forward<Args>(args)...);
    // return ::new (const_cast<void *>(static_cast<const volatile void *>(
    //     ::ala::addressof(*p)))) T(::ala::forward<Args>(args)...);
}

} // namespace ala

#endif // HEAD