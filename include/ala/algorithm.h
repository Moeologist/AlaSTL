#ifndef _ALA_ALGORITHM_H
#define _ALA_ALGORITHM_H

#include <ala/utility.h>
#include <ala/iterator.h>
#include <ala/random.h>
#include <ala/detail/algorithm_base.h>

namespace ala {

// Non-modifying sequence operations

template<class Iter, class T>
constexpr Iter find(Iter first, Iter last, const T &value) {
    for (; first != last; ++first)
        if (*first == value)
            return first;
    return last;
}
template<class Iter, class UnaryPred>
constexpr Iter find_if(Iter first, Iter last, UnaryPred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            return first;
    return last;
}
template<class Iter, class UnaryPred>
constexpr Iter find_if_not(Iter first, Iter last, UnaryPred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            return first;
    return last;
}

template<class Iter, class UnaryPred>
constexpr bool all_of(Iter first, Iter last, UnaryPred pred) {
    return ala::find_if_not(first, last, pred) == last;
}

template<class Iter, class UnaryPred>
constexpr bool any_of(Iter first, Iter last, UnaryPred pred) {
    return ala::find_if(first, last, pred) != last;
}

template<class Iter, class UnaryPred>
constexpr bool none_of(Iter first, Iter last, UnaryPred pred) {
    return ala::find_if(first, last, pred) == last;
}

template<class Iter, class Fn>
constexpr Fn for_each(Iter first, Iter last, Fn f) {
    for (; first != last; ++first)
        f(*first);
    return f;
}

template<class Iter, class Size, class Fn>
constexpr Iter for_each_n(Iter first, Size n, Fn f) {
    for (Size i = 0; i < n; ++first, ++i)
        f(*first);
    return first;
}

template<class ForwardIterer1, class ForwardIterer2, class BinPred>
ForwardIterer1 search(ForwardIterer1 first1, ForwardIterer1 last1,
                      ForwardIterer2 first2, ForwardIterer2 last2, BinPred pred) {
    for (;; ++first1) {
        ForwardIterer1 it1 = first1;
        for (ForwardIterer2 it2 = first2;; ++it1, ++it2) {
            if (it2 == last2)
                return first1;
            if (it1 == last1)
                return last1;
            if (!pred(*it1, *it2))
                break;
        }
    }
}

template<class ForwardIterer1, class ForwardIterer2>
ForwardIterer1 search(ForwardIterer1 first1, ForwardIterer1 last1,
                      ForwardIterer2 first2, ForwardIterer2 last2) {
    return search(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIterer1, class ForwardIterer2, class BinPred>
constexpr ForwardIterer1 find_end(ForwardIterer1 first1, ForwardIterer1 last1,
                                  ForwardIterer2 first2, ForwardIterer2 last2,
                                  BinPred pred) {
    if (first2 == last2)
        return last1;
    ForwardIterer1 ret = last1;
    while (true) {
        ForwardIterer1 ret1 = ala::search(first1, last1, first2, last2, pred);
        if (ret1 == last1) {
            break;
        } else {
            ret = ret1;
            first1 = ret1;
            ++first1;
        }
    }
    return ret;
}

template<class ForwardIterer1, class ForwardIterer2>
constexpr ForwardIterer1 find_end(ForwardIterer1 first1, ForwardIterer1 last1,
                                  ForwardIterer2 first2, ForwardIterer2 last2) {
    return find_end(first1, last2, first2, last2, equal_to<>());
}

template<class Iter, class ForwardIterer, class BinPred>
constexpr Iter find_first_of(Iter first1, Iter last1, ForwardIterer first2,
                             ForwardIterer last2, BinPred pred) {
    for (; first1 != last1; ++first1)
        for (ForwardIterer it = first2; it != last2; ++it)
            if (pred(*first1, *it))
                return first1;
    return last1;
}

template<class Iter, class ForwardIterer>
constexpr Iter find_first_of(Iter first1, Iter last1, ForwardIterer first2,
                             ForwardIterer last2) {
    return find_first_of(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIterer, class BinPred>
constexpr ForwardIterer adjacent_find(ForwardIterer first, ForwardIterer last,
                                      BinPred pred) {
    if (first == last)
        return last;
    ForwardIterer next = first;
    for (++next; next != last; ++next, ++first)
        if (pred(*first, *next))
            return first;
    return last;
}

template<class ForwardIterer>
constexpr ForwardIterer adjacent_find(ForwardIterer first, ForwardIterer last) {
    return adjacent_find(first, last, ala::equal_to<>());
}

template<class Iter, class T>
constexpr typename iterator_traits<Iter>::difference_type
count(Iter first, Iter last, const T &value) {
    typename iterator_traits<Iter>::difference_type ret = 0;
    for (; first != last; ++first)
        if (*first == value)
            ret++;
    return ret;
}

template<class Iter, class UnaryPred>
constexpr typename iterator_traits<Iter>::difference_type
count_if(Iter first, Iter last, UnaryPred pred) {
    typename iterator_traits<Iter>::difference_type ret = 0;
    for (; first != last; ++first)
        if (pred(*first))
            ret++;
    return ret;
}

template<class Iter1, class Iter2, class BinPred>
constexpr pair<Iter1, Iter2> mismatch(Iter1 first1, Iter1 last1, Iter2 first2,
                                      BinPred pred) {
    while (first1 != last1 && pred(*first1, *first2))
        ++first1, ++first2;
    return ala::make_pair(first1, first2);
}

template<class Iter1, class Iter2>
constexpr pair<Iter1, Iter2> mismatch(Iter1 first1, Iter1 last1, Iter2 first2) {
    return mismatch(first1, last1, first2, equal_to<>());
}

template<class Iter1, class Iter2, class BinPred>
constexpr pair<Iter1, Iter2> mismatch(Iter1 first1, Iter1 last1, Iter2 first2,
                                      Iter2 last2, BinPred pred) {
    while (first1 != last1 && first2 != last2 && pred(*first1, *first2))
        ++first1, ++first2;
    return ala::make_pair(first1, first2);
}

template<class Iter1, class Iter2>
constexpr pair<Iter1, Iter2> mismatch(Iter1 first1, Iter1 last1, Iter2 first2,
                                      Iter2 last2) {
    return mismatch(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIterer, class Size, class T, class BinPred>
constexpr ForwardIterer search_n(ForwardIterer first, ForwardIterer last,
                                 Size count, const T &value, BinPred pred) {
    if (count <= 0)
        return first;
    for (; first != last; ++first) {
        if (!pred(*first, value)) {
            continue;
        }
        ForwardIterer candidate = first;
        Size cur_count = 0;
        while (true) {
            ++cur_count;
            if (cur_count >= count)
                return candidate;
            ++first;
            if (first == last)
                return last;
            if (!pred(*first, value))
                break;
        }
    }
    return last;
}

template<class ForwardIterer, class Size, class T>
constexpr ForwardIterer search_n(ForwardIterer first, ForwardIterer last,
                                 Size count, const T &value) {
    return search_n(first, last, count, value, equal_to<>());
}

// Modifying sequence operations
template<class Iter, class OutIter>
constexpr OutIter copy(Iter first, Iter last, OutIter out) {
    while (first != last)
        *out++ = *first++;
    return out;
}

template<class Iter, class Size, class OutIter>
constexpr OutIter copy_n(Iter first, Size count, OutIter out) {
    if (count > 0)
        for (Size i = 0; i < count; ++i)
            *out++ = *first++;
    return out;
}

template<class Iter, class OutIter, class UnaryPred>
constexpr OutIter copy_if(Iter first, Iter last, OutIter out, UnaryPred pred) {
    while (first != last)
        if (pred(*first))
            *out++ = *first++;
    return out;
}

template<class BidirIter1, class BidirIter2>
constexpr BidirIter2 copy_backward(BidirIter1 first, BidirIter1 last,
                                   BidirIter2 out) {
    while (first != last)
        *--out = *--last;
    return out;
}

template<class Iter, class OutIter>
constexpr OutIter move(Iter first, Iter last, OutIter out) {
    while (first != last)
        *out++ = ala::move(*first++);
    return out;
}

template<class BidirIter1, class BidirIter2>
constexpr BidirIter2 move_backward(BidirIter1 first, BidirIter1 last,
                                   BidirIter2 out) {
    while (first != last)
        *--out = ala::move(*--last);
    return out;
}

template<class ForwardIterer1, class ForwardIterer2>
constexpr ForwardIterer2 swap_ranges(ForwardIterer1 first1, ForwardIterer1 last1,
                                     ForwardIterer2 first2) {
    while (first1 != last1)
        ala::iter_swap(first1++, first2++);
    return first2;
}

template<class Iter, class OutIter, class UnaryOperation>
constexpr OutIter transform(Iter first, Iter last, OutIter out,
                            UnaryOperation unary_op) {
    while (first != last)
        *out++ = unary_op(*first++);
    return out;
}

template<class Iter1, class Iter2, class OutIter, class BinaryOperation>
constexpr OutIter transform(Iter1 first1, Iter1 last1, Iter2 first2,
                            OutIter out, BinaryOperation binary_op) {
    while (first1 != last1)
        *out++ = binary_op(*first1++, *first2++);
    return out;
}

template<class ForwardIterer, class T>
constexpr void replace(ForwardIterer first, ForwardIterer last,
                       const T &old_value, const T &new_value) {
    for (; first != last; ++first)
        if (*first == old_value)
            *first = new_value;
}

template<class ForwardIterer, class UnaryPred, class T>
constexpr void replace_if(ForwardIterer first, ForwardIterer last,
                          UnaryPred pred, const T &new_value) {
    for (; first != last; ++first)
        if (pred(*first))
            *first = new_value;
}

template<class Iter, class OutIter, class T>
constexpr OutIter replace_copy(Iter first, Iter last, OutIter out,
                               const T &old_value, const T &new_value) {
    for (; first != last; ++first)
        *out++ = (*first == old_value) ? new_value : *first;
    return out;
}

template<class Iter, class OutIter, class UnaryPred, class T>
constexpr OutIter replace_copy_if(Iter first, Iter last, OutIter out,
                                  UnaryPred pred, const T &new_value) {
    for (; first != last; ++first)
        *out++ = pred(*first) ? new_value : *first;
    return out;
}

template<class ForwardIterer, class T>
constexpr void fill(ForwardIterer first, ForwardIterer last, const T &value) {
    for (; first != last; ++first)
        *first = value;
}

template<class OutIter, class Size, class T>
constexpr OutIter fill_n(OutIter first, Size count, const T &value) {
    if (count > 0)
        for (Size i = 0; i < count; ++i)
            *first++ = value;
    return first;
}

template<class ForwardIterer, class Generator>
constexpr void generate(ForwardIterer first, ForwardIterer last, Generator gen) {
    for (; first != last; ++first)
        *first = gen();
}

template<class OutIter, class Size, class Generator>
constexpr OutIter generate_n(OutIter first, Size count, Generator gen) {
    if (count > 0)
        for (Size i = 0; i < count; ++i)
            *first++ = gen();
    return first;
}

template<class ForwardIterer, class T>
constexpr ForwardIterer remove(ForwardIterer first, ForwardIterer last,
                               const T &value) {
    if (first != last)
        for (ForwardIterer i = first; ++i != last;)
            if (*i != value)
                *first++ = ala::move(*i);
    return first;
}

template<class ForwardIterer, class UnaryPred>
constexpr ForwardIterer remove_if(ForwardIterer first, ForwardIterer last,
                                  UnaryPred pred) {
    if (first != last)
        for (ForwardIterer i = first; ++i != last;)
            if (!pred(*i))
                *first++ = ala::move(*i);
    return first;
}

template<class Iter, class OutIter, class T>
constexpr OutIter remove_copy(Iter first, Iter last, OutIter out, const T &value) {
    for (; first != last; ++first)
        if (!(*first == value))
            *out++ = *first;
    return out;
}

template<class Iter, class OutIter, class UnaryPred>
constexpr OutIter remove_copy_if(Iter first, Iter last, OutIter out,
                                 UnaryPred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            *out++ = *first;
    return out;
}

template<class ForwardIterer>
constexpr ForwardIterer unique(ForwardIterer first, ForwardIterer last) {
    if (first == last)
        return last;
    ForwardIterer ret = first;
    while (++first != last)
        if (!(*ret == *first) && ++ret != first)
            *ret = ala::move(*first);
    return ++ret;
}

template<class ForwardIterer, class BinPred>
constexpr ForwardIterer unique(ForwardIterer first, ForwardIterer last,
                               BinPred pred) {
    if (first == last)
        return last;
    ForwardIterer ret = first;
    while (++first != last)
        if (!pred(*ret, *first) && ++ret != first)
            *ret = ala::move(*first);
    return ++ret;
}

// TODO: make this work with input iterator
template<class ForwardIterer, class OutIter>
constexpr OutIter unique_copy(ForwardIterer first, ForwardIterer last,
                              OutIter out) {
    if (first == last)
        return out;
    ForwardIterer ret = first;
    while (++first != last)
        if (!(*ret == *first) || ++ret != first)
            *out++ = *ret++;
    return out;
}

template<class ForwardIterer, class OutIter, class BinPred>
constexpr OutIter unique_copy(ForwardIterer first, ForwardIterer last,
                              OutIter out, BinPred pred) {
    if (first == last)
        return out;
    ForwardIterer ret = first;
    while (++first != last)
        if (!pred(*ret, *first) || ++ret != first)
            *out++ = *ret++;
    return out;
}

template<class BidirIter>
void reverse(BidirIter first, BidirIter last) {
    while ((first != last) && (first != --last))
        iter_swap(first++, last);
}

template<class BidirIter, class OutIter>
constexpr OutIter reverse_copy(BidirIter first, BidirIter last, OutIter out) {
    while (first != --last)
        *out = *last;
}

template<class ForwardIterer>
constexpr ForwardIterer rotate(ForwardIterer first, ForwardIterer middle,
                               ForwardIterer last) {
    if (first == middle)
        return last;
    if (middle == last)
        return first;
    ForwardIterer next_mid = middle;
    while (middle != last) {
        if (first == next_mid)
            next_mid = middle;
        ala::iter_swap(first++, middle++);
    }
    rotate(first, next_mid, last);
    return first;
}

template<class ForwardIterer, class OutIter>
constexpr OutIter rotate_copy(ForwardIterer first, ForwardIterer middle,
                              ForwardIterer last, OutIter out) {
    return ala::copy(first, middle, ala::copy(middle, last, out));
}

// TODO: uniform_distributtion
// template<class RandomIter, class URBG>
// constexpr void shuffle(RandomIter first, RandomIter last, URBG &&g) {
//     typedef typename iterator_traits<RandomIter>::difference_type diff_t;
//     typedef uniform_int_distribution<diff_t> uid_t;
//     typedef typename uid_t::param_type param_t;
//     diff_t len = last - first;
//     if (len < 2)
//         return;
//     uid_t uid;
//     for (--last, --len; first < last; ++first, --d) {
//         diff_t offset = uid(g, param_t(0, len));
//         if (offset != difference_type(0))
//             ala::iter_swap(first, first + offset);
//     }
// }

// // TODO: make this work with input iterator
// template<class PopulationIterator, class SampleIterator, class Distance, class URBG>
// constexpr SampleIterator sample(PopulationIterator first, PopulationIterator last,
//                                 SampleIterator out, Distance n, URBG &&g) {
//     typedef typename iterator_traits<RandomIter>::difference_type diff_t;
//     typedef uniform_int_distribution<diff_t> uid_t;
//     typedef typename uid_t::param_type param_t;

//     Distance len = ala::distance(first, last);
//     uid_t uid;
//     for (n = ala::min(n, len); n != 0; ++first) {
//         if (uid(g, param_t(0, --len)) < n) {
//             *out++ = *first;
//             --n;
//         }
//     }
//     return out;
// }

// Partitioning operations
template<class Iter, class UnaryPred>
constexpr bool is_partitioned(Iter first, Iter last, UnaryPred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            break;
    for (; first != last; ++first)
        if (pred(*first))
            return false;
    return true;
}

template<class ForwardIterer, class UnaryPred>
constexpr ForwardIterer partition(ForwardIterer first, ForwardIterer last,
                                  UnaryPred pred) {
    first = ala::find_if_not(first, last, pred);
    if (first == last)
        return first;
    ForwardIterer i = first;
    for (++i; i != last; ++i)
        if (pred(*i))
            ala::iter_swap(i, first++);
    return first;
}

template<class BidirIter, class UnaryPred>
constexpr BidirIter stable_partition(BidirIter first, BidirIter last,
                                     UnaryPred pred) {
    typedef typename iterator_traits<BidirIter>::value_type T;
    auto len = ala::distance(first, last);
    T *tmp = new T[len];
    auto out = tmp;

    first = ala::find_if_not(first, last, pred);
    if (first == last)
        return first;

    BidirIter i = first;
    *out++ = ala::move(*first);
    for (++i; i != last; ++i)
        if (pred(*i))
            *first++ = ala::move(*i);
        else
            *out++ = ala::move(*i);

    ala::copy(tmp, out, first);
    delete[] tmp;
    return first;
}

template<class Iter, class OutIter1, class OutIter2, class UnaryPred>
constexpr pair<OutIter1, OutIter2>
partition_copy(Iter first, Iter last, OutIter1 out_true, OutIter2 out_false,
               UnaryPred pred) {
    while (first != last) {
        if (pred(*first++))
            *out_true++ = *first;
        else
            *out_false++ = *first;
    }
    return ala::pair<OutIter1, OutIter2>(out_true, out_false);
}

template<class ForwardIterer, class UnaryPred>
constexpr ForwardIterer partition_point(ForwardIterer first, ForwardIterer last,
                                        UnaryPred pred) {
    auto len = ala::distance(first, last);
    if (len < 1)
        return last;
    while (len != 0) {
        ForwardIterer i = first;
        ala::advance(i, len / 2);
        if (pred(*i))
            first = ++i;
        len = len / 2;
    }
    return first;
}

// Sorting operations (see ala/detail/sort.h)
template<class ForwardIterer, class Comp>
constexpr ForwardIterer is_sorted_until(ForwardIterer first, ForwardIterer last,
                                        Comp comp) {
    if (first != last) {
        ForwardIterer next = first;
        while (++next != last) {
            if (comp(*next, *first))
                return next;
            first = next;
        }
    }
    return last;
}

template<class ForwardIterer>
constexpr ForwardIterer is_sorted_until(ForwardIterer first, ForwardIterer last) {
    return ala::is_sorted_until(first, last, less<>());
}

template<class ForwardIterer, class Comp>
constexpr bool is_sorted(ForwardIterer first, ForwardIterer last, Comp comp) {
    return ala::is_sorted_until(first, last, comp) == last;
}

template<class ForwardIterer>
constexpr bool is_sorted(ForwardIterer first, ForwardIterer last) {
    return ala::is_sorted(first, last, less<>());
}

// Set operations (on sorted ranges)
template<class Iter1, class Iter2, class Comp>
constexpr bool includes(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                        Comp comp) {
    for (; first2 != last2; ++first1) {
        if (first1 == last1 || comp(*first2, *first1))
            return false;
        if (!comp(*first1, *first2))
            ++first2;
    }
    return true;
}

template<class Iter1, class Iter2>
constexpr bool includes(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2) {
    return ala::includes(first1, last1, first2, last2, less<>());
}

template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_union(Iter1 first1, Iter1 last1, Iter2 first2,
                            Iter2 last2, OutIter out, Comp comp) {
    for (; first1 != last1; ++out) {
        if (first2 == last2)
            return ala::copy(first1, last1, out);
        if (comp(*first2, *first1)) {
            *out = *first2++;
        } else {
            *out = *first1;
            if (!comp(*first1, *first2))
                ++first2;
            ++first1;
        }
    }
    return ala::copy(first2, last2, out);
}

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_union(Iter1 first1, Iter1 last1, Iter2 first2,
                            Iter2 last2, OutIter out) {
    return ala::set_union(first1, last1, first2, last2, out, less<>());
}

template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_intersection(Iter1 first1, Iter1 last1, Iter2 first2,
                                   Iter2 last2, OutIter out, Comp comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } else {
            if (!comp(*first2, *first1)) {
                *out++ = *first1++;
            }
            ++first2;
        }
    }
    return out;
}

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_intersection(Iter1 first1, Iter1 last1, Iter2 first2,
                                   Iter2 last2, OutIter out) {
    return ala::set_intersection(first1, last1, first2, last2, out, less<>());
}

template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_difference(Iter1 first1, Iter1 last1, Iter2 first2,
                                 Iter2 last2, OutIter out, Comp comp) {
    while (first1 != last1) {
        if (first2 == last2)
            return ala::copy(first1, last1, out);
        if (comp(*first1, *first2)) {
            *out++ = *first1++;
        } else {
            if (!comp(*first2, *first1)) {
                ++first1;
            }
            ++first2;
        }
    }
    return out;
}

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_difference(Iter1 first1, Iter1 last1, Iter2 first2,
                                 Iter2 last2, OutIter out) {
    return ala::set_difference(first1, last1, first2, last2, out, less<>());
}

template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_symmetric_difference(Iter1 first1, Iter1 last1,
                                           Iter2 first2, Iter2 last2,
                                           OutIter out, Comp comp) {
    while (first1 != last1) {
        if (first2 == last2)
            return ala::copy(first1, last1, out);
        if (comp(*first1, *first2)) {
            *out++ = *first1++;
        } else {
            if (comp(*first2, *first1)) {
                *out++ = *first2;
            } else {
                ++first1;
            }
            ++first2;
        }
    }
    return ala::copy(first2, last2, out);
}

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_symmetric_difference(Iter1 first1, Iter1 last1,
                                           Iter2 first2, Iter2 last2,
                                           OutIter out) {
    return ala::set_difference(first1, last1, first2, last2, out, less<>());
}

// Minimum/maximum operations
#ifndef min
template<class T>
constexpr const T &min(const T &a, const T &b) {
    return min(a, b, less<>());
}

template<class T, class Comp>
constexpr const T &min(const T &a, const T &b, Comp comp) {
    return comp(b, a) ? b : a;
}
#endif

#ifndef max
template<class T>
constexpr const T &max(const T &a, const T &b) {
    return max(a, b, less<>());
}

template<class T, class Comp>
constexpr const T &max(const T &a, const T &b, Comp comp) {
    return comp(a, b) ? b : a;
}
#endif

template<class T, class Comp>
constexpr pair<const T &, const T &> minmax(const T &a, const T &b, Comp comp) {
    return pair<const T &, const T &>(min(a, b, comp), max(a, b, comp));
}

template<class T>
constexpr pair<const T &, const T &> minmax(const T &a, const T &b) {
    return ala::minmax(a, b, less<>());
}

template<class ForwardIterer, class Comp>
constexpr ForwardIterer min_element(ForwardIterer first, ForwardIterer last,
                                    Comp comp) {
    if (first == last)
        return last;
    ForwardIterer min = first;
    for (++first; first != last; ++first) {
        if (comp(*first, *min))
            min = first;
    }
    return min;
}

template<class ForwardIterer>
constexpr ForwardIterer min_element(ForwardIterer first, ForwardIterer last) {
    return ala::min_element(first, last, less<>());
}

template<class ForwardIterer, class Comp>
constexpr ForwardIterer max_element(ForwardIterer first, ForwardIterer last,
                                    Comp comp) {
    if (first == last)
        return last;
    ForwardIterer max = first;
    for (++first; first != last; ++first) {
        if (comp(*max, *first))
            max = first;
    }
    return max;
}

template<class ForwardIterer>
constexpr ForwardIterer max_element(ForwardIterer first, ForwardIterer last) {
    return ala::max_element(first, last, less<>());
}

template<class ForwardIterer, class Comp>
constexpr pair<ForwardIterer, ForwardIterer>
minmax_element(ForwardIterer first, ForwardIterer last, Comp comp) {
    if (first == last)
        return last;
    ForwardIterer min = first;
    ForwardIterer max = first;
    for (++first; first != last; ++first) {
        if (comp(*first, *min))
            min = first;
        else if (comp(*max, *first))
            max = first;
    }
    return ala::make_pair(min, max);
}

template<class ForwardIterer>
constexpr pair<ForwardIterer, ForwardIterer> minmax_element(ForwardIterer first,
                                                            ForwardIterer last) {
    return ala::minmax_element(first, last, less<>());
}

#ifndef min
template<class T, class Comp>
constexpr T min(initializer_list<T> t, Comp comp) {
    return ala::min_element(t.begin(), t.end(), comp);
}
template<class T>
constexpr T min(initializer_list<T> t) {
    return ala::min(t, less<>());
}
#endif

#ifndef max
template<class T, class Comp>
constexpr T max(initializer_list<T> t, Comp comp) {
    return ala::max_element(t.begin(), t.end(), comp);
}
template<class T>
constexpr T max(initializer_list<T> t) {
    return ala::max(t, less<>());
}
#endif

template<class T, class Comp>
constexpr pair<T, T> minmax(initializer_list<T> t, Comp comp) {
    return ala::minmax_element(t.begin(), t.end(), comp);
}
template<class T>
constexpr pair<T, T> minmax(initializer_list<T> t) {
    return minmax(t, less<>());
}

template<class T, class Compare>
constexpr const T &clamp(const T &v, const T &lo, const T &hi, Compare comp) {
    return assert(!comp(hi, lo)), comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
    return clamp(v, lo, hi, less<>());
}

// Permutation operations
template<class BidirIter, class Comp>
constexpr bool prev_permutation(BidirIter first, BidirIter last, Comp comp) {
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

template<class BidirIter>
constexpr bool prev_permutation(BidirIter first, BidirIter last) {
    return ala::prev_permutation(first, last, less<>());
}

template<class BidirIter, class Comp>
constexpr bool next_permutation(BidirIter first, BidirIter last, Comp comp) {
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

template<class BidirIter>
constexpr bool next_permutation(BidirIter first, BidirIter last) {
    return ala::next_permutation(first, last, less<>());
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, BinPred pred) {
    typedef typename iterator_traits<ForwardIter1>::value_type T;
    if (first1 != last1) {
        ForwardIter2 last2 = first2;
        ala::advance(last2, ala::distance(first1, last1));
        for (ForwardIter1 i = first1; i != last1; ++i) {
            auto unary = [&](const T &x) { return pred(x, *i); };
            if (i != ala::find_if(first1, i, unary))
                continue;
            if (ala::count_if(i, last1, unary) !=
                ala::count_if(first2, last2, unary))
                return false;
        }
    }
    return true;
}

template<class ForwardIter1, class ForwardIter2>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2) {
    return ala::is_permutation(first1, last1, first2, equal<>());
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2,
                              BinPred pred) {
    if (ala::distance(first1, last1) == ala::distance(first2, last2))
        return is_permutation(first1, last1, first2, pred);
    return false;
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2) {
    return ala::is_permutation(first1, last1, first2, last2, equal<>());
}

} // namespace ala

#include <ala/detail/sort.h>

#endif // HEAD