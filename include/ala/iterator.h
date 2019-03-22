#ifndef _ALA_ITERATOR_H
#define _ALA_ITERATOR_H

namespace ala {
template <typename Iter> struct iterator_traits {
  using value_type = typename Iter::value_type;
};

template <typename Iter> struct iterator_traits<const Iter> {
  using value_type = typename Iter::value_type;
};

template <typename Iter> struct iterator_traits<Iter *> {
  using value_type = Iter;
};

template <typename Iter> struct iterator_traits<const Iter *> {
  using value_type = Iter;
};

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag {};
struct bidirectional_iterator_tag {};
struct random_access_iterator_tag {};
struct contiguous_iterator_tag {};

template <class Iter> struct reverse_iterator {
  typedef typename Iter::iterator_category iterator_category;
  typedef typename Iter::value_type value_type;
  typedef typename Iter::difference_type difference_type;
  typedef typename Iter::pointer pointer;
  typedef typename Iter::reference reference;
  typedef Iter iterator_type;

  constexpr reverse_iterator() : current() {}
  constexpr explicit reverse_iterator(iterator_type base) : current(base) {}

  template <class U>
  constexpr reverse_iterator(const reverse_iterator<U> &other)
      : current(other.current) {}

  constexpr iterator_type base() const { return current; }

  template <class U>
  constexpr reverse_iterator &operator=(const reverse_iterator<U> &other) {
    current = other.current;
  }

  constexpr reference operator*() const { return *current; }
  constexpr pointer operator->() const { return current.operator->(); }
  constexpr reference operator[](difference_type n) const;

  constexpr reverse_iterator &operator++() { return --current; }
  constexpr reverse_iterator &operator--() { return ++current; }
  constexpr reverse_iterator operator++(int) { return current++; }
  constexpr reverse_iterator operator--(int) { return current--; }
  constexpr reverse_iterator operator+(difference_type n) const;
  constexpr reverse_iterator operator-(difference_type n) const;
  constexpr reverse_iterator &operator+=(difference_type n);
  constexpr reverse_iterator &operator-=(difference_type n);

protected:
  iterator_type current;
};

template <class Iter1, class Iter2>
constexpr bool operator==(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs) {
  return lhs.base() == rhs.base();
}

template <class Iter1, class Iter2>
constexpr bool operator!=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs) {
  return lhs.base() != rhs.base();
}

template <class Iter1, class Iter2>
constexpr bool operator<(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs);

template <class Iter1, class Iter2>
constexpr bool operator<=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs);

template <class Iter1, class Iter2>
constexpr bool operator>(const reverse_iterator<Iter1> &lhs,
                         const reverse_iterator<Iter2> &rhs);

template <class Iter1, class Iter2>
constexpr bool operator>=(const reverse_iterator<Iter1> &lhs,
                          const reverse_iterator<Iter2> &rhs);

template <class Iter>
constexpr reverse_iterator<Iter> make_reverse_iterator(Iter i) {
    return reverse_iterator<Iter>(i);
};

} // namespace ala

#endif