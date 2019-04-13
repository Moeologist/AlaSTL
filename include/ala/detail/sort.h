// ForIt: forward iterator, BiIt: bidirectional iterator, RanIt: random access iterator

#ifndef _ALA_DETAIL_SORT_H
#define _ALA_DETAIL_SORT_H

#include <ala/type_traits.h>
#include <ala/iterator.h>

namespace ala {

template<typename ForIt>
void select_sort(ForIt first, ForIt last) {
    if (first == last)
        return;
    ForIt i = first, next = first;
    for (++next; next != last; ++i, ++next) {
        ForIt min = i;
        for (ForIt j = i; j != last; ++j)
            if (*j < *min)
                min = j;
        iter_swap(i, min);
    }
}

template<typename BiIt>
void insertion_sort(BiIt first, BiIt last) {
    typedef typename iterator_traits<BiIt>::value_type T;
    if (first == last)
        return;
    BiIt sorted = first;
    BiIt cur, next;
    for (++sorted; sorted != last; ++sorted) {
        cur = next = sorted;
        T tmp = ala::forward<T>(*sorted);
        for (--cur; tmp < *cur && next != first; --cur, --next)
            *next = *cur;
        *next = tmp;
    }
}

// void _shell_sequence() {
//     uint64_t seq[64];
//     for (int i = 0; i < 32; ++i) {
//         seq[i * 2] = 9 * pow(4, i) - 9 * pow(2, i) + 1;
//         seq[i * 2 + 1] = pow(2, i + 2) * (pow(2, i + 2) - 3) + 1;
//     }
//     for (int i = 0; i < 64; ++i)
//         if (seq[i] > 0)
//             printf("0x%llxull,", seq[i]);
// }

template<typename RanIt>
void shell_sort(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    constexpr uint64_t seq[] = {0x1ull,
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
    RanIt i, j;
    for (--index; index >= 0; --index) {
        const size_t gap = seq[index];
        for (i = first + gap; i < last; ++i) {
            const T tmp = *i;
            for (j = i - gap; j >= first && tmp < *j; j = j - gap)
                *(j + gap) = *j;
            *(j + gap) = tmp;
        }
    }
}

namespace detail {

template<typename T>
T &&median(T &&a, T &&b, T &&c) {
    if (a < b)
        if (b < c)
            return forward<T>(b);
        else if (a < c)
            return forward<T>(c);
        else
            return forward<T>(a);
    else if (a < c)
        return forward<T>(a);
    else if (b < c)
        return forward<T>(c);
    return forward<T>(b);
}

template<typename It>
It iter_median(It a, It b, It c) {
    if (*a < *b)
        if (*b < *c)
            return b;
        else if (*a < *c)
            return c;
        else
            return a;
    else if (*a < *c)
        return a;
    else if (*b < *c)
        return c;
    return b;
}

template<typename T>
void set_median(T &a, T &b, T &c) {
    if (a < b) {
        if (b < c)
            ala::swap(a, b);
        else if (a < c)
            ala::swap(a, c);
    } else if (c < b)
        ala::swap(a, b);
    else if (c < a)
        ala::swap(a, c);
}

// improved quick-sort
// template<typename RanIt>
// void quick_sort_impl(RanIt first, RanIt last) {
//     typedef typename iterator_traits<RanIt>::value_type T;
//     ptrdiff_t length = last - first;
//     if (length < 2) {
//         return;
//     }
//     // T pivot = ala::detail::median(*first, *(first + length / 2), *(last - 1));
//     set_median(*first, *(first + length / 2), *(last - 1));
//     T pivot = ala::move(*first);
//     RanIt left = first, right = last - 1;
//     while (left < right) {
//         while (!(*right < pivot) && left < right)
//             // while (*pivot < *right)
//             --right;
//         if (left < right)
//             *left = ala::move(*right);
//         while (!(pivot < *left) && left < right)
//             // while (*left < *pivot)
//             ++left;
//         if (left < right)
//             *right = ala::move(*left);
//     }
//     *left = ala::move(pivot);
//     quick_sort_impl(first, left);
//     quick_sort_impl(left, last);
// }

template<typename RanIt>
void quick_sort_impl(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    ptrdiff_t length = last - first;
    if (length < 28) {
        return;
    }
    RanIt pivot = iter_median(first, first + length / 2, last - 1);
    RanIt left = first, right = last;
    for (;; ++left) {
        while (*left < *pivot)
            ++left;
        --right;
        while (*pivot < *right)
            --right;
        if (left >= right)
            break;

        iter_swap(left, right);
        if (left == pivot)
            pivot = right;
        else if (right == pivot)
            pivot = left;
    }
    quick_sort_impl(first, left);
    quick_sort_impl(left, last);
}

// quick-sort with three-way partion
template<typename RanIt>
void quick_sort_impl3(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    ptrdiff_t length = last - first;
    if (length < 28) {
        return;
    }
    swap(*first, *iter_median(first, first + length / 2, last - 1));
    // T pivot = *first;
    RanIt left = first, i = first + 1, right = last;
    // [first, left) < pivot, [left, right) == pivot, [right, end) > pivot
    while (i < right) {
        if (*i < *left)
            iter_swap(left++, i++);
        else if (*left < *i)
            iter_swap(i, --right);
        else
            ++i;
    }
    quick_sort_impl3(first, left);
    quick_sort_impl3(right, last);
}

} // namespace detail
#define ALA_ENABLE_3WAY_QUICK_SOR 0
template<typename RanIt>
void quick_sort(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    if (first < last) {
#if ALA_ENABLE_3WAY_QUICK_SOR
        detail::quick_sort_impl3(first, last);
#else
        detail::quick_sort_impl(first, last);
#endif
        insertion_sort(first, last);
    }
}

namespace detail {

template<typename RanIt>
void insertion_for_sort(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    RanIt cur, next;
    for (RanIt sorted = first; sorted != last; ++sorted) {
        cur = next = sorted;
        T tmp = ala::move(*sorted);
        for (--cur; tmp < *cur; --cur, --next) {
            *next = *cur;
        }
        *next = ala::move(tmp);
    }
}

template<typename RanIt>
void sort_impl(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    while (last - first > 32) {
        RanIt pivot = iter_median(first, first + (last - first) / 2, last - 1);
        RanIt left = first, right = last;
        for (;; ++left) {
            while (*left < *pivot)
                ++left;
            --right;
            while (*pivot < *right)
                --right;
            if (left >= right)
                break;
            iter_swap(left, right);
            if (left == pivot)
                pivot = right;
            else if (right == pivot)
                pivot = left;
        }
        sort_impl(left, last);
        last = left;
    }
}

} // namespace detail

// a quick-sort variety form eastl
template<typename RanIt>
constexpr void sort(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    if (first < last) {
        detail::sort_impl(first, last);
        insertion_sort(first, last);
    }
}

namespace detail {

template<typename RanIt>
void merge_sort_impl(RanIt first, RanIt last, RanIt tmp_first) {
    ptrdiff_t length = last - first;
    if (length < 28) {
        insertion_sort(first, last);
        return;
    }
    const RanIt mid = first + length / 2, tmp_mid = tmp_first + length / 2,
                tmp_last = tmp_first + length;
    merge_sort_impl(tmp_first, tmp_mid, first);
    merge_sort_impl(tmp_mid, tmp_last, mid);
    RanIt i = tmp_first, j = tmp_mid;
    while (i != tmp_mid && j != tmp_last)
        *first++ = (*i < *j || *i == *j) ? *i++ : *j++;
    while (i != tmp_mid)
        *first++ = *i++;
    while (j != tmp_last)
        *first++ = *j++;
}

} // namespace detail

// top-down merge-sort, no-copyback
template<typename RanIt>
void merge_sort(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    T *tmp = new T[last - first];
    detail::merge_sort_impl(first, last, tmp);
    delete[] tmp;
    return;
}

// bottom-up merge-sort, no-copyback
template<typename RanIt>
constexpr void stable_sort(RanIt first, RanIt last) {
    typedef typename iterator_traits<RanIt>::value_type T;
    ptrdiff_t length = last - first;
    RanIt tmp = new T[length];
    RanIt left;
    for (left = first; left < last - 32; left += 32) {
        RanIt next = left + 32;
        insertion_sort(left, next);
    }
    insertion_sort(left, last);
    bool flag = true;
    for (size_t size = 32; size < length; size <<= 1) {
        const RanIt right = flag ? last : tmp + length;
        RanIt mid, left = flag ? first : tmp, out_left = flag ? tmp : first;
        for (mid = left + size; left < right - (size << 1);
             left += size << 1, out_left += size << 1, mid = left + size)
            merge(left, mid, mid, left + (size << 1), out_left);
        if (mid < right)
            merge(left, mid, mid, right, out_left);
        else
            while (left != right)
                *out_left++ = *left++;
        flag = !flag;
    }
    if (!flag)
        for (RanIt t = tmp; first < last; ++first, ++t)
            *first = *t;
    delete[] tmp;
    return;
}

namespace detail {

template<typename RanIt>
void heap_sink(RanIt first, ptrdiff_t begin, ptrdiff_t end) {
    ptrdiff_t parent = begin, child = (parent << 1) + 1;
    while (child <= end) {
        if (child < end && first[child] < first[child + 1])
            ++child;
        if (!(first[parent] < first[child]))
            return;
        ala::iter_swap(first + parent, first + child);
        parent = child;
        child = (parent << 1) + 1;
    }
}

} // namespace detail

template<typename RanIt>
void heap_sort(RanIt first, RanIt last) {
    ptrdiff_t length = last - first;
    for (ptrdiff_t i = length / 2 - 1; i >= 0; --i)
        detail::heap_sink(first, i, length - 1);
    for (ptrdiff_t i = length - 1; i > 0; --i) {
        iter_swap(first, first + i);
        detail::heap_sink(first, 0, i - 1);
    }
}

} // namespace ala

#include <ala/detail/parallel/sort.h>

#endif