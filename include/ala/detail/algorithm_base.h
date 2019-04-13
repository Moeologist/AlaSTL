#ifndef _ALA_DETAIL_ALGORITHM_BASE_H
#define _ALA_DETAIL_ALGORITHM_BASE_H

#include <ala/detail/functional_base.h>
#include <ala/iterator.h>

namespace ala {

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
constexpr bool equal(It1 first1, It1 last1, It2 first2, It2 last2,
           BinPred pred) {
    if (ala::distance(first1, last1) != ala::distance(first2, last2))
        return false;
    return equal(first1, last1, first2, pred);
}

template<class It1, class It2>
constexpr bool equal(It1 first1, It1 last1, It2 first2, It2 last2) {
    return equal(first1, last1, first2, last2, equal_to<>());
}

template<class It1, class It2>
constexpr void iter_swap(It1 a, It2 b) {
    ala::swap(*a, *b);
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

} // namespace ala

#endif // HEAD