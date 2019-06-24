#include "h.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <cmath>
#include <ala/type_traits.h>

#if defined(__GNUC__) && !defined(__clang__)
#define no_opt optimize("O0")
#elif defined(__clang__)
#define no_opt optnone
#else
#warning "in_constexpr not tested/supported on different compilers"
#define no_opt
#endif

#if defined(CUSTOM_IS_CONSTEXPR_FLAG)
constexpr uint32_t IS_CONSTEXPR_FLAG = CUSTOM_IS_CONSTEXPR_FLAG;
#else
constexpr uint32_t IS_CONSTEXPR_FLAG = 0x5EEEEEFF;
#endif

template<typename T>
__attribute__((no_opt)) constexpr auto in_constexpr_impl(T) {
    return IS_CONSTEXPR_FLAG;
}

#define _CAT(x, y) x##y

#if __cplusplus >= 201703L
// A nicer C++17 approach
#define in_constexpr() \
    int _CAT(__unused, __LINE__) = 0; \
    __builtin_expect(in_constexpr_impl(_CAT(__unused, __LINE__)), 0)
#else
// A C++14 approach
#define in_constexpr()  bool _CAT(canary, __LINE__) = true) { \
        (void)_CAT(canary, __LINE__); \
    } \
    int _CAT(__unused, __LINE__) = 0; \
  if ( __builtin_expect(in_constexpr_impl(_CAT(__unused, __LINE__)), 0)
#endif

template<class T>
constexpr T _static_sqrt(T x) {
    T t = (x + T(1)) / 2;
    for (int i = 0; i < 8; ++i)
        t = t / 2 + x / t / 2;
    return t;
}

// template<class T>
// constexpr T _static_sqrt(T x) {
//     T xhalf = 0.5 * x;
//     x = 1;
//     for (int i = 0; i < 8; ++i)
//         x = x * (1.5 - xhalf * x * x);
//     return (1 / x);
// }

template<class T>
struct _Dummy {
    T _v;
    constexpr _Dummy(const T &t): _v(t) {}
};

template<int>
struct _try_constant_evaluate {};

// template<class T>
// constexpr auto static_sqrt(T x) -> decltype(_static_sqrt(x)) {
//     return _static_sqrt(x);
// }

template<class T>
constexpr auto static_sqrt(T x) {
    int _unused = 0;
    if (in_constexpr_impl(_unused))
        return _static_sqrt(x);
    else
        return ::sqrt(x);
}

int main() {
    constexpr double f = static_sqrt(0.01);
    double f1 = static_sqrt(0.01);
    // std::cin >> f1;

    std::cout
        << std::setprecision(128) << f << "\n"
        << f1 << "\n"
        << sqrt(0.01) << "\n"
        << "1.414213562373095048801688724209698078569671875376948073176...";
}