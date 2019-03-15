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


int main() {
    struct C {
        double Func(char, int &) noexcept;
    };
    static_assert(ala::is_same<ala::result_of_t<decltype(&C::Func)(C*, char, int &)>, double>::value, "");
    static_assert(ala::is_nothrow_invocable<decltype(&C::Func), C**, char, int &>::value, "");

    static_assert(tt<xx<J>>::value);

    static_assert(ala::_or_<ala::true_type, Err<int>>::value);
    // static_assert(ala::_or_<ala::false_type, Err<int>>::value);
    static_assert(ala::disjunction<ala::true_type, Err<int>>::value);
    // static_assert(ala::disjunction<ala::false_type, Err<int>>::value);
    return 0;
}