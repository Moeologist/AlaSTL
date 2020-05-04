#ifndef _ALA_ITERATOR_H
#define _ALA_ITERATOR_H

#include <ala/type_traits.h>
#include <ala/detail/memory_base.h>

namespace std {
#if defined(_LIBCPP_ABI_NAMESPACE)
inline namespace _LIBCPP_ABI_NAMESPACE {
#endif
class input_iterator_tag;
class output_iterator_tag;
class forward_iterator_tag;
class bidirectional_iterator_tag;
class random_access_iterator_tag;
#if defined(_LIBCPP_ABI_NAMESPACE)
}
#endif
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
        return *--tmp;
    }

    constexpr pointer operator->() const {
        Iter tmp = current;
        return ala::addressof(operator*());
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
    typename iterator_traits<Iter>::difference_type result = 0;
    for (; first != last; ++first)
        ++result;
    return result;
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
    return ala::begin(c);
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
    return ala::end(c);
}

template<class C>
constexpr auto rbegin(C &c) -> decltype(c.rbegin()) {
    return c.rbegin();
}

template<class C>
constexpr auto rbegin(const C &c) -> decltype(c.rbegin()) {
    return c.rbegin();
}

template<class T, size_t N>
constexpr reverse_iterator<T *> rbegin(T (&array)[N]) noexcept {
    return make_reverse_iterator(ala::end(array));
}

template<class T>
constexpr const T *rbegin(initializer_list<T> il) noexcept {
    return il.rbegin();
}

template<class C>
constexpr auto crbegin(const C &c) noexcept(noexcept(ala::rbegin(c)))
    -> decltype(ala::rbegin(c)) {
    return ala::rbegin(c);
}

template<class C>
constexpr auto rend(C &c) -> decltype(c.rend()) {
    return c.rend();
}

template<class C>
constexpr auto rend(const C &c) -> decltype(c.rend()) {
    return c.rend();
}

template<class T, size_t N>
constexpr T *rend(T (&array)[N]) noexcept {
    return make_reverse_iterator(ala::begin(array));
}

template<class T>
constexpr const T *rend(initializer_list<T> il) noexcept {
    return il.rend();
}

template<class C>
constexpr auto crend(const C &c) noexcept(noexcept(ala::rend(c)))
    -> decltype(ala::rend(c)) {
    return ala::rend(c);
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

// insert iterators
template<class Container>
struct back_insert_iterator {
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    constexpr back_insert_iterator() noexcept = default;

    constexpr explicit back_insert_iterator(Container &c)
        : container(ala::addressof(c)) {}

    constexpr back_insert_iterator<Container> &
    operator=(const typename Container::value_type &value) {
        container->push_back(value);
        return *this;
    }

    constexpr back_insert_iterator<Container> &
    operator=(typename Container::value_type &&value) {
        container->push_back(ala::move(value));
        return *this;
    }

    constexpr back_insert_iterator &operator*() {
        return *this;
    }

    constexpr back_insert_iterator &operator++() {
        return *this;
    }

    constexpr back_insert_iterator &operator++(int) {
        return *this;
    }

protected:
    Container *container;
};

template<class Container>
constexpr back_insert_iterator<Container> back_inserter(Container &c) {
    return ala::back_insert_iterator<Container>(c);
}

template<class Container>
struct front_insert_iterator {
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    constexpr front_insert_iterator() noexcept = default;

    constexpr explicit front_insert_iterator(Container &c)
        : container(ala::addressof(c)) {}

    constexpr front_insert_iterator<Container> &
    operator=(const typename Container::value_type &value) {
        container->push_front(value);
    }

    constexpr front_insert_iterator<Container> &
    operator=(typename Container::value_type &&value) {
        container->push_front(ala::move(value));
    }

    constexpr front_insert_iterator &operator*() {
        return *this;
    }

    constexpr front_insert_iterator &operator++() {
        return *this;
    }

    constexpr front_insert_iterator &operator++(int) {
        return *this;
    }

protected:
    Container *container;
};

template<class Container>
constexpr front_insert_iterator<Container> front_inserter(Container &c) {
    return ala::front_insert_iterator<Container>(c);
}

template<class Container>
struct insert_iterator {
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    constexpr insert_iterator() noexcept = default;

    constexpr insert_iterator(Container &c, typename Container::iterator i)
        : container(ala::addressof(c)), iter(i) {}

    constexpr insert_iterator<Container> &
    operator=(const typename Container::value_type &value) {
        container->insert(iter, value);
        ++iter;
    }

    constexpr insert_iterator<Container> &
    operator=(typename Container::value_type &&value) {
        container->insert(iter, ala::move(value));
        ++iter;
    }

    constexpr insert_iterator &operator*() {
        return *this;
    }

    constexpr insert_iterator &operator++() {
        return *this;
    }

    constexpr insert_iterator &operator++(int) {
        return *this;
    }

protected:
    Container *container;
    typename Container::iterator iter;
};

template<class Container>
constexpr insert_iterator<Container> inserter(Container &c,
                                              typename Container::iterator i) {
    return ala::insert_iterator<Container>(c, i);
}

template<class C>
constexpr auto size(const C &c) -> decltype(c.size()) {
    return c.size();
}

template<class C>
constexpr auto ssize(const C &c)
    -> common_type_t<ptrdiff_t, make_signed_t<decltype(c.size())>> {
    using R = common_type_t<ptrdiff_t, make_signed_t<decltype(c.size())>>;
    return static_cast<R>(c.size());
}

template<class T, size_t N>
constexpr size_t size(const T (&array)[N]) noexcept {
    return N;
}

template<class T, ptrdiff_t N>
constexpr ptrdiff_t ssize(const T (&array)[N]) noexcept {
    return N;
}

} // namespace ala

#endif