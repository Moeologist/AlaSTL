#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <utility>

#include "ala/random.h"
#include "ala/timer.h"
#include "ala/utility.h"
#include "ala/algorithm.h"
#include "ala/map.h"

#include <EASTL/sort.h>
#include <tbb/parallel_sort.h>

#ifdef _MSC_VER
#include <ppl.h>
#endif

using namespace ala;

template<typename F, typename Ret, typename A, typename... Rest>
A helper(Ret (F::*)(A, Rest...));

template<typename F, typename Ret, typename A, typename... Rest>
A helper(Ret (F::*)(A, Rest...) const);

template<typename F>
struct first_argument {
    typedef decltype(helper(&F::operator())) type;
};

template<typename F>
struct function_traits {
    typedef typename first_argument<F>::type params_type;
};

template<typename Ret, typename Arg, typename... Args>
struct function_traits<Ret (&)(Arg, Args...)> {
    typedef Ret result_type;
    typedef Arg params_type;
};

template<typename Ret, typename Arg, typename... Args>
struct function_traits<Ret (*)(Arg, Args...)> {
    typedef Ret result_type;
    typedef Arg params_type;
};

template<typename Ret, typename Arg, typename... Args>
struct function_traits<Ret(Arg, Args...)>: true_type {
    typedef Ret result_type;
    typedef Arg params_type;
};

template<typename Ret, typename Arg, typename... Args>
struct function_traits<Ret(Arg, Args...) const>: true_type {
    typedef Ret result_type;
    typedef Arg params_type;
};

template<typename...>
class FK;

template<typename Ptr>
bool check(Ptr ptr, size_t N) {
    typedef remove_cvref_t<decltype(*ptr)> T;
    auto a = new T[N];
    auto b = a + N;
    xoshiro128p gen = {4564};
    std::generate(a, b, [&]() { return gen(); });
    std::sort(a, b);
    return std::equal(a, b, ptr);
}

template<class function>
auto test_c_array(function &&f, size_t N) { //编译器推导T
    typedef remove_cvref_t<typename function_traits<function>::params_type> It;
    typedef typename iterator_traits<It>::value_type T;

    It a = new T[N];
    It b = a + N;
    xoshiro128p gen = {4564};
    std::generate(a, b, [&]() { return gen(); });

    auto dTime = timer(f, a, b);

    assert(std::is_sorted(a, b));
    assert(check(a, N));
    delete[] a;
    return dTime;
}

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

//@build_type=rel
int bx[9];

int main() {
    int ax[] = {4, 8, 0, 2, 5, 6, 7, 8, 2};
    partial_sort_copy(ax, ax + 9, bx, bx + 4);
    stable_partition(ax, ax + 9, [](const auto &x) { return x > 5; });

    make_heap(ax, ax + 9);
    ax[0] = 9;
    auto it = is_heap_until(ax, ax + 9);
    // test();
    auto k = []() {};
    static_assert(is_class_v<decltype(k)>);
    constexpr auto N = 10000;
    int arr[N] = {};
    for (int i = 0; i < N; ++i)
        arr[i] = i;
    std::shuffle(arr, arr + N, std::minstd_rand());

    int ar[N] = {};
    for (int i = 0; i < N; ++i)
        ar[i] = i;
    std::shuffle(ar, ar + N, std::mt19937());

    for (int i = 0; i < N; ++i) {
        ala::nth_element(arr, arr + ar[i], arr + N);
        if (arr[ar[i]] != ar[i])
            std::cout << ar[i] << "\n";
    }

    std::cout << "partial_sort:" << []() {
        auto N = 10000000;
        auto a = new int[N];
        auto b = a + N;
        xoshiro128p gen = {4564};
        std::generate(a, b, [&]() { return gen(); });

        auto dTime = timer([&]() { partial_sort(a, a + 1000000, b); });

        assert(std::is_sorted(a, a + 1000000));
        assert(!std::is_sorted(a + 1000000, b));
        delete[] a;
        return dTime;
    }() << std::endl;

    std::cout << "partial_sort_copy:" << []() {
        auto N = 10000000;
        auto a = new int[N];
        auto z = new int[N];
        auto b = a + N;
        xoshiro128p gen = {4564};
        std::generate(a, b, [&]() { return gen(); });

        auto dTime = timer([&]() { partial_sort_copy(a, b, z, z + 1000000); });

        assert(std::is_sorted(z, z + 1000000));
        delete[] a;
        return dTime;
    }() << std::endl;
    std::cout << "Merge:" << test_c_array(merge_sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "Sort:" << test_c_array(sort<uint64_t *>, 10000000) << std::endl;
    std::cout << "Stable:" << test_c_array(stable_sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "Select:" << test_c_array(select_sort<uint64_t *>, 100000)
              << std::endl;
    std::cout << "Insert:" << test_c_array(insertion_sort<uint64_t *>, 100000)
              << std::endl;
    std::cout << "Shell:" << test_c_array(shell_sort<uint64_t *>, 10000000)
              << std::endl;

    std::cout << "Quick:" << test_c_array(quick_sort<uint64_t *>, 10000000)
              << std::endl;

    std::cout << "Heap:" << test_c_array(heap_sort<uint64_t *>, 10000000)
              << std::endl;

    std::cout << "stdsort:" << test_c_array(std::sort<uint64_t *>, 10000000)
              << std::endl;
    std::cout << "eastlsort:" << test_c_array(eastl::sort<uint64_t *>, 10000000)
              << std::endl;

    std::cout << "ParallelSort:"
              << test_c_array(parallel_sort<uint64_t *>, 10000000) << std::endl;
    std::cout << "ParallelQSort:"
              << test_c_array(parallel_quick_sort<uint64_t *>, 10000000)
              << std::endl;

    std::cout
        << "TBBSort:"
        << test_c_array([](uint64_t *first,
                           uint64_t *last) { tbb::parallel_sort(first, last); },
                        10000000)
        << std::endl;
#ifdef _MSC_VER
    std::cout << "pplsort:"
              << test_c_array(concurrency::parallel_sort<uint64_t *>, 10000000)
              << std::endl;
#endif
    // getchar();
    return 0;
}