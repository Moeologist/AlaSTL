#ifndef _ALA_DETAIL_PTR_ITERATOR_H
#define _ALA_DETAIL_PTR_ITERATOR_H

#include <ala/iterator.h>

namespace ala {

template<class Value, class Ptr>
struct ptr_iterator {
    using iterator_concept = contiguous_iterator_tag;
    using iterator_category = random_access_iterator_tag;
    using value_type = Value;
    using difference_type = typename pointer_traits<Ptr>::difference_type;
    using pointer = Ptr;
    using reference = value_type &;

    constexpr ptr_iterator() {}

    constexpr ptr_iterator(const ptr_iterator &other): _ptr(other._ptr) {}

    template<class Value1, class Ptr1>
    constexpr ptr_iterator(const ptr_iterator<Value1, Ptr1> &other)
        : _ptr(other._ptr) {}

    constexpr reference operator*() const {
        return *_ptr;
    }

    constexpr pointer operator->() const {
        return _ptr;
    }

    template<class Value1, class Ptr1>
    constexpr bool operator==(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator!=(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return !(*this == rhs);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator<(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return _ptr < rhs._ptr;
    }

    template<class Value1, class Ptr1>
    constexpr bool operator>(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return rhs < *this;
    }

    template<class Value1, class Ptr1>
    constexpr bool operator<=(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return !(rhs < *this);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator>=(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return !(*this < rhs);
    }

    constexpr ptr_iterator &operator++() {
        ++_ptr;
        return *this;
    }

    constexpr ptr_iterator operator++(int) {
        ptr_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr ptr_iterator &operator--() {
        --_ptr;
        return *this;
    }

    constexpr ptr_iterator operator--(int) {
        ptr_iterator tmp(*this);
        --*this;
        return tmp;
    }

    constexpr ptr_iterator &operator+=(difference_type n) {
        _ptr += n;
        return *this;
    }

    constexpr ptr_iterator &operator-=(difference_type n) {
        _ptr -= n;
        return *this;
    }

    constexpr ptr_iterator operator+(difference_type n) const {
        ptr_iterator tmp(*this);
        tmp += n;
        return tmp;
    }

    constexpr ptr_iterator operator-(difference_type n) const {
        ptr_iterator tmp(*this);
        tmp -= n;
        return tmp;
    }

    template<class Value1, class Ptr1>
    constexpr difference_type
    operator-(const ptr_iterator<Value1, Ptr1> &rhs) const {
        return _ptr - rhs._ptr;
    }

    friend constexpr ptr_iterator operator+(difference_type lhs,
                                            ptr_iterator rhs) {
        return rhs + lhs;
    }

    constexpr reference operator[](difference_type n) const {
        return *(*this + n);
    }

protected:
    // using _invc_t =
    //     conditional_t<is_const<value_type>::value, remove_const_t<value_type>,
    //                   add_const_t<value_type>>;
    // friend class ptr_iterator<_invc_t, Ptr>;

    template<class, class>
    friend class ptr_iterator;

    template<class, class>
    friend class vector;

    template<class, size_t>
    friend class span;

    constexpr ptr_iterator(const Ptr &ptr): _ptr(ptr) {}

    constexpr operator Ptr() {
        return _ptr;
    }

    Ptr _ptr = static_cast<Ptr>(nullptr);
};
} // namespace ala

#endif