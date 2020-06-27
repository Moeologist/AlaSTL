#ifndef _ALA_MEMORY_H
#define _ALA_MEMORY_H

#include <ala/detail/allocator.h>
#include <ala/detail/uninitialized_memory.h>
#include <ala/detail/hash.h>
#include <ala/detail/functional_base.h>

namespace ala {

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
                                                Args &&... args) noexcept
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
constexpr T make_obj_using_allocator(const Alloc &alloc, Args &&... args);

template<class T, class Alloc, class... Args>
T *uninitialized_construct_using_allocator(T *p, const Alloc &alloc,
                                           Args &&... args);

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

    unique_ptr &operator=(
        conditional_t<is_move_assignable<deleter_type>::value, unique_ptr &&, _dummy>
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
    template<class U, char = 0 class = enable_if_t<
                          (is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
                          !is_reference<deleter_type>::value &&
                          is_move_constructible<deleter_type>::value>>
    unique_ptr(U p, remove_reference_t<deleter_type> &&rref) noexcept
        : _ptr(p), _deleter(ala::forward<decltype(rref)>(rref)) {}
    template<class U, short = 0 class = enable_if_t<
                          (is_same<U, nullptr_t>::value || _ptr_checker<U>::value) &&
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

    unique_ptr &operator=(
        conditional_t<is_move_assignable<deleter_type>::value, unique_ptr &&, _dummy>
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
enable_if_t<!is_array<T>::value, unique_ptr<T>> make_unique(Args &&... args) {
    return unique_ptr<T>(::new T(ala::forward<Args>(args)...));
}

template<class T>
enable_if_t<is_unbounded_array<T>::value, unique_ptr<T>>
make_unique_for_overwrite(size_t n) {
    return unique_ptr<T>(::new remove_extent_t<T>[n]);
}

template<class T, class... Args>
enable_if_t<is_bounded_array<T>::value, unique_ptr<T>>
make_unique_for_overwrite(Args &&...) = delete;

template<class T>
enable_if_t<!is_array<T>::value, unique_ptr<T>> make_unique_for_overwrite() {
    return unique_ptr<T>(::new T);
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

} // namespace ala

#endif // HEAD