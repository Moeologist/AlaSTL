#ifndef _ALA_DETAIL_ALLOCATOR_H
#define _ALA_DETAIL_ALLOCATOR_H

#include <ala/type_traits.h>

#define HAS_MEM(mem) \
    template<typename _type, typename = void_t<>> \
    struct _has_##mem: false_type {}; \
    template<typename _type> \
    struct _has_##mem<_type, void_t<typename _type::mem>>: true_type {};

#define HAS_MEM_TYPE(cls, mem, def) \
    template<typename _type, typename = void_t<>> \
    struct _##mem { \
        typedef def type; \
    }; \
    template<typename _type> \
    struct _##mem<_type, void_t<typename _type::mem>> { \
        typedef typename _type::mem type; \
    }; \
    typedef typename _##mem<cls>::type mem;

namespace ala {

template<class T>
constexpr T *addressof(T &arg) noexcept {
    return __builtin_addressof(arg);
}

template<typename Ptr>
struct pointer_traits {
    HAS_MEM(element_type)
    template<typename T, bool = _has_element_type<T>::value>
    struct _element_type {};

    template<typename T>
    struct _element_type<T, true> {
        typedef typename T::element_type type;
    };

    template<template<typename, typename...> class Template, typename T,
             typename... Args>
    struct _element_type<Template<T, Args...>, false> {
        typedef T type;
    };

    template<typename T, typename = void_t<>>
    struct _has_rebind_template: false_type {};

    template<typename T>
    struct _has_rebind_template<T, void_t<typename T::template rebind<void>>>
        : true_type {};

    template<typename T, typename U, bool = _has_rebind_template<T>::value>
    struct _rebind {};

    template<typename T, typename U>
    struct _rebind<T, U, true> {
        typedef typename T::template rebind<U> type;
    };

    template<template<typename, typename...> class Template, typename U,
             typename T, typename... Args>
    struct _rebind<Template<T, Args...>, U, false> {
        typedef Template<U, Args...> type;
    };

    typedef Ptr pointer;
    typedef typename _element_type<pointer>::type element_type;
    HAS_MEM_TYPE(pointer, difference_type, ptrdiff_t)

    template<typename U>
    using rebind = typename _rebind<pointer, U>::type;

    constexpr static pointer
    pointer_to(conditional_t<is_void_v<element_type>, void, element_type>
                   &r) noexcept {
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

    constexpr static pointer
    pointer_to(conditional_t<is_void_v<element_type>, void, element_type>
                   &r) noexcept {
        return ala::addressof(r);
    }
};

// clang-format off

template<class T>
struct allocator {
    typedef T         value_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef size_t    size_type;
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
        return static_cast<T *>(::operator new (n * sizeof(T)));
    }

    void deallocate(T *p, ala::size_t n) {
        ::operator delete (static_cast<void*>(p), n * sizeof(T));
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
    typedef Alloc                      allocator_type;
    typedef typename Alloc::value_type value_type;
    HAS_MEM_TYPE(allocator_type, pointer,            value_type*)
    HAS_MEM_TYPE(allocator_type, const_pointer,      typename pointer_traits<pointer>::template rebind<const value_type>)
    HAS_MEM_TYPE(allocator_type, void_pointer,       typename pointer_traits<pointer>::template rebind<void>)
    HAS_MEM_TYPE(allocator_type, const_void_pointer, typename pointer_traits<pointer>::template rebind<const void>)
    HAS_MEM_TYPE(allocator_type, difference_type,    typename pointer_traits<pointer>::difference_type)
    HAS_MEM_TYPE(allocator_type, size_type,          make_signed_t<difference_type>)
    HAS_MEM_TYPE(allocator_type, propagate_on_container_copy_assignment, false_type)
    HAS_MEM_TYPE(allocator_type, propagate_on_container_move_assignment, false_type)
    HAS_MEM_TYPE(allocator_type, propagate_on_container_swap,            false_type)
    HAS_MEM_TYPE(allocator_type, is_always_equal,                        false_type)
};

// clang-format on
} // namespace ala

#undef HAS_MEM
#undef HAS_MEM_TYPE

#endif