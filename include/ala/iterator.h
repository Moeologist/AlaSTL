#ifndef _ALA_ITERATOR_H
#define _ALA_ITERATOR_H

#include <ala/type_traits.h>

namespace std {
class input_iterator_tag;
class output_iterator_tag;
class forward_iterator_tag;
class bidirectional_iterator_tag;
class random_access_iterator_tag;
} // namespace std

namespace ala {

using ::std::input_iterator_tag;
using ::std::output_iterator_tag;
using ::std::forward_iterator_tag;
using ::std::bidirectional_iterator_tag;
using ::std::random_access_iterator_tag;

template<typename Iter, typename Void = void>
struct _iterator_traits_helper {};

template<typename Iter>
struct _iterator_traits_helper<
    Iter, void_t<typename Iter::difference_type, typename Iter::value_type,
                 typename Iter::pointer, typename Iter::reference,
                 typename Iter::iterator_category>> {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template<typename Iter>
struct iterator_traits: _iterator_traits_helper<Iter> {};

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
        return reverse_iterator(current--);
    }

    constexpr reverse_iterator operator--(int) {
        return reverse_iterator(current++);
    }

    constexpr reverse_iterator &operator+=(difference_type n) {
        current += n;
        return *this;
    }

    constexpr reverse_iterator operator+(difference_type n) const {
        reverse_iterator tmp = *this;
        tmp += n;
        return tmp;
    }

    constexpr reverse_iterator &operator-=(difference_type n) {
        current -= n;
        return *this;
    }

    constexpr reverse_iterator operator-(difference_type n) const {
        reverse_iterator tmp = *this;
        tmp -= n;
        return tmp;
    }

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
    return !(lhs == rhs);
}

template<class Iter1, class Iter2>
constexpr bool operator<(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs) {
    return lhs.base() < rhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator<=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs) {
    return !(lhs > rhs);
}

template<class Iter1, class Iter2>
constexpr bool operator>(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs) {
    return rhs.base() < lhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator>=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs) {
    return !(lhs > rhs);
}

template<class Iter>
constexpr reverse_iterator<Iter>
operator+(typename reverse_iterator<Iter>::difference_type n,
          const reverse_iterator<Iter> &it) {
    return reverse_iterator<Iter>(it.base() + n);
}

template<class Iter1, class Iter2>
constexpr auto operator-(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs)
    -> decltype(lhs.base() - rhs.base()) {
    return lhs.base() - rhs.base();
}

template<class Iter>
constexpr reverse_iterator<Iter> make_reverse_iterator(Iter i) {
    return reverse_iterator<Iter>(i);
};

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
constexpr enable_if_t<
    !is_base_of<random_access_iterator_tag,
                typename iterator_traits<Iter>::iterator_category>::value &&
    is_base_of<bidirectional_iterator_tag,
               typename iterator_traits<Iter>::iterator_category>::value>
advance(Iter &it, Distance n) {
    if (n > 0)
        for (Distance i = 0; i < n; ++i)
            ++it;
    else if (n < 0)
        for (Distance i = 0; i > n; --i)
            --it;
}

template<class Iter, class Distance>
constexpr enable_if_t<
    !is_base_of<random_access_iterator_tag,
                typename iterator_traits<Iter>::iterator_category>::value &&
    !is_base_of<bidirectional_iterator_tag,
                typename iterator_traits<Iter>::iterator_category>::value &&
    is_base_of<input_iterator_tag, typename iterator_traits<Iter>::iterator_category>::value>
advance(Iter &it, Distance n) {
    if (n > 0)
        for (Distance i = 0; i < n; ++i)
            ++it;
}

template<class BidirIter>
BidirIter prev(BidirIter it,
               typename iterator_traits<BidirIter>::difference_type n = 1) {
    ala::advance(it, -n);
    return it;
}

template<class ForwardIter>
ForwardIter next(ForwardIter it,
                 typename iterator_traits<ForwardIter>::difference_type n = 1) {
    ala::advance(it, n);
    return it;
}

template<class C>
constexpr auto begin(C &c) -> decltype(c.begin()) {
    return c.begin();
}

template<class C>
constexpr auto begin(const C &c) -> decltype(c.begin()) {
    return c.begin();
}

template<class T, size_t N>
constexpr T *begin(T (&array)[N]) noexcept {
    return array;
}

template<class T>
constexpr const T *begin(initializer_list<T> il) noexcept {
    return il.begin();
}

template<class C>
constexpr auto cbegin(const C &c) noexcept(noexcept(ala::begin(c)))
    -> decltype(ala::begin(c)) {
    return c.begin();
}

template<class C>
constexpr auto end(C &c) -> decltype(c.end()) {
    return c.end();
}

template<class C>
constexpr auto end(const C &c) -> decltype(c.end()) {
    return c.end();
}

template<class T, size_t N>
constexpr T *end(T (&array)[N]) noexcept {
    return array + N;
}

template<class T>
constexpr const T *end(initializer_list<T> il) noexcept {
    return il.end();
}

template<class C>
constexpr auto cend(const C &c) noexcept(noexcept(ala::end(c)))
    -> decltype(ala::end(c)) {
    return c.end();
}

template<class Iter>
class move_iterator {
public:
    using iterator_type = Iter;
    using iterator_category =
        typename iterator_traits<iterator_type>::iterator_category;
    using value_type = typename iterator_traits<iterator_type>::value_type;
    using difference_type = typename iterator_traits<iterator_type>::difference_type;
    using pointer = iterator_type;
    using _raw_ref_t = typename iterator_traits<iterator_type>::reference;
    using reference = conditional_t<
        is_reference<_raw_ref_t>::value,
        add_rvalue_reference_t<remove_reference_t<_raw_ref_t>>, _raw_ref_t>;

    constexpr move_iterator(): current() {}
    constexpr explicit move_iterator(iterator_type i): current(i) {}
    template<class U>
    constexpr move_iterator(const move_iterator<U> &other)
        : current(other.base()) {}

    template<class U>
    constexpr move_iterator &operator=(const move_iterator<U> &other) {
        current = other.current;
    }

    constexpr iterator_type base() const & {
        return current;
    }

    constexpr iterator_type base() && {
        return current;
    }

    constexpr reference operator*() const {
        return static_cast<reference>(*base());
    }

    constexpr pointer operator->() const {
        return base();
    }

    constexpr move_iterator &operator++() {
        ++current;
        return *this;
    }

    constexpr move_iterator &operator--() {
        --current;
        return *this;
    }

    constexpr move_iterator operator++(int) {
        return move_iterator(current++);
    }

    constexpr move_iterator operator--(int) {
        return move_iterator(current--);
    }

    constexpr move_iterator &operator+=(difference_type n) {
        current += n;
        return *this;
    }

    constexpr move_iterator operator+(difference_type n) const {
        move_iterator tmp = *this;
        tmp += n;
        return tmp;
    }

    constexpr move_iterator &operator-=(difference_type n) {
        current -= n;
        return *this;
    }

    constexpr move_iterator operator-(difference_type n) const {
        move_iterator tmp = *this;
        tmp -= n;
        return tmp;
    }

    constexpr reference operator[](difference_type n) const {
        return ala::move(base()[n]);
    }

private:
    iterator_type current; // exposition only
};

template<class Iter1, class Iter2>
constexpr bool operator==(const move_iterator<Iter1> &lhs,
                          const move_iterator<Iter2> &rhs) {
    return lhs.base() == rhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator!=(const move_iterator<Iter1> &lhs,
                          const move_iterator<Iter2> &rhs) {
    return !(lhs == rhs);
}

template<class Iter1, class Iter2>
constexpr bool operator<(const move_iterator<Iter1> &lhs,
                         const move_iterator<Iter2> &rhs) {
    return lhs.base() < rhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator<=(const move_iterator<Iter1> &lhs,
                          const move_iterator<Iter2> &rhs) {
    return !(lhs > rhs);
}

template<class Iter1, class Iter2>
constexpr bool operator>(const move_iterator<Iter1> &lhs,
                         const move_iterator<Iter2> &rhs) {
    return rhs.base() < lhs.base();
}

template<class Iter1, class Iter2>
constexpr bool operator>=(const move_iterator<Iter1> &lhs,
                          const move_iterator<Iter2> &rhs) {
    return !(lhs > rhs);
}

template<class Iter>
constexpr move_iterator<Iter>
operator+(typename move_iterator<Iter>::difference_type n,
          const move_iterator<Iter> &it) {
    return move_iterator<Iter>(it.base() + n);
}

template<class Iter1, class Iter2>
constexpr auto operator-(const move_iterator<Iter1> &lhs,
                         const move_iterator<Iter2> &rhs)
    -> decltype(lhs.base() - rhs.base()) {
    return lhs.base() - rhs.base();
}

template<class Iter>
move_iterator<Iter> make_move_iterator(Iter i) {
    return ala::move_iterator<Iter>(ala::move(i));
}

} // namespace ala

#endif