#ifndef PARALLEL_SORT_HPP
#define PARALLEL_SORT_HPP

#include <thread>
#include <vector>

#define _MAX_NUM_TASKS_PER_CORE 1024
#define _SORT_MAX_RECURSION_DEPTH 64
#define _FINE_GRAIN_CHUNK_SIZE 512

namespace ala {

// template <class T>
// void merge(T *arr, T *reg, int start1, int end1, int start2, int end2) { //该归并不往回复制
// 	int k = start1;
// 	while (start1 <= end1 && start2 <= end2)
// 		reg[k++] = arr[start1] < arr[start2] ? arr[start1++] : arr[start2++];
// 	while (start1 <= end1)
// 		reg[k++] = arr[start1++];
// 	while (start2 <= end2)
// 		reg[k++] = arr[start2++];
// }

// template <class T>
// void merge_copyback(T *arr, T *reg, int start1, int end1, int start2, int end2) { //该归并不往回复制
// 	int k = start1, start = start1;
// 	while (start1 <= end1 && start2 <= end2)
// 		reg[k++] = arr[start1] < arr[start2] ? arr[start1++] : arr[start2++];
// 	while (start1 <= end1)
// 		reg[k++] = arr[start1++];
// 	while (start2 <= end2)
// 		reg[k++] = arr[start2++];
// 	for (int i = start; i <= end2; i++)
// 		arr[i] = reg[i];
// }

// template <class T>
// void merge_sort_8(T *arr, T *reg, int start, int end) {
// 	int gap = (end - start + 1) / 8 - 1;
// 	std::thread t[8];
// 	t[0] = std::thread(merge_sort<T>, arr, reg, start + gap * 0 + 0, start + gap * 1 + 0);
// 	t[1] = std::thread(merge_sort<T>, arr, reg, start + gap * 1 + 1, start + gap * 2 + 1);
// 	t[2] = std::thread(merge_sort<T>, arr, reg, start + gap * 2 + 2, start + gap * 3 + 2);
// 	t[3] = std::thread(merge_sort<T>, arr, reg, start + gap * 3 + 3, start + gap * 4 + 3);
// 	t[4] = std::thread(merge_sort<T>, arr, reg, start + gap * 4 + 4, start + gap * 5 + 4);
// 	t[5] = std::thread(merge_sort<T>, arr, reg, start + gap * 5 + 5, start + gap * 6 + 5);
// 	t[6] = std::thread(merge_sort<T>, arr, reg, start + gap * 6 + 6, start + gap * 7 + 6);
// 	t[7] = std::thread(merge_sort<T>, arr, reg, start + gap * 7 + 7, end);
// 	t[0].join();
// 	t[1].join();
// 	t[2].join();
// 	t[3].join();
// 	t[4].join();
// 	t[5].join();
// 	t[6].join();
// 	t[7].join();

// 	t[0] = std::thread(merge_copyback<T>, arr, reg, start + gap * 0 + 0, start + gap * 1 + 0, start + gap * 1 + 1, start + gap * 2 + 1);
// 	t[1] = std::thread(merge_copyback<T>, arr, reg, start + gap * 2 + 2, start + gap * 3 + 2, start + gap * 3 + 3, start + gap * 4 + 3);
// 	t[2] = std::thread(merge_copyback<T>, arr, reg, start + gap * 4 + 4, start + gap * 5 + 4, start + gap * 5 + 5, start + gap * 6 + 5);
// 	t[3] = std::thread(merge_copyback<T>, arr, reg, start + gap * 6 + 6, start + gap * 7 + 6, start + gap * 7 + 7, end);
// 	t[0].join();
// 	t[1].join();
// 	t[2].join();
// 	t[3].join();

// 	t[0] = std::thread(merge_copyback<T>, arr, reg, start, start + gap * 2 + 1, start + gap * 2 + 2, start + gap * 4 + 3);
// 	t[1] = std::thread(merge_copyback<T>, arr, reg, start + gap * 4 + 4, start + gap * 6 + 5, start + gap * 6 + 6, end);
// 	t[0].join();
// 	t[1].join();

// 	merge_copyback<T>(arr, reg, start, start + gap * 4 + 3, start + gap * 4 + 4, end);

// 	// for (int i = start; i <= end; i++)
// 	// 	arr[i] = reg[i];
// }

// template <class T>
// void merge_sort_4(T *arr, T *reg, int start, int end) {
// 	int gap = (end - start + 1) / 4 - 1;
// 	std::thread t[4];

// 	t[0] = std::thread(merge_sort<T>, arr, reg, start + gap * 0 + 0, start + gap * 1 + 0);
// 	t[1] = std::thread(merge_sort<T>, arr, reg, start + gap * 1 + 1, start + gap * 2 + 1);
// 	t[2] = std::thread(merge_sort<T>, arr, reg, start + gap * 2 + 2, start + gap * 3 + 2);
// 	t[3] = std::thread(merge_sort<T>, arr, reg, start + gap * 3 + 3, end);
// 	t[0].join();
// 	t[1].join();
// 	t[2].join();
// 	t[3].join();

// 	t[0] = std::thread(merge<T>, arr, reg, start, start + gap * 1 + 0, start + gap * 1 + 1, start + gap * 2 + 1);
// 	t[1] = std::thread(merge<T>, arr, reg, start + gap * 2 + 2, start + gap * 3 + 2, start + gap * 3 + 3, end);
// 	t[0].join();
// 	t[1].join();

// 	merge<T>(reg, arr, start, start + gap * 2 + 1, start + gap * 2 + 2, end);
// }

// template <int N, typename T>
// void parallel_merge_sort(T *arr, T *reg, int start, int end) {
// 	int gap = (end - start + 1) / N - 1;
// 	std::thread *t[N];

// 	for (int i = 0; i < N - 1; i++)
// 		t[i] = new std::thread(merge_sort<T>, arr, reg, start + gap * i + i, start + gap * (i + 1) + i);
// 	t[N - 1] = new std::thread(merge_sort<T>, arr, reg, start + gap * (N - 1) + (N - 1), end);
// 	for (std::thread *it : t)
// 		it->join();

// 	int end1 = start + gap;
// 	for (int i = 0; i < N - 2; i++) {
// 		merge_copyback(arr, reg, start, end1, end1 + 1, end1 + 1 + gap);
// 		end1 += 1 + gap;
// 	}
// 	merge_copyback(arr, reg, start, end1, end1 + 1, end);
// }

// template <class T>
// void merge_sort_parallel(T *a, int m, int n) {
// 	T *b = new T[n + 1];
// 	merge_sort_8(a, b, m, n);
// 	delete[] b;
// }

namespace detail {

// improve quick-sort
template<typename RanIt>
inline void parallel_quick_sort_impl(RanIt m, RanIt n, ptrdiff_t limit) {
    typedef typename iterator_traits<RanIt>::value_type T;
    ptrdiff_t len = n - m;
    if (len < 48) {
        insertion_sort(m, n);
        return;
    }
    const T pivot = detail::median(*m, *(m + len / 2), *(n - 1));
    RanIt left = m, right = n;
    for (;; ++left) {
        while (*left < pivot)
            ++left;
        --right;
        while (pivot < *right)
            --right;
        if (left >= right)
            break;
        swap(*left, *right);
    }
    std::thread t;
    const bool div = left - m > limit;
    if (div)
        t = std::thread(parallel_quick_sort_impl<RanIt>, m, left, limit);
    else
        parallel_quick_sort_impl(m, left, limit);
    parallel_quick_sort_impl(left, n, limit);
    if (div)
        t.join();
}

} // namespace detail

template<typename RanIt>
inline void parallel_quick_sort(RanIt m, RanIt n) {
    if (m < n) {
        const int ths = std::thread::hardware_concurrency();
        detail::parallel_quick_sort_impl(m, n, (n - m) / 2 / ths);
    }
}

namespace detail {

template<typename RanIt>
inline void parallel_sort_impl(RanIt m, RanIt n, ptrdiff_t limit) {
    std::vector<std::thread> thread_pool;
    typedef typename iterator_traits<RanIt>::value_type T;
    while (n - m > 48) {
        const T pivot = forward<T>(
            detail::median(forward<T>(*m), forward<T>(*(m + (n - m) / 2)),
                           forward<T>(*(n - 1))));
        RanIt left = m, right = n;
        for (;; ++left) {
            while (*left < pivot)
                ++left;
            --right;
            while (pivot < *right)
                --right;
            if (left >= right)
                break;
            swap(*left, *right);
        }
        if (n - m > limit)
            thread_pool.emplace_back(parallel_sort_impl<RanIt>, left, n, limit);
        else
            parallel_sort_impl(left, n, limit);
        n = left;
    }
    insertion_sort(m, n);
    for (std::thread &th : thread_pool)
        th.join();
}

} // namespace detail

// a quick-sort variety form eastl
template<typename RanIt>
inline void parallel_sort(RanIt m, RanIt n) {
    if (m < n) {
        const int ths = std::thread::hardware_concurrency();
        detail::parallel_sort_impl(m, n, (n - m) / 2 / ths);
    }
}

} // namespace ala

#endif