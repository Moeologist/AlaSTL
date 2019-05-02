#include <ala/type_traits.h>
#include <ala/detail/integer_sequence.h>
#include <iostream>
using namespace ala;
template<class T> struct FK {static constexpr bool value = false;
static_assert(is_array_v<T>);};

class XX;

template<typename T>
struct tuple_element : false_type {};

template<>
struct tuple_element<XX> : true_type {};

template<typename X, bool b= _or_<true_type, FK<X>>::value>
struct z : false_type {};

template<typename X>
struct z<X, true> : true_type {};

int main() {
    static_assert(tuple_element<XX>::value);
    static_assert(is_same_v<make_integer_sequence<int, 3>, integer_sequence<int, 0, 1, 2>>);
    static_assert(is_same_v<make_integer_range<int, 3, 5>, integer_sequence<int, 3, 4>>);

    static_assert(is_same_v<get_integer_sequence<1, make_integer_range<int, 3, 5>>, integral_constant<int, 4>>);

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
    static_assert(is_same_v<reverse_integer_sequence<make_integer_range<unsigned, 10, 2, 3>>, integer_sequence<unsigned, 4, 7, 10>>);
    // static_assert(is_same_v<reverse_range<make_integer_range<unsigned, 10, 2, 3>>, integer_sequence<unsigned, 10, 7, 4>>);
    // FK<reverse_range<make_integer_range<unsigned, 10, 2, 3>>> fk;

    static_assert(z<int>::value);
    std::cout << (unsigned)9 % (unsigned)3;
}