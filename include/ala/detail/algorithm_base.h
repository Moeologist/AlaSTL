#ifndef _ALA_DETAIL_ALGORITHM_BASE_H
#define _ALA_DETAIL_ALGORITHM_BASE_H

#include <ala/detail/functional_base.h>
#include <ala/detail/pair.h>
#include <ala/iterator.h>

namespace ala {

// Minimum/maximum operations
#ifdef min
    #warning "undef min macro"
    #undef min
#endif

template<class T, class Comp>
constexpr const T &min(const T &a, const T &b, Comp comp) {
    return comp(b, a) ? b : a;
}

template<class T>
constexpr const T &min(const T &a, const T &b) {
    return ala::min(a, b, less<>());
}

#ifdef max
    #warning "undef max macro"
    #undef max
#endif

template<class T, class Comp>
constexpr const T &max(const T &a, const T &b, Comp comp) {
    return comp(a, b) ? b : a;
}

template<class T>
constexpr const T &max(const T &a, const T &b) {
    return ala::max(a, b, less<>());
}

// Modifying sequence operations

template<class InputIter, class OutputIter>
constexpr enable_if_t<
    !(is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
      is_same<typename iterator_traits<InputIter>::value_type,
              typename iterator_traits<OutputIter>::value_type>::value &&
      is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
copy(InputIter first, InputIter last, OutputIter out) {
    while (first != last)
        *out++ = *first++;
    return out;
}

template<class InputIter, class OutputIter>
constexpr enable_if_t<
    (is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
     is_same<typename iterator_traits<InputIter>::value_type,
             typename iterator_traits<OutputIter>::value_type>::value &&
     is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
copy(InputIter first, InputIter last, OutputIter out) {
#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
    if (ala::is_constant_evaluated()) {
        while (first != last)
            *out++ = *first++;
        return out + (last - first);
    }
#endif
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<InputIter>::value_type) *
                     (last - first));
    return out + (last - first);
}

template<class InputIter, class OutputIter>
constexpr enable_if_t<
    !(is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
      is_same<typename iterator_traits<InputIter>::value_type,
              typename iterator_traits<OutputIter>::value_type>::value &&
      is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
move(InputIter first, InputIter last, OutputIter out) {
    while (first != last)
        *out++ = ala::move(*first++);
    return out;
}

template<class InputIter, class OutputIter>
constexpr enable_if_t<
    (is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
     is_same<typename iterator_traits<InputIter>::value_type,
             typename iterator_traits<OutputIter>::value_type>::value &&
     is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
move(InputIter first, InputIter last, OutputIter out) {
#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
    if (ala::is_constant_evaluated()) {
        while (first != last)
            *out++ = ala::move(*first++);
        return out + (last - first);
    }
#endif
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<InputIter>::value_type) *
                     (last - first));
    return out + (last - first);
}

template<class InputIter, class Size, class OutputIter>
constexpr enable_if_t<
    !(is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
      is_same<typename iterator_traits<InputIter>::value_type,
              typename iterator_traits<OutputIter>::value_type>::value &&
      is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
copy_n(InputIter first, Size count, OutputIter out) {
    auto n = ala::_convert_to_integral(count);
    for (; 0 < n; --n)
        *out++ = *first++;
    return out;
}

template<class InputIter, class Size, class OutputIter>
constexpr enable_if_t<
    (is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
     is_same<typename iterator_traits<InputIter>::value_type,
             typename iterator_traits<OutputIter>::value_type>::value &&
     is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
copy_n(InputIter first, Size count, OutputIter out) {
    auto n = ala::_convert_to_integral(count);
#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
    if (ala::is_constant_evaluated()) {
        for (; 0 < n; --n)
            *out++ = *first++;
        return out + n;
    }
#endif
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<InputIter>::value_type) * n);
    return out + n;
}

template<class InputIter, class Size, class OutputIter>
constexpr enable_if_t<
    !(is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
      is_same<typename iterator_traits<InputIter>::value_type,
              typename iterator_traits<OutputIter>::value_type>::value &&
      is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
move_n(InputIter first, Size count, OutputIter out) {
    auto n = ala::_convert_to_integral(count);
    for (; 0 < n; --n)
        *out++ = ala::move(*first++);
    return out;
}

template<class InputIter, class Size, class OutputIter>
constexpr enable_if_t<
    (is_trivial<typename iterator_traits<InputIter>::value_type>::value &&
     is_same<typename iterator_traits<InputIter>::value_type,
             typename iterator_traits<OutputIter>::value_type>::value &&
     is_pointer<InputIter>::value && is_pointer<OutputIter>::value),
    OutputIter>
move_n(InputIter first, Size count, OutputIter out) {
    auto n = ala::_convert_to_integral(count);
#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
    if (ala::is_constant_evaluated()) {
        for (; 0 < n; --n)
            *out++ = ala::move(*first++);
        return out;
    }
#endif
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<InputIter>::value_type) * n);
    return out + n;
}

template<class BidirIter1, class BidirIter2>
constexpr enable_if_t<
    !(is_trivial<typename iterator_traits<BidirIter1>::value_type>::value &&
      is_same<typename iterator_traits<BidirIter1>::value_type,
              typename iterator_traits<BidirIter2>::value_type>::value &&
      is_pointer<BidirIter1>::value && is_pointer<BidirIter2>::value),
    BidirIter2>
copy_backward(BidirIter1 first, BidirIter1 last, BidirIter2 out) {
    while (first != last)
        *--out = *--last;
    return out;
}

template<class BidirIter1, class BidirIter2>
constexpr enable_if_t<
    (is_trivial<typename iterator_traits<BidirIter1>::value_type>::value &&
     is_same<typename iterator_traits<BidirIter1>::value_type,
             typename iterator_traits<BidirIter2>::value_type>::value &&
     is_pointer<BidirIter1>::value && is_pointer<BidirIter2>::value),
    BidirIter2>
copy_backward(BidirIter1 first, BidirIter1 last, BidirIter2 out) {
    size_t len = last - first;
    size_t nbyte = sizeof(typename iterator_traits<BidirIter1>::value_type) *
                   (last - first);
#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
    if (ala::is_constant_evaluated()) {
        while (first != last)
            *--out = *--last;
        return out;
    }
#endif
    ala::memmove((void *)(out - len), (void *)(first), nbyte);
    return out - len;
}

template<class BidirIter1, class BidirIter2>
constexpr enable_if_t<
    !(is_trivial<typename iterator_traits<BidirIter1>::value_type>::value &&
      is_same<typename iterator_traits<BidirIter1>::value_type,
              typename iterator_traits<BidirIter2>::value_type>::value &&
      is_pointer<BidirIter1>::value && is_pointer<BidirIter2>::value),
    BidirIter2>
move_backward(BidirIter1 first, BidirIter1 last, BidirIter2 out) {
    while (first != last)
        *--out = ala::move(*--last);
    return out;
}

template<class BidirIter1, class BidirIter2>
constexpr enable_if_t<
    (is_trivial<typename iterator_traits<BidirIter1>::value_type>::value &&
     is_same<typename iterator_traits<BidirIter1>::value_type,
             typename iterator_traits<BidirIter2>::value_type>::value &&
     is_pointer<BidirIter1>::value && is_pointer<BidirIter2>::value),
    BidirIter2>
move_backward(BidirIter1 first, BidirIter1 last, BidirIter2 out) {
    size_t len = last - first;
    size_t nbyte = sizeof(typename iterator_traits<BidirIter1>::value_type) *
                   (last - first);
#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
    if (ala::is_constant_evaluated()) {
        while (first != last)
            *--out = ala::move(*--last);
        return out;
    }
#endif
    ala::memmove((void *)(out - len), (void *)(first), nbyte);
    return out - len;
}

template<class ForwardIter, class T>
constexpr void fill(ForwardIter first, ForwardIter last, const T &value) {
    for (; first != last; ++first)
        *first = value;
}

template<class Iter1, class Iter2>
constexpr void iter_swap(Iter1 a, Iter2 b) {
    using ala::swap;
    swap(*a, *b);
}

template<class ForwardIter1, class ForwardIter2>
constexpr ForwardIter2 swap_ranges(ForwardIter1 first1, ForwardIter1 last1,
                                   ForwardIter2 first2) {
    while (first1 != last1)
        ala::iter_swap(first1++, first2++);
    return first2;
}

// Comparison operations

template<class Iter1, class Iter2, class BinPred>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, BinPred pred) {
    for (; first1 != last1; ++first1, (void)++first2)
        if (!pred(*first1, *first2))
            return false;
    return true;
}

template<class Iter1, class Iter2>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2) {
    return ala::equal(first1, last1, first2, equal_to<>());
}

template<class Iter1, class Iter2, class BinPred>
constexpr bool _equal_dispatch(Iter1 first1, Iter1 last1, Iter2 first2,
                               Iter2 last2, BinPred pred, false_type) {
    for (; first1 != last1 && first2 != last2; ++first1, (void)++first2)
        if (!pred(*first1, *first2))
            return false;
    if (first1 == last1 && first2 == last2)
        return true;
    return false;
}

template<class Iter1, class Iter2, class BinPred>
constexpr bool _equal_dispatch(Iter1 first1, Iter1 last1, Iter2 first2,
                               Iter2 last2, BinPred pred, true_type) {
    if (ala::distance(first1, last1) != ala::distance(first2, last2))
        return false;
    return ala::equal(first1, last1, first2, pred);
}

template<class Iter1, class Iter2, class BinPred>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                     BinPred pred) {
    using tag_t =
        _and_<is_base_of<random_access_iterator_tag, _iter_tag_t<Iter1>>,
              is_base_of<random_access_iterator_tag, _iter_tag_t<Iter2>>>;
    return ala::_equal_dispatch(first1, last1, first2, last2, pred, tag_t{});
}

template<class Iter1, class Iter2>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2) {
    return ala::equal(first1, last1, first2, last2, equal_to<>());
}

template<class Iter1, class Iter2, class Compare>
constexpr bool lexicographical_compare(Iter1 first1, Iter1 last1, Iter2 first2,
                                       Iter2 last2, Compare comp) {
    for (; (first1 != last1) && (first2 != last2); ++first1, (void)++first2) {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
    return (first1 == last1) && (first2 != last2);
}

template<class Iter1, class Iter2>
constexpr bool lexicographical_compare(Iter1 first1, Iter1 last1, Iter2 first2,
                                       Iter2 last2) {
    return ala::lexicographical_compare(first1, last1, first2, last2, less<>());
}

// Binary search operations (on sorted ranges)
template<class ForwardIter, class T, class Comp>
constexpr ForwardIter lower_bound(ForwardIter first, ForwardIter last,
                                  const T &value, Comp comp) {
    typename iterator_traits<ForwardIter>::difference_type len, step;
    len = ala::distance(first, last);

    while (len > 0) {
        ForwardIter i = first;
        step = len / 2;
        ala::advance(i, step);
        if (comp(*i, value)) {
            first = ++i;
            len -= step + 1;
        } else
            len = step;
    }
    return first;
}

template<class ForwardIter, class T>
constexpr ForwardIter lower_bound(ForwardIter first, ForwardIter last,
                                  const T &value) {
    return ala::lower_bound(first, last, value, less<>());
}

template<class ForwardIter, class T, class Comp>
constexpr ForwardIter upper_bound(ForwardIter first, ForwardIter last,
                                  const T &value, Comp comp) {
    typename iterator_traits<ForwardIter>::difference_type len, step;
    len = ala::distance(first, last);

    while (len > 0) {
        ForwardIter i = first;
        step = len / 2;
        ala::advance(i, step);
        if (!comp(value, *i)) {
            first = ++i;
            len -= step + 1;
        } else
            len = step;
    }
    return first;
}

template<class ForwardIter, class T>
constexpr ForwardIter upper_bound(ForwardIter first, ForwardIter last,
                                  const T &value) {
    return ala::upper_bound(first, last, value, less<>());
}

template<class ForwardIter, class T, class Comp>
constexpr pair<ForwardIter, ForwardIter>
equal_range(ForwardIter first, ForwardIter last, const T &value, Comp comp) {
    auto lower = ala::lower_bound(first, last, value, comp);
    return ala::make_pair(lower, ala::upper_bound(lower, last, value, comp));
}

template<class ForwardIter, class T>
constexpr pair<ForwardIter, ForwardIter>
equal_range(ForwardIter first, ForwardIter last, const T &value) {
    return ala::equal_range(first, last, value, less<>());
}

template<class ForwardIter, class T, class Comp>
constexpr bool binary_search(ForwardIter first, ForwardIter last,
                             const T &value, Comp comp) {
    first = ala::lower_bound(first, last, value, comp);
    return (!(first == last) && !(comp(value, *first)));
}

template<class ForwardIter, class T>
constexpr bool binary_search(ForwardIter first, ForwardIter last, const T &value) {
    return ala::binary_search(first, last, value, less<>());
}

template<class InputIter, class T>
constexpr InputIter find(InputIter first, InputIter last, const T &value) {
    for (; first != last; ++first)
        if (*first == value)
            return first;
    return last;
}

template<class InputIter, class UnaryPred>
constexpr InputIter find_if(InputIter first, InputIter last, UnaryPred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            return first;
    return last;
}

template<class ForwardIter, class T>
constexpr ForwardIter remove(ForwardIter first, ForwardIter last, const T &value) {
    first = ala::find(first, last, value);
    if (first != last)
        for (ForwardIter i = first; ++i != last;)
            if (!(*i == value))
                *first++ = ala::move(*i);
    return first;
}

template<class ForwardIter, class UnaryPred>
constexpr ForwardIter remove_if(ForwardIter first, ForwardIter last,
                                UnaryPred pred) {
    first = ala::find_if(first, last, pred);
    if (first != last)
        for (ForwardIter i = first; ++i != last;)
            if (!pred(*i))
                *first++ = ala::move(*i);
    return first;
}

template<class ForwardIter>
constexpr ForwardIter _rotate_left(ForwardIter first, ForwardIter last) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    T tmp = ala::move(*first);
    ForwardIter r = ala::move(ala::next(first), last, first);
    *r = ala::move(tmp);
    return r;
}

template<class BidirIter>
constexpr BidirIter _rotate_right(BidirIter first, BidirIter last) {
    using T = typename iterator_traits<BidirIter>::value_type;
    BidirIter mid = ala::prev(last);
    T tmp = ala::move(*mid);
    BidirIter r = ala::move_backward(first, mid, last);
    *first = ala::move(tmp);
    return r;
}

template<class ForwardIter>
constexpr ForwardIter _rotate_forward(ForwardIter first, ForwardIter middle,
                                      ForwardIter last) {
    ForwardIter next = middle;
    while (middle != last) {
        if (first == next)
            next = middle;
        ala::iter_swap(first++, middle++);
    }
    middle = next;
    ForwardIter r = first;
    for (; middle != first; middle = next) {
        next = middle;
        while (middle != last) {
            if (first == next)
                next = middle;
            ala::iter_swap(first++, middle++);
        }
    }
    return r;
}

template<class ForwardIter>
constexpr ForwardIter _rotate_dispatch(ForwardIter first, ForwardIter middle,
                                       ForwardIter last, forward_iterator_tag) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    if (is_trivially_move_assignable<T>::value) {
        if (ala::next(first) == middle)
            return ala::_rotate_left(first, last);
    }
    return ala::_rotate_forward(first, middle, last);
}

template<class ForwardIter>
constexpr ForwardIter _rotate_dispatch(ForwardIter first, ForwardIter middle,
                                       ForwardIter last,
                                       bidirectional_iterator_tag) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    if (is_trivially_move_assignable<T>::value) {
        if (ala::next(first) == middle)
            return ala::_rotate_left(first, last);
        if (ala::next(middle) == last)
            return ala::_rotate_right(first, last);
    }
    return ala::_rotate_forward(first, middle, last);
}

template<class ForwardIter>
constexpr ForwardIter rotate(ForwardIter first, ForwardIter middle,
                             ForwardIter last) {
    using tag_t = _iter_tag_t<ForwardIter>;
    if (first == middle)
        return last;
    if (middle == last)
        return first;
    return ala::_rotate_dispatch(first, middle, last, tag_t{});
}

} // namespace ala

#endif // HEAD