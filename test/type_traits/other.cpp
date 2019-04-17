#include <ala/type_traits.h>
using namespace ala;

template<class T>
struct Err {};

// template<class T>
// struct Err<T*> {
//     using type = ala::false_type;
// };

template<class T>
struct ErrNO;

template<typename T, bool = is_class<T>::value>
struct xx: false_type {
    decltype(++declval<ErrNO<T>>()) xxx;
};

template<typename T>
struct xx<T, true>: true_type {};

struct J;

template<class T>
using tt = T;

#include <iostream>
#include <type_traits>

struct A {
    int fun() const &;
};

template<typename>
struct PM_traits {};

template<class T, class U>
struct PM_traits<U T::*> {
    using member_type = U;
};

int f();

template<typename...>
struct Fuck;


void vv() {
    std::cout << std::boolalpha;
    std::cout << std::is_function<A>::value << '\n';
    std::cout << std::is_function<int(int)>::value << '\n';
    std::cout << std::is_function<decltype(f)>::value << '\n';
    std::cout << std::is_function<int>::value << '\n';

    using T = PM_traits<decltype(&A::fun)>::member_type; // T 为 int() const&
    std::cout << std::is_function<T>::value << '\n';
    // Fuck<T> vt;
    static_assert(is_same_v<add_pointer_t<T>, std::add_pointer_t<T>>);
    Fuck<std::add_pointer_t<T>> fk;
}

int main() {
    vv();
    struct C {
        double Func(char, int &) noexcept;
    };
    static_assert(
        ala::is_same<ala::result_of_t<decltype (&C::Func)(C *, char, int &)>,
                     double>::value,
        "");
    static_assert(
        ala::is_nothrow_invocable<decltype(&C::Func), C **, char, int &>::value,
        "");

    static_assert(tt<xx<J>>::value);

    static_assert(ala::_or_<ala::true_type, Err<int>>::value);
    // static_assert(ala::_or_<ala::false_type, Err<int>>::value);
    static_assert(ala::disjunction<ala::true_type, Err<int>>::value);
    // static_assert(ala::disjunction<ala::false_type, Err<int>>::value);
    return 0;
}