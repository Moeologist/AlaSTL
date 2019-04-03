// #include <iostream>
#include <ala/tuple.h>
#include <tuple>
// #include <iostream>
// #include <string>
// #include <stdexcept>
// #include <cassert>

template<typename...>
struct FK;

//@cflags=-stdlib=libc++
int main() {
    static_assert(ala::get<double>(ala::tuple{1, 2, 1.0}) == 1);
    static_assert(!(ala::tuple{1, 2} == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2} == ala::tuple{1, 2, 1}));
    static_assert(ala::tuple{1, 2} < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{1, 2}));
    static_assert(ala::tuple{1, 2, 1} < ala::tuple{1, 2});
    static_assert(ala::tuple{1, 2, 1.0} == ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{1, 2, 1.0} < ala::tuple{1, 2, 1}));
    static_assert(ala::tuple{} < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{} < ala::tuple{}));
    // static_assert(ala::tuple() == ala::tuple{});
    static_assert(!(ala::tuple{} == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{}));
    static_assert(ala::tuple_cat(ala::tuple{1, 2}, ala::tuple{3, 3.0}) ==
                  ala::tuple{1.0, 2, 3, 3});
    ala::tuple_cat(ala::tuple(1), ala::tuple(2));

    // FK<ala::_tuple_cat_t<ala::tuple<float&&>, ala::tuple<int>>> pp;
    // FK<decltype(ala::tuple_cat(ala::tuple(1), ala::tuple(2)))> fk;
    // FK<decltype(std::tuple_cat(std::tuple(1), std::tuple(2)))> fk1;
    // FK<decltype(ala::tuple(1))> fk2;
    // FK<decltype(ala::tuple<float &, char &&, int>(x, ala::move(y), z))> fk;

    static_assert(ala::tuple_size<ala::tuple<float &, char &&, int>>::value == 3);
    static_assert(
        ala::tuple_size<const ala::tuple<float &, char &&, int>>::value == 3);
    static_assert(ala::tuple_size<ala::tuple<float &, char &&, int>>::value == 3);
    int x;
    ala::tie(ala::ignore, x) = ala::tuple(1, 3);
    assert(x == 3);

    const auto &[a, b] = ala::tuple<float, char>(1.2, 'c');

    struct A {
        A() = delete;
        A(int){};
        A(const A &) {
            int i;
        };
    };
    struct B {};

    ala::tuple<A, B> tpp{0, {}};

    // ala::tuple fsdkj{ala::tuple{1, 2}};
    auto fff = ala::get<int>(ala::pair<int, double>());
    ala::tuple fsdkj{ala::pair{1,2}};

    auto cccc = ala::choice<ala::is_class>(tpp);
    // FK<decltype(cccc)> xyz;
}