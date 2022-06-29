#ifndef _ALA_DETAIL_ALLOCATOR_H
#define _ALA_DETAIL_ALLOCATOR_H

#include <ala/detail/memory_base.h>

#ifdef _ALA_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4348)
#endif

namespace ala {

#if _ALA_ENABLE_ALIGNED_NEW
using ::std::align_val_t;
#endif

using ::std::bad_array_new_length;
using ::std::bad_alloc;

template<class T>
struct allocator {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = add_lvalue_reference_t<T>;
    using const_reference = add_lvalue_reference_t<add_const_t<T>>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = true_type;
    template<class U>
    struct rebind {
        using other = allocator<U>;
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

    template<class U, class... Args>
    void construct(U *p, Args &&...args) {
        ala::construct_at<U>(p, ala::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U *p) {
        ala::destroy_at<U>(p);
    }

    size_type max_size() const noexcept {
        return numeric_limits<size_type>::max() / sizeof(value_type);
    }

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

template<class Alloc>
struct allocator_traits {
    using allocator_type = Alloc;
    using value_type = typename allocator_type::value_type;
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
        using type = typename T::template rebind<U>::other;
    };

    template<template<typename, typename...> class Templt, typename U,
             typename T, typename... Args>
    struct _get_rebind<Templt<T, Args...>, U, false> {
        using type = Templt<U, Args...>;
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
        void_t<decltype(declval<A>().construct(declval<P>(), declval<Args>()...))>,
        A, P, Args...>: true_type {};

    template<typename Void, typename A, typename P>
    struct _has_destroy: false_type {};

    template<typename A, typename P>
    struct _has_destroy<void_t<decltype(declval<A>().destroy(declval<P>()))>, A, P>
        : true_type {};

    template<typename Pointer, typename... Args>
    static enable_if_t<_has_construct<void, allocator_type &, Pointer, Args...>::value>
    construct(allocator_type &a, Pointer p, Args &&...args) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        // static_assert(is_same<T, value_type>::value,
        //               "Can not process incompatible type");
        a.construct(p, ala::forward<Args>(args)...);
    }

    template<typename Pointer, typename... Args>
    static enable_if_t<!_has_construct<void, allocator_type &, Pointer, Args...>::value>
    construct(allocator_type &a, Pointer p, Args &&...args) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        // static_assert(is_same<T, value_type>::value,
        //               "Can not process incompatible type");
        ala::construct_at<T>(ala::to_address(p), ala::forward<Args>(args)...);
    }

    template<typename Pointer>
    static enable_if_t<_has_destroy<void, allocator_type &, Pointer>::value>
    destroy(allocator_type &a, Pointer p) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        // static_assert(is_same<T, value_type>::value,
        //               "Can not process incompatible type");
        a.destroy(p);
    }

    template<typename Pointer>
    static enable_if_t<!_has_destroy<void, allocator_type &, Pointer>::value>
    destroy(allocator_type &a, Pointer p) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        // static_assert(is_same<T, value_type>::value,
        //               "Can not process incompatible type");
        ala::destroy_at<T>(ala::to_address(p));
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
            "or use stateless(is_always_equal) allocator"
            "Never define ALA_USE_ALLOC_REBIND in your project!!!");
#endif // always use rebind in always_equal allocator
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
            "it is necessary for ala's node-based container, "
            "or use stateless(is_always_equal is true) allocator. "
            "Never define ALA_USE_ALLOC_REBIND in your project!!!");
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

    template<typename Void, typename U, typename A, typename P, typename... Args>
    struct _has_construct_obj: false_type {};

    template<typename A, typename U, typename P, typename... Args>
    struct _has_construct_obj<
        void_t<decltype(declval<A &>().template construct_object<U>(
            declval<P>(), declval<Args>()...))>,
        U, A, P, Args...>: true_type {};

    template<typename Void, typename U, typename A, typename P>
    struct _has_destroy_obj: false_type {};

    template<typename A, typename U, typename P>
    struct _has_destroy_obj<
        void_t<decltype(declval<A &>().template destroy_object<U>(declval<P>()))>,
        U, A, P>: true_type {};

    template<typename U, typename Pointer, typename... Args>
    static enable_if_t<
        _has_construct_obj<void, U, allocator_type &, Pointer, Args...>::value>
    construct_object(allocator_type &a, Pointer p, Args &&...args) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        static_assert(is_same<T, remove_cv_t<U>>::value,
                      "Can not process incompatible type");
        a.template construct_object<U>(p, ala::forward<Args>(args)...);
    }

    template<typename U, typename Pointer, typename... Args>
    static enable_if_t<
        !_has_construct_obj<void, U, allocator_type &, Pointer, Args...>::value>
    construct_object(allocator_type &a, Pointer p, Args &&...args) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        static_assert(is_same<T, remove_cv_t<U>>::value,
                      "Can not process incompatible type");
        ala::construct_at<U>(ala::to_address(p), ala::forward<Args>(args)...);
    }

    template<typename U, typename Pointer>
    static enable_if_t<_has_destroy_obj<void, U, allocator_type &, Pointer>::value>
    destroy_object(allocator_type &a, Pointer p) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        static_assert(is_same<T, remove_cv_t<U>>::value,
                      "Can not process incompatible type");
        a.template destroy_object<U>(p);
    }

    template<typename U, typename Pointer>
    static enable_if_t<!_has_destroy_obj<void, U, allocator_type &, Pointer>::value>
    destroy_object(allocator_type &a, Pointer p) {
        using T = remove_cv_t<typename pointer_traits<Pointer>::element_type>;
        static_assert(is_same<T, remove_cv_t<U>>::value,
                      "Can not process incompatible type");
        ala::destroy_at<T>(ala::to_address(p));
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
        return numeric_limits<difference_type>::max() / sizeof(value_type);
    }
};

template<typename T, typename = void>
struct _is_allocator: false_type {};

template<typename Alloc>
struct _is_allocator<Alloc, void_t<typename Alloc::value_type,
                                   decltype(declval<Alloc &>().allocate(size_t{}))>>
    : true_type {};

} // namespace ala

template<class Pointer, class Alloc>
struct pointer_holder {
    Pointer _ptr = nullptr;
    Alloc &_alloc;
    size_t _size = 0;
    using _alloc_traits = ala::allocator_traits<Alloc>;

    pointer_holder(Alloc &a, size_t s): _alloc(a) {
        _ptr = _alloc.allocate(s);
        _size = s;
    }
    pointer_holder(Alloc &a, Pointer p, size_t s): _alloc(a) {
        _ptr = p;
        _size = s;
    }
    ~pointer_holder() {
        if (_ptr)
            _alloc.deallocate(_ptr, _size);
    }
    Pointer release() {
        Pointer tmp = _ptr;
        _ptr = nullptr;
        return tmp;
    }
    Pointer get() {
        return _ptr;
    }
};

template<class T, class Alloc>
struct pointer_holder<T *, Alloc> {
    using Pointer = T *;
    Pointer _ptr = nullptr;
    Alloc &_alloc;
    size_t _size = 0;
    using _alloc_traits = ala::allocator_traits<Alloc>;

    pointer_holder(Alloc &a, size_t s): _alloc(a) {
        _ptr = _alloc_traits::template allocate_object<T>(_alloc, s);
        _size = s;
    }
    pointer_holder(Alloc &a, Pointer p, size_t s): _alloc(a) {
        _ptr = p;
        _size = s;
    }
    ~pointer_holder() {
        if (_ptr)
            _alloc_traits::template deallocate_object<T>(_alloc, _ptr, _size);
    }
    Pointer release() {
        Pointer tmp = _ptr;
        _ptr = nullptr;
        return tmp;
    }
    Pointer get() {
        return _ptr;
    }
};

#ifdef _ALA_MSVC
    #pragma warning(pop)
#endif

#endif