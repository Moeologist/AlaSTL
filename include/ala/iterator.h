#ifndef _ALA_ITERATOR_H
#define _ALA_ITERATOR_H

#include <ala/type_traits.h>
#include <iterator>

namespace ala {

using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;

template<typename Iter>
struct iterator_traits {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template<typename T>
struct iterator_traits<T *> {
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = random_access_iterator_tag;
};

template<typename T>
struct iterator_traits<const T *> {
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = random_access_iterator_tag;
};

template<class Iter>
struct reverse_iterator {
    typedef typename iterator_traits<Iter>::iterator_category iterator_category;
    typedef typename iterator_traits<Iter>::value_type value_type;
    typedef typename iterator_traits<Iter>::difference_type difference_type;
    typedef typename iterator_traits<Iter>::pointer pointer;
    typedef typename iterator_traits<Iter>::reference reference;
    typedef Iter iterator_type;
    static_assert(is_base_of<bidirectional_iterator_tag, iterator_category>::value,
                  "iterator can not reverse");

    constexpr reverse_iterator(): current() {}
    constexpr explicit reverse_iterator(iterator_type base): current(base) {}

    template<class U>
    constexpr reverse_iterator(const reverse_iterator<U> &other)
        : current(other.current) {}

    constexpr iterator_type base() const {
        return current;
    }

    template<class U>
    constexpr reverse_iterator &operator=(const reverse_iterator<U> &other) {
        current = other.current;
    }

    constexpr reference operator*() const {
        Iter tmp = current;
        return (--tmp).operator*();
    }

    constexpr pointer operator->() const {
        Iter tmp = current;
        return (--tmp).operator->();
    }

    constexpr reference operator[](difference_type n) const;

    constexpr reverse_iterator &operator++() {
        --current;
        return *this;
    }

    constexpr reverse_iterator &operator--() {
        ++current;
        return *this;
    }

    constexpr reverse_iterator operator++(int) {
        return current--;
    }

    constexpr reverse_iterator operator--(int) {
        return current++;
    }

    constexpr reverse_iterator operator+(difference_type n) const;
    constexpr reverse_iterator operator-(difference_type n) const;
    constexpr reverse_iterator &operator+=(difference_type n);
    constexpr reverse_iterator &operator-=(difference_type n);

protected:
    template<class>
    friend class reverse_iterator;
    iterator_type current;
};

template<class Iter1, class Iter2>
constexpr bool operator==(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs) {
    return lhs.base() == rhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator!=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs) {
    return lhs.base() != rhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator<(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs);

template<class Iter1, class Iter2>
constexpr bool operator<=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs);

template<class Iter1, class Iter2>
constexpr bool operator>(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs);

template<class Iter1, class Iter2>
constexpr bool operator>=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs);

template<class Iter>
constexpr reverse_iterator<Iter> make_reverse_iterator(Iter i) {
    return reverse_iterator<Iter>(i);
};

template<class Iter>
struct const_iterator: public Iter {
    typedef typename Iter::iterator_category iterator_category;
    typedef typename Iter::difference_type difference_type;
    typedef const typename Iter::value_type value_type;
    typedef const value_type *pointer;
    typedef const value_type &reference;

    constexpr const_iterator(const Iter &base): Iter(base) {}

    constexpr reference operator*() const {
        return this->Iter::operator*();
    }

    constexpr pointer operator->() const {
        return this->Iter::operator->();
    }

    constexpr reference operator[](difference_type n) const {
        return this->Iter::operator[](n);
    }
};

template<class Iter1, class Iter2>
constexpr bool operator==(const const_iterator<Iter1> &lhs,
                          const const_iterator<Iter2> &rhs) {
    return static_cast<const Iter1 &>(lhs) == static_cast<const Iter2 &>(rhs);
}

template<class Iter1, class Iter2>
constexpr bool operator!=(const const_iterator<Iter1> &lhs,
                          const const_iterator<Iter2> &rhs) {
    return static_cast<const Iter1 &>(lhs) != static_cast<const Iter2 &>(rhs);
}

template<class Iter1, class Iter2>
constexpr bool operator<(const const_iterator<Iter1> &lhs,
                         const const_iterator<Iter2> &rhs);

template<class Iter1, class Iter2>
constexpr bool operator<=(const const_iterator<Iter1> &lhs,
                          const const_iterator<Iter2> &rhs);

template<class Iter1, class Iter2>
constexpr bool operator>(const const_iterator<Iter1> &lhs,
                         const const_iterator<Iter2> &rhs);

template<class Iter1, class Iter2>
constexpr bool operator>=(const const_iterator<Iter1> &lhs,
                          const const_iterator<Iter2> &rhs);

template<class Iter>
constexpr enable_if_t<is_base_of<random_access_iterator_tag,
                                 typename iterator_traits<Iter>::iterator_category>::value,
                      typename iterator_traits<Iter>::difference_type>
distance(Iter first, Iter last) {
    return last - first;
}

template<class Iter>
constexpr enable_if_t<!is_base_of<random_access_iterator_tag,
                                  typename iterator_traits<Iter>::iterator_category>::value,
                      typename iterator_traits<Iter>::difference_type>
distance(Iter first, Iter last) {
    typename iterator_traits<Iter>::difference_type ret = 0;
    for (; first != last; ++first)
        ++ret;
    return ret;
}

template<class Iter, class Distance>
constexpr enable_if_t<is_base_of<random_access_iterator_tag,
                                 typename iterator_traits<Iter>::iterator_category>::value>
advance(Iter &it, Distance n) {
    it += n;
}

template<class Iter, class Distance>
constexpr enable_if_t<!is_base_of<
    random_access_iterator_tag, typename iterator_traits<Iter>::iterator_category>::value>
advance(Iter &it, Distance n) {
    if (n > 0)
        for (Distance i = 0; i < n; ++i)
            ++it;
    else if (n < 0)
        for (Distance i = 0; i > n; --i)
            --it;
}

} // namespace ala

#endif