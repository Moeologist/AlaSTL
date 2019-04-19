#ifndef _ALA_DETAIL_ALLOCATOR_H
#define _ALA_DETAIL_ALLOCATOR_H

#include <ala/detail/external.h>
#include <ala/type_traits.h>
#include <ala/detail/macro.h>

namespace ala {

template<class T>
const T *addressof(const T &&) = delete;

template<class T>
constexpr T *addressof(T &arg) noexcept {
    return __builtin_addressof(arg);
}

template<typename Ptr>
struct pointer_traits {
    ALA_HAS_MEM_TYPE(element_type)
    template<typename T, bool = _has_element_type<T>::value>
    struct _get_element_type {};

    template<typename T>
    struct _get_element_type<T, true> {
        typedef typename T::element_type type;
    };

    template<template<typename, typename...> class Template, typename T, typename... Args>
    struct _get_element_type<Template<T, Args...>, false> {
        typedef T type;
    };

    template<template<typename...> class...>
    using void_template = void;

    template<typename T, typename = void_template<>>
    struct _has_rebind_template: false_type {};

    template<typename T>
    struct _has_rebind_template<T, void_template<T::template rebind>>
        : true_type {};

    template<typename T, typename U, bool = _has_rebind_template<T>::value>
    struct _get_rebind;

    template<typename T, typename U>
    struct _get_rebind<T, U, true> {
        typedef typename T::template rebind<U> type;
    };

    template<template<typename, typename...> class Template, typename U,
             typename T, typename... Args>
    struct _get_rebind<Template<T, Args...>, U, false> {
        typedef Template<U, Args...> type;
    };

    typedef Ptr pointer;
    typedef typename _get_element_type<pointer>::type element_type;
    ALA_HAS_MEM_TYPEDEF(pointer, difference_type, ptrdiff_t)

    template<typename U>
    using rebind = typename _get_rebind<pointer, U>::type;

    struct _void_dummy {};

    constexpr static pointer
    pointer_to(conditional_t<is_void<element_type>::value, _void_dummy,
                             element_type> &r) noexcept {
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

    struct _void_dummy {};
    constexpr static pointer
    pointer_to(conditional_t<is_void<element_type>::value, _void_dummy,
                             element_type> &r) noexcept {
        return ala::addressof(r);
    }
};

template<class T>
struct allocator {
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef true_type propagate_on_container_move_assignment;
    typedef true_type is_always_equal;
    template<class U>
    struct rebind {
        typedef allocator<U> other;
    };

    constexpr allocator() noexcept {};
    constexpr allocator(const allocator<T> &) noexcept {}
    template<typename U>
    constexpr allocator(const allocator<U> &) noexcept {}
    ~allocator() {}

    ALA_NODISCARD T *allocate(ala::size_t n) {
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    void deallocate(T *p, ala::size_t n) {
        ::operator delete(static_cast<void *>(p), n * sizeof(T));
    }

    template<class U, class... Args>
    void construct(U *p, Args &&... args) {
        ::new ((void *)p) U(ala::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U *p) {
        p->~U();
    }
};

template<class T, class U>
bool operator==(const allocator<T> &, const allocator<U> &) noexcept {
    return true;
}

template<class T, class U>
bool operator!=(const allocator<T> &, const allocator<U> &) noexcept {
    return false;
}

template<class Alloc>
struct allocator_traits {
    // clang-format off
    typedef Alloc                      allocator_type;
    typedef typename allocator_type::value_type value_type;
    ALA_HAS_MEM_TYPEDEF(allocator_type, pointer,            value_type*)
    ALA_HAS_MEM_TYPEDEF(allocator_type, const_pointer,      typename pointer_traits<pointer>::template rebind<const value_type>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, void_pointer,       typename pointer_traits<pointer>::template rebind<void>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, const_void_pointer, typename pointer_traits<pointer>::template rebind<const void>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, difference_type,    typename pointer_traits<pointer>::difference_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, size_type,          make_unsigned_t<difference_type>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, propagate_on_container_copy_assignment, false_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, propagate_on_container_move_assignment, false_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, propagate_on_container_swap,            false_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, is_always_equal,                        false_type)
    // clang-format on

    template<template<typename...> class...>
    using void_template = void;

    template<typename T, typename = void_template<>>
    struct _has_rebind_template: false_type {};

    template<typename T>
    struct _has_rebind_template<T, void_template<T::template rebind>>
        : true_type {};

    template<typename T, typename U, bool = _has_rebind_template<T>::value>
    struct _get_rebind;

    template<typename T, typename U>
    struct _get_rebind<T, U, true> {
        typedef typename T::template rebind<U>::other type;
    };

    template<template<typename, typename...> class Template, typename U,
             typename T, typename... Args>
    struct _get_rebind<Template<T, Args...>, U, false> {
        typedef Template<U, Args...> type;
    };

    template<typename T>
    using rebind_alloc = typename _get_rebind<allocator_type, T>::type;

    template<typename T>
    using rebind_traits = ala::allocator_traits<rebind_alloc<T>>;

    ALA_HAS_MEM(select_on_container_copy_construction)

    template<typename Dummy>
    static enable_if_t<_has_select_on_container_copy_construction<Dummy>::value, Dummy>
    _select_on_container_copy_construction(const Dummy &a) {
        return a.select_on_container_copy_construction();
    }

    template<typename Dummy>
    static enable_if_t<!_has_select_on_container_copy_construction<Dummy>::value, Dummy>
    _select_on_container_copy_construction(const Dummy &a) {
        return a;
    }

    template<typename Void, typename A, typename P, typename... Args>
    struct _has_construct: ala::false_type {};

    template<typename A, typename P, typename... Args>
    struct _has_construct<
        void_t<decltype(declval<A>().construct(declval<P>(), declval<Args>()...))>,
        A, P, Args...>: ala::true_type {};

    template<typename Void, typename A, typename P>
    struct _has_destroy: ala::false_type {};

    template<typename A, typename P>
    struct _has_destroy<void_t<decltype(declval<A>().destroy(declval<P>()))>, A, P>
        : ala::true_type {};

    template<typename T, typename... Args>
    static enable_if_t<_has_construct<void, allocator_type &, T *, Args...>::value>
    construct(allocator_type &a, T *p, Args &&... args) {
        a.construct(p, ala::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    static enable_if_t<!_has_construct<void, allocator_type &, T *, Args...>::value>
    construct(allocator_type &a, T *p, Args &&... args) {
        ::new ((void *)p) T(ala::forward<Args>(args)...);
    }

    template<typename T>
    static enable_if_t<_has_destroy<void, allocator_type &, T *>::value>
    destroy(allocator_type &a, T *p) {
        a.destroy(p);
    }

    template<typename T>
    static enable_if_t<!_has_destroy<void, allocator_type &, T *>::value>
    destroy(allocator_type &a, T *p) {
        p->~T();
    }

    static allocator_type
    select_on_container_copy_construction(const allocator_type &a) {
        return allocator_traits::template _select_on_container_copy_construction<
            allocator_type>(a);
    }
};

} // namespace ala

#endif