#include <ala/type_traits.h>
#include <ala/detail/integer_sequence.h>
#include <iostream>
using namespace ala;

template<class...>
struct FK;

int main() {

    // FK<make_integer_range<int, numeric_limits<int>::max(), numeric_limits<int>::max()-10, 1>> fk1;
    // FK<make_integer_range<int, -10, numeric_limits<int>::max(), 1>> fk1;
    // FK<reverse_range<make_integer_range<unsigned, 10, 2, 3>>> fk;
    static_assert(!is_same_v<make_integer_range<int, 0, 100000>, integer_sequence<int>>);
    // static_assert(make_integer_range<int, 0, 100000>::get(10086)==10086);
    static_assert(is_same_v<make_integer_sequence<int, 3>, integer_sequence<int, 0, 1, 2>>);
    static_assert(is_same_v<make_integer_range<int, 3, 5>, integer_sequence<int, 3, 4>>);
    static_assert(is_same_v<make_integer_range<int, 0, 1000, 500>, integer_sequence<int, 0, 500>>);
    static_assert(is_same_v<make_integer_range<int, 0, 1, 500>, integer_sequence<int, 0>>);
    static_assert(is_same_v<make_integer_range<int, 0, 0>, integer_sequence<int>>);
    static_assert(is_same_v<cat_integer_sequence<make_integer_range<int, 3, 5>, make_integer_range<int, 3, 5>>, integer_sequence<int, 3, 4, 3,4>>);
    static_assert(is_same_v<make_integer_range<int, 3, 4>, integer_sequence<int, 3>>);
    static_assert(is_same_v<make_integer_range<int, 3, 5, 3>, integer_sequence<int, 3>>);
    static_assert(is_same_v<make_integer_range<int, 3, 3>, integer_sequence<int>>);
    static_assert(is_same_v<make_integer_range<int, 5, 3>, integer_sequence<int, 5, 4>>);
    static_assert(is_same_v<make_integer_range<int, 5, 2>, integer_sequence<int, 5, 4, 3>>);
    static_assert(is_same_v<make_integer_range<int, 10, 2, 3>, integer_sequence<int, 10, 7, 4>>);
    static_assert(is_same_v<make_integer_range<int, 0, 30, 17>, integer_sequence<int, 0, 17>>);
    static_assert(is_same_v<make_integer_range<int, 10, 0, 3>, integer_sequence<int, 10, 7, 4, 1>>);
    static_assert(is_same_v<make_integer_range<int, -5, -8, 2>, integer_sequence<int, -5, -7>>);
    static_assert(is_same_v<make_integer_range<int, -5, -8, 3>, integer_sequence<int, -5>>);
    static_assert(is_same_v<make_integer_range<int, -8, -8, 3>, integer_sequence<int>>);
    static_assert(is_same_v<make_integer_range<int, -4, 4, 3>, integer_sequence<int, -4, -1, 2>>);
    static_assert(is_same_v<make_integer_range<unsigned, 10, 2, 3>, integer_sequence<unsigned, 10, 7, 4>>);
    static_assert(is_same_v<make_integer_range<unsigned, 6, 0, 3>, integer_sequence<unsigned, 6, 3>>);
}