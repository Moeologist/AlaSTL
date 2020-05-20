#ifndef _ALA_DETAIL_ALLOCATOR_H
#define _ALA_DETAIL_ALLOCATOR_H

#include <new>

#include <ala/type_traits.h>
#include <ala/detail/macro.h>

#ifdef _ALA_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4348)
#endif

namespace ala {
#if _ALA_ENABLE_ALIGNED_NEW
using ::std::align_val_t;
#endif

using ::std::bad_array_new_length;

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
    struct _get_element_type;

    template<typename T>
    struct _get_element_type<T, true> {
        typedef typename T::element_type type;
    };

    template<template<typename, typename...> class Templt, typename T, typename... Args>
    struct _get_element_type<Templt<T, Args...>, false> {
        typedef T type;
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
        typedef typename T::template rebind<U> type;
    };

    template<template<typename, typename...> class Templt, typename U,
             typename T, typename... Args>
    struct _get_rebind<Templt<T, Args...>, U, false> {
        typedef Templt<U, Args...> type;
    };

    typedef Ptr pointer;
    typedef typename _get_element_type<pointer>::type element_type;
    ALA_HAS_MEM_TYPEDEF(pointer, difference_type, ptrdiff_t)

    template<typename U>
    using rebind = typename _get_rebind<pointer, U>::type;

    struct _dummy {};

    constexpr static pointer pointer_to(
        conditional_t<is_void<element_type>::value, _dummy, element_type &> r) noexcept {
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

    struct _dummy {};
    constexpr static pointer pointer_to(
        conditional_t<is_void<element_type>::value, _dummy, element_type> &r) noexcept {
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

    ALA_NODISCARD T *allocate(size_t n) {
#if _ALA_ENABLE_ALIGNED_NEW
        return static_cast<T *>(
            ::operator new(n * sizeof(T), (align_val_t)alignof(T)));
#else
        return static_cast<T *>(::operator new(n * sizeof(T)));
#endif
    }

    void deallocate(T *p, size_t n) {
#if _ALA_ENABLE_ALIGNED_NEW
        ::operator delete(static_cast<void *>(p), (align_val_t)alignof(T));
#else
        ::operator delete(static_cast<void *>(p));
#endif
    }

    // template<class U, class... Args>
    // void construct(U *p, Args &&... args) {
    //     ::new ((void *)p) U(ala::forward<Args>(args)...);
    // }

    // template<class U>
    // void destroy(U *p) {
    //     p->~U();
    // }

    ALA_NODISCARD void *allocate_bytes(size_t nbytes,
                                       size_t alignment = alignof(max_align_t)) {
        if (numeric_limits<size_t>::max() < nbytes)
            throw bad_array_new_length();
#if _ALA_ENABLE_ALIGNED_NEW
        return static_cast<void *>(::operator new(nbytes, (align_val_t)alignment));
#else
        return static_cast<void *>(::operator new(nbytes));
#endif
    }

    void deallocate_bytes(void *p, size_t nbytes,
                          size_t alignment = alignof(max_align_t)) {
#if _ALA_ENABLE_ALIGNED_NEW
        ::operator delete(static_cast<void *>(p), (align_val_t)alignment);
#else
        ::operator delete(static_cast<void *>(p));
#endif
    }

    template<class U>
    ALA_NODISCARD U *allocate_object(size_t n = 1) {
        return static_cast<U *>(this->allocate_bytes(n * sizeof(U), alignof(U)));
    }

    template<class U>
    void deallocate_object(U *p, size_t n = 1) {
        this->deallocate_bytes(p, n * sizeof(U), alignof(U));
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

template<class Alloc>
struct allocator_traits {
    typedef Alloc allocator_type;
    typedef typename allocator_type::value_type value_type;
    // clang-format off
    ALA_HAS_MEM_TYPEDEF(allocator_type, pointer,            value_type*)
    ALA_HAS_MEM_TYPEDEF(allocator_type, const_pointer,      typename pointer_traits<pointer>::template rebind<const value_type>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, void_pointer,       typename pointer_traits<pointer>::template rebind<void>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, const_void_pointer, typename pointer_traits<pointer>::template rebind<const void>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, difference_type,    typename pointer_traits<pointer>::difference_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, size_type,          make_unsigned_t<difference_type>)
    ALA_HAS_MEM_TYPEDEF(allocator_type, propagate_on_container_copy_assignment, false_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, propagate_on_container_move_assignment, false_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, propagate_on_container_swap,            false_type)
    ALA_HAS_MEM_TYPEDEF(allocator_type, is_always_equal,    typename is_empty<allocator_type>::type)
    // clang-format on

    template<typename T, typename U, typename = void>
    struct _has_rebind_template: false_type {};

    template<typename T, typename U>
    struct _has_rebind_template<T, U, void_t<typename T::template rebind<U>::other>>
        : true_type {};

    template<typename T, typename U, bool = _has_rebind_template<T, U>::value>
    struct _get_rebind;

    template<typename T, typename U>
    struct _get_rebind<T, U, true> {
        typedef typename T::template rebind<U>::other type;
    };

    template<template<typename, typename...> class Templt, typename U,
             typename T, typename... Args>
    struct _get_rebind<Templt<T, Args...>, U, false> {
        typedef Templt<U, Args...> type;
    };

    template<typename T>
    using rebind_alloc = typename _get_rebind<allocator_type, T>::type;

    template<typename T>
    using rebind_traits = ala::allocator_traits<rebind_alloc<T>>;

    template<typename Void, typename A>
    struct _has_select: false_type {};

    template<typename A>
    struct _has_select<
        void_t<decltype(declval<const A &>().select_on_container_copy_construction())>, A>
        : true_type {};

    template<typename Dummy = allocator_type>
    static enable_if_t<_has_select<void, Dummy>::value, allocator_type>
    select_on_container_copy_construction(const Dummy &a) {
        return a.select_on_container_copy_construction();
    }

    template<typename Dummy = allocator_type>
    static enable_if_t<!_has_select<void, Dummy>::value, allocator_type>
    select_on_container_copy_construction(const Dummy &a) {
        return a;
    }

    template<typename Void, typename A, typename P, typename... Args>
    struct _has_construct: false_type {};

    template<typename A, typename P, typename... Args>
    struct _has_construct<
        void_t<decltype(declval<A &>().construct(declval<P>(), declval<Args>()...))>,
        A, P, Args...>: true_type {};

    template<typename Void, typename A, typename P>
    struct _has_destroy: false_type {};

    template<typename A, typename P>
    struct _has_destroy<void_t<decltype(declval<A &>().destroy(declval<P>()))>, A, P>
        : true_type {};

    template<typename Pointer, typename... Args>
    static enable_if_t<_has_construct<void, allocator_type &, Pointer, Args...>::value>
    construct(allocator_type &a, Pointer p, Args &&... args) {
        using T = typename pointer_traits<Pointer>::element_type;
        static_assert(is_same<remove_cv_t<T>, value_type>::value,
                      "Can not process incompatible type");
        a.construct(p, ala::forward<Args>(args)...);
    }

    template<typename Pointer, typename... Args>
    static enable_if_t<!_has_construct<void, allocator_type &, Pointer, Args...>::value>
    construct(allocator_type &a, Pointer p, Args &&... args) {
        using T = typename pointer_traits<Pointer>::element_type;
        // static_assert(is_same<remove_cv_t<T>, value_type>::value,
        //               "Can not process incompatible type");
        void *raw = static_cast<void *>(ala::to_address(p));
        ::new (raw) T(ala::forward<Args>(args)...);
    }

    template<typename Pointer>
    static enable_if_t<_has_destroy<void, allocator_type &, Pointer>::value>
    destroy(allocator_type &a, Pointer p) {
        using T = typename pointer_traits<Pointer>::element_type;
        static_assert(is_same<remove_cv_t<T>, value_type>::value,
                      "Can not process incompatible type");
        a.destroy(p);
    }

    template<typename Pointer>
    static enable_if_t<!_has_destroy<void, allocator_type &, Pointer>::value>
    destroy(allocator_type &a, Pointer p) {
        using T = typename pointer_traits<Pointer>::element_type;
        // static_assert(is_same<remove_cv_t<T>, value_type>::value,
        //               "Can not process incompatible type");
        (*p).~T();
    }

    template<typename Void, typename A, typename U>
    struct _choose_alloc_obj_helper: integral_constant<int, 0> {};

    template<typename A, typename U>
    struct _choose_alloc_obj_helper<void_t<decltype(declval<A &>().allocate_bytes(
                                        declval<size_t>(), declval<size_t>()))>,
                                    A, U>: integral_constant<int, 1> {};

    template<typename Void, typename A, typename U>
    struct _choose_alloc_obj: _choose_alloc_obj_helper<void, A, U> {};

    template<typename A, typename U>
    struct _choose_alloc_obj<
        void_t<decltype(declval<A &>().template allocate_object<U>(declval<size_t>()))>,
        A, U>: integral_constant<int, 2> {};

    template<class U>
    ALA_NODISCARD static enable_if_t<
        _choose_alloc_obj<void, allocator_type, U>::value == 0, U *>
    allocate_object(allocator_type &a, size_t n = 1) {
#if !ALA_USE_ALLOC_REBIND
        static_assert(
            is_always_equal::value,
            "Your allocator has no allocate_object(or allocate_bytes), "
            "it is necessary for node-based container, "
            "or use stateless(is_always_equal) allocator");
#endif // always use rebind in is_always_equal allocator
        return ala::to_address(rebind_alloc<U>(a).allocate(n));
    }

    template<class U>
    ALA_NODISCARD static enable_if_t<
        _choose_alloc_obj<void, allocator_type, U>::value == 1, U *>
    allocate_object(allocator_type &a, size_t n = 1) {
        return a.allocate_bytes(n * sizeof(U), alignof(U));
    }

    template<class U>
    ALA_NODISCARD static enable_if_t<
        _choose_alloc_obj<void, allocator_type, U>::value == 2, U *>
    allocate_object(allocator_type &a, size_t n = 1) {
        return a.template allocate_object<U>(n);
    }

    template<typename Void, typename A, typename U>
    struct _choose_dealloc_obj_helper: integral_constant<int, 0> {};

    template<typename A, typename U>
    struct _choose_dealloc_obj_helper<
        void_t<decltype(declval<A &>().deallocate_bytes(
            declval<U *>(), declval<size_t>(), declval<size_t>()))>,
        A, U>: integral_constant<int, 1> {};

    template<typename Void, typename A, typename U>
    struct _choose_dealloc_obj: _choose_alloc_obj_helper<void, A, U> {};

    template<typename A, typename U>
    struct _choose_dealloc_obj<
        void_t<decltype(declval<A &>().template deallocate_object<U>(
            declval<U *>(), declval<size_t>()))>,
        A, U>: integral_constant<int, 2> {};

    template<class U>
    static enable_if_t<_choose_dealloc_obj<void, allocator_type, U>::value == 0>
    deallocate_object(allocator_type &a, U *p, size_t n = 1) {
#if !ALA_USE_ALLOC_REBIND
        static_assert(
            is_always_equal::value,
            "Your allocator has no allocate_object(or allocate_bytes), "
            "it is necessary for node-based container, "
            "or use stateless(is_always_equal) allocator");
#endif // always use rebind in is_always_equal allocator
        using pointer = typename rebind_traits<U>::pointer;
        pointer wp = pointer_traits<pointer>::pointer_to(*p);
        return rebind_alloc<U>(a).deallocate(wp, n);
    }

    template<class U>
    static enable_if_t<_choose_dealloc_obj<void, allocator_type, U>::value == 1>
    deallocate_object(allocator_type &a, U *p, size_t n = 1) {
        a.deallocate_bytes(p, n * sizeof(U), alignof(U));
    }

    template<class U>
    static enable_if_t<_choose_dealloc_obj<void, allocator_type, U>::value == 2>
    deallocate_object(allocator_type &a, U *p, size_t n = 1) {
        a.template deallocate_object<U>(p, n);
    }

    template<typename Void, typename A>
    struct _has_max_size: false_type {};

    template<typename A>
    struct _has_max_size<void_t<decltype(declval<const A &>().max_size())>, A>
        : true_type {};

    template<typename Dummy = allocator_type>
    static enable_if_t<_has_max_size<void, Dummy>::value, size_type>
    max_size(const allocator_type &a) {
        size_type sz = a.max_size();
        size_type mx = numeric_limits<difference_type>::max();
        return sz < mx ? sz : mx;
    }

    template<typename Dummy = allocator_type>
    static enable_if_t<!_has_max_size<void, Dummy>::value, size_type>
    max_size(const allocator_type &a) {
        return numeric_limits<difference_type>::max();
    }
};

} // namespace ala

#ifdef _ALA_MSVC
    #pragma warning(pop)
#endif

#endif