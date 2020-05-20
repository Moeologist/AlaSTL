#ifndef _ALA_DETAIL_LINKED_LIST_H
#define _ALA_DETAIL_LINKED_LIST_H

#include <ala/detail/allocator.h>
#include <ala/iterator.h>

namespace ala {

template<class T>
struct l_node {
    T _data;
    l_node *_pre, *_suc;
};

template<class Data>
constexpr l_node<Data> *next_node(l_node<Data> *_ptr) {
    return _ptr->_suc;
}

template<class Data>
constexpr l_node<Data> *prev_node(l_node<Data> *_ptr) {
    return _ptr->_pre;
}

template<class Value, class Ptr, bool IsConst = false>
struct l_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef Value value_type;
    typedef typename pointer_traits<Ptr>::difference_type difference_type;
    typedef conditional_t<IsConst, const value_type, value_type> *pointer;
    typedef conditional_t<IsConst, const value_type, value_type> &reference;

    constexpr l_iterator() {}
    constexpr l_iterator(const l_iterator &other): _ptr(other._ptr) {}
    constexpr l_iterator(const l_iterator<Value, Ptr, !IsConst> &other)
        : _ptr(other._ptr) {}
    constexpr l_iterator(const Ptr &ptr): _ptr(ptr) {}

    constexpr reference operator*() const {
        return _ptr->_data;
    }

    constexpr pointer operator->() const {
        return ala::addressof(_ptr->_data);
    }

    constexpr bool operator==(const l_iterator &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const l_iterator &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr bool operator==(const l_iterator<Value, Ptr, !IsConst> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const l_iterator<Value, Ptr, !IsConst> &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr l_iterator &operator++() {
        _ptr = next_node(_ptr);
        return *this;
    }

    constexpr l_iterator operator++(int) {
        l_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr l_iterator &operator--() {
        _ptr = prev_node(_ptr);
        return *this;
    }

    constexpr l_iterator operator--(int) {
        l_iterator tmp(*this);
        --*this;
        return tmp;
    }

protected:
    friend class l_iterator<Value, Ptr, !IsConst>;
    template<class, class>
    friend class list;
    Ptr _ptr = nullptr;
};

} // namespace ala

#endif // _ALA_DETAIL_LINKED_LIST_H