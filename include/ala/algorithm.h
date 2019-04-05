#ifndef _ALA_ALGORITHM_H
#define _ALA_ALGORITHM_H

#include "ala/utility.h"

namespace ala {

template<class It1, class It2>
inline void iter_swap(It1 a, It2 b) {
    ala::swap(*a, *b);
}

// template <typename It>
// inline void shuffle(It m, It n) {
// 	size_t num = limit - m;
// 	for (size_t i = 0; i < n - m; ++i)
// 		swap(*(m + i), *(m + xorgen() % num));
// }

template<typename ForIt>
inline ForIt merge(ForIt first, ForIt last, ForIt f1, ForIt l1, ForIt out) {
    while (first != last && f1 != l1)
        *out++ = (*first < *f1 || *first == *f1) ? *first++ : *f1++;
    while (first != last)
        *out++ = *first++;
    while (f1 != l1)
        *out++ = *f1++;
    return out;
}

template<class BiIt>
bool next_permutation(BiIt first, BiIt last) {
    if (first == last)
        return false;
    BiIt i = first;
    if (++i == last)
        return false;
    while (true) {
        BiIt i1, i2;
        i1 = i;
        if (*--i < *i1) {
            i2 = last;
            while (!(*i < *--i2))
                ;
            std::iter_swap(i, i2);
            std::reverse(i1, last);
            return true;
        }
        if (i == first) {
            std::reverse(first, last);
            return false;
        }
    }
}

} // namespace ala

#include <ala/detail/sort.h>

#endif // HEAD