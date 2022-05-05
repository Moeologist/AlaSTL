#ifndef _ALA_DETAIL_META_H
#define _ALA_DETAIL_META_H

#ifndef _ALA_DETAIL_TRAITS_DECLARE_H
    #error Never include this head directly, use ala/type_traits.h instead
#endif

namespace ala {
// clang-format off

// short-circuit evaluation
template<typename...> struct _or_lazy_;
template<> struct _or_lazy_<>: false_type {};
template<typename B1> struct _or_lazy_<B1>: B1 {};
template<typename B1, typename... Bs> struct _or_lazy_<B1, Bs...>: conditional_t<bool(B1::value), B1, _or_lazy_<Bs...>> {};

template<typename...> struct _and_lazy_;
template<> struct _and_lazy_<>: true_type {};
template<typename B1> struct _and_lazy_<B1>: B1 {};
template<typename B1, typename... Bs> struct _and_lazy_<B1, Bs...>: conditional_t<bool(B1::value), _and_lazy_<Bs...>, B1> {};

template<typename...Bools>
constexpr bool _or_constexpr_helper_(Bools...bs) {
#if 0
// #if _ALA_ENABLE_FOLD_EXPRESSIONS
    return (false || ... || bs);
#else
    bool a[] = {bs...};
    bool b = false;
    for (size_t i = 0; i < sizeof(a)/sizeof(bool);++i)
        b = b || a[i];
    return b;
#endif
}

template<typename...Bools>
constexpr bool _and_constexpr_helper_(Bools...bs) {
#if 0
// #if _ALA_ENABLE_FOLD_EXPRESSIONS
    return (true && ... && bs);
#else
    bool a[] = {bs...};
    bool b = true;
    for (size_t i = 0; i < sizeof(a)/sizeof(bool);++i)
        b = b && a[i];
    return b;
#endif
}

// non-short-circuit evaluation
template<typename... Bools>
struct _or_constexpr_: bool_constant<_or_constexpr_helper_(Bools::value...)> {};

template<typename... Bools>
struct _and_constexpr_: bool_constant<_and_constexpr_helper_(Bools::value...)> {};

template<bool, typename... Bools>
struct _or_helper_: _or_constexpr_<Bools...> {};

template<typename... Bools>
struct _or_helper_<true, Bools...>: _or_lazy_<Bools...> {};

template<bool, typename... Bools>
struct _and_helper_: _and_constexpr_<Bools...> {};

template<typename... Bools>
struct _and_helper_<true, Bools...>: _and_lazy_<Bools...> {};

template<typename... Bools>
struct _or_: _or_helper_<(sizeof...(Bools)<ALA_TEMPLATE_RECURSIVE_DEPTH), Bools...> {};

template<typename... Bools>
struct _and_: _and_helper_<(sizeof...(Bools)<ALA_TEMPLATE_RECURSIVE_DEPTH), Bools...> {};

// template<typename... Bools>
// struct _or_: _or_lazy_<Bools...> {};

// template<typename... Bools>
// struct _and_: _and_lazy_<Bools...> {};

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

template<typename From, typename To>
using _copy_cvref_t = typename _get_cvref<From>::template rebind<To>;

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

#if _ALA_ENABLE_INT128T
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

template<template<class...> class Templt, class T1>
struct _meta_reduce_impl<Templt, T1>: T1 {};

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

// template<typename VT>
// struct _vtype_traits {};

// template<typename T, T v, template<typename, T> class Templt>
// struct _vtype_traits<Templt<T, v>> {
//     template<typename T1, T1 v1>
//     using rebind = Templt<T1, v1>;

//     template<T v1>
//     using rebind_val = Templt<T, v1>;
// };

// template<class T1, class T2>
// struct _max_
//     : _vtype_traits<T1>::template rebind<
//           common_type_t<typename T1::value_type, typename T2::value_type>,
//           (T1::value < T2::value ? T2::value : T1::value)> {};

// template<class T1, class T2>
// struct _min_
//     : _vtype_traits<T1>::template rebind<
//           common_type_t<typename T1::value_type, typename T2::value_type>,
//           (T1::value < T2::value ? T1::value : T2::value)> {};

// template<class T1, class T2>
// struct _add_: _vtype_traits<T1>::template rebind<decltype(T1::value + T2::value),
//                                                  T1::value + T2::value> {};

// template<class T1, class T2>
// struct _mul_: _vtype_traits<T1>::template rebind<decltype(T1::value * T2::value),
//                                                  T1::value * T2::value> {};

template<class T1, class T2>
struct _max_
    : integral_constant<common_type_t<typename T1::value_type, typename T2::value_type>,
                        (T1::value < T2::value ? T2::value : T1::value)> {};

template<class T1, class T2>
struct _min_
    : integral_constant<common_type_t<typename T1::value_type, typename T2::value_type>,
                        (T1::value < T2::value ? T1::value : T2::value)> {};

template<class T1, class T2>
struct _add_
    : integral_constant<decltype(T1::value + T2::value), T1::value + T2::value> {
};

template<class T1, class T2>
struct _mul_
    : integral_constant<decltype(T1::value * T2::value), T1::value * T2::value> {
};

template<class... Ts>
struct _maximal_: _meta_reduce<_max_, Ts...> {};

template<class... Ts>
struct _minimal_: _meta_reduce<_min_, Ts...> {};

template<class... Ts>
struct _sum_: _meta_reduce<_add_, Ts...> {};

template<class... Ts>
struct _prod_: _meta_reduce<_mul_, Ts...> {};

template<class T, class...>
using _sfinae_checker = T;

}; // namespace ala

#endif