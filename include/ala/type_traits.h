// surfix: _impl - aux class, _helper - aux base class, _test - aux function
#ifndef _ALA_TYPE_TRAITS_H
#define _ALA_TYPE_TRAITS_H

#ifdef _ALA_MSVC
#pragma warning(push)
#pragma warning(disable : 4180)
#pragma warning(disable : 4197)
#endif

#include <ala/detail/traits_declare.h>

// clang-format off

namespace ala {

template<typename T>
struct _is_implicitly_default_constructible_impl {
    template<typename T1>
    static void _help(const T1 &);

    template<typename T1, typename = decltype(_help<const T1 &>({}))>
    static true_type _test(int);

    template<typename T1>
    static false_type _test(...);

    typedef decltype(_test<T>(0)) type;
};

template<typename T, bool>
struct _is_implicitly_default_constructible_helper: false_type {};

template<typename T>
struct _is_implicitly_default_constructible_helper<T, true>
    : _is_implicitly_default_constructible_impl<T>::type {};

template<typename T>
struct is_implicitly_default_constructible
    : _is_implicitly_default_constructible_helper<T, is_default_constructible<T>::value> {};

template<typename T>
constexpr ALA_VAR_INLINE bool is_implicitly_default_constructible_v =
    is_implicitly_default_constructible<T>::value;

template<typename T>
struct _class_of_memptr;

template<typename Class, typename Ret>
struct _class_of_memptr<Ret Class::*> { typedef Class type; };

template<typename T>
using _class_of_memptr_t = typename _class_of_memptr<T>::type;

template<typename Type, template<typename...> class Template>
struct is_specification : false_type {};

template<template<typename...> class Template, typename... TArgs>
struct is_specification<Template<TArgs...>, Template> : true_type {};

template<typename T>
struct reference_wrapper;

#include <ala/detail/utility_inc.h>

template<typename...> struct _or_;
template<> struct _or_<> : false_type {};
template<typename B1> struct _or_<B1> : B1 {};
template<typename B1, typename... B> struct _or_<B1, B...> : conditional_t<bool(B1::value), B1, _or_<B...>> {};

template<typename...> struct _and_;
template<> struct _and_<> : true_type {};
template<typename B1> struct _and_<B1> : B1 {};
template<typename B1, typename... B> struct _and_<B1, B...> : conditional_t<bool(B1::value), _and_<B...>, B1> {};

template<typename B>
struct _not_ : bool_constant<!bool(B::value)> {};

template<typename T> struct is_void : is_same<void, remove_cv_t<T>> {};

template<typename T> struct is_null_pointer : is_same<nullptr_t, remove_cv_t<T>> {};

template<typename> struct _is_integral_helper :            false_type {};
template<> struct _is_integral_helper<bool> :               true_type {};
template<> struct _is_integral_helper<char> :               true_type {};
template<> struct _is_integral_helper<signed char> :        true_type {};
template<> struct _is_integral_helper<unsigned char> :      true_type {};
template<> struct _is_integral_helper<wchar_t> :            true_type {};
template<> struct _is_integral_helper<char16_t> :           true_type {};
template<> struct _is_integral_helper<char32_t> :           true_type {};
template<> struct _is_integral_helper<short> :              true_type {};
template<> struct _is_integral_helper<unsigned short> :     true_type {};
template<> struct _is_integral_helper<int> :                true_type {};
template<> struct _is_integral_helper<unsigned int> :       true_type {};
template<> struct _is_integral_helper<long> :               true_type {};
template<> struct _is_integral_helper<unsigned long> :      true_type {};
template<> struct _is_integral_helper<long long> :          true_type {};
template<> struct _is_integral_helper<unsigned long long> : true_type {};
template<typename T> struct is_integral : _is_integral_helper<remove_cv_t<T>> {};

template<typename> struct _is_floating_point_helper :             false_type {};
template<>         struct _is_floating_point_helper<float> :       true_type {};
template<>         struct _is_floating_point_helper<double> :      true_type {};
template<>         struct _is_floating_point_helper<long double> : true_type {};
template<typename T> struct is_floating_point : _is_floating_point_helper<remove_cv_t<T>> {};

template<typename T> struct is_array :                      false_type {};
template<typename T> struct is_array<T[]> :                  true_type {};
template<typename T, size_t Size> struct is_array<T[Size]> : true_type {};

template<typename T> struct is_enum  : bool_constant<__is_enum(T)>  {};
template<typename T> struct is_union : bool_constant<__is_union(T)> {};
template<typename T> struct is_class : bool_constant<__is_class(T)> {};

template<typename> struct is_function : false_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...)> :                     true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...)> :                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const> :               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile> :            true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile> :      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const> :          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile> :       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile> : true_type {};

template<typename Ret, typename... Args> struct is_function<Ret(Args...) &> :                      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const &> :                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile &> :             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile &> :       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) &> :                 true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const &> :           true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile &> :        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile &> :  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) &&> :                     true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const &&> :               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile &&> :            true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile &&> :      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) &&> :                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const &&> :          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile &&> :       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile &&> : true_type {};

#if _ALA_ENABLE_NOEXCEPT_TYPE
template<typename Ret, typename... Args> struct is_function<Ret(Args...) noexcept> :                        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) noexcept> :                   true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const noexcept> :                  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile noexcept> :               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile noexcept> :         true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const noexcept> :             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile noexcept> :          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile noexcept> :    true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) & noexcept> :                      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const & noexcept> :                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile & noexcept> :             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile & noexcept> :       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) & noexcept> :                 true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const & noexcept> :           true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile & noexcept> :        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile & noexcept> :  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) && noexcept> :                     true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const && noexcept> :               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile && noexcept> :            true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile && noexcept> :      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) && noexcept> :                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const && noexcept> :          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile && noexcept> :       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile && noexcept> : true_type {};
#endif

template<typename T> struct _is_pointer_helper :     false_type {};
template<typename T> struct _is_pointer_helper<T *> : true_type {};
template<typename T> struct is_pointer : _is_pointer_helper<remove_cv_t<T>> {};

template<typename T> struct is_lvalue_reference :     false_type {};
template<typename T> struct is_lvalue_reference<T&> : true_type {};

template<typename T> struct is_rvalue_reference :      false_type {};
template<typename T> struct is_rvalue_reference<T&&> : true_type {};

template<typename>                   struct _is_member_object_pointer_helper : false_type {};
template<typename T, typename Class> struct _is_member_object_pointer_helper<T Class::*> : _not_<is_function<T>> {};
template<typename T> struct is_member_object_pointer : _is_member_object_pointer_helper<remove_cv_t<T>> {};

template<typename>                   struct _is_member_function_pointer_helper : false_type {};
template<typename T, typename Class> struct _is_member_function_pointer_helper<T Class::*> : is_function<T> {};
template<typename T> struct is_member_function_pointer : _is_member_function_pointer_helper<remove_cv_t<T>> {};

template<typename T> struct is_reference : _or_<is_lvalue_reference<T>, is_rvalue_reference<T>> {};

template<typename T> struct is_arithmetic : _or_<is_integral<T>, is_floating_point<T>> {};

template<typename T> struct is_fundamental : _or_<is_arithmetic<T>, is_void<T>, is_null_pointer<T>> {};

template<typename T> struct is_object : _not_<_or_<is_function<T>, is_reference<T>, is_void<T>>> {};

template<typename T> struct is_scalar : _or_<is_arithmetic<T>, is_enum<T>, is_pointer<T>, is_member_pointer<T>, is_null_pointer<T>> {};

template<typename T> struct is_compound : _not_<is_fundamental<T>> {};

template<typename T>                 struct _is_member_pointer_helper : false_type {};
template<typename T, typename Class> struct _is_member_pointer_helper<T Class::*> : true_type {};
template<typename T> struct is_member_pointer : _is_member_pointer_helper<remove_cv_t<T>> {};

template<typename T> struct is_const          : false_type {};
template<typename T> struct is_const<const T> : true_type {};

template<typename T> struct is_volatile             : false_type {};
template<typename T> struct is_volatile<volatile T> : true_type {};

template<typename T> struct is_trivial : bool_constant<__is_trivial(T)> {};

template<typename T> struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};

template<typename T> struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};

template<typename T> struct is_pod : bool_constant<__is_pod(T)> {};

template<typename T> struct is_literal_type : bool_constant<__is_literal_type(T)> {};

#if (defined(_ALA_CLANG) && !__is_identifier(__has_unique_object_representations)) || \
    (defined(_ALA_GCC) && __GNUC__ >= 7) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1911)
template<typename T> struct has_unique_object_representations : bool_constant<__has_unique_object_representations(remove_cv_t<T>)> {};
#endif

#if (defined(_ALA_CLANG) && !__is_identifier(__is_aggregate)) || \
    (defined(_ALA_GCC) && __GNUC__ * 1000 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ >= 7001) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1915)
template<typename T> struct is_aggregate : bool_constant<__is_aggregate(remove_cv_t<T>)> {};
#endif

template<typename T> struct is_empty : bool_constant<__is_empty(T)> {};

template<typename T> struct is_polymorphic : bool_constant<__is_polymorphic(T)> {};

template<typename T> struct is_abstract : bool_constant<__is_abstract(T)> {};

template<typename T> struct is_final : bool_constant<__is_final(T)> {};

template<typename T, bool = is_arithmetic<T>::value> struct _is_unsigned_helper : bool_constant<T(0) < T(-1)> {};
template<typename T>                                 struct _is_unsigned_helper<T, false> : false_type {};
template<typename T> struct is_unsigned : _is_unsigned_helper<T> {};

template<typename T, bool = is_arithmetic<T>::value> struct _is_signed_helper : bool_constant<T(-1) < T(0)> {};
template<typename T>                                 struct _is_signed_helper<T, false> : false_type {};
template<typename T> struct is_signed : _is_signed_helper<T> {};

template<typename T>
struct _is_array_known_bounds : _and_<is_array<T>,
                                      bool_constant<(extent<T, 0>::value > 0)>> {};

template<typename T>
struct _is_array_unknown_bounds : _and_<is_array<T>,
                                        _not_<bool_constant<(extent<T, 0>::value > 0)>>> {};

// destructible
template<typename, typename = void_t<>>
struct _is_destructible_impl : false_type {};
template<typename T>
struct _is_destructible_impl<T, void_t<decltype(declval<T &>().~T())>> : true_type {};

template<typename T, bool = _or_<is_void<T>, _is_array_unknown_bounds<T>, is_function<T>>::value,
                     bool = _or_<is_reference<T>, is_scalar<T>>::value>
struct _is_destructible_helper : false_type {};

template<typename T>
struct _is_destructible_helper<T, false, false> : _is_destructible_impl<remove_all_extents_t<T>>::type {};
template<typename T>
struct _is_destructible_helper<T, false, true> : true_type {};

template<typename T>
struct is_destructible : _is_destructible_helper<T> {};

template<typename T>
struct is_trivially_destructible : _and_<is_destructible<T>, bool_constant<__has_trivial_destructor(T)>> {};

template<typename T, bool = is_destructible<T>::value>
struct _is_nt_destructible_helper  : false_type {};
template<typename T>
struct _is_nt_destructible_helper<T, true> : bool_constant<noexcept(declval<T>().~T())> {}; // can not use _and_ while use noexcept instantly

template<typename T>
struct is_nothrow_destructible : _is_nt_destructible_helper<T> {};
template<typename T, size_t N>
struct is_nothrow_destructible<T[N]> : is_nothrow_destructible<T> {};
template<typename T>
struct is_nothrow_destructible<T &> : true_type {};
template<typename T>
struct is_nothrow_destructible<T &&> : true_type {};

// constructible
template<typename, typename = void_t<>>
struct _is_default_constructible_impl : false_type {};
template<typename T>
struct _is_default_constructible_impl<T, void_t<decltype(T())>> : true_type {};

template<typename T, bool = is_array<T>::value>
struct _is_default_constructible_helper: _and_<_not_<is_void<T>>,
                                               _is_default_constructible_impl<T>> {};
template<typename T>
struct _is_default_constructible_helper<T, true> : _and_<_is_array_known_bounds<T>,
                                                         _is_default_constructible_impl<remove_all_extents_t<T>>> {};

template<typename T>
struct is_default_constructible : _is_default_constructible_helper<T> {};

template<typename T, bool = is_array<T>::value>
struct _is_nt_default_constructible_helper : bool_constant<noexcept(T())> {};
template<typename T>
struct _is_nt_default_constructible_helper<T, true> : _and_<_is_array_known_bounds<T>,
                                                            bool_constant<noexcept(remove_all_extents_t<T>())>> {};

template<typename T>
struct is_nothrow_default_constructible : _and_<is_default_constructible<T>, _is_nt_default_constructible_helper<T>> {};

template<typename From, typename To, typename = void_t<>>
struct _is_static_castable_impl : false_type {};
template<typename From, typename To>
struct _is_static_castable_impl<From, To, void_t<decltype(static_cast<To>(declval<From>()))>> : true_type {};

// ref special case
template<typename T, typename Arg>
struct _is_base_to_derived_cast {
    typedef remove_cvref_t<T> T1;
    typedef remove_cvref_t<Arg> Arg1;
    typedef _and_<_not_<is_same<Arg1, T1>>,
                  is_base_of<Arg1, T1>,
                  _not_<is_constructible<T1, Arg>>> type;
};

template<typename T, typename Arg>
struct _is_lvalue_to_rvalue_cast : false_type {};

template<typename T, typename Arg>
struct _is_lvalue_to_rvalue_cast<T&&, Arg&> {
    typedef remove_cvref_t<T> T1;
    typedef remove_cvref_t<Arg> Arg1;
    typedef _and_<_not_<is_function<T1>>,
                  _or_<is_same<T1, Arg1>,
                       is_base_of<T1, Arg1>>> type;
};

template<typename T, typename Arg, typename = void_t<>>
struct _is_a_constructible_impl : false_type {};
template<typename T, typename Arg>
struct _is_a_constructible_impl<T, Arg, void_t<decltype(::new T(declval<Arg>()))>> : true_type {};

template<typename T, typename Arg>
struct _is_a_constructible_helper : _and_<is_destructible<T>, _is_a_constructible_impl<T, Arg>> {};

template<typename T, typename Arg, bool = is_reference<T>::value>
struct _is_a_constructible : _and_<_is_static_castable_impl<Arg, T>,
                                   _not_<typename _is_base_to_derived_cast<T, Arg>::type>,
                                   _not_<typename _is_lvalue_to_rvalue_cast<T, Arg>::type>> {};
// Clang version under 4.0 static_cast not work well

template<typename T, typename Arg>
struct _is_a_constructible<T, Arg, false> : _is_a_constructible_helper<T, Arg> {};

template<typename T, typename... Args>
struct _is_va_constructible_impl {
    template<typename T1, typename... Args1, typename = decltype(T1(declval<Args1>()...))>
    static true_type _test(int);

    template<typename, typename...>
    static false_type _test(...);

    typedef decltype(_test<T, Args...>(0)) type;
};

template<typename T, typename... Args>
struct _is_va_constructible : _is_va_constructible_impl<T, Args...>::type {
    static_assert(sizeof...(Args) > 1, "only useful for more than 1 arguments");
};

template<typename T, typename... Args>
struct is_constructible : _is_va_constructible<T, Args...> {};

template<typename T, typename Arg>
struct is_constructible<T, Arg> : _is_a_constructible<T, Arg> {};

template<typename T>
struct is_constructible<T> : is_default_constructible<T> {};

template<typename T, typename... Args>
struct is_trivially_constructible : _and_<is_constructible<T, Args...>,
                                          bool_constant<__is_trivially_constructible(remove_extent_t<T>, Args...)>> {};
// Add remove_extent in bif to avoid crash

template<typename T, typename... Args>
struct _is_nothrow_constructible_helper : bool_constant<noexcept(T(declval<Args>()...))> {};
template<typename T, typename Arg>
struct _is_nothrow_constructible_helper<T, Arg> : bool_constant<noexcept(static_cast<T>(declval<Arg>()))> {};
template<typename T>
struct _is_nothrow_constructible_helper<T> : is_nothrow_default_constructible<T> {};

template<typename T, typename... Args>
struct is_nothrow_constructible : _and_<is_constructible<T, Args...>, _is_nothrow_constructible_helper<T, Args...>> {};

template<typename T, typename U, typename = void_t<>>
struct _is_assignable_impl : false_type {};
template<typename T, typename U>
struct _is_assignable_impl<T, U, void_t<decltype(declval<T>() = declval<U>())>> : true_type {};

template<typename T, typename U, bool = _or_<is_void<T>, is_void<U>>::value>
struct _is_assignable_helper : _is_assignable_impl<T, U>::type {};
template<typename T, typename U>
struct _is_assignable_helper<T, U, true> : false_type {};

template<typename T, typename U>
struct is_assignable : _is_assignable_helper<T, U> {};

template<typename T, typename U>
struct is_trivially_assignable : _and_<is_assignable<T, U>, bool_constant<__is_trivially_assignable(T, U)>>  {};

template<typename T, typename U, bool = is_assignable<T, U>::value>
struct _is_nt_assignable_helper : false_type {};
template<typename T, typename U>
struct _is_nt_assignable_helper<T, U, true> : bool_constant<noexcept(declval<T>() = declval<U>())> {};

template<typename T, typename U>
struct is_nothrow_assignable : _is_nt_assignable_helper<T, U> {};

template<typename From, typename To>
struct _is_convertible_impl {
    template<typename To1>
    static void _convert(To1);

    template<typename From1, typename To1, typename = decltype(_convert<To1>(declval<From1>()))>
    static true_type _test(int);

    template<typename, typename>
    static false_type _test(...);

    typedef decltype(_test<From, To>(0)) type;
};

template<typename From, typename To,
          bool = _or_<is_void<From>, is_function<To>, is_array<To>>::value>
struct _is_convertible_helper : is_void<To> {};

template<typename From, typename To>
struct _is_convertible_helper<From, To, false> : _is_convertible_impl<From, To> {};

template<typename From, typename To>
struct is_convertible : _is_convertible_helper<From, To>::type {};

template<typename T> struct is_trivially_default_constructible : is_trivially_constructible<T> {};

template<typename T> struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_trivially_copy_constructible : is_trivially_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_nothrow_copy_constructible : is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

template<typename T> struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_trivially_move_constructible : is_trivially_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};

template<typename T> struct is_copy_assignable : is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};
template<typename T> struct is_trivially_copy_assignable : is_trivially_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};
template<typename T> struct is_nothrow_copy_assignable : is_nothrow_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template<typename T> struct is_move_assignable : is_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};
template<typename T> struct is_trivially_move_assignable : is_trivially_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};
template<typename T> struct is_nothrow_move_assignable : is_nothrow_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};

template<typename T> struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};

template<typename T>
struct _is_referenceable_impl {
    template<typename T1, typename = T1&> static true_type _test(int);
    template<typename>                   static false_type _test(...);
    typedef decltype(_test<T>(0)) type;
};

template<typename T>
struct _is_referenceable : _is_referenceable_impl<T>::type {};

template<typename T, typename U = T>
struct _is_swappable_with_impl {
    template<typename Lhs, typename Rhs, typename = decltype(ala::swap(declval<Lhs>(), declval<Rhs>()))>
    static true_type _test(int);
    template<typename, typename>
    static false_type _test(...);
    typedef _and_<decltype((_test<T, U>(0))), decltype((_test<U, T>(0)))> type;
};

template<typename T, typename U, bool NotVoid = _or_<is_void<T>, is_void<U>>::value>
struct _is_swappable_with_helper : _is_swappable_with_impl<T, U>::type {};

template<typename T, typename U>
struct _is_swappable_with_helper<T, U, true> : false_type {};

template<typename T, typename U>
struct is_swappable_with : _is_swappable_with_helper<T, U> {};

template<typename T>
struct is_swappable : conditional_t<_is_referenceable<T>::value,
                                    is_swappable_with<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>,
                                    false_type> {};

template<typename T, typename U, bool>
struct _is_nothrow_swappable_helper : bool_constant<noexcept(ala::swap(declval<T>(), declval<U>())) &&
                                                    noexcept(ala::swap(declval<U>(), declval<T>()))> {};

template<typename T, typename U>
struct _is_nothrow_swappable_helper<T, U, false> : false_type {};

template<typename T, typename U>
struct is_nothrow_swappable_with : _is_nothrow_swappable_helper<T, U, is_swappable_with<T, U>::value> {};

template<typename T>
struct is_nothrow_swappable : conditional_t<_is_referenceable<T>::value,
                                            is_nothrow_swappable_with<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>,
                                            false_type> {};

template<typename T> struct alignment_of : integral_constant<size_t, alignof(T)> {};

template<typename T>           struct rank :       integral_constant<size_t, 0> {};
template<typename T>           struct rank<T[]> :  integral_constant<size_t, rank<T>::value + 1> {};
template<typename T, size_t N> struct rank<T[N]> : integral_constant<size_t, rank<T>::value + 1> {};

template<typename T, unsigned N = 0>       struct extent :          integral_constant<size_t, 0> {};
template<typename T>                       struct extent<T[], 0> :  integral_constant<size_t, 0> {};
template<typename T, unsigned N>           struct extent<T[], N> :  extent<T, N-1> {};
template<typename T, size_t I>             struct extent<T[I], 0> : integral_constant<size_t, I> {};
template<typename T, size_t I, unsigned N> struct extent<T[I], N> : extent<T, N-1> {};

template<typename T, typename U> struct is_same :      false_type {};
template<typename T>             struct is_same<T, T> : true_type {};

template<typename Base, typename Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};

template<typename T>           struct remove_const             { typedef T type;    };
template<typename T>           struct remove_const<const T>    { typedef T type;    };
template<typename T>           struct remove_const<const T[]>  { typedef T type[];  };
template<typename T, size_t N> struct remove_const<const T[N]> { typedef T type[N]; };

template<typename T>           struct remove_volatile                { typedef T type;    };
template<typename T>           struct remove_volatile<volatile T>    { typedef T type;    };
template<typename T>           struct remove_volatile<volatile T[]>  { typedef T type[];  };
template<typename T, size_t N> struct remove_volatile<volatile T[N]> { typedef T type[N]; };

template<typename T> struct remove_cv { typedef remove_volatile_t<remove_const_t<T>> type; };
template<typename T> struct add_cv { typedef const volatile T type; };
template<typename T> struct add_const { typedef const T type; };
template<typename T> struct add_volatile { typedef volatile T type; };

template<typename T> struct remove_reference      { typedef T type; };
template<typename T> struct remove_reference<T&>  { typedef T type; };
template<typename T> struct remove_reference<T&&> { typedef T type; };

template<typename T> struct add_lvalue_reference                      { typedef T&                  type; };
template<typename T> struct add_lvalue_reference<T&>                  { typedef T&                  type; };
// template<typename T> struct add_lvalue_reference<const T&>            { typedef T&                  type; };
// template<typename T> struct add_lvalue_reference<volatile T&>         { typedef T&                  type; };
// template<typename T> struct add_lvalue_reference<const volatile T&>   { typedef T&                  type; };
template<>           struct add_lvalue_reference<void>                { typedef void                type; };
template<>           struct add_lvalue_reference<const void>          { typedef const void          type; };
template<>           struct add_lvalue_reference<volatile void>       { typedef volatile void       type; };
template<>           struct add_lvalue_reference<const volatile void> { typedef const volatile void type; };

template<typename T> struct add_rvalue_reference                      { typedef T&&                 type; };
template<typename T> struct add_rvalue_reference<T&>                  { typedef T&                  type; };
// template<typename T> struct add_rvalue_reference<const T&>            { typedef T&                  type; };
// template<typename T> struct add_rvalue_reference<volatile T&>         { typedef T&                  type; };
// template<typename T> struct add_rvalue_reference<const volatile T&>   { typedef T&                  type; };
template<>           struct add_rvalue_reference<void>                { typedef void                type; };
template<>           struct add_rvalue_reference<const void>          { typedef const void          type; };
template<>           struct add_rvalue_reference<volatile void>       { typedef volatile void       type; };
template<>           struct add_rvalue_reference<const volatile void> { typedef const volatile void type; };

template<typename T> struct make_signed                          { typedef T                               type; };
template<> struct make_signed<unsigned char>                     { typedef signed char                     type; };
template<> struct make_signed<unsigned short>                    { typedef signed short                    type; };
template<> struct make_signed<unsigned int>                      { typedef signed int                      type; };
template<> struct make_signed<unsigned long>                     { typedef signed long                     type; };
template<> struct make_signed<unsigned long long>                { typedef signed long long                type; };
template<> struct make_signed<char>                              { typedef signed char                     type; };
template<> struct make_signed<const unsigned char>               { typedef const signed char               type; };
template<> struct make_signed<const unsigned short>              { typedef const signed short              type; };
template<> struct make_signed<const unsigned int>                { typedef const signed int                type; };
template<> struct make_signed<const unsigned long>               { typedef const signed long               type; };
template<> struct make_signed<const unsigned long long>          { typedef const signed long long          type; };
template<> struct make_signed<const char>                        { typedef const signed char               type; };
template<> struct make_signed<volatile unsigned char>            { typedef volatile signed char            type; };
template<> struct make_signed<volatile unsigned short>           { typedef volatile signed short           type; };
template<> struct make_signed<volatile unsigned int>             { typedef volatile signed int             type; };
template<> struct make_signed<volatile unsigned long>            { typedef volatile signed long            type; };
template<> struct make_signed<volatile unsigned long long>       { typedef volatile signed long long       type; };
template<> struct make_signed<volatile char>                     { typedef volatile signed char            type; };
template<> struct make_signed<const volatile unsigned char>      { typedef const volatile signed char      type; };
template<> struct make_signed<const volatile unsigned short>     { typedef const volatile signed short     type; };
template<> struct make_signed<const volatile unsigned int>       { typedef const volatile signed int       type; };
template<> struct make_signed<const volatile unsigned long>      { typedef const volatile signed long      type; };
template<> struct make_signed<const volatile unsigned long long> { typedef const volatile signed long long type; };
template<> struct make_signed<const volatile char>               { typedef const volatile signed char      type; };

#if (defined(__WCHAR_MAX__) && (__WCHAR_MAX__ == 4294967295U))
template<> struct make_signed<wchar_t>                { typedef int32_t                type; };
template<> struct make_signed<const wchar_t>          { typedef const int32_t          type; };
template<> struct make_signed<volatile wchar_t>       { typedef volatile int32_t       type; };
template<> struct make_signed<const volatile wchar_t> { typedef const volatile int32_t type; };
#elif (defined(__WCHAR_MAX__) && (__WCHAR_MAX__ == 65535))
template<> struct make_signed<wchar_t>                { typedef int16_t                type; };
template<> struct make_signed<const wchar_t>          { typedef const int16_t          type; };
template<> struct make_signed<volatile wchar_t>       { typedef volatile int16_t       type; };
template<> struct make_signed<const volatile wchar_t> { typedef const volatile int16_t type; };
#endif

template<typename T> struct make_unsigned                        { typedef T                                 type; };
template<> struct make_unsigned<signed char>                     { typedef unsigned char                     type; };
template<> struct make_unsigned<signed short>                    { typedef unsigned short                    type; };
template<> struct make_unsigned<signed int>                      { typedef unsigned int                      type; };
template<> struct make_unsigned<signed long>                     { typedef unsigned long                     type; };
template<> struct make_unsigned<signed long long>                { typedef unsigned long long                type; };
template<> struct make_unsigned<char>                            { typedef unsigned char                     type; };
template<> struct make_unsigned<const signed char>               { typedef const unsigned char               type; };
template<> struct make_unsigned<const signed short>              { typedef const unsigned short              type; };
template<> struct make_unsigned<const signed int>                { typedef const unsigned int                type; };
template<> struct make_unsigned<const signed long>               { typedef const unsigned long               type; };
template<> struct make_unsigned<const signed long long>          { typedef const unsigned long long          type; };
template<> struct make_unsigned<const char>                      { typedef const unsigned char               type; };
template<> struct make_unsigned<volatile signed char>            { typedef volatile unsigned char            type; };
template<> struct make_unsigned<volatile signed short>           { typedef volatile unsigned short           type; };
template<> struct make_unsigned<volatile signed int>             { typedef volatile unsigned int             type; };
template<> struct make_unsigned<volatile signed long>            { typedef volatile unsigned long            type; };
template<> struct make_unsigned<volatile signed long long>       { typedef volatile unsigned long long       type; };
template<> struct make_unsigned<volatile char>                   { typedef volatile unsigned char            type; };
template<> struct make_unsigned<const volatile signed char>      { typedef const volatile unsigned char      type; };
template<> struct make_unsigned<const volatile signed short>     { typedef const volatile unsigned short     type; };
template<> struct make_unsigned<const volatile signed int>       { typedef const volatile unsigned int       type; };
template<> struct make_unsigned<const volatile signed long>      { typedef const volatile unsigned long      type; };
template<> struct make_unsigned<const volatile signed long long> { typedef const volatile unsigned long long type; };
template<> struct make_unsigned<const volatile char>             { typedef const volatile unsigned char      type; };

#if defined(__WCHAR_MAX__) && (__WCHAR_MAX__ != 4294967295U)
template<> struct make_unsigned<wchar_t>                { typedef uint32_t                type; };
template<> struct make_unsigned<const wchar_t>          { typedef const uint32_t          type; };
template<> struct make_unsigned<volatile wchar_t>       { typedef volatile uint32_t       type; };
template<> struct make_unsigned<const volatile wchar_t> { typedef const volatile uint32_t type; };
#elif defined(__WCHAR_MAX__) && (__WCHAR_MAX__ != 65535)
template<> struct make_unsigned<wchar_t>                { typedef uint16_t                type; };
template<> struct make_unsigned<const wchar_t>          { typedef const uint16_t          type; };
template<> struct make_unsigned<volatile wchar_t>       { typedef volatile uint16_t       type; };
template<> struct make_unsigned<const volatile wchar_t> { typedef const volatile uint16_t type; };
#endif

template<typename T>           struct remove_extent       { typedef T type; };
template<typename T>           struct remove_extent<T[]>  { typedef T type; };
template<typename T, size_t N> struct remove_extent<T[N]> { typedef T type; };

template<typename T>           struct remove_all_extents       { typedef T                       type; };
template<typename T>           struct remove_all_extents<T[]>  { typedef remove_all_extents_t<T> type; };
template<typename T, size_t N> struct remove_all_extents<T[N]> { typedef remove_all_extents_t<T> type; };

template<typename T> struct remove_pointer                    { typedef T type; };
template<typename T> struct remove_pointer<T*>                { typedef T type; };
template<typename T> struct remove_pointer<T* const>          { typedef T type; };
template<typename T> struct remove_pointer<T* volatile>       { typedef T type; };
template<typename T> struct remove_pointer<T* const volatile> { typedef T type; };

template<typename T, bool is_fun = false> struct _add_pointer_helper                        { typedef remove_reference_t<T>* type; };
template<typename T>                      struct _add_pointer_helper<T, true>               { typedef T type;                 };
template<typename T, typename... Args>    struct _add_pointer_helper<T(Args...), true>      { typedef T(*type)(Args...);      };
template<typename T, typename... Args>    struct _add_pointer_helper<T(Args..., ...), true> { typedef T(*type)(Args..., ...); };
template<typename T> struct add_pointer : _add_pointer_helper<T, is_function<T>::value> {};

template<size_t Len, size_t Align = 8>
struct aligned_storage {
    struct type {
        alignas(Align) unsigned char data[Len];
    };
};

template<size_t... Vals> struct _maximum;
template<>               struct _maximum<> :    integral_constant<size_t, 0> {};
template<size_t Val>     struct _maximum<Val> : integral_constant<size_t, Val> {};
template<size_t First, size_t Second, size_t... Rest>
struct _maximum<First, Second, Rest...> : _maximum<(First < Second ? Second : First), Rest...> {};

template<size_t Len, typename... Types>
struct aligned_union {
    constexpr static size_t alignment_value = _maximum<alignof(Types)...>::value;
    struct type {
        alignas(alignment_value) char _s[_maximum<Len, sizeof(Types)...>::value];
    };
};

template<typename T>
struct decay {
    typedef remove_reference_t<T> U;
    typedef conditional_t<is_array<U>::value,
                          remove_extent_t<U> *,
                          conditional_t<is_function<U>::value,
                                        add_pointer_t<U>,
                                        remove_cv_t<U>>> type;
};

template< typename T >
struct remove_cvref { typedef remove_cv_t<remove_reference_t<T>> type; };

template<bool B, typename T> struct enable_if {};
template<typename T> struct enable_if<true, T> { typedef T type; };

template<bool B, typename T, typename F> struct conditional      { typedef T type; };
template<typename T, typename F> struct conditional<false, T, F> { typedef F type; };

template<typename... T> struct common_type;
template<typename T> struct common_type<T> { typedef decay_t<T> type; };
template<typename T, typename U> struct common_type<T, U> { typedef decay_t<decltype(true ? declval<T>() : declval<U>())> type; };
template<typename T, typename U, typename... V> struct common_type<T, U, V...> { typedef common_type_t<common_type_t<T, U>, V...> type; };

template<typename T, bool = is_enum<T>::value> struct _underlying_type_helper {};
template<typename T> struct _underlying_type_helper<T, true> { typedef __underlying_type(T) type; };

template<typename T> struct underlying_type : _underlying_type_helper<T> {};

#if !defined(_ALA_MSVC) || _MSC_VER >= 1910
template<typename Fn, typename... Args>
struct _invoke_result_impl {
    template<typename T>
    struct _success_type { typedef T type; };
    struct _failure_type {};

    template<typename Fn1, typename... Args1>
    static _success_type<decltype(invoke(declval<Fn1>(), declval<Args1>()...))> _test(int);

    template<typename...>
    static _failure_type _test(...);

    typedef decltype(_test<Fn, Args...>(0)) type;
};

template<typename Fn, typename... Args>
struct invoke_result : _invoke_result_impl<Fn, Args...>::type {};

template<typename Fn, typename... Args>
struct result_of<Fn(Args...)> : invoke_result<Fn, Args...> {};

template<typename Result, typename Ret, typename = void_t<>>
struct _is_invocable_impl : false_type {};

template<typename Result, typename Ret>
struct _is_invocable_impl<Result, Ret, void_t<typename Result::type>> : _or_<is_void<Ret>,
                                                                             is_convertible<typename Result::type, Ret>> {};

template<typename Fn, typename... Args>
struct is_invocable : _is_invocable_impl<invoke_result<Fn, Args...>, void> {};

template<typename Ret, typename Fn, typename... Args>
struct is_invocable_r : _is_invocable_impl<_is_invocable_impl<Fn, Args...>, Ret> {};

template<typename Fn, typename... Args>
struct _is_nt_invocable : bool_constant<noexcept(invoke(declval<Fn>(), declval<Args>()...))> {};

template<typename Result, typename Ret, typename = void_t<>>
struct _is_nt_invocable_r_impl : false_type {};

template<typename Result, typename Ret>
struct _is_nt_invocable_r_impl<Result, Ret, void_t<typename Result::type>> : _or_<is_void<Ret>,
                                                                                  _and_<is_convertible<typename Result::type, Ret>,
                                                                                        is_nothrow_constructible<Ret, typename Result::type>>> {};
template<typename Fn, typename... Args>
struct is_nothrow_invocable : _and_<is_invocable<Fn, Args...>, _is_nt_invocable<Fn, Args...>> {};

template<typename Ret, typename Fn, typename... Args>
struct is_nothrow_invocable_r : _and_<_is_nt_invocable_r_impl<typename invoke_result<Fn, Args...>::type, Ret>,
                                      _is_nt_invocable<Fn, Args...>> {};
#endif

template<typename... B> struct conjunction : _and_<B...> {};
template<typename... B> struct disjunction : _or_<B...> {};
template<typename B>    struct negation    : _not_<B> {};

enum class endian {
#if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#else
    little = 0,
    big    = 1,
    native = little
#endif
};

template<typename T> struct type_identity { typedef T type; };
template<typename T> using type_identity_t = typename type_identity<T>::type;

template<typename T> struct unwrap_reference { using type = T; };
template<typename T> struct unwrap_reference<reference_wrapper<T>> { using type = T&; };
template<typename T> using unwrap_reference_t = typename unwrap_reference<T>::type;

template<typename T> struct unwrap_ref_decay : unwrap_reference<decay_t<T>> {};
template<typename T> using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;

} // namespace ala

// clang-format on

#ifdef _ALA_MSVC
#pragma warning(pop)
#endif

#endif // HEAD