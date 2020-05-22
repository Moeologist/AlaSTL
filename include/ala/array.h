#ifndef _ALA_ARRAY_H
#define _ALA_ARRAY_H

#include <ala/detail/algorithm_base.h>
#include <ala/detail/tuple_operator.h>

namespace ala {

template<class T, size_t N>
struct array {
    // types:
    typedef T &reference;
    typedef const T &const_reference;
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;

    T _data[N];

    constexpr void fill(const T &u) {
        ala::fill(begin(), end(), u);
    }

    constexpr void
    swap(array<T, N> &other) noexcept(is_nothrow_swappable<T>::value) {
        ala::swap_ranges(begin(), end(), other.begin());
    }

    // iterators:
    constexpr iterator begin() noexcept {
        return _data;
    }

    constexpr const_iterator begin() const noexcept {
        return cbegin();
    }

    constexpr iterator end() noexcept {
        return _data + N;
    }

    constexpr const_iterator end() const noexcept {
        return cend();
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return crbegin();
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return crend();
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(const_cast<array *>(this)->begin());
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(const_cast<array *>(this)->end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // capacity:
    constexpr size_type size() const noexcept {
        return N;
    }

    constexpr size_type max_size() const noexcept {
        return N;
    }

    constexpr bool empty() const noexcept {
        return false;
    }

    // element access:
    constexpr reference operator[](size_type n) {
        return _data[n];
    }

    constexpr const_reference operator[](size_type n) const {
        return _data[n];
    }

    constexpr reference at(size_type n) {
        if (!(n < N))
            throw out_of_range("ala::array index out of range");
        return _data[n];
    }

    constexpr const_reference at(size_type n) const {
        if (!(n < N))
            throw out_of_range("ala::array index out of range");
        return _data[n];
    }

    constexpr reference front() {
        return _data[0];
    }

    constexpr const_reference front() const {
        return _data[0];
    }

    constexpr reference back() {
        return _data[N - 1];
    }

    constexpr const_reference back() const {
        return _data[N - 1];
    }

    constexpr T *data() noexcept {
        return _data;
    }

    constexpr const T *data() const noexcept {
        return _data;
    }
};

template<class T>
struct array<T, 0> {
    // types:
    typedef T &reference;
    typedef const T &const_reference;
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;

    // aligned_storage_t<sizeof(T), alignof(T)> _placehold;

    struct _array_struct {
        T _m[1];
    };

    alignas(_array_struct) conditional_t<is_const<T>::value, const char,
                                         char> _placehold[sizeof(_array_struct)];

    constexpr pointer _data() const noexcept {
        return (pointer)_placehold;
    }

    constexpr void fill(const T &u) noexcept {
        static_assert(!is_const<T>::value,
                      "cannot fill zero-sized array of type 'const T'");
    }

    constexpr void swap(array<T, 0> &) noexcept {
        static_assert(!is_const<T>::value,
                      "cannot swap zero-sized array of type 'const T'");
    }

    // iterators:
    constexpr iterator begin() noexcept {
        return nullptr;
    }

    constexpr const_iterator begin() const noexcept {
        return cbegin();
    }

    constexpr iterator end() noexcept {
        return nullptr;
    }

    constexpr const_iterator end() const noexcept {
        return cend();
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return crbegin();
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return crend();
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(const_cast<array *>(this)->begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(const_cast<array *>(this)->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // capacity:
    constexpr size_type size() const noexcept {
        return 0;
    }

    constexpr size_type max_size() const noexcept {
        return 0;
    }

    constexpr bool empty() const noexcept {
        return true;
    }

    // element access:
    constexpr reference operator[](size_type n) {
        return _data()[n];
    }

    constexpr const_reference operator[](size_type n) const {
        return _data()[n];
    }

    constexpr reference at(size_type n) {
        throw out_of_range("ala::array index out of range");
        return _data()[0];
    }

    constexpr const_reference at(size_type n) const {
        throw out_of_range("ala::array index out of range");
        return _data()[0];
    }

    constexpr reference front() {
        return _data()[0];
    }

    constexpr const_reference front() const {
        return _data()[0];
    }

    constexpr reference back() {
        return _data()[0];
    }

    constexpr const_reference back() const {
        return _data()[0];
    }

    constexpr T *data() noexcept {
        return nullptr;
    }

    constexpr const T *data() const noexcept {
        return nullptr;
    }
};

template<class T, size_t N>
struct array;

template<class T, size_t N>
constexpr bool operator==(const array<T, N> &lhs, const array<T, N> &rhs) {
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, size_t N>
constexpr bool operator!=(const array<T, N> &lhs, const array<T, N> &rhs) {
    return !(lhs == rhs);
}

template<class T, size_t N>
constexpr bool operator<(const array<T, N> &lhs, const array<T, N> &rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                   rhs.end());
}

template<class T, size_t N>
constexpr bool operator>(const array<T, N> &lhs, const array<T, N> &rhs) {
    return rhs < lhs;
}

template<class T, size_t N>
constexpr bool operator<=(const array<T, N> &lhs, const array<T, N> &rhs) {
    return !(rhs < lhs);
}

template<class T, size_t N>
constexpr bool operator>=(const array<T, N> &lhs, const array<T, N> &rhs) {
    return !(lhs < rhs);
}

template<class T, size_t N>
constexpr enable_if_t<N == 0 || is_swappable<T>::value>
swap(array<T, N> &lhs, array<T, N> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class T, class... Rest>
struct _is_all_same {
    static_assert(_and_<is_same<T, Rest>...>::value,
                  "N4687 26.3.7.2 [array.cons]/2: "
                  "Requires: (is_same_v<T, U> && ...) is true. Otherwise the "
                  "program is ill-formed.");
    using type = T;
};

template<class T, class... Rest>
array(T, Rest...)
    -> array<typename _is_all_same<T, Rest...>::type, 1 + sizeof...(Rest)>;
#endif

template<size_t I, class T, size_t N>
constexpr T &get(array<T, N> &a) noexcept {
    static_assert(I < N, "ala::array index out of range");
    return a[I];
}

template<size_t I, class T, size_t N>
constexpr T &&get(array<T, N> &&a) noexcept {
    static_assert(I < N, "ala::array index out of range");
    return static_cast<T &&>(a[I]);
}

template<size_t I, class T, size_t N>
constexpr const T &get(const array<T, N> &a) noexcept {
    static_assert(I < N, "ala::array index out of range");
    return a[I];
}

template<size_t I, class T, size_t N>
constexpr const T &&get(const array<T, N> &&a) noexcept {
    static_assert(I < N, "ala::array index out of range");
    return static_cast<const T &&>(a[I]);
}

} // namespace ala

#endif