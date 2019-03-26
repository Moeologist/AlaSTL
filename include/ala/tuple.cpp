// #include <iostream>
#include <ala/tuple.h>
#include <tuple>
// #include <iostream>
// #include <string>
// #include <stdexcept>
// #include <cassert>

namespace std {

template<typename Head, typename... Tail>
struct tuple_element<0, ala::tuple<Head, Tail...>> {
    typedef Head type;
};

template<size_t I, typename Head, typename... Tail>
struct tuple_element<I, ala::tuple<Head, Tail...>>
    : tuple_element<I - 1, ala::tuple<Tail...>> {
    static_assert(I <= sizeof...(Tail), "out of range");
};

template<typename... Ts>
struct tuple_size<ala::tuple<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

} // namespace std

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
    static_assert(ala::tuple() < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple() < ala::tuple{}));
    static_assert(ala::tuple() == ala::tuple{});
    static_assert(!(ala::tuple() == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{}));
    float x{};
    char y{};
    int z{};
    const auto &[a, b, c] = ala::tuple<float &, char &&, int>(x, ala::move(y), z);
}