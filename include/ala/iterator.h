#ifndef _ALA_ITERATOR_H
#define _ALA_ITERATOR_H

#include <ala/type_traits.h>
#include <ala/detail/memory_base.h>
#include <ala/detail/functional_base.h>
#include <ala/concepts.h>

#if ALA_USE_STD_ITER_TAG

    #include <iterator>

#else

ALA_BEGIN_NAMESPACE_STD

class input_iterator_tag;
class output_iterator_tag;
class forward_iterator_tag;
class bidirectional_iterator_tag;
class random_access_iterator_tag;
class contiguous_iterator_tag;

ALA_END_NAMESPACE_STD

#endif

namespace ala {

#if ALA_USE_STD_ITER_TAG

using ::std::input_iterator_tag;
using ::std::output_iterator_tag;
using ::std::forward_iterator_tag;
using ::std::bidirectional_iterator_tag;
using ::std::random_access_iterator_tag;
    #if ALA_API_VER >= 20
using ::std::contiguous_iterator_tag;
    #else
struct contiguous_iterator_tag: random_access_iterator_tag {};
    #endif

#else

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag: input_iterator_tag {};
struct bidirectional_iterator_tag: forward_iterator_tag {};
struct random_access_iterator_tag: bidirectional_iterator_tag {};
struct contiguous_iterator_tag: random_access_iterator_tag {};

#endif

// clang-format off
template<class Tag> struct _trans_iter_tag { using type = Tag; };
template<> struct _trans_iter_tag<::std::input_iterator_tag> { using type = input_iterator_tag; };
template<> struct _trans_iter_tag<::std::output_iterator_tag> { using type = output_iterator_tag; };
template<> struct _trans_iter_tag<::std::forward_iterator_tag> { using type = forward_iterator_tag; };
template<> struct _trans_iter_tag<::std::bidirectional_iterator_tag> { using type = bidirectional_iterator_tag; };
template<> struct _trans_iter_tag<::std::random_access_iterator_tag> { using type = random_access_iterator_tag; };
template<> struct _trans_iter_tag<::std::contiguous_iterator_tag> { using type = contiguous_iterator_tag; };
// clang-format on

template<class Tag>
using _trans_iter_tag_t = typename _trans_iter_tag<Tag>::type;

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
struct iterator_traits: _iterator_traits_helper<Iter> {
    using _iter_traits_no_spec = int;
};

template<typename T>
struct iterator_traits<T *> {
    using difference_type = ptrdiff_t;
#if ALA_API_VER < 20
    using value_type = T;
#else
    using value_type = remove_cv_t<T>;
#endif
    using pointer = T *;
    using reference = T &;
    using iterator_category = random_access_iterator_tag;
#if ALA_API_VER >= 20
    using iterator_concept = contiguous_iterator_tag;
#endif
};

#if ALA_API_VER < 20
template<typename T>
struct iterator_traits<const T *> {
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = random_access_iterator_tag;
};
#endif

ALA_HAS_MEM_TYPE(_iter_traits_no_spec)
ALA_HAS_MEM_TYPE(iterator_category)
ALA_HAS_MEM_TYPE(iterator_concept)

template<bool FallBack, class Iter, class Traits,
         bool HasNoSpec =
             FallBack &&_has__iter_traits_no_spec<iterator_traits<Iter>>::value>
struct _iter_concept_def {
    using _tag_t = random_access_iterator_tag;
};

template<bool FallBack, class Iter, class Traits>
struct _iter_concept_def<FallBack, Iter, Traits, false> {};

template<bool FallBack, class Iter, class Traits,
         bool HasCategory = _has_iterator_category<Traits>::value>
struct _iter_concept_category {
    using _tag_t = typename Traits::iterator_category;
};

template<bool FallBack, class Iter, class Traits>
struct _iter_concept_category<FallBack, Iter, Traits, false>
    : _iter_concept_def<FallBack, Iter, Traits> {};

template<bool FallBack, class Iter, class Traits,
         bool HasConcept = _has_iterator_concept<Traits>::value>
struct _iter_concept_concept {
    using _tag_t = typename Traits::iterator_concept;
};

template<bool FallBack, class Iter, class Traits>
struct _iter_concept_concept<FallBack, Iter, Traits, false>
    : _iter_concept_category<FallBack, Iter, Traits> {};

template<class Iter, class Traits = conditional_t<
                         _has__iter_traits_no_spec<iterator_traits<Iter>>::value,
                         Iter, iterator_traits<Iter>>>
using _iter_concept_t =
    _trans_iter_tag_t<typename _iter_concept_concept<true, Iter, Traits>::_tag_t>;

template<class Iter, class Traits = conditional_t<
                         _has__iter_traits_no_spec<iterator_traits<Iter>>::value,
                         Iter, iterator_traits<Iter>>>
using _iter_tag_t =
    _trans_iter_tag_t<typename _iter_concept_concept<false, Iter, Traits>::_tag_t>;

template<class Iter>
struct reverse_iterator {
    using iterator_category = typename iterator_traits<Iter>::iterator_category;
    using value_type = typename iterator_traits<Iter>::value_type;
    using difference_type = typename iterator_traits<Iter>::difference_type;
    using pointer = typename iterator_traits<Iter>::pointer;
    using reference = typename iterator_traits<Iter>::reference;
    using iterator_type = Iter;
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
    -> decltype(rhs.base() - lhs.base()) {
    return rhs.base() - lhs.base();
}

template<class Iter>
constexpr reverse_iterator<Iter> make_reverse_iterator(Iter i) {
    return reverse_iterator<Iter>(i);
};

template<class Iter>
constexpr typename iterator_traits<Iter>::difference_type
_distance_dispatch(Iter first, Iter last, random_access_iterator_tag) {
    return last - first;
}

template<class Iter>
constexpr typename iterator_traits<Iter>::difference_type
_distance_dispatch(Iter first, Iter last, input_iterator_tag) {
    typename iterator_traits<Iter>::difference_type result = 0;
    for (; first != last; ++first)
        ++result;
    return result;
}

template<class Iter>
constexpr typename iterator_traits<Iter>::difference_type distance(Iter first,
                                                                   Iter last) {
    using tag_t = _iter_tag_t<Iter>;
    return ala::_distance_dispatch(first, last, tag_t{});
}

template<class Iter, class Distance>
constexpr void _advance_dispatch(random_access_iterator_tag, Iter &it,
                                 Distance n) {
    it += n;
}

template<class Iter, class Distance>
constexpr void _advance_dispatch(Iter &it, Distance n,
                                 bidirectional_iterator_tag) {
    if (n > 0)
        for (Distance i = 0; i < n; ++i)
            ++it;
    else if (n < 0)
        for (Distance i = 0; i > n; --i)
            --it;
}

template<class Iter, class Distance>
constexpr void _advance_dispatch(Iter &it, Distance n, input_iterator_tag) {
    if (n > 0)
        for (Distance i = 0; i < n; ++i)
            ++it;
}

template<class Iter, class Distance>
constexpr void advance(Iter &it, Distance n) {
    using tag_t = _iter_tag_t<Iter>;
    return ala::_advance_dispatch(it, n, tag_t{});
}

template<class BidirIter>
constexpr BidirIter
prev(BidirIter it, typename iterator_traits<BidirIter>::difference_type n = 1) {
    ala::advance(it, -n);
    return it;
}

template<class ForwardIter>
constexpr ForwardIter
next(ForwardIter it,
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
constexpr move_iterator<Iter> make_move_iterator(Iter i) {
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

template<class C>
ALA_NODISCARD constexpr auto empty(const C &c) -> decltype(c.empty()) {
    return c.empty();
}

template<class T, size_t N>
ALA_NODISCARD constexpr bool empty(const T (&array)[N]) noexcept {
    return false;
}

template<class E>
ALA_NODISCARD constexpr bool empty(initializer_list<E> il) noexcept {
    return il.size() == 0;
}

template<class C>
constexpr auto data(C &c) -> decltype(c.data()) {
    return c.data();
}

template<class C>
constexpr auto data(const C &c) -> decltype(c.data()) {
    return c.data();
}

template<class T, size_t N>
constexpr T *data(T (&array)[N]) noexcept {
    return array;
}

template<class E>
constexpr const E *data(initializer_list<E> il) noexcept {
    return il.begin();
}

} // namespace ala

#if _ALA_ENABLE_CONCEPTS

namespace ala {
namespace ranges {
namespace _iter_move {

void iter_move();

template<class I>
concept __enable_adl = __class_or_enum<remove_cvref_t<I>> &&
                       requires(I &&i) { iter_move(ala::forward<I>(i)); };

struct _cpo_fn {
    template<class I>
        requires __class_or_enum<remove_cvref_t<I>> && __enable_adl<I>
    ALA_NODISCARD constexpr decltype(auto) operator()(I &&i) const
        noexcept(noexcept(iter_move(ala::forward<I>(i)))) {
        return iter_move(ala::forward<I>(i));
    }

    template<class I>
        requires(!(__class_or_enum<remove_cvref_t<I>> && __enable_adl<I>)) &&
                requires(I &&i) { *ala::forward<I>(i); }
    ALA_NODISCARD constexpr decltype(auto) operator()(I &&i) const
        noexcept(noexcept(*ala::forward<I>(i))) {
        if constexpr (is_lvalue_reference_v<decltype(*ala::forward<I>(i))>) {
            return ala::move(*ala::forward<I>(i));
        } else {
            return *ala::forward<I>(i);
        }
    }
};
} // namespace _iter_move
inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V _iter_move::_cpo_fn iter_move;
} // namespace _cpos
} // namespace ranges

template<class>
struct _rmcv_for_obj {};

template<class T>
    requires is_object_v<T>
struct _rmcv_for_obj<T> {
    using value_type = remove_cv_t<T>;
};

template<class T>
concept __has_member_value_type = requires { typename T::value_type; };

template<class T>
concept __has_member_element_type = requires { typename T::element_type; };

template<class>
struct indirectly_readable_traits {};

template<class I>
    requires is_array_v<I>
struct indirectly_readable_traits<I> {
    using value_type = remove_cv_t<remove_extent_t<I>>;
};

template<class I>
struct indirectly_readable_traits<const I>: indirectly_readable_traits<I> {};

template<class T>
struct indirectly_readable_traits<T *>: _rmcv_for_obj<T> {};

template<__has_member_value_type T>
struct indirectly_readable_traits<T>: _rmcv_for_obj<typename T::value_type> {};

template<__has_member_element_type T>
struct indirectly_readable_traits<T>: _rmcv_for_obj<typename T::element_type> {};

template<__has_member_value_type T>
    requires __has_member_element_type<T>
struct indirectly_readable_traits<T> {};

template<__has_member_value_type T>
    requires __has_member_element_type<T> &&
             same_as<remove_cv_t<typename T::element_type>,
                     remove_cv_t<typename T::value_type>>
struct indirectly_readable_traits<T>: _rmcv_for_obj<typename T::value_type> {};

template<class Iter>
using iter_value_t = typename conditional_t<
    _has__iter_traits_no_spec<iterator_traits<remove_cvref_t<Iter>>>::value,
    indirectly_readable_traits<remove_cvref_t<Iter>>,
    iterator_traits<remove_cvref_t<Iter>>>::value_type;

template<class T>
using _lvalue_ref_t = T &;

template<class T>
concept __referenceable = requires { typename _lvalue_ref_t<T>; };

template<class T>
concept __dereferenceable =
    requires(T &t) {
        { *t } -> __referenceable; // not required to be equality-preserving
    };

template<__dereferenceable T>
using iter_reference_t = decltype(*declval<T &>());

template<class>
struct incrementable_traits {};

template<class T>
    requires is_object_v<T>
struct incrementable_traits<T *> {
    using difference_type = ptrdiff_t;
};

template<class I>
struct incrementable_traits<const I>: incrementable_traits<I> {};

template<class T>
concept __has_member_difference_type = requires { typename T::difference_type; };

template<__has_member_difference_type T>
struct incrementable_traits<T> {
    using difference_type = typename T::difference_type;
};

template<class T>
concept __has_integral_minus = requires(const T &x, const T &y) {
                                   { x - y } -> integral;
                               };

template<__has_integral_minus T>
    requires(!__has_member_difference_type<T>)
struct incrementable_traits<T> {
    using difference_type = make_signed_t<decltype(declval<T>() - declval<T>())>;
};

template<class T>
using iter_difference_t = typename conditional_t<
    _has__iter_traits_no_spec<iterator_traits<remove_cvref_t<T>>>::value,
    incrementable_traits<remove_cvref_t<T>>,
    iterator_traits<remove_cvref_t<T>>>::difference_type;

template<__dereferenceable T>
    requires requires(T &t) {
                 { ranges::iter_move(t) } -> __referenceable;
             }
using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<T &>()));

template<class T>
concept __indirectly_readable_impl =
    requires(const T t) {
        typename iter_value_t<T>;
        typename iter_reference_t<T>;
        typename iter_rvalue_reference_t<T>;
        { *t } -> same_as<iter_reference_t<T>>;
        { ranges::iter_move(t) } -> same_as<iter_rvalue_reference_t<T>>;
    } && common_reference_with<iter_reference_t<T> &&, iter_value_t<T> &> &&
    common_reference_with<iter_reference_t<T> &&, iter_rvalue_reference_t<T> &&> &&
    common_reference_with<iter_rvalue_reference_t<T> &&, const iter_value_t<T> &>;

template<class T>
concept indirectly_readable = __indirectly_readable_impl<remove_cvref_t<T>>;

template<indirectly_readable T>
using iter_common_reference_t =
    common_reference_t<iter_reference_t<T>, iter_value_t<T> &>;

template<class Out, class T>
concept indirectly_writable = requires(Out &&o, T &&t) {
                                  *o = ala::forward<T>(t);
                                  *ala::forward<Out>(o) = ala::forward<T>(t);
                                  const_cast<const iter_reference_t<Out> &&>(
                                      *o) = ala::forward<T>(t);
                                  const_cast<const iter_reference_t<Out> &&>(
                                      *ala::forward<Out>(o)) = ala::forward<T>(t);
                                  // not required to be equality-preserving
                              };

template<class T>
concept __signed_integer_like = signed_integral<T>;

template<class I>
concept weakly_incrementable =
    // TODO: remove this once the clang bug is fixed (bugs.llvm.org/PR48173).
    (!same_as<I, bool>) && // Currently, clang does not handle bool correctly.
    movable<I> && requires(I i) {
                      typename iter_difference_t<I>;
                      requires __signed_integer_like<iter_difference_t<I>>;
                      { ++i } -> same_as<I &>;
                      i++; // not required to be equality-preserving
                  };

template<class I>
concept incrementable = regular<I> && weakly_incrementable<I> &&
                        requires(I i) {
                            { i++ } -> same_as<I>;
                        };

template<class I>
concept input_or_output_iterator = requires(I i) {
                                       { *i } -> __referenceable;
                                   } && weakly_incrementable<I>;

template<class S, class I>
concept sentinel_for = semiregular<S> && input_or_output_iterator<I> &&
                       __weakly_equality_comparable_with<S, I>;

template<class, class>
ALA_INLINE_CONSTEXPR_V bool disable_sized_sentinel_for = false;

template<class S, class I>
concept sized_sentinel_for = sentinel_for<S, I> && !
disable_sized_sentinel_for<remove_cv_t<S>, remove_cv_t<I>>
    &&requires(const I &i, const S &s) {
          { s - i } -> same_as<iter_difference_t<I>>;
          { i - s } -> same_as<iter_difference_t<I>>;
      };

template<class I>
concept input_iterator = input_or_output_iterator<I> && indirectly_readable<I> &&
                         requires { typename _iter_concept_t<I>; } &&
                         derived_from<_iter_concept_t<I>, input_iterator_tag>;

template<class I, class T>
concept output_iterator = input_or_output_iterator<I> &&
                          indirectly_writable<I, T> &&
                          requires(I i, T &&t) {
                              *i++ = ala::forward<T>(
                                  t); // not required to be equality-preserving
                          };

template<class I>
concept forward_iterator = input_iterator<I> &&
                           derived_from<_iter_concept_t<I>, forward_iterator_tag> &&
                           incrementable<I> && sentinel_for<I, I>;

template<class I>
concept bidirectional_iterator =
    forward_iterator<I> &&
    derived_from<_iter_concept_t<I>, bidirectional_iterator_tag> &&
    requires(I i) {
        { --i } -> same_as<I &>;
        { i-- } -> same_as<I>;
    };

template<class I>
concept random_access_iterator =
    bidirectional_iterator<I> &&
    derived_from<_iter_concept_t<I>, random_access_iterator_tag> &&
    totally_ordered<I> && sized_sentinel_for<I, I> &&
    requires(I i, const I j, const iter_difference_t<I> n) {
        { i += n } -> same_as<I &>;
        { j + n } -> same_as<I>;
        { n + j } -> same_as<I>;
        { i -= n } -> same_as<I &>;
        { j - n } -> same_as<I>;
        { j[n] } -> same_as<iter_reference_t<I>>;
    };

template<class I>
concept contiguous_iterator =
    random_access_iterator<I> &&
    derived_from<_iter_concept_t<I>, contiguous_iterator_tag> &&
    is_lvalue_reference_v<iter_reference_t<I>> &&
    same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>> &&
    requires(const I &i) {
        { ala::to_address(i) } -> same_as<add_pointer_t<iter_reference_t<I>>>;
    };

// clang-format off
template<class F, class It>
concept indirectly_unary_invocable = indirectly_readable<It> &&
                                     copy_constructible<F> &&
                                     invocable<F &, iter_value_t<It> &> &&
                                     invocable<F &, iter_reference_t<It>> &&
                                     invocable<F &, iter_common_reference_t<It>> &&
                                     common_reference_with<invoke_result_t<F &, iter_value_t<It> &>,
                                                           invoke_result_t<F &, iter_reference_t<It>>>;

template<class F, class It>
concept indirectly_regular_unary_invocable = indirectly_readable<It> &&
                                             copy_constructible<F> &&
                                             regular_invocable<F &, iter_value_t<It> &> &&
                                             regular_invocable<F &, iter_reference_t<It>> &&
                                             regular_invocable<F &, iter_common_reference_t<It>> &&
                                             common_reference_with<invoke_result_t<F &, iter_value_t<It> &>,
                                                                   invoke_result_t<F &, iter_reference_t<It>>>;

template<class F, class It>
concept indirect_unary_predicate = indirectly_readable<It> &&
                                   copy_constructible<F> &&
                                   predicate<F &, iter_value_t<It> &> &&
                                   predicate<F &, iter_reference_t<It>> &&
                                   predicate<F &, iter_common_reference_t<It>>;

template<class F, class It1, class It2>
concept indirect_binary_predicate = indirectly_readable<It1> &&
                                    indirectly_readable<It2> &&
                                    copy_constructible<F> &&
                                    predicate<F &, iter_value_t<It1> &, iter_value_t<It2> &> &&
                                    predicate<F &, iter_value_t<It1> &, iter_reference_t<It2>> &&
                                    predicate<F &, iter_reference_t<It1>, iter_value_t<It2> &> &&
                                    predicate<F &, iter_reference_t<It1>, iter_reference_t<It2>> &&
                                    predicate<F &, iter_common_reference_t<It1>, iter_common_reference_t<It2>>;

template<class F, class It1, class It2 = It1>
concept indirect_equivalence_relation = indirectly_readable<It1> &&
                                        indirectly_readable<It2> &&
                                        copy_constructible<F> &&
                                        equivalence_relation<F &, iter_value_t<It1> &, iter_value_t<It2> &> &&
                                        equivalence_relation<F &, iter_value_t<It1> &, iter_reference_t<It2>> &&
                                        equivalence_relation<F &, iter_reference_t<It1>, iter_value_t<It2> &> &&
                                        equivalence_relation<F &, iter_reference_t<It1>, iter_reference_t<It2>> &&
                                        equivalence_relation<F &, iter_common_reference_t<It1>, iter_common_reference_t<It2>>;

template<class F, class It1, class It2 = It1>
concept indirect_strict_weak_order = indirectly_readable<It1> &&
                                     indirectly_readable<It2> &&
                                     copy_constructible<F> &&
                                     strict_weak_order<F &,iter_value_t<It1>&, iter_value_t<It2> &> &&
                                     strict_weak_order<F &, iter_value_t<It1> &, iter_reference_t<It2>> &&
                                     strict_weak_order<F &, iter_reference_t<It1>, iter_value_t<It2> &> &&
                                     strict_weak_order<F &, iter_reference_t<It1>, iter_reference_t<It2>> &&
                                     strict_weak_order<F &, iter_common_reference_t<It1>, iter_common_reference_t<It2>>;

template<class F, class... _Its>
requires(indirectly_readable<_Its> &&...) && invocable<F, iter_reference_t<_Its>...>
using indirect_result_t = invoke_result_t<F, iter_reference_t<_Its>...>;

template<class In, class Out>
concept indirectly_movable = indirectly_readable<In> &&
                             indirectly_writable<Out, iter_rvalue_reference_t<In>>;

template<class In, class Out>
concept indirectly_movable_storable = indirectly_movable<In, Out> &&
                                      indirectly_writable<Out, iter_value_t<In>> &&
                                      movable<iter_value_t<In>> &&
                                      constructible_from<iter_value_t<In>, iter_rvalue_reference_t<In>> &&
                                      assignable_from <iter_value_t<In>&, iter_rvalue_reference_t<In>> ;

template<class In, class Out>
concept indirectly_copyable = indirectly_readable<In> &&
                              indirectly_writable<Out, iter_reference_t<In>>;

template<class In, class Out>
concept indirectly_copyable_storable = indirectly_copyable<In, Out> &&
                                       indirectly_writable<Out, iter_value_t<In>&> && 
                                       indirectly_writable<Out, const iter_value_t<In>&> && 
                                       indirectly_writable<Out, iter_value_t<In>&&> && 
                                       indirectly_writable<Out, const iter_value_t<In>&&> && 
                                       copyable<iter_value_t<In>> && 
                                       constructible_from<iter_value_t<In>, iter_reference_t<In>> && 
                                       assignable_from<iter_value_t<In>&, iter_reference_t<In>>;

// clang-format on

namespace ranges {
namespace _iter_swap {

template<class I1, class I2>
void iter_swap(I1, I2) = delete;

template<class I1, class I2>
concept __enable_adl = (__class_or_enum<remove_cvref_t<I1>> ||
                        __class_or_enum<remove_cvref_t<I2>>) &&
                       requires(I1 &&x, I2 &&y) {
                           iter_swap(ala::forward<I1>(x), ala::forward<I2>(y));
                       };

template<class I1, class I2>
concept __readable_and_swappable =
    indirectly_readable<I1> && indirectly_readable<I2> &&
    swappable_with<iter_reference_t<I1>, iter_reference_t<I2>>;

struct _cpo_fn {
    template<class I1, class I2>
        requires __enable_adl<I1, I2>
    constexpr void operator()(I1 &&x, I2 &&y) const
        noexcept(noexcept(iter_swap(ala::forward<I1>(x), ala::forward<I2>(y)))) {
        (void)iter_swap(ala::forward<I1>(x), ala::forward<I2>(y));
    }

    template<class I1, class I2>
        requires(!__enable_adl<I1, I2>) && __readable_and_swappable<I1, I2>
    constexpr void operator()(I1 &&x, I2 &&y) const
        noexcept(noexcept(ranges::swap(*ala::forward<I1>(x),
                                       *ala::forward<I2>(y)))) {
        ranges::swap(*ala::forward<I1>(x), *ala::forward<I2>(y));
    }

    template<class I1, class I2>
        requires(!__enable_adl<I1, I2> && !__readable_and_swappable<I1, I2>) &&
                indirectly_movable_storable<I1, I2> &&
                indirectly_movable_storable<I2, I1>
    constexpr void operator()(I1 &&x, I2 &&y) const
        noexcept(noexcept(iter_value_t<I2>(ranges::iter_move(y)))
                     &&noexcept(*y = ranges::iter_move(x)) &&noexcept(
                         *ala::forward<I1>(x) = declval<iter_value_t<I2>>())) {
        iter_value_t<I2> __old(ranges::iter_move(y));
        *y = ranges::iter_move(x);
        *ala::forward<I1>(x) = ala::move(__old);
    }
};
} // namespace _iter_swap
inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V _iter_swap::_cpo_fn iter_swap;
} // namespace _cpos
} // namespace ranges

template<class I1, class I2 = I1>
concept indirectly_swappable = indirectly_readable<I1> &&
                               indirectly_readable<I2> &&
                               requires(const I1 i1, const I2 i2) {
                                   ranges::iter_swap(i1, i1);
                                   ranges::iter_swap(i2, i2);
                                   ranges::iter_swap(i1, i2);
                                   ranges::iter_swap(i2, i1);
                               };

template<indirectly_readable I, indirectly_regular_unary_invocable<I> Proj>
struct projected {
    using value_type = remove_cvref_t<indirect_result_t<Proj &, I>>;
    indirect_result_t<Proj &, I> operator*() const; // not defined
};

template<weakly_incrementable I, class Proj>
struct incrementable_traits<projected<I, Proj>> {
    using difference_type = iter_difference_t<I>;
};

// clang-format off

template <class It1, class It2, class R, class P1 = identity, class P2 = identity>
concept indirectly_comparable = indirect_binary_predicate<R, projected<It1, P1>, projected<It2, P2>>;

template <class _It>
concept permutable = forward_iterator<_It> &&
                     indirectly_movable_storable<_It, _It> &&
                     indirectly_swappable<_It, _It>;

namespace ranges { struct less; }
template <class It1, class It2, class Out, class R = ranges::less, class P1 = identity, class P2 = identity>
concept mergeable = input_iterator<It1> &&
                    input_iterator<It2> &&
                    weakly_incrementable<Out> &&
                    indirectly_copyable<It1, Out> &&
                    indirectly_copyable<It2, Out> &&
                    indirect_strict_weak_order<R, projected<It1, P1>, projected<It2, P2>>;
} // namespace ala

#else

#include <ala/compat/iterator.h>

#endif // _ALA_ENABLE_CONCEPTS

#endif