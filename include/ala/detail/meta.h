#ifndef _ALA_DETAIL_META_H
#define _ALA_DETAIL_META_H

#ifndef _ALA_DETAIL_TRAITS_DECLARE_H
    #error Never include this head directly, use ala/type_traits.h instead
#endif

namespace ala {
// clang-format off
template<typename...> struct _or_;
template<> struct _or_<>: false_type {};
template<typename B1> struct _or_<B1>: B1 {};
template<typename B1, typename... Bs> struct _or_<B1, Bs...>: conditional_t<bool(B1::value), B1, _or_<Bs...>> {};

template<typename...> struct _and_;
template<> struct _and_<>: true_type {};
template<typename B1> struct _and_<B1>: B1 {};
template<typename B1, typename... Bs> struct _and_<B1, Bs...>: conditional_t<bool(B1::value), _and_<Bs...>, B1> {};

template<typename B>
struct _not_: bool_constant<!bool(B::value)> {};

template<typename T, bool = is_const<T>::value,
                     bool = is_volatile<T>::value>
struct _get_cv;

template<typename T> struct _get_cv<T, false, false> { template<typename U> using rebind = U; };
template<typename T> struct _get_cv<T, true,  false> { template<typename U> using rebind = const U; };
template<typename T> struct _get_cv<T, false, true>  { template<typename U> using rebind = volatile U; };
template<typename T> struct _get_cv<T, true,  true>  { template<typename U> using rebind = const volatile U; };

template<typename T, bool = is_lvalue_reference<T>::value,
                     bool = is_rvalue_reference<T>::value>
struct _get_ref;

template<typename T> struct _get_ref<T, false, false> { template<typename U> using rebind = U; };
template<typename T> struct _get_ref<T, true, false>  { template<typename U> using rebind = U&; };
template<typename T> struct _get_ref<T, false, true>  { template<typename U> using rebind = U&&; };

template<typename T> struct _get_cvref {
    template<typename U> using rebind =
    typename _get_ref<T>::template rebind<
        typename _get_cv<remove_reference_t<T>>::template rebind<U>>;
};

template<typename From, typename To>
using _copy_cv_t = typename _get_cv<From>::template rebind<To>;

constexpr int                _convert_to_integral(char val)               { return val; }
constexpr unsigned           _convert_to_integral(unsigned char val)      { return val; }
constexpr unsigned           _convert_to_integral(signed char val)        { return val; }
constexpr int                _convert_to_integral(short val)              { return val; }
constexpr unsigned           _convert_to_integral(unsigned short val)     { return val; }
constexpr int                _convert_to_integral(int val)                { return val; }
constexpr unsigned           _convert_to_integral(unsigned int val)       { return val; }
constexpr long               _convert_to_integral(long val)               { return val; }
constexpr unsigned long      _convert_to_integral(unsigned long val)      { return val; }
constexpr long long          _convert_to_integral(long long val)          { return val; }
constexpr unsigned long long _convert_to_integral(unsigned long long val) { return val; }

#ifdef _ALA_INT128
constexpr __int128_t  _convert_to_integral(__int128_t val)  { return val; }
constexpr __uint128_t _convert_to_integral(__uint128_t val) { return val; }
#endif

template<typename Float>
constexpr enable_if_t<is_floating_point<Float>::value, long long>
_convert_to_integral(Float val) {
    return val;
}

// clang-format on

template<template<class...> class Templt, class... Ts>
struct _meta_reduce_impl {};
template<template<class...> class Templt, class B1>
struct _meta_reduce_impl<Templt, B1>: B1 {};
template<template<class...> class Templt, class T1, class T2, class... Rest>
struct _meta_reduce_impl<Templt, T1, T2, Rest...>
    : _meta_reduce_impl<Templt, typename Templt<T1, T2>::type, Rest...> {};

template<class Void, template<class...> class Templt, class... Ts>
struct _meta_reduce_sfinae {};

template<template<class...> class Templt, class... Ts>
struct _meta_reduce_sfinae<void_t<_meta_reduce_impl<Templt, Ts...>>, Templt, Ts...>
    : _meta_reduce_impl<Templt, Ts...> {};

template<template<class...> class Templt, class... Ts>
struct _meta_reduce: _meta_reduce_sfinae<void, Templt, Ts...> {};

template<class T1, class T2>
struct _max_integral_constant
    : integral_constant<common_type_t<typename T1::value_type, typename T2::value_type>,
                        (T1::value < T2::value ? T2::value : T1::value)> {};

template<class T1, class T2>
struct _min_integral_constant
    : integral_constant<common_type_t<typename T1::value_type, typename T2::value_type>,
                        (T1::value < T2::value ? T1::value : T2::value)> {};

template<class T1, class T2>
struct _add_integral_constant
    : integral_constant<decltype(T1::value + T2::value), T1::value + T2::value> {
};

template<class T1, class T2>
struct _mul_integral_constant
    : integral_constant<decltype(T1::value * T2::value), T1::value * T2::value> {
};

}; // namespace ala

#endif