#ifndef _ALA_ALGORITHM_H
#define _ALA_ALGORITHM_H

#include <ala/utility.h>
#include <ala/iterator.h>
#include <ala/random.h>
#include <ala/detail/algorithm_base.h>

namespace ala {

template<class Iter, class T>
constexpr Iter find(Iter first, Iter last, const T &value) {
    for (; first != last; ++first)
        if (*first == value)
            return first;
    return last;
}
template<class Iter, class Pred>
constexpr Iter find_if(Iter first, Iter last, Pred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            return first;
    return last;
}
template<class Iter, class Pred>
constexpr Iter find_if_not(Iter first, Iter last, Pred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            return first;
    return last;
}

template<class Iter, class Pred>
constexpr bool all_of(Iter first, Iter last, Pred pred) {
    return ala::find_if_not(first, last, pred) == last;
}

template<class Iter, class Pred>
constexpr bool any_of(Iter first, Iter last, Pred pred) {
    return ala::find_if(first, last, pred) != last;
}

template<class Iter, class Pred>
constexpr bool none_of(Iter first, Iter last, Pred pred) {
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

template<class ForwardIter1, class ForwardIter2, class BinPred>
ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2, BinPred pred) {
    for (;; ++first1) {
        ForwardIter1 it1 = first1;
        for (ForwardIter2 it2 = first2;; ++it1, ++it2) {
            if (it2 == last2)
                return first1;
            if (it1 == last1)
                return last1;
            if (!pred(*it1, *it2))
                break;
        }
    }
}

template<class ForwardIter1, class ForwardIter2>
ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2) {
    return search(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                                ForwardIter2 first2, ForwardIter2 last2,
                                BinPred pred) {
    if (first2 == last2)
        return last1;
    ForwardIter1 ret = last1;
    while (true) {
        ForwardIter1 ret1 = ala::search(first1, last1, first2, last2, pred);
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

template<class ForwardIter1, class ForwardIter2>
constexpr ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                                ForwardIter2 first2, ForwardIter2 last2) {
    return find_end(first1, last2, first2, last2, equal_to<>());
}

template<class Iter, class ForwardIter, class BinPred>
constexpr Iter find_first_of(Iter first1, Iter last1, ForwardIter first2,
                             ForwardIter last2, BinPred pred) {
    for (; first1 != last1; ++first1)
        for (ForwardIter it = first2; it != last2; ++it)
            if (pred(*first1, *it))
                return first1;
    return last1;
}

template<class Iter, class ForwardIter>
constexpr Iter find_first_of(Iter first1, Iter last1, ForwardIter first2,
                             ForwardIter last2) {
    return find_first_of(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIter, class BinPred>
constexpr ForwardIter adjacent_find(ForwardIter first, ForwardIter last,
                                    BinPred pred) {
    if (first == last)
        return last;
    ForwardIter next = first;
    for (++next; next != last; ++next, ++first)
        if (pred(*first, *next))
            return first;
    return last;
}

template<class ForwardIter>
constexpr ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
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

template<class Iter, class Pred>
constexpr typename iterator_traits<Iter>::difference_type
count_if(Iter first, Iter last, Pred pred) {
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

template<class ForwardIter, class Size, class T, class BinPred>
constexpr ForwardIter search_n(ForwardIter first, ForwardIter last, Size count,
                               const T &value, BinPred pred) {
    if (count <= 0)
        return first;
    for (; first != last; ++first) {
        if (!pred(*first, value)) {
            continue;
        }
        ForwardIter candidate = first;
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

template<class ForwardIter, class Size, class T>
constexpr ForwardIter search_n(ForwardIter first, ForwardIter last, Size count,
                               const T &value) {
    return search_n(first, last, count, value, equal_to<>());
}

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

template<class Iter, class OutIter, class Pred>
constexpr OutIter copy_if(Iter first, Iter last, OutIter out, Pred pred) {
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

template<class ForwardIter1, class ForwardIter2>
constexpr ForwardIter2 swap_ranges(ForwardIter1 first1, ForwardIter1 last1,
                                   ForwardIter2 first2) {
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

template<class ForwardIter, class T>
constexpr void replace(ForwardIter first, ForwardIter last, const T &old_value,
                       const T &new_value) {
    for (; first != last; ++first)
        if (*first == old_value)
            *first = new_value;
}

template<class ForwardIter, class Pred, class T>
constexpr void replace_if(ForwardIter first, ForwardIter last, Pred pred,
                          const T &new_value) {
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

template<class Iter, class OutIter, class Pred, class T>
constexpr OutIter replace_copy_if(Iter first, Iter last, OutIter out, Pred pred,
                                  const T &new_value) {
    for (; first != last; ++first)
        *out++ = pred(*first) ? new_value : *first;
    return out;
}

template<class ForwardIter, class T>
constexpr void fill(ForwardIter first, ForwardIter last, const T &value) {
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

template<class ForwardIter, class Generator>
constexpr void generate(ForwardIter first, ForwardIter last, Generator gen) {
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

template<class ForwardIter, class T>
constexpr ForwardIter remove(ForwardIter first, ForwardIter last, const T &value) {
    if (first != last)
        for (ForwardIter i = first; ++i != last;)
            if (*i != value)
                *first++ = ala::move(*i);
    return first;
}

template<class ForwardIter, class Pred>
constexpr ForwardIter remove_if(ForwardIter first, ForwardIter last, Pred pred) {
    if (first != last)
        for (ForwardIter i = first; ++i != last;)
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

template<class Iter, class OutIter, class Pred>
constexpr OutIter remove_copy_if(Iter first, Iter last, OutIter out, Pred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            *out++ = *first;
    return out;
}

template<class ForwardIter>
constexpr ForwardIter unique(ForwardIter first, ForwardIter last) {
    if (first == last)
        return last;
    ForwardIter ret = first;
    while (++first != last)
        if (!(*ret == *first) && ++ret != first)
            *ret = ala::move(*first);
    return ++ret;
}

template<class ForwardIter, class BinPred>
constexpr ForwardIter unique(ForwardIter first, ForwardIter last, BinPred pred) {
    if (first == last)
        return last;
    ForwardIter ret = first;
    while (++first != last)
        if (!pred(*ret, *first) && ++ret != first)
            *ret = ala::move(*first);
    return ++ret;
}

// TODO: make this work with input iterator
template<class ForwardIter, class OutIter>
constexpr OutIter unique_copy(ForwardIter first, ForwardIter last, OutIter out) {
    if (first == last)
        return out;
    ForwardIter ret = first;
    while (++first != last)
        if (!(*ret == *first) || ++ret != first)
            *out++ = *ret++;
    return out;
}

template<class ForwardIter, class OutIter, class BinPred>
constexpr OutIter unique_copy(ForwardIter first, ForwardIter last, OutIter out,
                              BinPred pred) {
    if (first == last)
        return out;
    ForwardIter ret = first;
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

template<class ForwardIter>
constexpr ForwardIter rotate(ForwardIter first, ForwardIter middle,
                             ForwardIter last) {
    if (first == middle)
        return last;
    if (middle == last)
        return first;
    ForwardIter next_mid = middle;
    while (middle != last) {
        if (first == next_mid)
            next_mid = middle;
        ala::iter_swap(first++, middle++);
    }
    rotate(first, next_mid, last);
    return first;
}

template<class ForwardIter, class OutIter>
constexpr OutIter rotate_copy(ForwardIter first, ForwardIter middle,
                              ForwardIter last, OutIter out) {
    return ala::copy(first, middle, ala::copy(middle, last, out));
}

// TODO: uniform_distributtion
// template<class RandomIter, class URBG>
// constexpr void shuffle(RandomIter first, RandomIter last, URBG &&g) {
//     typedef typename iterator_traits<RandomIter>::difference_type diff_t;
//     typedef uniform_int_distribution<diff_t> uid_t;
//     typedef typename uid_t::param_type param_t;
//     diff_t n = last - first;
//     if (n < 2)
//         return;
//     uid_t uid;
//     for (--last, --n; first < last; ++first, --d) {
//         diff_t offset = uid(g, param_t(0, n));
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

//     Distance dis = ala::distance(first, last);
//     uid_t uid;
//     for (n = ala::min(n, dis); n != 0; ++first) {
//         if (uid(g, param_t(0, --dis)) < n) {
//             *out++ = *first;
//             --n;
//         }
//     }
//     return out;
// }

template<class Iter, class Pred>
constexpr bool is_partitioned(Iter first, Iter last, Pred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            break;
    for (; first != last; ++first)
        if (pred(*first))
            return false;
    return true;
}

template<class ForwardIter, class Pred>
constexpr ForwardIter partition(ForwardIter first, ForwardIter last, Pred pred) {
    first = ala::find_if_not(first, last, pred);
    if (first == last)
        return first;
    ForwardIter i = first;
    for (++i; i != last; ++i) {
        if (pred(*i)) {
            ala::iter_swap(i, first);
            ++first;
        }
    }
    return first;
}

template<class BidirIter, class Pred>
constexpr BidirIter stable_partition(BidirIter first, BidirIter last, Pred pred);

template<class Iter, class OutIter1, class OutIter2, class Pred>
constexpr pair<OutIter1, OutIter2> partition_copy(Iter first, Iter last,
                                                  OutIter1 out_true,
                                                  OutIter2 out_false, Pred pred) {
    while (first != last) {
        if (pred(*first++))
            *out_true++ = *first;
        else
            *out_false++ = *first;
    }
    return ala::pair<OutIter1, OutIter2>(out_true, out_false);
}

template<class ForwardIter, class Pred>
constexpr ForwardIter partition_point(ForwardIter first, ForwardIter last,
                                      Pred pred) {
    auto n = ala::distance(first, last);
    if (n < 1)
        return last;
    while (n != 0) {
        ForwardIter i = first;
        ala::advance(i, n / 2);
        if (pred(*i))
            first = ++i;
        n = n / 2;
    }
    return first;
}

template<class RandomIter>
constexpr void sort(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr void sort(RandomIter first, RandomIter last, Comp comp);

template<class RandomIter>
constexpr void stable_sort(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr void stable_sort(RandomIter first, RandomIter last, Comp comp);

template<class RandomIter>
constexpr void partial_sort(RandomIter first, RandomIter middle, RandomIter last);
template<class RandomIter, class Comp>
constexpr void partial_sort(RandomIter first, RandomIter middle,
                            RandomIter last, Comp comp);
template<class Iter, class RandomIter>
constexpr RandomIter partial_sort_copy(Iter first, Iter last,
                                       RandomIter result_first,
                                       RandomIter result_last);
template<class Iter, class RandomIter, class Comp>
constexpr RandomIter partial_sort_copy(Iter first, Iter last,
                                       RandomIter result_first,
                                       RandomIter result_last, Comp comp);

template<class ForwardIter>
constexpr bool is_sorted(ForwardIter first, ForwardIter last);
template<class ForwardIter, class Comp>
constexpr bool is_sorted(ForwardIter first, ForwardIter last, Comp comp);
template<class ForwardIter>
constexpr ForwardIter is_sorted_until(ForwardIter first, ForwardIter last);
template<class ForwardIter, class Comp>
constexpr ForwardIter is_sorted_until(ForwardIter first, ForwardIter last,
                                      Comp comp);

template<class RandomIter>
constexpr void nth_element(RandomIter first, RandomIter nth, RandomIter last);
template<class RandomIter, class Comp>
constexpr void nth_element(RandomIter first, RandomIter nth, RandomIter last,
                           Comp comp);
// 二分搜索：
template<class ForwardIter, class T>
constexpr ForwardIter lower_bound(ForwardIter first, ForwardIter last,
                                  const T &value);
template<class ForwardIter, class T, class Comp>
constexpr ForwardIter lower_bound(ForwardIter first, ForwardIter last,
                                  const T &value, Comp comp);

template<class ForwardIter, class T>
constexpr ForwardIter upper_bound(ForwardIter first, ForwardIter last,
                                  const T &value);
template<class ForwardIter, class T, class Comp>
constexpr ForwardIter upper_bound(ForwardIter first, ForwardIter last,
                                  const T &value, Comp comp);

template<class ForwardIter, class T>
constexpr pair<ForwardIter, ForwardIter>
equal_range(ForwardIter first, ForwardIter last, const T &value);
template<class ForwardIter, class T, class Comp>
constexpr pair<ForwardIter, ForwardIter>
equal_range(ForwardIter first, ForwardIter last, const T &value, Comp comp);

template<class ForwardIter, class T>
constexpr bool binary_search(ForwardIter first, ForwardIter last, const T &value);
template<class ForwardIter, class T, class Comp>
constexpr bool binary_search(ForwardIter first, ForwardIter last,
                             const T &value, Comp comp);

// 归并：
template<class Iter1, class Iter2, class OutIter>
constexpr OutIter merge(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                        OutIter out);
template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter merge(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                        OutIter out, Comp comp);

template<class BidirIter>
constexpr void inplace_merge(BidirIter first, BidirIter middle, BidirIter last);
template<class BidirIter, class Comp>
constexpr void inplace_merge(BidirIter first, BidirIter middle, BidirIter last,
                             Comp comp);

// 集合操作：
template<class Iter1, class Iter2>
constexpr bool includes(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2);
template<class Iter1, class Iter2, class Comp>
constexpr bool includes(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                        Comp comp);

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_union(Iter1 first1, Iter1 last1, Iter2 first2,
                            Iter2 last2, OutIter out);
template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_union(Iter1 first1, Iter1 last1, Iter2 first2,
                            Iter2 last2, OutIter out, Comp comp);

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_intersection(Iter1 first1, Iter1 last1, Iter2 first2,
                                   Iter2 last2, OutIter out);
template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_intersection(Iter1 first1, Iter1 last1, Iter2 first2,
                                   Iter2 last2, OutIter out, Comp comp);

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_difference(Iter1 first1, Iter1 last1, Iter2 first2,
                                 Iter2 last2, OutIter out);
template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_difference(Iter1 first1, Iter1 last1, Iter2 first2,
                                 Iter2 last2, OutIter out, Comp comp);

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter set_symmetric_difference(Iter1 first1, Iter1 last1,
                                           Iter2 first2, Iter2 last2,
                                           OutIter out);
template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter set_symmetric_difference(Iter1 first1, Iter1 last1,
                                           Iter2 first2, Iter2 last2,
                                           OutIter out, Comp comp);

// 堆操作：
template<class RandomIter>
constexpr void push_heap(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr void push_heap(RandomIter first, RandomIter last, Comp comp);
template<class RandomIter>
constexpr void pop_heap(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr void pop_heap(RandomIter first, RandomIter last, Comp comp);

template<class RandomIter>
constexpr void make_heap(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr void make_heap(RandomIter first, RandomIter last, Comp comp);

template<class RandomIter>
constexpr void sort_heap(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr void sort_heap(RandomIter first, RandomIter last, Comp comp);

template<class RandomIter>
constexpr bool is_heap(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr bool is_heap(RandomIter first, RandomIter last, Comp comp);
template<class RandomIter>
constexpr RandomIter is_heap_until(RandomIter first, RandomIter last);
template<class RandomIter, class Comp>
constexpr RandomIter is_heap_until(RandomIter first, RandomIter last, Comp comp);

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

template<class T>
constexpr T min(initializer_list<T> t);

template<class T, class Comp>
constexpr T min(initializer_list<T> t, Comp comp);

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

template<class T>
constexpr T max(initializer_list<T> t);
template<class T, class Comp>
constexpr T max(initializer_list<T> t, Comp comp);

template<class T>
constexpr pair<const T &, const T &> minmax(const T &a, const T &b);
template<class T, class Comp>
constexpr pair<const T &, const T &> minmax(const T &a, const T &b, Comp comp);
template<class T>
constexpr pair<T, T> minmax(initializer_list<T> t);
template<class T, class Comp>
constexpr pair<T, T> minmax(initializer_list<T> t, Comp comp);

template<class ForwardIter>
constexpr ForwardIter min_element(ForwardIter first, ForwardIter last);
template<class ForwardIter, class Comp>
constexpr ForwardIter min_element(ForwardIter first, ForwardIter last, Comp comp);

template<class ForwardIter>
constexpr ForwardIter max_element(ForwardIter first, ForwardIter last);
template<class ForwardIter, class Comp>
constexpr ForwardIter max_element(ForwardIter first, ForwardIter last, Comp comp);

template<class ForwardIter>
constexpr pair<ForwardIter, ForwardIter> minmax_element(ForwardIter first,
                                                        ForwardIter last);
template<class ForwardIter, class Comp>
constexpr pair<ForwardIter, ForwardIter>
minmax_element(ForwardIter first, ForwardIter last, Comp comp);

template<class Iter1, class Iter2>
constexpr bool lexicographical_compare(Iter1 first1, Iter1 last1, Iter2 first2,
                                       Iter2 last2);
template<class Iter1, class Iter2, class Comp>
constexpr bool lexicographical_compare(Iter1 first1, Iter1 last1, Iter2 first2,
                                       Iter2 last2, Comp comp);

template<typename RandomIter>
constexpr void shuffle(RandomIter first, RandomIter last) {
    typename iterator_traits<RandomIter>::difference_type dis = last - first;
    xoshiro128p gen;
    for (; first < last; ++first)
        swap(*first, *(first + gen() % dis));
}

template<class ForwardIter1, class ForwardIter2, class OutIter>
constexpr OutIter merge(ForwardIter1 first1, ForwardIter1 last1,
                            ForwardIter2 first2, ForwardIter2 last2, OutIter out) {
    while (first1 != last1 && first2 != last1)
        *out++ = (*first2 < *first1) ? *first2++ : *first1++;
    while (first2 != last1)
        *out++ = *first2++;
    while (first1 != last1)
        *out++ = *first1++;
    return out;
}

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

} // namespace ala

#include <ala/detail/sort.h>

#endif // HEAD