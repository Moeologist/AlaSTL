#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

#include "ala/random.h"
#include "ala/timer.h"
#include "ala/utility.h"
#include "ala/algorithm.h"

// #include <EASTL/sort.h>
// #include <tbb/parallel_sort.h>

#ifdef _MSC_VER
#include <ppl.h>
#endif

using namespace ala;

template<typename _F>
struct function_traits {};

template<typename _Ret, typename _Arg>
struct function_traits<_Ret (*)(_Arg)> {
    typedef _Ret result_type;
    typedef _Arg params_type;
};

template<typename _Ret, typename _Arg>
struct function_traits<_Ret (*)(_Arg, _Arg)> {
    typedef _Ret result_type;
    typedef _Arg params_type;
};

template<typename...>
class FK;

template<class function>
auto test_c_array(function f, int N) { //编译器推导T
    typedef remove_cvref_t<typename function_traits<function>::params_type> It;
    typedef typename iterator_traits<It>::value_type T;

    It a_ptr = new T[N];
    It a = a_ptr, b = a_ptr + N;
    xoshiro128p gen = {4564};
    std::generate(a, b, [&]() { return gen(); });

    std::sort(a, b, std::less());

    timer(f, a, b);
    auto dTime = timer(f, a, b);

    assert(std::is_sorted(a, b));
    delete[] a_ptr;
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
//@build_type=rel

void test() {
    constexpr auto N = 10000000;
    int *a_ptr = new int[N];
    int *a = a_ptr, *b = a_ptr + N;
    xoshiro128p gen = {4564};
    std::generate(a, b, [&]() { return gen(); });
    std::cout << timer([&]() {
        bool ret;
        for (int i = 0; i < 100000000; ++i)
            ret = ala::next_permutation(a, a + N);
        assert(a[0] != 1);
    }) << std::endl;
    delete[] a_ptr;
}

int main() {
    test();
    typedef int *pint;
    static_assert(is_same<const pint, int *const>::value);
    static_assert(is_same<remove_pointer_t<const pint>, int>::value);
    static_assert(is_same<remove_pointer_t<const int *>, const int>::value);
    // static_assert(is_same<const pint, const int *>::value);
    // FK<typename common_reference<const int&&,  int&&>::type> t;
    // FK<typename common_reference<const int&,  int&&>::type> t1;
    // FK<typename common_reference<int&&,  int&&>::type> t2;
    // FK<typename common_reference<int&,  int&&>::type> t3;
    // FK<typename _simple_common_reference<int&,  int&&>::type> t33;
    // FK<typename common_reference<int&, const int&>::type> t4;

    int arr[] = {1, 2, 3, 4, 5};
    // ala::prev_permutation(arr, arr + 5);
    // bool r = true;
    // for (int i = 0; r; ++i) {
    //     r = ala::next_permutation(arr, arr + 5);
    //     for (auto it : arr)
    //         std::cout << it;
    //     std::cout << "\n";
    // }
    std::cout << "Select:" << test_c_array(select_sort<uint64_t*>, 10000)
              << std::endl;
    std::cout << "Insert:"
              << test_c_array(insertion_sort<uint64_t*>, 10000)
              << std::endl;
    // std::cout << "Shell:" << test_c_array(shell_sort<uint64_t*>, 10000000)
    //           << std::endl;

    std::cout << "Quick:" << test_c_array(quick_sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "Merge:" << test_c_array(merge_sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "Heap:" << test_c_array(heap_sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "Heap:" << test_c_array(heap_sortx<uint64_t *>, 10000000)
              << std::endl;

    std::cout << "Sort:" << test_c_array(sort<uint64_t *>, 10000000) << std::endl;
    std::cout << "StableSort:" << test_c_array(stable_sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "stdsort:" << test_c_array(std::sort<uint64_t *>, 10000000)
              << std::endl;
    // std::cout << "eastlsort:" << test_c_array(eastl::sort<uint64_t*>, 10000000)
    //           << std::endl;

    std::cout << "ParallelSort:"
              << test_c_array(parallel_sort<uint64_t *>, 10000000) << std::endl;
    std::cout << "ParallelQSort:"
              << test_c_array(parallel_quick_sort<uint64_t *>, 10000000)
              << std::endl;

    // std::cout << "TBBSort:" << test_c_array_tbb<uint64_t*>(10000000) << std::endl;
#ifdef _MSC_VER
    std::cout << "pplsort:"
              << test_c_array(concurrency::parallel_sort<uint64_t *>, 10000000)
              << std::endl;
#endif
    getchar();
    return 0;
}