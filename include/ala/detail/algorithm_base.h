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
template<class T>
constexpr const T &min(const T &a, const T &b) {
    return min(a, b, less<>());
}

template<class T, class Comp>
constexpr const T &min(const T &a, const T &b, Comp comp) {
    return comp(b, a) ? b : a;
}

#ifdef max
#warning "undef max macro"
#undef max
#endif
template<class T>
constexpr const T &max(const T &a, const T &b) {
    return max(a, b, less<>());
}

template<class T, class Comp>
constexpr const T &max(const T &a, const T &b, Comp comp) {
    return comp(a, b) ? b : a;
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
    if (count > 0)
        for (Size i = 0; i < count; ++i)
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
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<InputIter>::value_type) * count);
    return out + count;
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
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<BidirIter1>::value_type) *
                     (last - first));
    return out + (last - first);
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
    ala::memmove((void *)(out), (void *)(first),
                 sizeof(typename iterator_traits<BidirIter1>::value_type) *
                     (last - first));
    return out + (last - first);
}

template<class ForwardIter, class T>
constexpr void fill(ForwardIter first, ForwardIter last, const T &value) {
    for (; first != last; ++first)
        *first = value;
}

template<class Iter1, class Iter2>
constexpr void iter_swap(Iter1 a, Iter2 b) {
    ala::swap(*a, *b);
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
    for (; first1 != last1; ++first1, ++first2) {
        if (!pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

template<class Iter1, class Iter2>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2) {
    return equal(first1, last1, first2, equal_to<>());
}

template<class Iter1, class Iter2, class BinPred>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                     BinPred pred) {
    if (ala::distance(first1, last1) != ala::distance(first2, last2))
        return false;
    return equal(first1, last1, first2, pred);
}

template<class Iter1, class Iter2>
constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2) {
    return equal(first1, last1, first2, last2, equal_to<>());
}

template<class Iter1, class Iter2, class Compare>
constexpr bool lexicographical_compare(Iter1 first1, Iter1 last1, Iter2 first2,
                                       Iter2 last2, Compare comp) {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
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
    return lexicographical_compare(first1, last1, first2, last2, less<>());
}

// Binary search operations (on sorted ranges)
template<class ForwardIterer, class T, class Comp>
constexpr ForwardIterer lower_bound(ForwardIterer first, ForwardIterer last,
                                    const T &value, Comp comp) {
    typename iterator_traits<ForwardIterer>::difference_type len, step;
    len = ala::distance(first, last);

    while (len > 0) {
        ForwardIterer i = first;
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

template<class ForwardIterer, class T>
constexpr ForwardIterer lower_bound(ForwardIterer first, ForwardIterer last,
                                    const T &value) {
    return ala::lower_bound(first, last, value, less<>());
}

template<class ForwardIterer, class T, class Comp>
constexpr ForwardIterer upper_bound(ForwardIterer first, ForwardIterer last,
                                    const T &value, Comp comp) {
    typename iterator_traits<ForwardIterer>::difference_type len, step;
    len = ala::distance(first, last);

    while (len > 0) {
        ForwardIterer i = first;
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

template<class ForwardIterer, class T>
constexpr ForwardIterer upper_bound(ForwardIterer first, ForwardIterer last,
                                    const T &value) {
    return ala::upper_bound(first, last, value, less<>());
}

template<class ForwardIterer, class T, class Comp>
constexpr pair<ForwardIterer, ForwardIterer>
equal_range(ForwardIterer first, ForwardIterer last, const T &value, Comp comp) {
    auto lower = ala::lower_bound(first, last, value, comp);
    return ala::make_pair(lower, ala::upper_bound(lower, last, value, comp));
}

template<class ForwardIterer, class T>
constexpr pair<ForwardIterer, ForwardIterer>
equal_range(ForwardIterer first, ForwardIterer last, const T &value) {
    return ala::equal_range(first, last, value, less<>());
}

template<class ForwardIterer, class T, class Comp>
constexpr bool binary_search(ForwardIterer first, ForwardIterer last,
                             const T &value, Comp comp) {
    first = ala::lower_bound(first, last, value, comp);
    return (!(first == last) && !(comp(value, *first)));
}

template<class ForwardIterer, class T>
constexpr bool binary_search(ForwardIterer first, ForwardIterer last,
                             const T &value) {
    return binary_search(first, last, value, less<>());
}

} // namespace ala

#endif // HEAD