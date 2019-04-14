// Sort Operation

#ifndef _ALA_DETAIL_SORT_H
#define _ALA_DETAIL_SORT_H

#include <ala/type_traits.h>
#include <ala/iterator.h>
#include <ala/detail/functional_base.h>

namespace ala {

template<typename ForwardIter, class Comp>
void select_sort(ForwardIter first, ForwardIter last, Comp comp) {
    if (first == last)
        return;
    ForwardIter i = first, next = first;
    for (++next; next != last; ++i, ++next) {
        ForwardIter min = i;
        for (ForwardIter j = i; j != last; ++j)
            if (comp(*j, *min))
                min = j;
        iter_swap(i, min);
    }
}

template<typename ForwardIter>
void select_sort(ForwardIter first, ForwardIter last) {
    return ala::select_sort(first, last, less<>());
}

template<typename BidirIter, class Comp>
void insertion_sort(BidirIter first, BidirIter last, Comp comp) {
    typedef typename iterator_traits<BidirIter>::value_type T;
    if (first == last)
        return;
    BidirIter sorted = first;
    BidirIter cur, next;
    for (++sorted; sorted != last; ++sorted) {
        cur = next = sorted;
        T tmp = ala::forward<T>(*sorted);
        for (--cur; comp(tmp, *cur) && next != first; --cur, --next)
            *next = *cur;
        *next = tmp;
    }
}

template<typename BidirIter>
void insertion_sort(BidirIter first, BidirIter last) {
    return ala::insertion_sort(first, last, less<>());
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

template<typename RandomIter>
void shell_sort(RandomIter first, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
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
    RandomIter i, j;
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
decltype(auto) median(T &&a, T &&b, T &&c) {
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

template<typename T>
void set_median1(T &a, T &b, T &c) {
    if (b < a) {
        if (a < c)
            ala::swap(b, a);
        else if (b < c)
            ala::swap(b, c);
    } else if (c < a)
        ala::swap(b, a);
    else if (c < b)
        ala::swap(b, c);
}

template<typename RandomIter>
void quick_sort_impl(RandomIter first, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    ptrdiff_t len = last - first;
    if (len < ALA_INSERTION_THRESHOLD)
        return;
    ala::detail::set_median(*first, *(first + len / 2), *(last - 1));
    RandomIter left = first + 1, right = last - 1;
    for (;; ++left, --right) {
        while (*left < *first) {
            // assert(left < last);
            ++left;
        }
        while (*first < *right) {
            // assert(right >= first);
            --right;
        }
        if (!(left < right))
            break;
        iter_swap(left, right);
    }
    quick_sort_impl(first, left);
    quick_sort_impl(left, last);
} // namespace detail

// quick-sort with three-way partion
template<typename RandomIter>
void quick_sort_three_way_impl(RandomIter first, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    ptrdiff_t len = last - first;
    if (len < ALA_INSERTION_THRESHOLD)
        return;
    swap(*first, *iter_median(first, first + len / 2, last - 1));
    RandomIter left = first, i = first + 1, right = last;
    // [first, left) < pivot, [left, right) == pivot, [right, end) > pivot
    while (i < right) {
        if (*i < *left)
            iter_swap(left++, i++);
        else if (*left < *i)
            iter_swap(i, --right);
        else
            ++i;
    }
    quick_sort_three_way_impl(first, left);
    quick_sort_three_way_impl(right, last);
}

} // namespace detail

template<typename RandomIter>
void quick_sort(RandomIter first, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    if (first < last) {
#if ALA_ENABLE_THREE_WAY_QUICK_SORT
        detail::quick_sort_three_way_impl(first, last);
#else
        detail::quick_sort_impl(first, last);
#endif
        insertion_sort(first, last);
    }
}

namespace detail {

template<typename RandomIter, class Comp>
void sort_impl(RandomIter first, RandomIter last, unsigned depth, Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    while (last - first > ALA_INSERTION_THRESHOLD) {
        auto len = last - first;
        ala::detail::set_median(*first, *(first + len / 2), *(last - 1));
        RandomIter left = first + 1, right = last - 1;
        for (;; ++left, --right) {
            while (comp(*left, *first)) {
                // assert(left < last);
                ++left;
            }
            while (comp(*first, *right)) {
                // assert(right >= first);
                --right;
            }
            if (!(left < right))
                break;
            iter_swap(left, right);
        }
        sort_impl(left, last, --depth, comp);
        last = left;
    }
    if (depth == 0)
        insertion_sort(first, last, comp);
}
} // namespace detail

// a quick-sort variety form eastl
template<typename RandomIter, class Comp>
constexpr void sort(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    if (first < last) {
        unsigned i = 0;
        for (ptrdiff_t s = last - first; s > 0; ++i)
            s >>= 1;
        detail::sort_impl(first, last, 2 * (i - 1), comp);
        insertion_sort(first, last, comp);
    }
}

template<typename RandomIter>
constexpr void sort(RandomIter first, RandomIter last) {
    return sort(first, last, less<>());
}

template<class Iter1, class Iter2, class OutIter, class Comp>
constexpr OutIter merge(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                        OutIter out, Comp comp) {
    while (first1 != last1 && first2 != last2)
        *out++ = comp(*first2, *first1) ? *first2++ : *first1++;
    while (first1 != last1)
        *out++ = *first1++;
    while (first2 != last2)
        *out++ = *first2++;
    return out;
}

template<class Iter1, class Iter2, class OutIter>
constexpr OutIter merge(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2,
                        OutIter out) {
    return ala::merge(first1, last1, first2, last2, out, less<>());
}

template<class BidirIt, class Comp>
void inplace_merge(BidirIt first, BidirIt middle, BidirIt last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    T *tmp = new T[last - first];
    ala::merge(first, middle, middle, last, tmp, comp);
    ala::move(tmp, tmp + (first - last), first);
    delete[] tmp;
}

template<class BidirIt>
void inplace_merge(BidirIt first, BidirIt middle, BidirIt last) {
    return ala::inplace_merge(first, middle, last, less<>());
}

namespace detail {

template<typename RandomIter>
void merge_sort_impl(RandomIter first, RandomIter last, RandomIter tmp_first) {
    ptrdiff_t len = last - first;
    if (len < ALA_INSERTION_THRESHOLD) {
        insertion_sort(first, last);
        return;
    }
    RandomIter mid = first + len / 2, tmp_mid = tmp_first + len / 2,
               tmp_last = tmp_first + len;
    merge_sort_impl(tmp_first, tmp_mid, first);
    merge_sort_impl(tmp_mid, tmp_last, mid);
    merge(tmp_first, tmp_mid, tmp_mid, tmp_last, first);
}

} // namespace detail

// top-down merge-sort, no-copyback
template<typename RandomIter>
void merge_sort(RandomIter first, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    T *tmp = new T[last - first];
    ala::copy(first, last, tmp);
    detail::merge_sort_impl(first, last, tmp);
    delete[] tmp;
    return;
}

// bottom-up merge-sort, no-copyback
template<typename RandomIter>
constexpr void stable_sort(RandomIter first, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    ptrdiff_t len = last - first;
    RandomIter tmp = new T[len];
    RandomIter left;
    for (left = first; left < last; left += ALA_INSERTION_THRESHOLD) {
        RandomIter next = left + ALA_INSERTION_THRESHOLD;
        insertion_sort(left, next < last ? next : last);
    }
    bool flag = true;
    for (size_t size = ALA_INSERTION_THRESHOLD; size < len; size <<= 1) {
        RandomIter mid, left = (flag ? first : tmp),
                        right = flag ? last : tmp + len,
                        out_left = (flag ? tmp : first);
        for (mid = left + size; left < right - (size << 1);
             left += size << 1, out_left += size << 1, mid = left + size)
            merge(left, mid, mid, left + (size << 1), out_left);
        if (mid < right)
            merge(left, mid, mid, right, out_left);
        else
            while (left != right)
                *out_left++ = ala::move(*left++);
        flag = !flag;
    }
    if (!flag)
        for (RandomIter i = tmp; first < last; ++first, ++i)
            *first = ala::move(*i);
    delete[] tmp;
    return;
}

namespace detail {

template<class RandomIter, class Distance, class Comp>
void heap_sink(RandomIter first, Distance begin, Distance end, Comp comp) {
    ptrdiff_t parent = begin, child = (parent << 1) + 1;
    while (child < end) {
        if (child < end - 1 && first[child] < first[child + 1])
            ++child;
        if (!(first[parent] < first[child]))
            return;
        ala::iter_swap(first + parent, first + child);
        parent = child;
        child = (parent << 1) + 1;
    }
}

} // namespace detail

// Heap operations
template<class RandomIter, class Comp>
constexpr void push_heap(RandomIter first, RandomIter last, Comp comp) {
    auto child = last - first, parent = (child - 1) >> 1;
    for (; parent >= 0 && comp(first[parent], first[child]);
         parent = (child - 1) >> 1) {
        ala::iter_swap(first + parent, first + child);
        child = parent;
    }
}

template<class RandomIter>
constexpr void push_heap(RandomIter first, RandomIter last) {
    return ala::push_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void pop_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    ala::iter_swap(first, last - 1);
    ala::detail::heap_sink(first, (diff_t)0, (diff_t)(first - last - 1), comp);
}

template<class RandomIter>
constexpr void pop_heap(RandomIter first, RandomIter last);

template<class RandomIter, class Comp>
constexpr void make_heap(RandomIter first, RandomIter last, Comp comp) {
    auto len = last - first;
    for (auto i = (len >> 1) - 1; i >= 0; --i)
        ala::detail::heap_sink(first, i, len, comp);
}

template<class RandomIter>
constexpr void make_heap(RandomIter first, RandomIter last) {
    ala::make_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr void sort_heap(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
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
    auto len = last - first;
    for (decltype(len) i = 0; i < (len >> 1) - 1; ++i) {
        if (comp(first[i], first[(i >> 1) - 1]))
            return false;
        if (i >> 1 < len && comp(first[i], first[i >> 1]))
            return false;
    }
    return true;
}

template<class RandomIter>
constexpr bool is_heap(RandomIter first, RandomIter last) {
    return is_heap(first, last, less<>());
}

template<class RandomIter, class Comp>
constexpr RandomIter is_heap_until(RandomIter first, RandomIter last, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
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

template<class RandomIter>
constexpr void partial_sort(RandomIter first, RandomIter middle, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    ptrdiff_t len = last - first;
    if (len < 2 || middle - first < 1)
        return;
    ala::detail::set_median(*first, *(first + len / 2), *(last - 1));
    RandomIter left = first + 1, right = last - 1;
    for (;; ++left, --right) {
        while (*left < *first) {
            assert(left < last);
            ++left;
        }
        while (*first < *right) {
            assert(right >= first);
            --right;
        }
        if (!(left < right))
            break;
        iter_swap(left, right);
    }
    if (left < middle) {
        sort(first, left);
        partial_sort(left, middle, last);
    } else
        partial_sort(first, middle, left);
}

template<class RandomIter, class Comp>
constexpr void partial_sort(RandomIter first, RandomIter middle,
                            RandomIter last, Comp comp);

template<class RandomIter, class Comp>
constexpr void nth_element(RandomIter first, RandomIter nth, RandomIter last,
                           Comp comp);

template<class RandomIter>
constexpr void nth_element(RandomIter first, RandomIter nth, RandomIter last) {
    typedef typename iterator_traits<RandomIter>::value_type T;
    auto len = last - first;
    while ((len = last - first) > 1) {
        ala::detail::set_median(*first, *(first + len / 2), *(last - 1));
        RandomIter left = first + 1, right = last - 1;
        for (;; ++left, --right) {
            while (*left < *first) {
                assert(left < last);
                ++left;
            }
            while (*first < *right) {
                assert(right >= first);
                --right;
            }
            if (!(left < right))
                break;
            iter_swap(left, right);
        }
        if (first < left - 1) {
            iter_swap(left - 1, first);
            if (nth < left - 1)
                last = left - 1;
            else if (left - 1 < nth)
                first = left;
            else
                return;
        }
    }
}

template<class Iter, class RandomIter, class Comp>
constexpr RandomIter partial_sort_copy(Iter first1, Iter last1, RandomIter first2,
                                       RandomIter last2, Comp comp) {
    typedef typename iterator_traits<RandomIter>::difference_type diff_t;
    diff_t n = 0;
    for (; first1 != last1 && n < last2 - first2; ++first1, ++n)
        first2[n] = *first1;
    if (first1 != last1) {
        ala::make_heap(first2, last2, comp);
        for (; first1 != last1; ++first1)
            if (comp(*first1, *first2)) {
                *first2 = *first1;
                ala::detail::heap_sink(first2, (diff_t)0, (diff_t)(n - 1), comp);
            }
        ala::sort_heap(first2, last2, comp);
        // ala::sort(first2, last2, comp);
        // for (; first1 != last1; ++first1) {
        //     diff_t pos = n - 1;
        //     for (; pos >= 0 && comp(*first1, *(first2 + pos)); --pos)
        //         ;
        //     if (++pos < n) {
        //         for (diff_t i = n - 1; i > pos; --i)
        //             first2[i] = first2[i - 1];
        //         first2[pos] = *first1;
        //     }
        // }
        return last2;
    } else {
        ala::sort(first2, first2 + n, comp);
        return first2 + n;
    }
}

template<class Iter, class RandomIter>
constexpr RandomIter partial_sort_copy(Iter first1, Iter last1,
                                       RandomIter first2, RandomIter last2) {
    return ala::partial_sort_copy(first1, last1, first2, last2, less<>());
}

} // namespace ala

#include <ala/detail/parallel/sort.h>

#endif
