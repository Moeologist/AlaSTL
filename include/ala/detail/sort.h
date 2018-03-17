// left-closed right-open interval, first: left iterator, last: tright iterator
// ForIt: forward iterator, BiIt: bidirectional iterator, RanIt: random access iterator

#ifndef DETAIL_SORT_H
#define DETAIL_SORT_H

namespace ala {

template <typename ForIt>
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

template <typename BiIt>
void insertion_sort(BiIt first, BiIt last) {
	typedef typename iterator_traits<BiIt>::value_type T;
	if (first == last)
		return;
	BiIt sorted = first, next, cur;
	for (++sorted; sorted != last; ++sorted) {
		const T temp = *sorted;
		cur = next = sorted;
		for (--cur; temp < *cur && next != first; --cur, --next)
			*next = *cur;
		*next = temp;
	}
}

// void gen_shell_sequence() {
// 	unsigned long long seq[128];
// 	for (int i = 0; i < 64; ++i) {
// 		seq[i * 2] = 9 * pow(4, i) - 9 * pow(2, i) + 1;
// 		seq[i * 2 + 1] = pow(2, i + 2) * (pow(2, i + 2) - 3) + 1;
// 	}
// 	for (int i = 0; i < 128; ++i)
// 		printf("%d", seq[i]);
// }
template <typename RanIt>
void shell_sort(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	const uint64_t seq[] = {0x1, 0x5, 0x13, 0x29, 0x6d, 0xd1, 0x1f9, 0x3a1, 0x871, 0xf41, 0x22e1, 0x3e81, 0x8dc1, 0xfd01, 0x23b81, 0x3fa01, 0x8f701, 0xff401, 0x23ee01, 0x3fe801, 0x8fdc01, 0xffd001, 0x23fb801, 0x3ffa001, 0x8ff7001, 0xfff4001, 0x23fee001, 0x3ffe8001, 0x8ffdc001, 0xfffd0001, 0x23ffb8001, 0x3fffa0001, 0x8fff70001, 0xffff40001, 0x23ffee0001, 0x3fffe80001, 0x8fffdc0001, 0xffffd00001, 0x23fffb80001, 0x3ffffa00001, 0x8ffff700001, 0xfffff400001, 0x23fffee00001, 0x3ffffe800001, 0x8ffffdc00001, 0xfffffd000001, 0x23ffffb800001, 0x3fffffa000001, 0x8fffff7000001, 0xffffff4000001, 0x23ffffee000000, 0x3fffffe8000000, 0x8fffffdc000000, 0xffffffd0000000, 0x23fffffb8000000, 0x3ffffffa0000000, 0x8ffffff70000000, 0xfffffff40000000, 0x23fffffee0000000, 0x3ffffffe80000000, 0x8ffffffdc0000000, 0xfffffffd00000000};
	int index = 0;
	while (last - first > seq[index])
		index++;
	RanIt i, j;
	for (--index; index >= 0; --index) {
		const size_t gap = seq[index];
		for (i = first + gap; i < last; ++i) {
			const T temp = *i;
			for (j = i - gap; j >= first && temp < *j; j = j - gap)
				*(j + gap) = *j;
			*(j + gap) = temp;
		}
	}
}

namespace detail {

template <typename T>
T &&median(T &&a, T &&b, T &&c) {
	if (a < b) {
		if (b < c)
			return forward<T>(b);
		else if (a < c)
			return forward<T>(c);
		else
			return forward<T>(a);
	}
	else if (a < c)
		return forward<T>(a);
	else if (b < c)
		return forward<T>(c);
	return forward<T>(b);
}

template <typename T>
void set_median(T &x, T &a, T &b, T &c) {
	if (a < b) {
		if (b < c)
			swap(x, b);
		else if (a < c)
			swap(x, c);
		else
			swap(x, a);
	}
	else if (a < c)
		swap(x, a);
	else if (b < c)
		swap(x, c);
	else
		swap(x, b);
}

// improved quick-sort
template <typename RanIt>
void quick_sort_impl(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	ptrdiff_t length = last - first;
	if (length < 28) {
		return;
	}
	const T pivot = detail::median(*first, *(first + length / 2), *(last - 1));
	RanIt left = first, right = last;
	for (;; ++left) {
		while (*left < pivot)
			++left;
		--right;
		while (pivot < *right)
			--right;
		if (left >= right)
			break;
		iter_swap(left, right);
	}
	quick_sort_impl(first, left);
	quick_sort_impl(left, last);
}

// improved quick-sort, 3-way partion
template <typename RanIt>
void quick_sort_impl3(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	ptrdiff_t length = last - first;
	if (length < 28) {
		return;
	}
	detail::set_median(*first, *(first + length / 4),
	                   *(first + length * 2 / 4), *(first + length * 3 / 4));
	const T pivot = *first;
	RanIt left = first, i = first + 1, right = last - 1;
	while (i <= right) {
		if (*i < pivot)
			iter_swap((left++), (i++));
		else if (!(*i == pivot))
			iter_swap(i, (right--));
		else
			++i;
	}
	quick_sort_impl3(first, left);
	quick_sort_impl3(right + 1, last);
}

} // namespace detail

template <typename RanIt>
void quick_sort(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	if (first < last) {
#if ALA_ENABLE_3WAY_QUICK_SORT
		detail::quick_sort_impl3(first, last);
#else
		detail::quick_sort_impl(first, last);
#endif
		insertion_sort(first, last);
	}
}

namespace detail {

template <typename RanIt>
void insertion_for_sort(RanIt first, RanIt last) {
	typename iterator_traits<RanIt>::value_type temp;
	for (RanIt sorted = first; sorted != last; ++sorted) {
		RanIt cur = sorted, next = sorted;
		temp = *sorted;
		for (--cur; temp < *cur; --cur, --next) {
			*next = *cur;
		}
		*next = temp;
	}
}

template <typename RanIt>
void sort_impl(RanIt first, RanIt last, unsigned int depth) {
	typedef typename iterator_traits<RanIt>::value_type T;
	while (last - first > 28 && depth > 0) {
		const T pivot = forward<T>(detail::median(forward<T>(*first),
		                                          forward<T>(*(first + (last - first) / 2)),
		                                          forward<T>(*(last - 1))));
		RanIt left = first, right = last;
		for (;; ++left) {
			while (*left < pivot)
				++left;
			--right;
			while (pivot < *right)
				--right;
			if (left >= right)
				break;
			iter_swap(left, right);
		}
		sort_impl(left, last, --depth);
		last = left;
	}
	if (depth == 0)
		insertion_sort(first, last);
}

} // namespace detail

// a quick-sort variety form eastl
template <typename RanIt>
void sort(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	if (first < last) {
		unsigned i = 0;
		ptrdiff_t s = last - first;
		for (; s; ++i)
			s >>= 1;
		detail::sort_impl(first, last, 2 * (i - 1));
		if (last - first > 28) {
			insertion_sort(first, first + 28);
			detail::insertion_for_sort(first + 28, last);
		}
		else
			insertion_sort(first, last);
	}
}

namespace detail {

template <typename RanIt>
void merge_sort_impl(RanIt first, RanIt last, RanIt temp_first) {
	ptrdiff_t length = last - first;
	if (length < 28) {
		insertion_sort(first, last);
		return;
	}
	const RanIt mid = first + length / 2, temp_mid = temp_first + length / 2, temp_last = temp_first + length;
	merge_sort_impl(temp_first, temp_mid, first);
	merge_sort_impl(temp_mid, temp_last, mid);
	RanIt i = temp_first, j = temp_mid;
	while (i != temp_mid && j != temp_last)
		*first++ = (*i < *j || *i == *j) ? *i++ : *j++;
	while (i != temp_mid)
		*first++ = *i++;
	while (j != temp_last)
		*first++ = *j++;
}

} // namespace detail

// top-down merge-sort, no-copyback
template <typename RanIt>
void merge_sort(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	T *temp(new T[last - first]);
	for (RanIt i = first, ti = temp; i != last; ++i, ++ti)
		*ti = *i;
	detail::merge_sort_impl(first, last, RanIt(temp));
	delete[] temp;
	return;
}

// bottom-up merge-sort, no-copyback
template <typename RanIt>
void stable_sort(RanIt first, RanIt last) {
	typedef typename iterator_traits<RanIt>::value_type T;
	ptrdiff_t length = last - first;
	RanIt temp(new T[length]);
	RanIt left;
	for (left = first; left < last - 32; left += 32) {
		RanIt next = left + 32;
		insertion_sort(left, next);
	}
	insertion_sort(left, last);
	bool flag = true;
	for (size_t size = 32; size < length; size <<= 1) {
		const RanIt right = flag ? last : temp + length;
		RanIt mid, left = flag ? first : temp, out_left = flag ? temp : first;
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
		for (RanIt t = temp; first < last; ++first, ++t)
			*first = *t;

	delete[] temp;
	return;
}

namespace detail {

template <typename RanIt>
void heap_sink(RanIt first, ptrdiff_t start, ptrdiff_t end) {
	ptrdiff_t parent = start, child = (parent << 1) + 1;
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

template <typename RanIt>
void heap_sort(RanIt first, RanIt last) {
	ptrdiff_t length = last - first;
	for (ptrdiff_t i = length / 2 - 1; i >= 0; --i)
		detail::heap_sink(first, i, length - 1);
	for (ptrdiff_t i = length - 1; i > 0; --i) {
		iter_swap(first, first + i);
		detail::heap_sink(first, 0, i - 1);
	}
}

template <typename RanIt>
inline void heap(RanIt first, RanIt last, RanIt offset) {
	RanIt parent = first, child = parent + (parent - offset) + 1;
	while (child <= last) {
		if (child < last && *child < *(child + 1))
			++child;
		if (*parent > *child)
			return;
		iter_swap(parent, child); //将一个三叉的最大值移至三叉顶
		parent = child;
		child = parent + (parent - offset) + 1;
	}
}

template <typename RanIt>
inline void heap_sortx(RanIt first, RanIt last) {
	int len = last - first;
	for (RanIt i = first + len / 2 - 1; i >= first; --i)
		heap(i, last - 1, first);
	for (RanIt i = last - 1; i > first; --i) {
		iter_swap(first, i);
		heap(first, i - 1, first);
	}
}

} // namespace ala

#endif