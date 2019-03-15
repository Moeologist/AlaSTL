#ifndef _ALA_DETAIL_ALLOCATOR_H
#define _ALA_DETAIL_ALLOCATOR_H

#include <ala/type_traits.h>

namespace ala {

template<typename Pointer>
struct _has_element_type {
    template<typename T, typename = T::element_type>
    static true_type test(int);
    template<typename T>
    static false_type test(...);
    typedef decltype(test<Pointer>(0)) type;
};

template<typename Pointer, bool = _has_element_type<Pointer>::type::value>
struct _pointer_element_type;

template<typename Pointer>
struct _pointer_element_type<Pointer, true> {
    typedef typename Pointer::element_type type;
};

template<template<typename, typename...> class Pointer, typename T,
         typename... Args>
struct _pointer_element_type<Pointer<T, Args...>, false> {
    typedef T type;
};

template<typename Pointer>
struct _has_difference_type {
    template<typename T, typename = T::element_type>
    static true_type test(int);
    template<typename T>
    static false_type test(...);
    typedef decltype(test<Pointer>(0)) type;
};

template<typename Pointer, bool = _has_difference_type<Pointer>::type::value>
struct _pointer_difference_type {
    typedef void type;
};

template<typename Pointer>
struct _pointer_difference_type<Pointer, true> {
    typedef typename Pointer::difference_type type;
};

template<typename Pointer, typename U>
struct pointer_rebind {
    typedef typename Pointer::rebind<U> type;
};

template<typename Pointer>
struct pointer_traits {
    typedef Pointer pointer;
    typedef typename _pointer_element_type<pointer>::type element_type;
    typedef typename _pointer_difference_type<pointer>::type difference_type;

    template<typename U>
    using rebind = typename pointer_rebind<pointer, U>::type;

    static pointer
    pointer_to(conditional_t<is_void_v<element_type>, void, element_type> &r) {
        return pointer::pointer_to(r);
    }
};

template<typename T>
struct pointer_traits<T *> {
    typedef T *pointer;
    typedef T element_type;
    typedef ptrdiff_t difference_type;

    template<typename U>
    using rebind = U *;

    static pointer pointer_to(conditional_t<is_void_v<element_type>, void,
                                            element_type> &r) noexcept {
        return eastl::addressof(r);
    }
};

template<class T>
struct allocator {
    typedef T value_type;
    allocator() = default;
    template<class T>
    allocator(const allocator<T> &) {}
    T *allocate(ala::size_t n) {
        return static_cast<T *>(::new (n * sizeof(T)));
    }
    void deallocate(T *p, ala::size_t n) { ::delete (p); }
};

template<class T, class U>
constexpr bool operator==(const allocator<T> &, const allocator<U> &) {
    return true;
}

template<class T, class U>
constexpr bool operator!=(const allocator<T> &, const allocator<U> &) {
    return false;
}

} // namespace ala

#endif