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
constexpr OutputIter copy(InputIter first, InputIter last, OutputIter out) {
    while (first != last)
        *out++ = *first++;
    return out;
}

template<class InputIter, class OutputIter>
constexpr OutputIter move(InputIter first, InputIter last, OutputIter out) {
    while (first != last)
        *out++ = ala::move(*first++);
    return out;
}

template<class ForwardIter, class T>
constexpr void fill(ForwardIter first, ForwardIter last, const T &value) {
    for (; first != last; ++first)
        *first = value;
}

template<class It1, class It2>
constexpr void iter_swap(It1 a, It2 b) {
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

template<class It1, class It2, class BinPred>
constexpr bool equal(It1 first1, It1 last1, It2 first2, BinPred pred) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

template<class It1, class It2>
constexpr bool equal(It1 first1, It1 last1, It2 first2) {
    return equal(first1, last1, first2, equal_to<>());
}

template<class It1, class It2, class BinPred>
constexpr bool equal(It1 first1, It1 last1, It2 first2, It2 last2, BinPred pred) {
    if (ala::distance(first1, last1) != ala::distance(first2, last2))
        return false;
    return equal(first1, last1, first2, pred);
}

template<class It1, class It2>
constexpr bool equal(It1 first1, It1 last1, It2 first2, It2 last2) {
    return equal(first1, last1, first2, last2, equal_to<>());
}

template<class It1, class It2, class Compare>
constexpr bool lexicographical_compare(It1 first1, It1 last1, It2 first2,
                                       It2 last2, Compare comp) {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
    return (first1 == last1) && (first2 != last2);
}

template<class It1, class It2>
constexpr bool lexicographical_compare(It1 first1, It1 last1, It2 first2,
                                       It2 last2) {
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