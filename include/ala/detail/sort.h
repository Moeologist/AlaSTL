// Sort Operation

#ifndef _ALA_DETAIL_SORT_H
#define _ALA_DETAIL_SORT_H

#include <ala/detail/algorithm_base.h>
#include <ala/detail/allocator.h>
#include <ala/detail/uninitialized_memory.h>

namespace ala {

template<class ForwardIter, class Comp>
constexpr void select_sort(ForwardIter first, ForwardIter last, Comp comp) {
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
constexpr void select_sort(ForwardIter first, ForwardIter last) {
    return ala::select_sort(first, last, less<>());
}

template<class BidirIter, class Comp>
constexpr void insertion_sort(BidirIter first, BidirIter last, Comp comp) {
    using T = typename iterator_traits<BidirIter>::value_type;
    if (first == last)
        return;
    BidirIter sorted = first;
    BidirIter cur, next;
    for (++sorted; sorted != last; ++sorted) {
        cur = next = sorted;
        if (next == first)
            break;
        T tmp(ala::move(*sorted));
        for (--cur; comp(tmp, *cur);) {
            *next = ala::move(*cur);
            --next;
            if (next == first)
                break;
            --cur;
        }
        *next = ala::move(tmp);
    }
}

template<class BidirIter>
constexpr void insertion_sort(BidirIter first, BidirIter last) {
    return ala::insertion_sort(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void shell_sort(RandomIter first, RandomIter last, Comp comp) {
    using T = typename iterator_traits<RandomIter>::value_type;
    uint_fast64_t seq[64];
    for (int i = 0; i < 32; ++i) {
        seq[i * 2] = 9 * pow(4, i) - 9 * pow(2, i) + 1;
        seq[i * 2 + 1] = pow(2, i + 2) * (pow(2, i + 2) - 3) + 1;
    }
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
constexpr void shell_sort(RandomIter first, RandomIter last) {
    return ala::shell_sort(first, last, less<>());
}

template<class T, class Comp>
constexpr void set_median(T &a, T &b, T &c, Comp comp) {
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
constexpr RandomIter partition_c(RandomIter first, RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    diff_t len = last - first;
    assert(!(len < 3));
    ala::set_median(*first, *(first + len / 2), *(last - 1), comp);
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
constexpr void quick_sort_impl(RandomIter first, RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    diff_t len = last - first;
    if (len <= ALA_INSERTION_THRESHOLD)
        return;
    RandomIter left = ala::partition_c(first, last, comp);
    ala::quick_sort_impl(first, left, comp);
    ala::quick_sort_impl(left, last, comp);
}

// quick-sort with three-way partion
template<class RandomIter, class Comp>
constexpr void quick_sort_three_way_impl(RandomIter first, RandomIter last,
                                         Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    diff_t len = last - first;
    if (len <= ALA_INSERTION_THRESHOLD)
        return;
    ala::set_median(*first, *(first + len / 2), *(last - 1));
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
    ala::quick_sort_three_way_impl(first, left, comp);
    ala::quick_sort_three_way_impl(right, last, comp);
}

template<class RandomIter, class Comp>
constexpr void heap_sort(RandomIter first, RandomIter last, Comp comp);

template<class RandomIter, class Comp>
constexpr void sort_impl(RandomIter first, RandomIter last, unsigned depth,
                         Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    diff_t len = last - first;
    while ((len = last - first) > ALA_INSERTION_THRESHOLD) {
        RandomIter left = ala::partition_c(first, last, comp);
        ala::sort_impl(left, last, --depth, comp);
        last = left;
    }
    if (depth == 0)
        ala::heap_sort(first, last, comp);
}

template<class RandomIter, class Comp>
constexpr void quick_sort(RandomIter first, RandomIter last, Comp comp) {
    if (first < last) {
#if ALA_ENABLE_THREE_WAY_QUICK_SORT
        ala::quick_sort_three_way_impl(first, last, comp);
#else
        ala::quick_sort_impl(first, last, comp);
#endif
        ala::insertion_sort(first, last, comp);
    }
}

template<class RandomIter>
constexpr void quick_sort(RandomIter first, RandomIter last) {
    return ala::quick_sort(first, last, less<>());
}

// a quick-sort variety form eastl
template<class RandomIter, class Comp>
constexpr void sort(RandomIter first, RandomIter last, Comp comp) {
    using T = typename iterator_traits<RandomIter>::value_type;
    if (first < last) {
        unsigned i = 0;
        for (ptrdiff_t s = last - first; s > 0; ++i)
            s >>= 1;
        ala::sort_impl(first, last, 2 * (i - 1), comp);
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
constexpr bool merge_sort_impl(RandomIter1 first, RandomIter2 tmp, Size len,
                               Comp comp) {
    if (len <= ALA_INSERTION_THRESHOLD) {
        ala::insertion_sort(first, first + len);
        return false;
    }
    Size llen = len / 2, rlen = len - len / 2;
    bool l_tmp = ala::merge_sort_impl(first, tmp, llen, comp);
    bool r_tmp = ala::merge_sort_impl(first + llen, tmp + llen, rlen, comp);
    bool tmp_in = l_tmp || r_tmp; // merge tmp into first or merge fisrt into tmp
    if (!tmp_in) {
        RandomIter1 in = first;
        RandomIter2 out = tmp;
        ala::merge_mv(in, in + llen, in + llen, in + len, out, comp);
        return !tmp_in;
    } else if (!l_tmp) {
        ala::move(first, first + llen, tmp);
    } else if (!r_tmp) {
        ala::move(first + llen, first + len, tmp + llen);
    }
    RandomIter2 in = tmp;
    RandomIter1 out = first;
    ala::merge_mv(in, in + llen, in + llen, in + len, out, comp);
    return !tmp_in;
}

template<class BidirIter, class Comp>
constexpr void inplace_merge(BidirIter first, BidirIter middle, BidirIter last,
                             Comp comp) {
    using diff_t = typename iterator_traits<BidirIter>::difference_type;
    using T = typename iterator_traits<BidirIter>::value_type;
    diff_t len = ala::distance(first, last);
    allocator<T> alloc;
    pointer_holder<T *, allocator<T>> ph(alloc, len);
    T *tmp = ph.get();
    ala::merge_umv(first, middle, middle, last, tmp, comp);
    ala::move(tmp, tmp + len, first);
    ala::destroy_n(tmp, len);
}

template<class BidirIter>
constexpr void inplace_merge(BidirIter first, BidirIter middle, BidirIter last) {
    return ala::inplace_merge(first, middle, last, less<>());
}

// top-down merge-sort, no-copyback
template<class RandomIter, class Comp>
constexpr void merge_sort(RandomIter first, RandomIter last, Comp comp) {
    using T = typename iterator_traits<RandomIter>::value_type;
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    diff_t len = last - first;
    allocator<T> alloc;
    pointer_holder<T *, allocator<T>> ph(alloc, len);
    T *tmp = ph.get();
    ala::uninitialized_move(first, last, tmp);
    if (!ala::merge_sort_impl(tmp, first, len, comp))
        ala::move(tmp, tmp + len, first);
    ala::destroy_n(tmp, len);
}

template<class RandomIter>
constexpr void merge_sort(RandomIter first, RandomIter last) {
    ala::merge_sort(first, last, less<>());
}

// bottom-up merge-sort, no-copyback
template<class RandomIter, class Comp>
constexpr void stable_sort(RandomIter first, RandomIter last, Comp comp) {
    return merge_sort(first, last, comp);
    using T = typename iterator_traits<RandomIter>::value_type;
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    diff_t len = last - first;
    allocator<T> alloc;
    pointer_holder<T *, allocator<T>> ph(alloc, len);
    T *tmp = ph.get();
    ala::uninitialized_move(first, last, tmp);
    for (T *left = tmp; left < tmp + len; left += ALA_INSERTION_THRESHOLD) {
        T* next = left + ALA_INSERTION_THRESHOLD;
        ala::insertion_sort(left, ala::min(next, tmp + len), comp);
    }
    bool flag = false;
    for (diff_t size = ALA_INSERTION_THRESHOLD; size < len; size <<= 1) {
        diff_t size2 = size << 1;
        if (flag) {
            RandomIter left = first, right = last;
            T *out = tmp;
            for (; left < right; left += size2, (void)(out += size2))
                ala::merge_mv(left, ala::min(left + size, right),
                              ala::min(left + size, right),
                              ala::min(left + size2, right), out, comp);
        } else {
            T *left = tmp, *right = tmp + len;
            RandomIter out = first;
            for (; left < right; left += size2, (void)(out += size2))
                ala::merge_mv(left, ala::min(left + size, right),
                              ala::min(left + size, right),
                              ala::min(left + size2, right), out, comp);
        }
        flag = !flag;
    }
    if (!flag)
        for (T *i = tmp; first < last; ++first, ++i)
            *first = ala::move(*i);
    ala::destroy_n(tmp, len);
}

template<class RandomIter>
constexpr void stable_sort(RandomIter first, RandomIter last) {
    ala::merge_sort(first, last, less<>());
}

template<class RandomIter, class Distance, class Comp>
constexpr void heap_sink(RandomIter first, Distance begin, Distance end,
                         Comp comp) {
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

// Heap operations
template<class RandomIter, class Comp>
constexpr void push_heap(RandomIter first, RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
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
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    if (first == last)
        return;
    ala::iter_swap(first, last - 1);
    ala::heap_sink(first, (diff_t)0, (diff_t)(last - 1 - first), comp);
}

template<class RandomIter>
constexpr void pop_heap(RandomIter first, RandomIter last) {
    return ala::pop_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void make_heap(RandomIter first, RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    if (first == last)
        return;
    diff_t len = last - first;
    for (diff_t i = (len >> 1); i > 0; --i)
        ala::heap_sink(first, i - 1, len, comp);
}

template<class RandomIter>
constexpr void make_heap(RandomIter first, RandomIter last) {
    ala::make_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void sort_heap(RandomIter first, RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    if (first == last)
        return;
    diff_t len = last - first;
    for (diff_t i = len - 1; i > 0; --i) {
        ala::iter_swap(first, first + i);
        ala::heap_sink(first, (diff_t)0, i, comp);
    }
}

template<class RandomIter>
constexpr void sort_heap(RandomIter first, RandomIter last) {
    ala::sort_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr bool is_heap(RandomIter first, RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
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
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
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
constexpr void heap_sort(RandomIter first, RandomIter last, Comp comp) {
    ala::make_heap(first, last, comp);
    ala::sort_heap(first, last, comp);
}

template<class RandomIter>
constexpr void heap_sort(RandomIter first, RandomIter last) {
    ala::heap_sort(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void partial_sort(RandomIter first, RandomIter middle,
                            RandomIter last, Comp comp) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
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
    RandomIter left = ala::partition_c(first, last, comp);
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
    using T = typename iterator_traits<RandomIter>::value_type;
    auto len = last - first;
    while ((len = last - first) > 1) {
        if (len < 3) {
            if (comp(*(first + 1), *first))
                ala::iter_swap(first + 1, first);
            return;
        }
        RandomIter left = ala::partition_c(first, last, comp);
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
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
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
                ala::heap_sink(first2, (diff_t)0, (diff_t)n, comp);
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
