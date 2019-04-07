#ifndef _ALA_ITERATOR_H
#define _ALA_ITERATOR_H

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag {};
struct bidirectional_iterator_tag {};
struct random_access_iterator_tag {};
struct contiguous_iterator_tag {};

namespace ala {
template<typename It>
struct iterator_traits {
    // using difference_type = typename It::difference_type;
    using value_type = typename It::value_type;
    // using pointer = typename It::pointer;
    // using reference = typename It::reference;
    // using iterator_category = typename It::iterator_category;
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

template<class It>
struct reverse_iterator {
    typedef typename It::iterator_category iterator_category;
    typedef typename It::value_type value_type;
    typedef typename It::difference_type difference_type;
    typedef typename It::pointer pointer;
    typedef typename It::reference reference;
    typedef It iterator_type;
    static_assert(
        is_same<iterator_category, bidirectional_iterator_tag>::value ||
            is_same<iterator_category, bidirectional_iterator_tag>::value ||
            is_same<iterator_category, bidirectional_iterator_tag>::value,
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
        It tmp = current;
        return (--tmp).operator*();
    }

    constexpr pointer operator->() const {
        It tmp = current;
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
    iterator_type current;
};

template<class It1, class It2>
constexpr bool operator==(const reverse_iterator<It1> &lhs,
                          const reverse_iterator<It2> &rhs) {
    return lhs.base() == rhs.base();
}

template<class It1, class It2>
constexpr bool operator!=(const reverse_iterator<It1> &lhs,
                          const reverse_iterator<It2> &rhs) {
    return lhs.base() != rhs.base();
}

template<class It1, class It2>
constexpr bool operator<(const reverse_iterator<It1> &lhs,
                         const reverse_iterator<It2> &rhs);

template<class It1, class It2>
constexpr bool operator<=(const reverse_iterator<It1> &lhs,
                          const reverse_iterator<It2> &rhs);

template<class It1, class It2>
constexpr bool operator>(const reverse_iterator<It1> &lhs,
                         const reverse_iterator<It2> &rhs);

template<class It1, class It2>
constexpr bool operator>=(const reverse_iterator<It1> &lhs,
                          const reverse_iterator<It2> &rhs);

template<class It>
constexpr reverse_iterator<It> make_reverse_iterator(It i) {
    return reverse_iterator<It>(i);
};

template<class It>
struct const_iterator: public It {
    typedef typename It::iterator_category iterator_category;
    typedef typename It::difference_type difference_type;
    typedef const typename It::value_type value_type;
    typedef const typename It::value_type *pointer;
    typedef const typename It::value_type &reference;

    constexpr const_iterator(const It &base): It(base) {}

    constexpr reference operator*() const {
        return this->It::operator*();
    }

    constexpr pointer operator->() const {
        return this->It::operator->();
    }

    constexpr reference operator[](difference_type n) const {
        return this->It::operator[](n);
    }
};

template<class It1, class It2>
constexpr bool operator==(const const_iterator<It1> &lhs,
                          const const_iterator<It2> &rhs) {
    return static_cast<const It1 &>(lhs) == static_cast<const It2 &>(rhs);
}

template<class It1, class It2>
constexpr bool operator!=(const const_iterator<It1> &lhs,
                          const const_iterator<It2> &rhs) {
    return static_cast<const It1 &>(lhs) != static_cast<const It2 &>(rhs);
}

template<class It1, class It2>
constexpr bool operator<(const const_iterator<It1> &lhs,
                         const const_iterator<It2> &rhs);

template<class It1, class It2>
constexpr bool operator<=(const const_iterator<It1> &lhs,
                          const const_iterator<It2> &rhs);

template<class It1, class It2>
constexpr bool operator>(const const_iterator<It1> &lhs,
                         const const_iterator<It2> &rhs);

template<class It1, class It2>
constexpr bool operator>=(const const_iterator<It1> &lhs,
                          const const_iterator<It2> &rhs);

} // namespace ala

#endif