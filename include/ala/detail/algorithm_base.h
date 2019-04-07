#ifndef _ALA_DETAIL_ALGORITHM_BASE_H
#define _ALA_DETAIL_ALGORITHM_BASE_H

#include <ala/detail/functional_base.h>

namespace ala {

template<class It1, class It2>
constexpr bool equal(It1 first1, It1 last1, It2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

template<class It1, class It2>
constexpr void iter_swap(It1 a, It2 b) {
    ala::swap(*a, *b);
}

template<class It1, class It2>
constexpr bool lexicographical_compare(It1 first1, It1 last1,
                                       It2 first2, It2 last2) {
    return lexicographical_compare(first1, last1, first2, last2, less<>());
}

template<class It1, class It2, class Compare>
constexpr bool lexicographical_compare(It1 first1, It1 last1,
                                       It2 first2, It2 last2,
                                       Compare comp) {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
    return (first1 == last1) && (first2 != last2);
}

}

#endif // HEAD