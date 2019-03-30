// #include <iostream>
#include <ala/tuple.h>
#include <tuple>
// #include <iostream>
// #include <string>
// #include <stdexcept>
// #include <cassert>


template<typename ... >
struct FK;

int main() {
    ala::tuple tp{1, 2, 1.3};
    ala::tuple tp1{};
    ala::tuple<int, int> tp2{1.2, 2.3};
    tp = ala::tuple{1, 2, 1.1};
    tp = ala::tuple{1, 2, 1};
    tp == ala::tuple{1, 2, 1};
    static_assert(ala::choice<0, 1>(ala::tuple{1, 2, 1.0}) == ala::tuple{1, 2.0});
    static_assert(ala::get<double>(ala::tuple{1, 2, 1.0}) == 1);
    static_assert(ala::tuple_cat(ala::tuple{1, 2}, ala::tuple{3, 3.0}) ==
                  ala::tuple{1.0, 2, 3, 3});
    static_assert(!(ala::tuple{1, 2} == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2} == ala::tuple{1, 2, 1}));
    static_assert(ala::tuple{1, 2} < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{1, 2}));
    static_assert(ala::tuple{1, 2, 1} < ala::tuple{1, 2});
    static_assert(ala::tuple{1, 2, 1.0} == ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{1, 2, 1.0} < ala::tuple{1, 2, 1}));
    static_assert(ala::tuple{} < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{} < ala::tuple{}));
    static_assert(ala::tuple{} == ala::tuple{});
    static_assert(!(ala::tuple{} == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{}));
    float x{};
    char y{};
    int z{};
    // FK<decltype(ala::tuple<float &, char &&, int>(x, ala::move(y), z))> fk;
    static_assert(ala::tuple_size<ala::tuple<float &, char &&, int>>::value==3);
    // static_assert(std::tuple_size<const std::tuple<float &, char &&, int>>::value==3);
    // static_assert(std::tuple_size<std::tuple<float &, char &&, int>>::value==3);
    const auto &[a, b, c] = ala::tuple<float &, char &&, int>(x, ala::move(y), z);
}