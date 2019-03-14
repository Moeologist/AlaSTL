#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

#include "ala/detail/xorshift.h"
#include "ala/timer.h"
#include "ala/utility.h"
#include "ala/algorithm.h"

// #include <EASTL/sort.h>
// #include <tbb/parallel_sort.h>

#ifdef _MSC_VER
#include <ppl.h>
#endif

using namespace ala;

template <typename _It>
struct iterator_traits { using value_type = typename _It::value_type; };

template <typename _It>
struct iterator_traits<const _It> { using value_type = typename _It::value_type; };

template <typename _It>
struct iterator_traits<_It *> { using value_type = _It; };

template <typename _It>
struct iterator_traits<const _It *> { using value_type = _It; };

template <typename _F>
struct function_traits {};

template <typename _Ret, typename _Arg>
struct function_traits<_Ret (*)(_Arg)> {
	typedef _Ret result_type;
	typedef _Arg params_type;
};

template <typename _Ret, typename _Arg>
struct function_traits<_Ret (*)(_Arg, _Arg)> {
	typedef _Ret result_type;
	typedef _Arg params_type;
};

struct C {
	int a;
	// int mem[100];
	C(unsigned int a_) : a(a_) {}
	C() : a(0) {}

	bool operator<(const C &rhs) const {
		return this->a < rhs.a;
	}
	bool operator>(const C &rhs) const {
		return this->a > rhs.a;
	}
	bool operator==(const C &rhs) const {
		return this->a == rhs.a;
	}

	struct iterator_type {
		C *ptr;
		typedef C value_type;

		iterator_type(C *p) : ptr(p) {}
		iterator_type() : ptr(nullptr) {}
		iterator_type &operator++() {
			++ptr;
			return *this;
		}
		iterator_type operator++(int) {
			iterator_type temp(ptr);
			++ptr;
			return temp;
		}
		bool operator==(const iterator_type &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const iterator_type &rhs) const {
			return ptr != rhs.ptr;
		}
		C &operator*() const {
			return *ptr;
		}
	};
	struct biterator_type {
		C *ptr;
		typedef C value_type;

		biterator_type(C *p) : ptr(p) {}
		biterator_type() : ptr(nullptr) {}
		biterator_type &operator++() {
			++ptr;
			return *this;
		}
		biterator_type operator++(int) {
			biterator_type temp(ptr);
			++ptr;
			return temp;
		}
		biterator_type &operator--() {
			--ptr;
			return *this;
		}
		bool operator==(const biterator_type &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const biterator_type &rhs) const {
			return ptr != rhs.ptr;
		}
		C &operator*() const {
			return *ptr;
		}
	};
	struct riterator_type {
		C *ptr;
		typedef C value_type;

		riterator_type(C *p) : ptr(p) {}
		riterator_type() : ptr(nullptr) {}
		riterator_type &operator++() {
			++ptr;
			return *this;
		}
		riterator_type operator++(int) {
			riterator_type temp(ptr);
			++ptr;
			return temp;
		}
		C operator [](ptrdiff_t offset){
			return ptr[offset];
		}
		riterator_type &operator--() {
			--ptr;
			return *this;
		}
		riterator_type operator+(const ptrdiff_t dif) {
			return riterator_type(ptr + dif);
		}
		riterator_type operator-(const ptrdiff_t dif) {
			return riterator_type(ptr - dif);
		}
		ptrdiff_t operator-(const riterator_type &rhs) const {
			return ptr - rhs.ptr;
		}
		// riterator_type &operator+=(const ptrdiff_t dif) {
		// 	ptr += dif;
		// 	return *this;
		// }
		// riterator_type &operator-=(const ptrdiff_t dif){
		// 	ptr -= dif;
		// 	return *this;
		// }
		bool operator>(const riterator_type &rhs) const {
			return ptr > rhs.ptr;
		}
		bool operator<(const riterator_type &rhs) const {
			return ptr < rhs.ptr;
		}
		bool operator>=(const riterator_type &rhs) const {
			return ptr >= rhs.ptr;
		}
		bool operator<=(const riterator_type &rhs) const {
			return ptr <= rhs.ptr;
		}
		bool operator==(const riterator_type &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const riterator_type &rhs) const {
			return ptr != rhs.ptr;
		}
		C &operator*() const {
			return *ptr;
		}
	};
};

template <class function, class T = int>
auto test_vector(function f, int N) { //编译器推导T
	std::vector<int> v(N, 0);

	auto gen = xoroshiro128plus(4564);
	std::generate(v.begin(), v.end(), gen);

	auto dTime = timer(f, v.begin(), v.end());

	if (!std::all_of(v.begin(), v.end() - 1, [](T &x) { return x <= *(&x + 1); }))
		std::cout << "buggy sort vec!\n";

	return dTime;
}

template <class function>
auto test_c_array(function f, int N) { //编译器推导T
	typedef typename remove_cv<typename remove_reference<typename function_traits<function>::params_type>::type>::type It;
	typedef typename iterator_traits<It>::value_type T;

	T *a_ptr = new T[N];
	It a = a_ptr, b = a_ptr + N;
	auto gen = xoroshiro128plus(4564);
	std::generate(a, b, [&]() { return gen(); });

	T *ar_ptr = new T[N];
	It ar = ar_ptr, br = ar_ptr + N;
	auto genr = xoroshiro128plus(4564);
	std::generate(ar, br, [&]() { return genr(); });

	std::sort(ar_ptr, ar_ptr + N);

	auto dTime = timer(f, a, b);

	for (It i = a, bound = a, ir = ar; bound != b; ++i, ++ir) {
		if (!(*i == *ir)) {
			std::cout << "buggy sort array!\n";
			break;
		}
		bound = i;
		bound++;
	}
	// for (It i = a, next; next != b; ++i) {
	// 	next = i;
	// 	++next;
	// 	if (!(*i < *next || *i == *next)) {
	// 		std::cout << "buggy sort array!\n";
	// 		break;
	// 	}
	// 	++next;
	// }
	delete[] a_ptr;
	delete[] ar_ptr;
	return dTime;
}

// template <class It>
// auto test_c_array_tbb(int N) { //编译器推导T
// 	typedef typename iterator_traits<It>::value_type T;

// 	T *a_ptr = new T[N];
// 	It a = a_ptr, b = a_ptr + N;
// 	auto gen = xoroshiro128plus(4564);
// 	std::generate(a, b, [&]() { return gen(); });

// 	auto dTime = timer([=](){tbb::parallel_sort(a, b);});

// 	delete[] a_ptr;
// 	return dTime;
// }

int main() {
	// std::cout << "Select:" << test_c_array(select_sort<C::iterator_type>, 10000) << std::endl;
	// std::cout << "Insert:" << test_c_array(insertion_sort<C::biterator_type>, 10000) << std::endl;
	// std::cout << "Shell:" << test_c_array(shell_sort<C::riterator_type>, 10000) << std::endl;

	// std::cout << "Quick:" << test_c_array(quick_sort<C::riterator_type>, 10000000) << std::endl;
	// std::cout << "Merge:" << test_c_array(merge_sort<C::riterator_type>, 10000000) << std::endl;
	std::cout << "Heap:" << test_c_array(heap_sort<C::riterator_type>, 10000000) << std::endl;
	std::cout << "Heap:" << test_c_array(heap_sortx<C::riterator_type>, 10000000) << std::endl;

	std::cout << "Sort:" << test_c_array(sort<C *>, 10000000) << std::endl;
	// std::cout << "StableSort:" << test_c_array(stable_sort<C *>,10000000) << std::endl;
	// std::cout << "stdsort:" << test_c_array(std::sort<C *>, 10000000) << std::endl;
	// std::cout << "eastlsort:" << test_c_array(eastl::sort<C *>, 10000000) << std::endl;

	std::cout << "ParallelSort:" << test_c_array(parallel_sort<C *>, 100000000) << std::endl;
	std::cout << "ParallelQSort:" << test_c_array(parallel_quick_sort<C *>, 10000000) << std::endl;

	// std::cout << "TBBSort:" << test_c_array_tbb<C *>(10000000) << std::endl;
#ifdef _MSC_VER
	std::cout << "pplsort:" << test_c_array(concurrency::parallel_sort<C *>, 10000000) << std::endl;
#endif
	getchar();
	return 0;
}