// #include <iostream>
#include <ala/tuple.h>
#include <tuple>
#include <iostream>
// #include <string>
// #include <stdexcept>
// #include <cassert>

template<typename...>
struct FK;

//@cflags=-Xclang -stdlib=libc++

struct X {
    int x = 1;
    ~X() {
        x = 0;
        std::cout << "dest";
    }
    X() {
        x = 1;
        std::cout << "cons";
    }
    X(const X &) {
        std::cout << "copy";
    }
    X(X &&) {
        std::cout << "move";
    }
};
int main() {
    static_assert(ala::get<double>(ala::tuple{1, 2, 1.0}) == 1);
    ala::tuple lv{1., 2};
    lv = ala::tuple{100, 2};
    static_assert(!(ala::tuple{1, 2} == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2} == ala::tuple{1, 2, 1}));
    static_assert(ala::tuple{1, 2} < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{1, 2}));
    static_assert(ala::tuple{1, 2, 1} < ala::tuple{1, 2});
    static_assert(ala::tuple{1, 2, 1.0} == ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{1, 2, 1.0} < ala::tuple{1, 2, 1}));
    static_assert(ala::tuple{} < ala::tuple{1, 2, 1});
    static_assert(!(ala::tuple{} < ala::tuple{}));
    static_assert(ala::tuple() == ala::tuple{});
    static_assert(!(ala::tuple{} == ala::tuple{1, 2, 1}));
    static_assert(!(ala::tuple{1, 2, 1} == ala::tuple{}));
    static_assert(ala::tuple_cat(ala::tuple{1, 2}, ala::tuple{3, 3.0}) ==
                  ala::tuple{1.0, 2, 3, 3});
    ala::tuple_cat(ala::tuple(1), ala::tuple(2));

    // FK<ala::_tuple_cat_t<ala::tuple<float&&>, ala::tuple<int>>> pp;
    int a1 = 111, a2 = 2;
    // FK<decltype(ala::tuple_cat(ala::tuple<int&, int&>(a1, a2), ala::tuple<int&>(a2)))> fk;
    auto cat = ala::tuple_cat(ala::tuple<int>(1),
                              ala::tuple<const int &, int &, int &>(a1, a2, a2),
                              ala::tuple<int &, const X &&>(a2, X()));
    auto con = ala::choice<ala::is_reference>(cat);
    // FK<decltype(cat)> fffffff;
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
    ala::tuple fsdkj{ala::pair{1, 2}};

    auto cccc = ala::choice<ala::is_class>(tpp);
    // FK<decltype(cccc)> xyz;
}