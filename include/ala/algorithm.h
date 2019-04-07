#ifndef _ALA_ALGORITHM_H
#define _ALA_ALGORITHM_H

#include <ala/utility.h>
#include <ala/iterator.h>
#include <ala/random.h>
#include <ala/detail/algorithm_base.h>

namespace ala {

template<typename RanIt>
constexpr void shuffle(RanIt first, RanIt last) {
    typename iterator_traits<RanIt>::difference_type dis = last - first;
    xoshiro128p gen;
    for (; first < last; ++first)
        swap(*first, *(first + gen() % dis));
}

template<typename ForIt>
constexpr ForIt merge(ForIt first, ForIt last, ForIt first1, ForIt last1,
                      ForIt out) {
    while (first != last && first1 != last1)
        *out++ = (!(*first1 < *first)) ? *first++ : *first1++;
    while (first != last)
        *out++ = *first++;
    while (first1 != last1)
        *out++ = *first1++;
    return out;
}

template<class BiIt>
void reverse(BiIt first, BiIt last) {
    while ((first != last) && (first != --last))
        iter_swap(first++, last);
}

template<class BiIt, class Comp>
constexpr bool prev_permutation(BiIt first, BiIt last, Comp comp) {
    if (first == last)
        return false;
    auto cur = last, pre = last;
    if (--cur == first)
        return false;
    ----pre;
    while (cur != first && comp(*pre, *cur))
        --cur, --pre;
    if (cur == first) {
        reverse(first, last);
        return false;
    }
    cur = last, --cur;
    while (cur != first && comp(*pre, *cur))
        --cur;
    iter_swap(cur, pre);
    reverse(++pre, last);
    return true;
}


template<class BiIt>
constexpr bool prev_permutation(BiIt first, BiIt last) {
    return ala::prev_permutation(first, last, less<>());
}

template<class BiIt, class Comp>
constexpr bool next_permutation(BiIt first, BiIt last, Comp comp) {
    if (first == last)
        return false;
    auto cur = last, pre = last;
    if (--cur == first)
        return false;
    ----pre;
    while (cur != first && comp(*cur, *pre))
        --cur, --pre;
    if (cur == first) {
        reverse(first, last);
        return false;
    }
    cur = last, --cur;
    while (cur != first && comp(*cur, *pre))
        --cur;
    iter_swap(cur, pre);
    reverse(++pre, last);
    return true;
}

template<class BiIt>
constexpr bool next_permutation(BiIt first, BiIt last) {
    return ala::next_permutation(first, last, less<>());
}

} // namespace ala

#include <ala/detail/sort.h>

#endif // HEAD