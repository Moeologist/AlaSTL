#include <ala/array.h>
#include <ala/vector.h>
#include <array>
#include <ala/tuple.h>
#include <ala/detail/pair.h>
#include <ala/detail/allocator.h>
#include <iostream>

template<class...>
struct FK;

struct pod {
    pod &operator=(const pod &) = delete;
    pod &operator=(pod &&) {
        return *this;
    };
};

ala::tuple<int, int> pp() {
    return {1, 2};
}

template<class T>
struct D {
    T t;
};

template<class T>
using Tp = void;

// template<class T>
// using Tp<T&> = int;

template<class T>
inline constexpr auto Val = 0;

template<class T>
inline constexpr auto Val<T&> = 1;

int main() {
    // Tp<int&> i;
    auto qqp = Val<int&>;
    static_assert(ala::is_trivially_destructible_v<D<ala::vector<int>>>);
    using namespace ala;
    using ar = array<int, 2>;
    pp();

    tuple a(1); // unquestionably, tuple<int>
    // tuple b(a, a); // unquestionably, tuple<tuple<int>, tuple<int>>
    tuple c(a); // ??
    FK<decltype(c)> fk;

    // pair pr(1, 2);
    // auto pr1 = pair(1, 2);
    // pair pr2{1, 2};
    // auto pr3 = pair{1, 2};

    pair pr{1, 2};

    auto t1 = tuple<>(); // libstdc++ tuple<> 是 incomplete type, 即使不用libstdc++，gcc 也报错,
    tuple<> t2{};      // 非libstdc++可以写
    auto t3 = tuple{}; // 非libstdc++可以写

    // pr换成右值也一样，以下写法gcc全跪，任何标准库实现，推出空的tuple类型
    tuple tp{pr};
    tuple tp1(pr);
    auto tp2 = tuple(pr);
    auto tp3 = tuple{pr};

    ar as;
    allocator<int> alloc;
    auto p = alloc.allocate(0);
    static_assert(is_same_v<int, signed int>, "f**k");
    ala::copy(as.begin(), as.end(), as.begin());
    constexpr ar x = {};
    ar y = {1, 2};
    apply([](auto &&x, auto &&y) { return ++x + ++y; }, y);
    x.cbegin();
    x.crbegin();
    return 0;
}