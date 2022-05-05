#ifndef _ALA_MEMORY_H
#define _ALA_MEMORY_H

#include <ala/detail/allocator.h>
#include <ala/detail/uninitialized_memory.h>
#include <ala/detail/hash.h>
#include <ala/detail/functional_base.h>

#if ALA_USE_RTTI
    #include <typeinfo>
#endif

namespace ala {
#if ALA_USE_RTTI
using ::std::type_info;
#endif

template<class T, class Alloc, class = void>
struct _use_allocator_impl: false_type {};

template<class T, class Alloc>
struct _use_allocator_impl<
    T, Alloc, enable_if_t<is_convertible<Alloc, typename T::allocator_type>::value>>
    : true_type {};

template<class T, class Alloc>
struct uses_allocator: _use_allocator_impl<T, Alloc> {};

#ifdef _ALA_INLINE_VAE
template<class T, class Alloc>
inline constexpr bool uses_allocator_v = uses_allocator<T, Alloc>::value;
#endif

// uses-allocator construction
template<class T, class Alloc, class... Args>
constexpr auto uses_allocator_construction_args(const Alloc &alloc,
                                                Args &&...args) noexcept
    -> enable_if_t<!is_specification<T, pair>::value>;

template<class T, class Alloc, class Tuple1, class Tuple2>
constexpr auto uses_allocator_construction_args(const Alloc &alloc,
                                                piecewise_construct_t,
                                                Tuple1 &&x, Tuple2 &&y) noexcept
    -> enable_if_t<is_specification<T, pair>::value>;

template<class T, class Alloc>
constexpr auto uses_allocator_construction_args(const Alloc &alloc) noexcept
    -> enable_if_t<is_specification<T, pair>::value>;

template<class T, class Alloc, class U, class V>
constexpr auto uses_allocator_construction_args(const Alloc &alloc, U &&u,
                                                V &&v) noexcept
    -> enable_if_t<is_specification<T, pair>::value>;

template<class T, class Alloc, class U, class V>
constexpr auto uses_allocator_construction_args(const Alloc &alloc,
                                                const pair<U, V> &pr) noexcept
    -> enable_if_t<is_specification<T, pair>::value>;

template<class T, class Alloc, class U, class V>
constexpr auto uses_allocator_construction_args(const Alloc &alloc,
                                                pair<U, V> &&pr) noexcept
    -> enable_if_t<is_specification<T, pair>::value>;

template<class T, class Alloc, class... Args>
constexpr T make_obj_using_allocator(const Alloc &alloc, Args &&...args);

template<class T, class Alloc, class... Args>
T *uninitialized_construct_using_allocator(T *p, const Alloc &alloc,
                                           Args &&...args);

// pointer alignment
inline void *align(size_t alignment, size_t size, void *&ptr, size_t &space) {
    size_t masked = reinterpret_cast<uintptr_t>(ptr) & (alignment - 1);
    size_t offset = 0;
    if (masked != 0)
        offset = alignment - masked;
    if (space < offset || space - offset < size)
        return nullptr;
    ptr = static_cast<void *>(static_cast<char *>(ptr) + offset);
    space -= offset;
    return ptr;
}

template<size_t N, class T>
[[nodiscard]] constexpr T *assume_aligned(T *ptr);

// class template unique_­ptr
template<class T>
struct default_delete {
    constexpr default_delete() noexcept = default;
    template<class U, class = enable_if_t<is_convertible<U *, T *>::value>>
    default_delete(const default_delete<U> &) noexcept {}
    void operator()(T *ptr) const {
        static_assert(sizeof(T) > 0,
                      "default_delete can not delete incomplete type");
        delete ptr;
    }
};

template<class T>
struct default_delete<T[]> {
    constexpr default_delete() noexcept = default;
    template<class U, class = enable_if_t<is_convertible<U (*)[], T (*)[]>::value>>
    default_delete(const default_delete<U[]> &) noexcept {}
    template<class U>
    enable_if_t<is_convertible<U (*)[], T (*)[]>::value> operator()(U *ptr) const {
        static_assert(sizeof(T) > 0,
                      "default_delete can not delete incomplete type");
        delete[] ptr;
    }
};

template<class T, class Deleter = default_delete<T>>
class unique_ptr {
public:
    ALA_HAS_MEM_TYPEDEF(remove_reference_t<Deleter>, pointer, T *)
    using element_type = T;
    using deleter_type = Deleter;

private:
    pointer _ptr;
    deleter_type _deleter;

    template<class, class>
    friend class unique_ptr;

public:
    // constructors
    template<class Dummy = deleter_type,
             class = enable_if_t<!is_pointer<Dummy>::value &&
                                 is_default_constructible<Dummy>::value>>
    constexpr unique_ptr() noexcept: _ptr(), _deleter() {}

    template<class Dummy = deleter_type,
             class = enable_if_t<!is_pointer<Dummy>::value &&
                                 is_default_constructible<Dummy>::value>>
    constexpr unique_ptr(nullptr_t) noexcept: _ptr(), _deleter() {}

    template<class Dummy = deleter_type,
             class = enable_if_t<!is_pointer<Dummy>::value &&
                                 is_default_constructible<Dummy>::value>>
    explicit unique_ptr(pointer p) noexcept: _ptr(p), _deleter() {}

#if ALA_LANG > 201703L
    template<class Dummy = deleter_type,
             class = enable_if_t<is_copy_constructible<Dummy>::value>>
    unique_ptr(pointer p, const deleter_type &lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}

    template<class Dummy = deleter_type, char = 0,
             class = enable_if_t<!is_reference<Dummy>::value &&
                                 is_move_constructible<Dummy>::value>>
    unique_ptr(pointer p, remove_reference_t<deleter_type> &&rref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(rref)>(rref)) {}

    template<class Dummy = deleter_type, short = 0,
             class = enable_if_t<is_reference<Dummy>::value &&
                                 is_move_constructible<Dummy>::value>>
    unique_ptr(pointer p, remove_reference_t<deleter_type> &&rref) = delete;
#else
    template<class Dummy = deleter_type,
             class = enable_if_t<!is_reference<Dummy>::value &&
                                 is_copy_constructible<Dummy>::value>>
    unique_ptr(pointer p, const deleter_type &lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class Dummy = deleter_type, char = 0,
             class = enable_if_t<!is_reference<Dummy>::value &&
                                 is_move_constructible<Dummy>::value>>
    unique_ptr(pointer p, deleter_type &&rref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(rref)>(rref)) {}

    template<class Dummy = deleter_type, short = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 !is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(pointer p, deleter_type lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class Dummy = deleter_type, int = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 !is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(pointer p, remove_reference_t<deleter_type> &&rref) = delete;

    template<class Dummy = deleter_type, long = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(pointer p, deleter_type lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class Dummy = deleter_type, long long = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(pointer p, const remove_reference_t<deleter_type> &&rref) = delete;
#endif

    template<class Dummy = deleter_type,
             class = enable_if_t<is_move_constructible<Dummy>::value>>
    unique_ptr(unique_ptr &&other) noexcept
        : _ptr(other._ptr), _deleter(ala::forward<deleter_type>(other._deleter)) {
        other._ptr = nullptr;
    }

    template<class U, class E,
             class = enable_if_t<
                 is_convertible<typename unique_ptr<U, E>::pointer, pointer>::value &&
                 !is_array<U>::value &&
                 ((is_reference<deleter_type>::value && is_same<deleter_type, E>::value) ||
                  (!is_reference<deleter_type>::value &&
                   is_convertible<E, deleter_type>::value))>>
    unique_ptr(unique_ptr<U, E> &&other) noexcept
        : _ptr(other._ptr), _deleter(ala::forward<E>(other._deleter)) {
        other._ptr = nullptr;
    }

    // destructor
    ~unique_ptr() {
        reset(nullptr);
    }

    // assignment
    struct _dummy {
        _dummy() = delete;
        _dummy(const _dummy &) = delete;
        _dummy(_dummy &&) = delete;
    };

    unique_ptr &
    operator=(conditional_t</*is_move_assignable<deleter_type>::value*/ true,
                            unique_ptr &&, _dummy>
                  other) noexcept {
        this->reset(other.release());
        get_deleter() = ala::forward<deleter_type>(other.get_deleter());
        return *this;
    }

    template<class U, class E>
    enable_if_t<is_convertible<typename unique_ptr<U, E>::pointer, pointer>::value &&
                    !is_array<U>::value && is_assignable<deleter_type &, E &&>::value,
                unique_ptr &>
    operator=(unique_ptr<U, E> &&other) noexcept {
        this->reset(other.release());
        get_deleter() = ala::forward<E>(other.get_deleter());
        return *this;
    }

    unique_ptr &operator=(nullptr_t) noexcept {
        reset(nullptr);
        return *this;
    }

    // observers
    add_lvalue_reference_t<T> operator*() const {
        return *get();
    }

    pointer operator->() const noexcept {
        return get();
    }

    pointer get() const noexcept {
        return _ptr;
    }

    deleter_type &get_deleter() noexcept {
        return _deleter;
    }

    const deleter_type &get_deleter() const noexcept {
        return _deleter;
    }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    // modifiers
    pointer release() noexcept {
        pointer p = _ptr;
        _ptr = nullptr;
        return p;
    }

    void reset(pointer p = pointer()) noexcept {
        pointer tmp = _ptr;
        _ptr = p;
        if (tmp != nullptr)
            get_deleter()(tmp);
    }

    void swap(unique_ptr &other) noexcept {
        ala::_swap_adl(_ptr, other._ptr);
        ala::_swap_adl(_deleter, other._deleter);
    }

    // disable copy from lvalue
    unique_ptr(const unique_ptr &) = delete;
    unique_ptr &operator=(const unique_ptr &) = delete;
};

template<class T, class Deleter>
class unique_ptr<T[], Deleter> {
public:
    ALA_HAS_MEM_TYPEDEF(remove_reference_t<Deleter>, pointer, T *)
    using element_type = T;
    using deleter_type = Deleter;

private:
    pointer _ptr;
    deleter_type _deleter;

    template<class U>
    using _ptr_checker =
        _or_<is_same<U, pointer>,
             _and_<is_same<pointer, element_type *>, is_pointer<U>,
                   is_convertible<remove_pointer_t<U> (*)[], element_type (*)[]>>>;

    template<class, class>
    friend class unique_ptr;

public:
    // constructors

    template<class Dummy = deleter_type,
             class = enable_if_t<!is_pointer<Dummy>::value &&
                                 is_default_constructible<Dummy>::value>>
    constexpr unique_ptr() noexcept: _ptr(), _deleter() {}

    template<class Dummy = deleter_type,
             class = enable_if_t<!is_pointer<Dummy>::value &&
                                 is_default_constructible<Dummy>::value>>
    constexpr unique_ptr(nullptr_t) noexcept: _ptr(), _deleter() {}

    template<class U, class = enable_if_t<_ptr_checker<U>::value>>
    explicit unique_ptr(U p) noexcept: _ptr(p), _deleter() {}

#if ALA_LANG > 201703L
    template<class U,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 is_copy_constructible<deleter_type>::value>>
    unique_ptr(U p, const deleter_type &lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}

    template<class U, char = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 !is_reference<deleter_type>::value &&
                                 is_move_constructible<deleter_type>::value>>
    unique_ptr(U p, remove_reference_t<deleter_type> &&rref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(rref)>(rref)) {}

    template<class U, short = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 is_reference<deleter_type>::value &&
                                 is_move_constructible<deleter_type>::value>>
    unique_ptr(U p, remove_reference_t<deleter_type> &&rref) = delete;
#else
    template<class U,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 !is_reference<deleter_type>::value &&
                                 is_copy_constructible<deleter_type>::value>>
    unique_ptr(U p, const deleter_type &lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class U, char = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 !is_reference<deleter_type>::value &&
                                 is_move_constructible<deleter_type>::value>>
    unique_ptr(U p, deleter_type &&rref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(rref)>(rref)) {}

    template<class U, short = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 is_lvalue_reference<deleter_type>::value &&
                                 !is_const<remove_reference_t<deleter_type>>::value>>
    unique_ptr(U p, deleter_type lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class U, int = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 is_lvalue_reference<deleter_type>::value &&
                                 !is_const<remove_reference_t<deleter_type>>::value>>
    unique_ptr(U p, remove_reference_t<deleter_type> &&rref) = delete;

    template<class U, long = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 is_lvalue_reference<deleter_type>::value &&
                                 is_const<remove_reference_t<deleter_type>>::value>>
    unique_ptr(U p, deleter_type lref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class U, long long = 0,
             class = enable_if_t<(is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                                 is_lvalue_reference<deleter_type>::value &&
                                 is_const<remove_reference_t<deleter_type>>::value>>
    unique_ptr(U p, const remove_reference_t<deleter_type> &&rref) = delete;
#endif

// fix libc++/test/std/utilities/smartptr/unique.ptr/unique.ptr.class/unique.ptr.ctor/null.pass.cpp
// extras
#if ALA_LANG > 201703L
    template<class Dummy = deleter_type,
             class = enable_if_t<is_copy_constructible<Dummy>::value>>
    unique_ptr(nullptr_t, const deleter_type &lref) noexcept
        : _ptr(nullptr), _deleter(ala::forward<decltype(lref)>(lref)) {}

    template<class Dummy = deleter_type, char = 0,
             class = enable_if_t<!is_reference<Dummy>::value &&
                                 is_move_constructible<Dummy>::value>>
    unique_ptr(nullptr_t, remove_reference_t<deleter_type> &&rref) noexcept
        : _ptr(nullptr), _deleter(ala::forward<decltype(rref)>(rref)) {}

    template<class Dummy = deleter_type, short = 0,
             class = enable_if_t<is_reference<Dummy>::value &&
                                 is_move_constructible<Dummy>::value>>
    unique_ptr(nullptr_t, remove_reference_t<deleter_type> &&rref) = delete;
#else
    template<class Dummy = deleter_type,
             class = enable_if_t<!is_reference<Dummy>::value &&
                                 is_copy_constructible<Dummy>::value>>
    unique_ptr(nullptr_t, const deleter_type &lref) noexcept
        : _ptr(nullptr), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class Dummy = deleter_type, char = 0,
             class = enable_if_t<!is_reference<Dummy>::value &&
                                 is_move_constructible<Dummy>::value>>
    unique_ptr(nullptr_t, deleter_type &&rref) noexcept
        : _ptr(nullptr), _deleter(ala::forward<decltype(rref)>(rref)) {}

    template<class Dummy = deleter_type, short = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 !is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(nullptr_t, deleter_type lref) noexcept
        : _ptr(nullptr), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class Dummy = deleter_type, int = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 !is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(nullptr_t, remove_reference_t<deleter_type> &&rref) = delete;

    template<class Dummy = deleter_type, long = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(nullptr_t, deleter_type lref) noexcept
        : _ptr(nullptr), _deleter(ala::forward<decltype(lref)>(lref)) {}
    template<class Dummy = deleter_type, long long = 0,
             class = enable_if_t<is_lvalue_reference<Dummy>::value &&
                                 is_const<remove_reference_t<Dummy>>::value>>
    unique_ptr(nullptr_t, const remove_reference_t<deleter_type> &&rref) = delete;
#endif

    template<class Dummy = deleter_type,
             class = enable_if_t<is_move_constructible<Dummy>::value>>
    unique_ptr(unique_ptr &&other) noexcept
        : _ptr(other._ptr), _deleter(ala::forward<deleter_type>(other._deleter)) {
        other._ptr = nullptr;
    }

    template<class U, class E,
             class = enable_if_t<
                 is_array<U>::value && is_same<pointer, element_type *>::value &&
                 is_convertible<typename unique_ptr<U, E>::element_type (*)[],
                                element_type (*)[]>::value &&
                 ((is_reference<deleter_type>::value && is_same<deleter_type, E>::value) ||
                  (!is_reference<deleter_type>::value &&
                   is_convertible<E, deleter_type>::value))>>
    unique_ptr(unique_ptr<U, E> &&other) noexcept
        : _ptr(other._ptr), _deleter(ala::forward<E>(other._deleter)) {
        other._ptr = nullptr;
    }

    // destructor
    ~unique_ptr() {
        reset(nullptr);
    }

    // assignment
    struct _dummy {
        _dummy() = delete;
        _dummy(const _dummy &) = delete;
        _dummy(_dummy &&) = delete;
    };

    unique_ptr &
    operator=(conditional_t</*is_move_assignable<deleter_type>::value*/ true,
                            unique_ptr &&, _dummy>
                  other) noexcept {
        this->reset(other.release());
        get_deleter() = ala::forward<deleter_type>(other.get_deleter());
        return *this;
    }

    template<class U, class E>
    enable_if_t<is_array<U>::value && is_same<pointer, element_type *>::value &&
                    is_convertible<typename unique_ptr<U, E>::element_type (*)[],
                                   element_type (*)[]>::value &&
                    is_assignable<deleter_type &, E &&>::value,
                unique_ptr &>
    operator=(unique_ptr<U, E> &&other) noexcept {
        this->reset(other.release());
        get_deleter() = ala::forward<E>(other.get_deleter());
        return *this;
    }

    unique_ptr &operator=(nullptr_t) noexcept {
        reset(nullptr);
        return *this;
    }

    // observers
    T &operator[](size_t i) const {
        return get()[i];
    }

    pointer get() const noexcept {
        return _ptr;
    }

    deleter_type &get_deleter() noexcept {
        return _deleter;
    }

    const deleter_type &get_deleter() const noexcept {
        return _deleter;
    }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    // modifiers
    pointer release() noexcept {
        pointer p = _ptr;
        _ptr = nullptr;
        return p;
    }

    template<class U>
    enable_if_t<_ptr_checker<U>::value> reset(U p) noexcept {
        pointer tmp = _ptr;
        _ptr = p;
        if (tmp != nullptr)
            get_deleter()(tmp);
    }

    void reset(nullptr_t p = nullptr) noexcept {
        pointer tmp = _ptr;
        _ptr = p;
        if (tmp != nullptr)
            get_deleter()(tmp);
    }

    void swap(unique_ptr &other) noexcept {
        ala::_swap_adl(_ptr, other._ptr);
        ala::_swap_adl(_deleter, other._deleter);
    }

    // disable copy from lvalue
    unique_ptr(const unique_ptr &) = delete;
    unique_ptr &operator=(const unique_ptr &) = delete;
};

template<class T>
enable_if_t<is_unbounded_array<T>::value, unique_ptr<T>> make_unique(size_t n) {
    return unique_ptr<T>(::new remove_extent_t<T>[n]());
}

template<class T, class... Args>
enable_if_t<is_bounded_array<T>::value, unique_ptr<T>>
make_unique(Args &&...) = delete;

template<class T, class... Args>
enable_if_t<!is_array<T>::value, unique_ptr<T>> make_unique(Args &&...args) {
    return unique_ptr<T>(new T(ala::forward<Args>(args)...));
}

template<class T>
enable_if_t<is_unbounded_array<T>::value, unique_ptr<T>>
make_unique_for_overwrite(size_t n) {
    return unique_ptr<T>(new remove_extent_t<T>[n]);
}

template<class T, class... Args>
enable_if_t<is_bounded_array<T>::value, unique_ptr<T>>
make_unique_for_overwrite(Args &&...) = delete;

template<class T>
enable_if_t<!is_array<T>::value, unique_ptr<T>> make_unique_for_overwrite() {
    return unique_ptr<T>(new T);
}

template<class T, class D>
enable_if_t<is_swappable<D>::value>
swap(unique_ptr<T, D> &lhs,
     unique_ptr<T, D> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template<class T1, class D1, class T2, class D2>
bool operator==(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) {
    return lhs.get() == rhs.get();
}

template<class T1, class D1, class T2, class D2>
bool operator!=(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) {
    return lhs.get() != rhs.get();
}

template<class T1, class D1, class T2, class D2>
bool operator<(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) {
    return less<common_type_t<typename unique_ptr<T1, D1>::pointer,
                              typename unique_ptr<T2, D2>::pointer>>{}(lhs.get(),
                                                                       rhs.get());
}

template<class T1, class D1, class T2, class D2>
bool operator>(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) {
    return rhs < lhs;
}

template<class T1, class D1, class T2, class D2>
bool operator<=(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) {
    return !(rhs < lhs);
}

template<class T1, class D1, class T2, class D2>
bool operator>=(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) {
    return !(lhs < rhs);
}

template<class T, class D>
bool operator==(const unique_ptr<T, D> &lhs, nullptr_t) noexcept {
    return !lhs;
}

template<class T, class D>
bool operator==(nullptr_t, const unique_ptr<T, D> &rhs) noexcept {
    return !rhs;
}

template<class T, class D>
bool operator!=(const unique_ptr<T, D> &lhs, nullptr_t) noexcept {
    return (bool)lhs;
}

template<class T, class D>
bool operator!=(nullptr_t, const unique_ptr<T, D> &rhs) noexcept {
    return (bool)rhs;
}

template<class T, class D>
bool operator<(const unique_ptr<T, D> &lhs, nullptr_t) {
    return less<typename unique_ptr<T, D>::pointer>{}(lhs.get(), nullptr);
}

template<class T, class D>
bool operator<(nullptr_t, const unique_ptr<T, D> &rhs) {
    return less<typename unique_ptr<T, D>::pointer>{}(nullptr, rhs.get());
}

template<class T, class D>
bool operator>(const unique_ptr<T, D> &lhs, nullptr_t) {
    return nullptr < lhs;
}

template<class T, class D>
bool operator>(nullptr_t, const unique_ptr<T, D> &rhs) {
    return rhs < nullptr;
}

template<class T, class D>
bool operator<=(const unique_ptr<T, D> &lhs, nullptr_t) {
    return !(nullptr < lhs);
}

template<class T, class D>
bool operator<=(nullptr_t, const unique_ptr<T, D> &rhs) {
    return !(rhs < nullptr);
}

template<class T, class D>
bool operator>=(const unique_ptr<T, D> &lhs, nullptr_t) {
    return !(lhs < nullptr);
}
template<class T, class D>
bool operator>=(nullptr_t, const unique_ptr<T, D> &rhs) {
    return !(nullptr < rhs);
}

struct bad_weak_ptr: exception {
    bad_weak_ptr() noexcept {}
    virtual char const *what() const noexcept {
        return "bad_weak_ptr";
    }
};

struct _ctblk_base {
    size_t _weak = 1;
    size_t _shared = 1;
    virtual ~_ctblk_base() {}
    virtual void inc_shared() = 0;
    virtual void dec_shared() = 0;
    virtual size_t get_shared() = 0;
    virtual void inc_weak() = 0;
    virtual void dec_weak() = 0;
    virtual size_t get_weak() = 0;
#if ALA_USE_RTTI
    virtual void *get_deleter(const type_info &info) = 0;
#endif
};

template<class Pointer, class Deleter, class Alloc, size_t AllocOnce = 0>
struct _ctblk: _ctblk_base {
    // TODO atomic, thread safety
    Pointer _ptr;
    Deleter _deleter;
    Alloc _alloc;

    _ctblk(Pointer p, Deleter d, Alloc a)
        : _ptr(p), _deleter(ala::move(d)), _alloc(a) {}

    ~_ctblk(){};

    void inc_shared() override {
        ++_weak;
        ++_shared;
    }

    void dec_shared() override {
        --_shared;
        if (_shared == 0) {
            Pointer tmp = _ptr;
            _ptr = nullptr;
            _deleter(tmp);
        }
        dec_weak();
    }

    size_t get_shared() override {
        return _shared;
    }

    void inc_weak() override {
        ++_weak;
    }

    void dec_weak() override {
        --_weak;
        if (_weak == 0) {
            using traits_t = allocator_traits<Alloc>;
            using ph_t = conditional_t<(AllocOnce == 0), _ctblk,
                                       aligned_storage_t<AllocOnce>>;
            Alloc a = _alloc;
            traits_t::template destroy_object<_ctblk>(a, this);
            traits_t::template deallocate_object<ph_t>(a,
                                                       reinterpret_cast<ph_t *>(
                                                           this));
        }
    }

    size_t get_weak() override {
        return _weak;
    }
#if ALA_USE_RTTI
    void *get_deleter(const type_info &info) override {
        if (info == typeid(Deleter))
            return ala::addressof(_deleter);
        return nullptr;
    }
#endif
};

// class template shared_ptr
template<class T>
class weak_ptr;

template<class T>
class enable_shared_from_this;

template<class T>
class shared_ptr {
public:
    using element_type = remove_extent_t<T>;
    using weak_type = weak_ptr<T>;

private:
    _ctblk_base *_cb = nullptr;
    element_type *_ptr = nullptr;

    template<class Y, class = void>
    struct _is_del_plain: false_type {};

    template<class Y>
    struct _is_del_plain<Y, void_t<decltype(delete declval<Y *&>())>>
        : true_type {};

    template<class Y, class = void>
    struct _is_del_array: false_type {};

    template<class Y>
    struct _is_del_array<Y, void_t<decltype(delete[] declval<Y *&>())>>
        : true_type{};

    template<class Y>
    struct _is_deletable
        : conditional_t<is_array<T>::value, _is_del_array<Y>, _is_del_plain<Y>> {
    };

    template<class Pointer, class D, class = void>
    struct _check_deleter: false_type {};

    template<class Pointer, class D>
    struct _check_deleter<Pointer, D,
                          void_t<decltype(declval<D &>()(declval<Pointer &>()))>>
        : true_type {};

    // template<class Array, class Y>
    // struct _is_convable_helper: is_convertible<Y *, T *> {};

    // template<class U, size_t N, class Y>
    // struct _is_convable_helper<U[N], Y>: is_convertible<Y (*)[N], T *> {};

    // template<class U, class Y>
    // struct _is_convable_helper<U[], Y>: is_convertible<Y (*)[], T *> {};

    template<class Y>
    struct _is_convable
        : is_convertible<remove_extent_t<Y> *, remove_extent_t<T> *> {};

    template<class Derived, class Y>
    enable_if_t<is_convertible<Derived *, const enable_shared_from_this<Y> *>::value>
    _weak_this_helper(Derived *p,
                      const enable_shared_from_this<Y> *base) noexcept {
        using rmcv_t = remove_cv_t<Y>;
        if (base && base->weak_this.expired())
            const_cast<enable_shared_from_this<Y> *>(base)->weak_this =
                shared_ptr<rmcv_t>(*this, const_cast<rmcv_t *>(
                                              static_cast<const Y *>(p)));
    }

    void _weak_this_helper(...) noexcept {}

    void _weak_this() noexcept {
        this->_weak_this_helper(_ptr, _ptr);
    }

    template<class Pointer, class D, class A>
    void _construct(Pointer p, D d, A a, bool delete_on_excpetion = true) {
        static_assert(is_pointer<Pointer>::value ||
                          is_null_pointer<Pointer>::value,
                      "Internal error");
        using traits_t = allocator_traits<A>;
        using blk_t = _ctblk<Pointer, D, A>;
        using holder_t = pointer_holder<blk_t *, A>;
        try {
            holder_t holder(a, 1);
            traits_t::template construct_object<blk_t>(a, holder.get(), p,
                                                       ala::move(d), a);
            _cb = holder.release();
            _ptr = p;
            _weak_this();
        } catch (...) {
            if (delete_on_excpetion)
                d(p);
            throw;
        }
    }

    template<class Y, class D>
    void _ctor_uptr_helper(unique_ptr<Y, D> &&other, false_type) {
        if (!other)
            return;
        using A = allocator<T>;
        this->_construct(other.get(), other.get_deleter(), A(), false);
        other.release();
    }

    template<class Y, class D>
    void _ctor_uptr_helper(unique_ptr<Y, D> &&other, true_type) {
        if (!other)
            return;
        using A = allocator<T>;
        this->_construct(other.get(), ala::ref(other.get_deleter()), A(), false);
        other.release();
    }

    template<class U, class A, class... Args>
    friend shared_ptr<U> allocate_shared(const A &a, Args &&...args);

    template<class D, class U>
    friend D *get_deleter(const shared_ptr<U> &p) noexcept;

    template<class U>
    friend class weak_ptr;

    template<class U>
    friend class shared_ptr;

public:
    // constructors
    constexpr shared_ptr() noexcept {}
    constexpr shared_ptr(nullptr_t) noexcept: shared_ptr() {}
    template<class Y,
             class = enable_if_t<_is_deletable<Y>::value && _is_convable<Y>::value>>
    explicit shared_ptr(Y *p) {
        using D = conditional_t<is_array<T>::value, default_delete<Y[]>,
                                default_delete<Y>>;
        using A = allocator<T>;
        this->_construct(p, D(), A());
    }
    template<class Y, class D,
             class = enable_if_t<is_move_constructible<D>::value &&
                                 _is_convable<Y>::value && _check_deleter<Y *, D>::value>>
    shared_ptr(Y *p, D d) {
        using A = allocator<T>;
        this->_construct(p, ala::move(d), A());
    }
    template<class Y, class D, class A,
             class = enable_if_t<is_move_constructible<D>::value &&
                                 _is_convable<Y>::value && _check_deleter<Y *, D>::value>>
    shared_ptr(Y *p, D d, A a) {
        this->_construct(p, ala::move(d), a);
    }
    template<class D, class = enable_if_t<is_move_constructible<D>::value &&
                                          _check_deleter<nullptr_t, D>::value>>
    shared_ptr(nullptr_t p, D d) {
        using A = allocator<T>;
        this->_construct(p, ala::move(d), A());
    }
    template<class D, class A,
             class = enable_if_t<is_move_constructible<D>::value &&
                                 _check_deleter<nullptr_t, D>::value>>
    shared_ptr(nullptr_t p, D d, A a) {
        this->_construct(p, ala::move(d), a);
    }
    template<class Y>
    shared_ptr(const shared_ptr<Y> &other, element_type *p) noexcept
        : _cb(other._cb), _ptr(p) {
        if (_cb != nullptr)
            _cb->inc_shared();
    }
    template<class Y>
    shared_ptr(shared_ptr<Y> &&other, element_type *p) noexcept
        : _cb(other._cb), _ptr(p) {
        other._cb = nullptr;
        other._ptr = nullptr;
    }
    shared_ptr(const shared_ptr &other) noexcept
        : _cb(other._cb), _ptr(other._ptr) {
        if (_cb != nullptr)
            _cb->inc_shared();
    }
    // _is_convable<Y> in template parameter cause clang unhappy
    template<class Y>
    shared_ptr(
        _sfinae_checker<const shared_ptr<Y> &, enable_if_t<_is_convable<Y>::value>>
            other) noexcept
        : _cb(other._cb), _ptr(other._ptr) {
        if (_cb != nullptr)
            _cb->inc_shared();
    }
    shared_ptr(shared_ptr &&other) noexcept: _cb(other._cb), _ptr(other._ptr) {
        other._cb = nullptr;
        other._ptr = nullptr;
    }
    template<class Y>
    shared_ptr(_sfinae_checker<shared_ptr<Y> &&, enable_if_t<_is_convable<Y>::value>>
                   other) noexcept
        : _cb(other._cb), _ptr(other._ptr) {
        other._cb = nullptr;
        other._ptr = nullptr;
    }
    template<class Y, class = enable_if_t<_is_convable<Y>::value>>
    explicit shared_ptr(const weak_ptr<Y> &other) {
        if (other.expired())
            throw bad_weak_ptr();
        _cb = other._cb;
        _ptr = other._ptr;
        if (_cb != nullptr)
            _cb->inc_shared();
    }

    template<class Y, class D,
             class = enable_if_t<_is_convable<Y>::value &&
                                 is_convertible<typename unique_ptr<Y, D>::pointer,
                                                element_type *>::value>>
    shared_ptr(unique_ptr<Y, D> &&other) {
        this->_ctor_uptr_helper(ala::move(other), is_reference<D>());
    }

    // destructor
    ~shared_ptr() {
        if (_cb != nullptr) {
            _cb->dec_shared();
        }
        _cb = nullptr;
        _ptr = nullptr;
    }

    // assignment
    shared_ptr &operator=(const shared_ptr &other) noexcept {
        if (this != ala::addressof(other))
            shared_ptr(other).swap(*this);
        return *this;
    }
    template<class Y>
    shared_ptr &operator=(const shared_ptr<Y> &other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }
    shared_ptr &operator=(shared_ptr &&other) noexcept {
        if (this != ala::addressof(other))
            shared_ptr(ala::move(other)).swap(*this);
        return *this;
    }
    template<class Y>
    shared_ptr &operator=(shared_ptr<Y> &&other) noexcept {
        shared_ptr(ala::move(other)).swap(*this);
        return *this;
    }
    template<class Y, class D>
    shared_ptr &operator=(unique_ptr<Y, D> &&other) {
        shared_ptr(ala::move(other)).swap(*this);
        return *this;
    }

    // modifiers
    void swap(shared_ptr &other) noexcept {
        ala::_swap_adl(_ptr, other._ptr);
        ala::_swap_adl(_cb, other._cb);
    }

    void reset() noexcept {
        shared_ptr().swap(*this);
    }
    template<class Y>
    void reset(Y *p) {
        shared_ptr(p).swap(*this);
    }
    template<class Y, class D>
    void reset(Y *p, D d) {
        shared_ptr(p, d).swap(*this);
    }
    template<class Y, class D, class A>
    void reset(Y *p, D d, A a) {
        shared_ptr(p, d, a).swap(*this);
    }

    // observers
    element_type *get() const noexcept {
        return _ptr;
    }
    add_lvalue_reference_t<T> operator*() const noexcept {
        return *get();
    }
    add_pointer_t<T> operator->() const noexcept {
        return get();
    }
    add_lvalue_reference_t<element_type> operator[](ptrdiff_t i) const {
        static_assert(is_array<T>::value, "shared_ptr<T*> has no operator []");
        return get()[i];
    }
    long use_count() const noexcept {
        if (_cb)
            return _cb->get_shared();
        return 0;
    }
    explicit operator bool() const noexcept {
        return get() != nullptr;
    }
    bool unique() const noexcept {
        return use_count() == 1;
    }
    template<class Y>
    bool owner_before(const shared_ptr<Y> &other) const noexcept {
        return _cb < other._cb;
    }
    template<class Y>
    bool owner_before(const weak_ptr<Y> &other) const noexcept {
        return _cb < other._cb;
    }
};

// shared_ptr creation

template<class Alloc>
struct _allocator_deleter {
    Alloc &_alloc;
    constexpr _allocator_deleter(Alloc &a): _alloc(a) {}
    template<class Pointer>
    void operator()(Pointer ptr) const {
        using traits_t = allocator_traits<Alloc>;
        // no dealloc operation
        traits_t::destroy(_alloc, ptr);
    }
};

template<class T, class A, class... Args>
shared_ptr<T> allocate_shared(const A &a, Args &&...args) {
    using D = _allocator_deleter<A>;
    using traits_t = allocator_traits<A>;
    using _blk_t = _ctblk<T *, D, A>;
    using blk_t = _ctblk<T *, D, A, sizeof(_blk_t) + sizeof(T)>;
    using ph_t = aligned_storage_t<sizeof(blk_t) + sizeof(T)>;
    using holder_t = pointer_holder<ph_t *, A>;
    A alloc(a);
    holder_t holder(alloc, 1);
    blk_t *blk = reinterpret_cast<blk_t *>(holder.get());
    T *ptr = reinterpret_cast<T *>(reinterpret_cast<char *>(blk) + sizeof(blk_t));
    traits_t::template construct_object<blk_t>(alloc, blk, ptr, D(alloc), alloc);
    traits_t::construct(blk->_alloc, ptr, ala::forward<Args>(args)...);
    shared_ptr<T> sp;
    sp._cb = reinterpret_cast<blk_t *>(holder.release());
    sp._ptr = ptr;
    sp._weak_this();
    return sp;
}

template<class T, class... Args>
enable_if_t<!is_array<T>::value, shared_ptr<T>> make_shared(Args &&...args) {
    return ala::allocate_shared<T>(allocator<T>(), ala::forward<Args>(args)...);
}

// C++20
// template<class T>
// shared_ptr<T> make_shared(size_t N); // T is U[]
// template<class T, class A>
// shared_ptr<T> allocate_shared(const A &a, size_t N); // T is U[]

// template<class T>
// shared_ptr<T> make_shared(); // T is U[N]
// template<class T, class A>
// shared_ptr<T> allocate_shared(const A &a); // T is U[N]

// template<class T>
// shared_ptr<T> make_shared(size_t N, const remove_extent_t<T> &u); // T is U[]
// template<class T, class A>
// shared_ptr<T> allocate_shared(const A &a, size_t N,
//                               const remove_extent_t<T> &u); // T is U[]

// template<class T>
// shared_ptr<T> make_shared(const remove_extent_t<T> &u); // T is U[N]
// template<class T, class A>
// shared_ptr<T> allocate_shared(const A &a, const remove_extent_t<T> &u); // T is U[N]

// template<class T>
// shared_ptr<T> make_shared_for_overwrite(); // T is not U[]
// template<class T, class A>
// shared_ptr<T> allocate_shared_for_overwrite(const A &a); // T is not U[]

// template<class T>
// shared_ptr<T> make_shared_for_overwrite(size_t N); // T is U[]
// template<class T, class A>
// shared_ptr<T> allocate_shared_for_overwrite(const A &a, size_t N); // T is U[]

// shared_ptr comparisons
template<class T, class U>
bool operator==(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return lhs.get() == rhs.get();
}
template<class T, class U>
bool operator!=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return !(lhs == rhs);
}
template<class T, class U>
bool operator<(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return ala::less<>()(lhs.get(), rhs.get());
}
template<class T, class U>
bool operator>(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return rhs < lhs;
}
template<class T, class U>
bool operator<=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return !(rhs < lhs);
}
template<class T, class U>
bool operator>=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return !(lhs < rhs);
}

template<class T>
bool operator==(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return !lhs;
}
template<class T>
bool operator==(nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return !rhs;
}
template<class T>
bool operator!=(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return (bool)lhs;
}
template<class T>
bool operator!=(nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return (bool)rhs;
}
template<class T>
bool operator<(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return less<typename shared_ptr<T>::element_type *>()(lhs.get(), nullptr);
}
template<class T>
bool operator<(nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return less<typename shared_ptr<T>::element_type *>()(nullptr, rhs.get());
}
template<class T>
bool operator>(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return nullptr < lhs;
}
template<class T>
bool operator>(nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return rhs < nullptr;
}
template<class T>
bool operator<=(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return !(nullptr < lhs);
}
template<class T>
bool operator<=(nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return !(rhs < nullptr);
}
template<class T>
bool operator>=(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return !(lhs < nullptr);
}
template<class T>
bool operator>=(nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return !(nullptr < rhs);
}

// shared_ptr specialized algorithms
template<class T>
void swap(shared_ptr<T> &lhs, shared_ptr<T> &rhs) noexcept {
    return lhs.swap(rhs);
}

// shared_ptr casts
template<class T, class U>
shared_ptr<T> static_pointer_cast(const shared_ptr<U> &r) noexcept {
    return ala::shared_ptr<T>(
        r, static_cast<typename shared_ptr<T>::element_type *>(r.get()));
}
template<class T, class U>
shared_ptr<T> static_pointer_cast(shared_ptr<U> &&r) noexcept {
    return ala::static_pointer_cast(r);
}
template<class T, class U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U> &r) noexcept {
    if (auto p = dynamic_cast<typename shared_ptr<T>::element_type *>(r.get()))
        return ala::shared_ptr<T>(r, p);
    else
        return ala::shared_ptr<T>();
}
template<class T, class U>
shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> &&r) noexcept {
    return ala::static_pointer_cast(r);
}
template<class T, class U>
shared_ptr<T> const_pointer_cast(const shared_ptr<U> &r) noexcept {
    return ala::shared_ptr<T>(
        r, const_cast<typename shared_ptr<T>::element_type *>(r.get()));
}
template<class T, class U>
shared_ptr<T> const_pointer_cast(shared_ptr<U> &&r) noexcept {
    return ala::const_pointer_cast(r);
}
template<class T, class U>
shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U> &r) noexcept {
    return ala::shared_ptr<T>(
        r, reinterpret_cast<typename shared_ptr<T>::element_type *>(r.get()));
}
template<class T, class U>
shared_ptr<T> reinterpret_pointer_cast(shared_ptr<U> &&r) noexcept {
    return ala::reinterpret_pointer_cast(r);
}

// shared_ptr get_deleter
#if ALA_USE_RTTI
template<class D, class T>
D *get_deleter(const shared_ptr<T> &p) noexcept {
    if (p._cb != nullptr)
        return reinterpret_cast<D *>(p._cb->get_deleter(typeid(D)));
    return nullptr;
}
#endif

// shared_ptr I/O
// template<class E, class T, class U>
// basic_ostream<E, T> &operator<<(basic_ostream<E, T> &os, const shared_ptr<U> &p);

template<class T>
class weak_ptr {
    _ctblk_base *_cb = nullptr;
    T *_ptr = nullptr;

    template<class U>
    friend class weak_ptr;

    template<class U>
    friend class shared_ptr;

public:
    using element_type = remove_extent_t<T>;

    // constructors
    constexpr weak_ptr() noexcept {}
    template<class Y, class = enable_if_t<is_convertible<Y *, T *>::value>>
    weak_ptr(const shared_ptr<Y> &other) noexcept
        : _cb(other._cb), _ptr(other._ptr) {
        if (_cb != nullptr)
            _cb->inc_weak();
    }
    weak_ptr(const weak_ptr &other) noexcept: _cb(other._cb), _ptr(other._ptr) {
        if (_cb != nullptr)
            _cb->inc_weak();
    }
    template<class Y, class = enable_if_t<is_convertible<Y *, T *>::value>>
    weak_ptr(const weak_ptr<Y> &other) noexcept
        : _cb(other._cb), _ptr(other._ptr) {
        if (_cb != nullptr)
            _cb->inc_weak();
    }
    weak_ptr(weak_ptr &&other) noexcept: _cb(other._cb), _ptr(other._ptr) {
        other._cb = nullptr;
        other._ptr = nullptr;
    }
    template<class Y, class = enable_if_t<is_convertible<Y *, T *>::value>>
    weak_ptr(weak_ptr<Y> &&other) noexcept: _cb(other._cb), _ptr(other._ptr) {
        other._cb = nullptr;
        other._ptr = nullptr;
    }

    // destructor
    ~weak_ptr() {
        if (_cb != nullptr) {
            _cb->dec_weak();
        }
        _cb = nullptr;
        _ptr = nullptr;
    }

    // assignment
    weak_ptr &operator=(const weak_ptr &other) noexcept {
        if (ala::addressof(other) != this)
            weak_ptr(other).swap(*this);
        return *this;
    }
    template<class Y>
    weak_ptr &operator=(const weak_ptr<Y> &other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }
    template<class Y>
    weak_ptr &operator=(const shared_ptr<Y> &other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }
    weak_ptr &operator=(weak_ptr &&other) noexcept {
        if (ala::addressof(other) != this)
            weak_ptr(ala::move(other)).swap(*this);
        return *this;
    }
    template<class Y>
    weak_ptr &operator=(weak_ptr<Y> &&other) noexcept {
        weak_ptr(ala::move(other)).swap(*this);
        return *this;
    }

    // modifiers
    void swap(weak_ptr &other) noexcept {
        ala::_swap_adl(_cb, other._cb);
        ala::_swap_adl(_ptr, other._ptr);
    }
    void reset() noexcept {
        weak_ptr().swap(*this);
    }

    // observers
    long use_count() const noexcept {
        if (_cb == nullptr)
            return 0;
        return _cb->get_shared();
    }
    bool expired() const noexcept {
        return this->use_count() == 0;
    }
    shared_ptr<T> lock() const noexcept {
        return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
    }
    template<class U>
    bool owner_before(const shared_ptr<U> &other) const noexcept {
        return _cb < other._cb;
    }
    template<class U>
    bool owner_before(const weak_ptr<U> &other) const noexcept {
        return _cb < other._cb;
    }
};

// weak_ptr specialized algorithms
template<class T>
void swap(weak_ptr<T> &lhs, weak_ptr<T> &rhs) noexcept {
    return lhs.swap(rhs);
}

// class template owner_less
template<class T = void>
struct owner_less;

template<class T>
struct owner_less<shared_ptr<T>> {
    using result_type = bool;
    using first_argument_type = shared_ptr<T>;
    using second_argument_type = shared_ptr<T>;

    bool operator()(const shared_ptr<T> &lhs,
                    const shared_ptr<T> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    bool operator()(const shared_ptr<T> &lhs,
                    const weak_ptr<T> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    bool operator()(const weak_ptr<T> &lhs,
                    const shared_ptr<T> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
};
template<class T>
struct owner_less<weak_ptr<T>> {
    using result_type = bool;
    using first_argument_type = weak_ptr<T>;
    using second_argument_type = weak_ptr<T>;
    bool operator()(const weak_ptr<T> &lhs,
                    const weak_ptr<T> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    bool operator()(const shared_ptr<T> &lhs,
                    const weak_ptr<T> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    bool operator()(const weak_ptr<T> &lhs,
                    const shared_ptr<T> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
};
template<>
struct owner_less<void> {
    template<class T, class U>
    bool operator()(const shared_ptr<T> &lhs,
                    const shared_ptr<U> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    template<class T, class U>
    bool operator()(const shared_ptr<T> &lhs,
                    const weak_ptr<U> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    template<class T, class U>
    bool operator()(const weak_ptr<T> &lhs,
                    const shared_ptr<U> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    template<class T, class U>
    bool operator()(const weak_ptr<T> &lhs,
                    const weak_ptr<U> &rhs) const noexcept {
        return lhs.owner_before(rhs);
    }
    using is_transparent = int;
};

// class template enable_shared_from_this
template<class T>
class enable_shared_from_this {
protected:
    constexpr enable_shared_from_this() noexcept {}
    enable_shared_from_this(const enable_shared_from_this &) noexcept {}
    enable_shared_from_this &operator=(const enable_shared_from_this &) noexcept {
        return *this;
    }
    ~enable_shared_from_this() {}

public:
    shared_ptr<T> shared_from_this() {
        return shared_ptr<T>(weak_this);
    }
    shared_ptr<T const> shared_from_this() const {
        return shared_ptr<const T>(weak_this);
    }
    weak_ptr<T> weak_from_this() noexcept {
        return weak_this;
    }
    weak_ptr<T const> weak_from_this() const noexcept {
        return weak_this;
    }

private:
    template<class U>
    friend class shared_ptr;
    weak_ptr<T> weak_this; // exposition only
};

// hash support
template<class T>
struct hash;

template<class T, class D>
struct hash<_sfinae_checker<
    unique_ptr<T, D>,
    enable_if_t<_is_hashable<typename unique_ptr<T, D>::pointer>::value>>> {
    typedef unique_ptr<T, D> argument_type;
    typedef size_t result_type;

    result_type operator()(const argument_type &p) const {
        return hash<typename unique_ptr<T, D>::pointer>()(p.get());
    }
};

template<class T>
struct hash<_sfinae_checker<
    shared_ptr<T>,
    enable_if_t<_is_hashable<typename shared_ptr<T>::element_type *>::value>>> {
    typedef shared_ptr<T> argument_type;
    typedef size_t result_type;

    result_type operator()(const argument_type &p) const {
        return hash<typename shared_ptr<T>::element_type *>()(p.get());
    }
};

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class T>
shared_ptr(weak_ptr<T>) -> shared_ptr<T>;
template<class T, class D>
shared_ptr(unique_ptr<T, D>) -> shared_ptr<T>;
template<class T>
weak_ptr(shared_ptr<T>) -> weak_ptr<T>;
#endif

} // namespace ala

#endif // HEAD