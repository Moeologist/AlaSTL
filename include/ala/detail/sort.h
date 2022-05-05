// Sort Operation

#ifndef _ALA_DETAIL_SORT_H
#define _ALA_DETAIL_SORT_H

#include <ala/detail/algorithm_base.h>
#include <ala/detail/allocator.h>
#include <ala/detail/uninitialized_memory.h>

namespace ala {

template<class ForwardIter, class Comp>
void select_sort(ForwardIter first, ForwardIter last, Comp comp) {
    if (first == last)
        return;
    ForwardIter i = first, next = first;
    for (++next; next != last; ++i, ++next) {
        ForwardIter min = i;
        for (ForwardIter j = i; j != last; ++j)
            if (comp(*j, *min))
                min = j;
        ala::iter_swap(i, min);
    }
}

template<class ForwardIter>
void select_sort(ForwardIter first, ForwardIter last) {
    return ala::select_sort(first, last, less<>());
}

template<class BidirIter, class Comp>
void insertion_sort(BidirIter first, BidirIter last, Comp comp) {
    typedef typename iterator_traits<BidirIter>::value_type T;
    if (first == last)
        return;
    BidirIter sorted = first;
    BidirIter cur, next;
    for (++sorted; sorted != last; ++sorted) {
        cur = next = sorted;
        T tmp(ala::move(*sorted));
        for (--cur; next != first && comp(tmp, *cur); --cur, (void)--next)
            *next = ala::move(*cur);
        *next = ala::move(tmp);
    }
}

template<class BidirIter>
void insertion_sort(BidirIter first, BidirIter last) {
    return ala::insertion_sort(first, last, less<>());
}

// void _shell_sequence() {
//     uint_fast64_t seq[64];
//     for (int i = 0; i < 32; ++i) {
//         seq[i * 2] = 9 * pow(4, i) - 9 * pow(2, i) + 1;
//         seq[i * 2 + 1] = pow(2, i + 2) * (pow(2, i + 2) - 3) + 1;
//     }
//     for (int i = 0; i < 64; ++i)
//         if (seq[i] > 0)
//             printf("0x%llxull,", seq[i]);
// }

template<class RandomIter, class Comp>
void shell_sort(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    constexpr uint_fast64_t seq[] = {0x1ull,
                                     0x5ull,
                                     0x13ull,
                                     0x29ull,
                                     0x6dull,
                                     0xd1ull,
                                     0x1f9ull,
                                     0x3a1ull,
                                     0x871ull,
                                     0xf41ull,
                                     0x22e1ull,
                                     0x3e81ull,
                                     0x8dc1ull,
                                     0xfd01ull,
                                     0x23b81ull,
                                     0x3fa01ull,
                                     0x8f701ull,
                                     0xff401ull,
                                     0x23ee01ull,
                                     0x3fe801ull,
                                     0x8fdc01ull,
                                     0xffd001ull,
                                     0x23fb801ull,
                                     0x3ffa001ull,
                                     0x8ff7001ull,
                                     0xfff4001ull,
                                     0x23fee001ull,
                                     0x3ffe8001ull,
                                     0x8ffdc001ull,
                                     0xfffd0001ull,
                                     0x23ffb8001ull,
                                     0x3fffa0001ull,
                                     0x8fff70001ull,
                                     0xffff40001ull,
                                     0x23ffee0001ull,
                                     0x3fffe80001ull,
                                     0x8fffdc0001ull,
                                     0xffffd00001ull,
                                     0x23fffb80001ull,
                                     0x3ffffa00001ull,
                                     0x8ffff700001ull,
                                     0xfffff400001ull,
                                     0x23fffee00001ull,
                                     0x3ffffe800001ull,
                                     0x8ffffdc00001ull,
                                     0xfffffd000001ull,
                                     0x23ffffb800001ull,
                                     0x3fffffa000001ull,
                                     0x8fffff7000001ull,
                                     0xffffff4000001ull,
                                     0x23ffffee000000ull,
                                     0x3fffffe8000000ull,
                                     0x8fffffdc000000ull,
                                     0xffffffd0000000ull,
                                     0x23fffffb8000000ull,
                                     0x3ffffffa0000000ull,
                                     0x8ffffff70000000ull,
                                     0xfffffff40000000ull,
                                     0x23fffffee0000000ull,
                                     0x3ffffffe80000000ull,
                                     0x8ffffffdc0000000ull,
                                     0xfffffffd00000000ull};
    int index = 0;
    while (last - first > seq[index])
        index++;
    RandomIter i, j;
    for (--index; index >= 0; --index) {
        const size_t gap = seq[index];
        for (i = first + gap; i < last; ++i) {
            const T tmp = *i;
            for (j = i - gap; j >= first && comp(tmp, *j); j = j - gap)
                *(j + gap) = *j;
            *(j + gap) = tmp;
        }
    }
}

template<class RandomIter>
void shell_sort(RandomIter first, RandomIter last) {
    return ala::shell_sort(first, last, less<>());
}

namespace detail {

template<class T, class Comp>
void set_median(T &a, T &b, T &c, Comp comp) {
    if (comp(a, b)) {
        if (comp(b, c))
            ala::_swap_adl(a, b);
        else if (comp(a, c))
            ala::_swap_adl(a, c);
    } else if (comp(c, b))
        ala::_swap_adl(a, b);
    else if (comp(c, a))
        ala::_swap_adl(a, c);
}

template<class RandomIter, class Comp>
RandomIter partition_c(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t len = last - first;
    assert(!(len < 3));
    ala::detail::set_median(*first, *(first + len / 2), *(last - 1), comp);
    RandomIter left = first + 1, right = last - 1;
    for (;; (void)++left, --right) {
        while (comp(*left, *first))
            ++left;
        while (comp(*first, *right))
            --right;
        if (!(left < right))
            break;
        ala::iter_swap(left, right);
    }
    return left;
}

template<class RandomIter, class Comp>
void quick_sort_impl(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t len = last - first;
    if (len <= ALA_INSERTION_THRESHOLD)
        return;
    RandomIter left = ala::detail::partition_c(first, last, comp);
    ala::detail::quick_sort_impl(first, left, comp);
    ala::detail::quick_sort_impl(left, last, comp);
}

// quick-sort with three-way partion
template<class RandomIter, class Comp>
void quick_sort_three_way_impl(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t len = last - first;
    if (len <= ALA_INSERTION_THRESHOLD)
        return;
    ala::detail::set_median(*first, *(first + len / 2), *(last - 1));
    RandomIter left = first, i = first + 1, right = last;
    // [first, left) < pivot, [left, right) == pivot, [right, end) > pivot
    while (i < right) {
        if (comp(*i, *left))
            ala::iter_swap(left++, i++);
        else if (comp(*left, *i))
            ala::iter_swap(i, --right);
        else
            ++i;
    }
    ala::detail::quick_sort_three_way_impl(first, left, comp);
    ala::detail::quick_sort_three_way_impl(right, last, comp);
}

template<class RandomIter, class Comp>
constexpr void sort_impl(RandomIter first, RandomIter last, unsigned depth,
                         Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t len = last - first;
    while ((len = last - first) > ALA_INSERTION_THRESHOLD) {
        RandomIter left = ala::detail::partition_c(first, last, comp);
        ala::detail::sort_impl(left, last, --depth, comp);
        last = left;
    }
    if (depth == 0)
        ala::insertion_sort(first, last, comp);
}

} // namespace detail

template<class RandomIter, class Comp>
void quick_sort(RandomIter first, RandomIter last, Comp comp) {
    if (first < last) {
#if ALA_ENABLE_THREE_WAY_QUICK_SORT
        detail::quick_sort_three_way_impl(first, last, comp);
#else
        ala::detail::quick_sort_impl(first, last, comp);
#endif
        ala::insertion_sort(first, last, comp);
    }
}

template<class RandomIter>
void quick_sort(RandomIter first, RandomIter last) {
    return ala::quick_sort(first, last, less<>());
}

// a quick-sort variety form eastl
template<class RandomIter, class Comp>
constexpr void sort(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    if (first < last) {
        unsigned i = 0;
        for (ptrdiff_t s = last - first; s > 0; ++i)
            s >>= 1;
        ala::detail::sort_impl(first, last, 2 * (i - 1), comp);
        ala::insertion_sort(first, last, comp);
    }
}

template<class RandomIter>
constexpr void sort(RandomIter first, RandomIter last) {
    ala::sort(first, last, less<>());
}

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter merge(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                           InputIter2 last2, OutputIter out, Comp comp) {
    while (first1 != last1 && first2 != last2)
        if (comp(*first2, *first1))
            *out++ = *first2++;
        else
            *out++ = *first1++;
    if (first1 != last1)
        return ala::copy(first1, last1, out);
    else
        return ala::copy(first2, last2, out);
}

template<class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter merge(InputIter1 first1, InputIter1 last1,
                           InputIter2 first2, InputIter2 last2, OutputIter out) {
    return ala::merge(first1, last1, first2, last2, out, less<>());
}

namespace detail {

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter merge_mv(InputIter1 first1, InputIter1 last1,
                              InputIter2 first2, InputIter2 last2,
                              OutputIter out, Comp comp) {
    while (first1 != last1 && first2 != last2)
        if (comp(*first2, *first1))
            *out++ = ala::move(*first2++);
        else
            *out++ = ala::move(*first1++);
    if (first1 != last1)
        return ala::move(first1, last1, out);
    else
        return ala::move(first2, last2, out);
}

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter merge_umv(InputIter1 first1, InputIter1 last1,
                               InputIter2 first2, InputIter2 last2,
                               OutputIter out, Comp comp) {
    while (first1 != last1 && first2 != last2)
        if (comp(*first2, *first1))
            ala::construct_at(out++, ala::move(*first2++));
        else
            ala::construct_at(out++, ala::move(*first1++));
    if (first1 != last1)
        return ala::uninitialized_move(first1, last1, out);
    else
        return ala::uninitialized_move(first2, last2, out);
}

template<class RandomIter1, class RandomIter2, class Size, class Comp>
bool merge_sort_impl(RandomIter1 first, RandomIter2 tmp, Size len, Comp comp) {
    if (len <= ALA_INSERTION_THRESHOLD) {
        ala::insertion_sort(first, first + len);
        return false;
    }
    Size llen = len / 2, rlen = len - len / 2;
    bool l_tmp = ala::detail::merge_sort_impl(first, tmp, llen, comp);
    bool r_tmp = ala::detail::merge_sort_impl(first + llen, tmp + llen, rlen,
                                              comp);
    bool tmp_in = l_tmp || r_tmp; // merge tmp into first or merge fisrt into tmp
    if (!tmp_in) {
        RandomIter1 in = first;
        RandomIter2 out = tmp;
        ala::detail::merge_mv(in, in + llen, in + llen, in + len, out, comp);
        return !tmp_in;
    } else if (!l_tmp) {
        ala::move(first, first + llen, tmp);
    } else if (!r_tmp) {
        ala::move(first + llen, first + len, tmp + llen);
    }
    RandomIter2 in = tmp;
    RandomIter1 out = first;
    ala::detail::merge_mv(in, in + llen, in + llen, in + len, out, comp);
    return !tmp_in;
}

} // namespace detail

template<class BidirIter, class Comp>
void inplace_merge(BidirIter first, BidirIter middle, BidirIter last, Comp comp) {
    typedef typename iterator_traits<BidirIter>::difference_type diff_t;
    typedef typename iterator_traits<BidirIter>::value_type T;
    diff_t len = ala::distance(first, last);
    allocator<T> alloc;
    pointer_holder<T *, allocator<T>> ph(alloc, len);
    T *tmp = ph.get();
    ala::detail::merge_umv(first, middle, middle, last, tmp, comp);
    ala::move(tmp, tmp + len, first);
    ala::destroy_n(tmp, len);
}

template<class BidirIter>
void inplace_merge(BidirIter first, BidirIter middle, BidirIter last) {
    return ala::inplace_merge(first, middle, last, less<>());
}

// top-down merge-sort, no-copyback
template<class RandomIter, class Comp>
void merge_sort(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t len = last - first;
    allocator<T> alloc;
    pointer_holder<T *, allocator<T>> ph(alloc, len);
    T *tmp = ph.get();
    ala::uninitialized_move(first, last, tmp);
    if (!ala::detail::merge_sort_impl(tmp, first, len, comp))
        ala::move(tmp, tmp + len, first);
    ala::destroy_n(tmp, len);
}

template<class RandomIter>
void merge_sort(RandomIter first, RandomIter last) {
    ala::merge_sort(first, last, less<>());
}

// bottom-up merge-sort, no-copyback
template<class RandomIter, class Comp>
constexpr void stable_sort(RandomIter first, RandomIter last, Comp comp) {
    return merge_sort(first, last, comp);
    // typedef typename iterator_traits<RandomIter>::value_type T;
    // typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    // diff_t len = last - first;
    // allocator<T> alloc;
    // pointer_holder<T *, allocator<T>> ph(alloc, len);
    // T *tmp = ph.get();
    // ala::uninitialized_move(first, last, tmp);
    // for (T* left = tmp; left < tmp+len; left += ALA_INSERTION_THRESHOLD) {
    //     RandomIter next = left + ALA_INSERTION_THRESHOLD;
    //     ala::insertion_sort(left, ala::min(next, tmp+len), comp);
    // }
    // bool flag = false;
    // for (diff_t size = ALA_INSERTION_THRESHOLD; size < len; size <<= 1) {
    //     diff_t size2 = size << 1;
    //     if (flag) {
    //         RandomIter left = first, right = last;
    //         T *out = tmp;
    //         for (; left < right; left += size2, (void)(out += size2))
    //             ala::detail::merge_mv(left, ala::min(left + size, right),
    //                                   ala::min(left + size, right),
    //                                   ala::min(left + size2, right), out, comp);
    //     } else {
    //         T *left = tmp, *right = tmp + len;
    //         RandomIter out = first;
    //         for (; left < right; left += size2, (void)(out += size2))
    //             ala::detail::merge_mv(left, ala::min(left + size, right),
    //                                   ala::min(left + size, right),
    //                                   ala::min(left + size2, right), out, comp);
    //     }
    //     flag = !flag;
    // }
    // if (!flag)
    //     for (T *i = tmp; first < last; ++first, ++i)
    //         *first = ala::move(*i);
    // ala::destroy_n(tmp, len);
}

template<class RandomIter>
constexpr void stable_sort(RandomIter first, RandomIter last) {
    ala::stable_sort(first, last, less<>());
}

namespace detail {

template<class RandomIter, class Distance, class Comp>
void heap_sink(RandomIter first, Distance begin, Distance end, Comp comp) {
    Distance parent = begin, child = (parent << 1) + 1;
    while (child < end) {
        if (child + 1 < end && comp(first[child], first[child + 1]))
            child = child + 1;
        if (!comp(first[parent], first[child]))
            return;
        ala::iter_swap(first + parent, first + child);
        parent = child;
        child = (child << 1) + 1;
    }
}

} // namespace detail

// Heap operations
template<class RandomIter, class Comp>
constexpr void push_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    if (first == last)
        return;
    diff_t child = last - 1 - first;
    diff_t parent = (child - 1) >> 1;
    while (0 < child && comp(first[parent], first[child])) {
        ala::iter_swap(first + parent, first + child);
        child = parent;
        parent = (parent - 1) >> 1;
    }
}

template<class RandomIter>
constexpr void push_heap(RandomIter first, RandomIter last) {
    return ala::push_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void pop_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    if (first == last)
        return;
    ala::iter_swap(first, last - 1);
    ala::detail::heap_sink(first, (diff_t)0, (diff_t)(last - 1 - first), comp);
}

template<class RandomIter>
constexpr void pop_heap(RandomIter first, RandomIter last) {
    return ala::pop_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void make_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    if (first == last)
        return;
    diff_t len = last - first;
    for (diff_t i = (len >> 1); i > 0; --i)
        ala::detail::heap_sink(first, i - 1, len, comp);
}

template<class RandomIter>
constexpr void make_heap(RandomIter first, RandomIter last) {
    ala::make_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void sort_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    if (first == last)
        return;
    diff_t len = last - first;
    for (diff_t i = len - 1; i > 0; --i) {
        ala::iter_swap(first, first + i);
        ala::detail::heap_sink(first, (diff_t)0, i, comp);
    }
}

template<class RandomIter>
constexpr void sort_heap(RandomIter first, RandomIter last) {
    ala::sort_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr bool is_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    if (first == last)
        return true;
    diff_t len = last - first;
    for (diff_t i = 0; i < (len >> 1); ++i) {
        diff_t child = (i << 1) + 1;
        if (comp(first[i], first[child]))
            return false;
        if (child + 1 < len && comp(first[i], first[child + 1]))
            return false;
    }
    return true;
}

template<class RandomIter>
constexpr bool is_heap(RandomIter first, RandomIter last) {
    return ala::is_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr RandomIter is_heap_until(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    if (first == last)
        return last;
    diff_t len = last - first;
    diff_t offset = 0;
    for (diff_t i = 0; i <= (len >> 1) - 1; ++i) {
        diff_t child = (i << 1) + 1;
        if (comp(first[i], first[child]))
            break;
        offset = child;
        if (child + 1 < len) {
            if (comp(first[i], first[child + 1]))
                break;
            offset = child + 1;
        }
    }
    return first + offset + 1;
}

template<class RandomIter>
constexpr RandomIter is_heap_until(RandomIter first, RandomIter last) {
    return ala::is_heap_until(first, last, less<>());
}

template<class RandomIter, class Comp>
void heap_sort(RandomIter first, RandomIter last, Comp comp) {
    ala::make_heap(first, last, comp);
    ala::sort_heap(first, last, comp);
}

template<class RandomIter>
void heap_sort(RandomIter first, RandomIter last) {
    ala::heap_sort(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void partial_sort(RandomIter first, RandomIter middle,
                            RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t len = last - first;
    if (middle - first < 1 || len < 2)
        return;
    if (!(middle < last))
        return ala::sort(first, last, comp);

    if (len < 3) {
        if (comp(*(first + 1), *first))
            ala::iter_swap(first + 1, first);
        return;
    }
    RandomIter left = ala::detail::partition_c(first, last, comp);
    if (left < middle) {
        ala::partial_sort(left, middle, last, comp);
        ala::sort(first, middle, comp);
    } else {
        ala::partial_sort(first, middle, left, comp);
    }
}

template<class RandomIter>
constexpr void partial_sort(RandomIter first, RandomIter middle, RandomIter last) {
    ala::partial_sort(first, middle, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void nth_element(RandomIter first, RandomIter nth, RandomIter last,
                           Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    auto len = last - first;
    while ((len = last - first) > 1) {
        if (len < 3) {
            if (comp(*(first + 1), *first))
                ala::iter_swap(first + 1, first);
            return;
        }
        RandomIter left = ala::detail::partition_c(first, last, comp);
        ala::iter_swap(left - 1, first);
        if (nth < left - 1)
            last = left - 1;
        else if (left - 1 < nth)
            first = left;
        else
            return;
    }
}

template<class RandomIter>
constexpr void nth_element(RandomIter first, RandomIter nth, RandomIter last) {
    ala::nth_element(first, nth, last, less<>());
}

template<class Iter, class RandomIter, class Comp>
constexpr RandomIter partial_sort_copy(Iter first1, Iter last1, RandomIter first2,
                                       RandomIter last2, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t n = 0;
    for (; first1 != last1 && n < last2 - first2; ++first1, (void)++n)
        first2[n] = *first1;
    if (n == 0)
        return first2;
    if (first1 != last1) {
        ala::make_heap(first2, last2, comp);
        for (; first1 != last1; ++first1)
            if (comp(*first1, *first2)) {
                *first2 = *first1;
                ala::detail::heap_sink(first2, (diff_t)0, (diff_t)n, comp);
            }
        ala::sort_heap(first2, last2, comp);
    } else {
        ala::sort(first2, first2 + n, comp);
    }
    return first2 + n;
}

template<class Iter, class RandomIter>
constexpr RandomIter partial_sort_copy(Iter first1, Iter last1,
                                       RandomIter first2, RandomIter last2) {
    return ala::partial_sort_copy(first1, last1, first2, last2, less<>());
}

} // namespace ala

#endif
