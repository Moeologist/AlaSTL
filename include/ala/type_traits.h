// surfix: _impl - sfinae class, _helper - base class, _test - aux function
#ifndef _ALA_TYPE_TRAITS_H
#define _ALA_TYPE_TRAITS_H

#include <ala/detail/traits_declare.h>

#ifdef _ALA_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4180)
    #pragma warning(disable : 4197)
#endif

#ifdef _ALA_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-volatile"
#endif

ALA_BEGIN_NAMESPACE_STD

template<typename...>
struct common_type;

template<typename, typename, template<typename> class, template<typename> class>
struct basic_common_reference;

ALA_END_NAMESPACE_STD

// clang-format off

namespace ala {

template<typename T, T Value>
struct integral_constant {
    static constexpr T value = Value;
    using value_type = T;
    using type = integral_constant<T, Value>;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

}

#include <ala/detail/meta.h>
#include <ala/detail/utility_base.h>

namespace ala {

template<bool B, typename T, typename F> struct conditional      { using type = T; };
template<typename T, typename F> struct conditional<false, T, F> { using type = F; };

template<typename T> struct is_void: is_same<void, remove_cv_t<T>> {};

template<typename T> struct is_null_pointer: is_same<nullptr_t, remove_cv_t<T>> {};

template<typename> struct _is_integral_helper:            false_type {};
template<> struct _is_integral_helper<bool>:               true_type {};
template<> struct _is_integral_helper<char>:               true_type {};
template<> struct _is_integral_helper<signed char>:        true_type {};
template<> struct _is_integral_helper<unsigned char>:      true_type {};
template<> struct _is_integral_helper<wchar_t>:            true_type {};
template<> struct _is_integral_helper<char16_t>:           true_type {};
template<> struct _is_integral_helper<char32_t>:           true_type {};
template<> struct _is_integral_helper<short>:              true_type {};
template<> struct _is_integral_helper<unsigned short>:     true_type {};
template<> struct _is_integral_helper<int>:                true_type {};
template<> struct _is_integral_helper<unsigned int>:       true_type {};
template<> struct _is_integral_helper<long>:               true_type {};
template<> struct _is_integral_helper<unsigned long>:      true_type {};
template<> struct _is_integral_helper<long long>:          true_type {};
template<> struct _is_integral_helper<unsigned long long>: true_type {};
#if _ALA_ENABLE_INT128T
template<> struct _is_integral_helper<__int128_t>:  true_type {};
template<> struct _is_integral_helper<__uint128_t>: true_type {};
#endif
#if _ALA_ENABLE_CHAR8T
template<> struct _is_integral_helper<char8_t>: true_type {};
#endif

template<typename T> struct is_integral: _is_integral_helper<remove_cv_t<T>> {};

template<typename> struct _is_floating_point_helper:             false_type {};
template<>         struct _is_floating_point_helper<float>:       true_type {};
template<>         struct _is_floating_point_helper<double>:      true_type {};
template<>         struct _is_floating_point_helper<long double>: true_type {};
template<typename T> struct is_floating_point: _is_floating_point_helper<remove_cv_t<T>> {};

template<typename T> struct is_array:                      false_type {};
template<typename T> struct is_array<T[]>:                  true_type {};
template<typename T, size_t Size> struct is_array<T[Size]>: true_type {};

template<typename T> struct is_enum:  bool_constant<__is_enum(T)>  {};
template<typename T> struct is_union: bool_constant<__is_union(T)> {};
template<typename T> struct is_class: bool_constant<__is_class(T)> {};

template<typename> struct is_function: false_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...)>:                        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const>:                  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile>:               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile>:         true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) &>:                      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const &>:                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile &>:             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile &>:       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) &&>:                     true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const &&>:               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile &&>:            true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile &&>:      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...)>:                   true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const>:             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile>:          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile>:    true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) &>:                 true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const &>:           true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile &>:        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile &>:  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) &&>:                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const &&>:          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile &&>:       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile &&>: true_type {};

#if _ALA_ENABLE_NOEXCEPT_TYPE
template<typename Ret, typename... Args> struct is_function<Ret(Args...) noexcept>:                        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const noexcept>:                  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile noexcept>:               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile noexcept>:         true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) & noexcept>:                      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const & noexcept>:                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile & noexcept>:             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile & noexcept>:       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) && noexcept>:                     true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const && noexcept>:               true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) volatile && noexcept>:            true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args...) const volatile && noexcept>:      true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) noexcept>:                   true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const noexcept>:             true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile noexcept>:          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile noexcept>:    true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) & noexcept>:                 true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const & noexcept>:           true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile & noexcept>:        true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile & noexcept>:  true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) && noexcept>:                true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const && noexcept>:          true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) volatile && noexcept>:       true_type {};
template<typename Ret, typename... Args> struct is_function<Ret(Args..., ...) const volatile && noexcept>: true_type {};
#endif

template<typename T> struct _is_pointer_helper:     false_type {};
template<typename T> struct _is_pointer_helper<T *>: true_type {};
template<typename T> struct is_pointer: _is_pointer_helper<remove_cv_t<T>> {};

template<typename T> struct is_lvalue_reference:    false_type {};
template<typename T> struct is_lvalue_reference<T&>: true_type {};

template<typename T> struct is_rvalue_reference:     false_type {};
template<typename T> struct is_rvalue_reference<T&&>: true_type {};

template<typename>                   struct _is_member_object_pointer_helper: false_type {};
template<typename T, typename Class> struct _is_member_object_pointer_helper<T Class::*>: is_object<T> {};
template<typename T> struct is_member_object_pointer: _is_member_object_pointer_helper<remove_cv_t<T>> {};

template<typename>                   struct _is_member_function_pointer_helper: false_type {};
template<typename T, typename Class> struct _is_member_function_pointer_helper<T Class::*>: is_function<T> {};
template<typename T> struct is_member_function_pointer: _is_member_function_pointer_helper<remove_cv_t<T>> {};

template<typename T> struct is_reference: _or_<is_lvalue_reference<T>, is_rvalue_reference<T>> {};

template<typename T> struct is_arithmetic: _or_<is_integral<T>, is_floating_point<T>> {};

template<typename T> struct is_fundamental: _or_<is_arithmetic<T>, is_void<T>, is_null_pointer<T>> {};

template<typename T> struct is_object: _not_<_or_<is_function<T>, is_reference<T>, is_void<T>>> {};

template<typename T> struct is_scalar: _or_<is_arithmetic<T>, is_enum<T>, is_pointer<T>, is_member_pointer<T>, is_null_pointer<T>> {};

template<typename T> struct is_compound: _not_<is_fundamental<T>> {};

template<typename T>                 struct _is_member_pointer_helper: false_type {};
template<typename T, typename Class> struct _is_member_pointer_helper<T Class::*>: true_type {};
template<typename T> struct is_member_pointer: _is_member_pointer_helper<remove_cv_t<T>> {};

template<typename T> struct is_const:          false_type {};
template<typename T> struct is_const<const T>: true_type {};

template<typename T> struct is_volatile:             false_type {};
template<typename T> struct is_volatile<volatile T>: true_type {};

template<typename T> struct is_trivial: bool_constant<__is_trivial(T)> {};

template<typename T> struct is_trivially_copyable: bool_constant<__is_trivially_copyable(T)> {};

template<typename T> struct is_standard_layout: bool_constant<__is_standard_layout(T)> {};

template<typename T> struct is_pod: bool_constant<__is_pod(T)> {};

template<typename T> struct is_literal_type: bool_constant<__is_literal_type(T)> {};

#if (defined(_ALA_CLANG) && !ALA_IS_IDENTIFIER(__has_unique_object_representations)) || \
    (defined(_ALA_GCC) && __GNUC__ >= 7) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1911)
template<typename T> struct has_unique_object_representations: bool_constant<__has_unique_object_representations(remove_cv_t<T>)> {};
#endif

#if (defined(_ALA_CLANG) && !ALA_IS_IDENTIFIER(__is_aggregate)) || \
    (defined(_ALA_GCC) && _ALA_GCC_VER >= 70001) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1915)
template<typename T> struct is_aggregate: bool_constant<__is_aggregate(remove_cv_t<T>)> {};
#endif

template<typename T> struct is_empty: bool_constant<__is_empty(T)> {};

template<typename T> struct is_polymorphic: bool_constant<__is_polymorphic(T)> {};

template<typename T> struct is_abstract: bool_constant<__is_abstract(T)> {};

template<typename T> struct is_final: bool_constant<__is_final(T)> {};

template<typename T, bool = is_arithmetic<T>::value> struct _is_unsigned_helper: bool_constant<T(0) < T(-1)> {};
template<typename T>                                 struct _is_unsigned_helper<T, false>: false_type {};
template<typename T> struct is_unsigned: _is_unsigned_helper<T> {};

template<typename T, bool = is_arithmetic<T>::value> struct _is_signed_helper: bool_constant<T(-1) < T(0)> {};
template<typename T>                                 struct _is_signed_helper<T, false>: false_type {};
template<typename T> struct is_signed: _is_signed_helper<T> {};

// destructible
template<typename, typename = void>
struct _is_destructible_impl: false_type {};
template<typename T>
struct _is_destructible_impl<T, void_t<decltype(declval<T &>().~T())>>: true_type {};

template<typename T, bool = is_reference<T>::value,
                     bool = is_object<T>::value>
struct _is_destructible_helper: false_type {};
template<typename T>
struct _is_destructible_helper<T, false, true>: _is_destructible_impl<T>::type {};
template<typename T, bool B>
struct _is_destructible_helper<T, true, B>: true_type {};

template<typename T>
struct is_destructible: _and_<_not_<is_unbounded_array<T>>,
                              _is_destructible_helper<remove_all_extents_t<T>>> {};


#if ALA_HAS_BUILTIN(__is_trivially_destructible) || defined(_ALA_MSVC)
template <class T> struct is_trivially_destructible: integral_constant<bool, __is_trivially_destructible(T)> {};
#else
template<typename T>
struct is_trivially_destructible: _and_<is_destructible<T>, bool_constant<__has_trivial_destructor(remove_all_extents_t<T>)>> {};
#endif

template<typename T, bool = is_reference<T>::value,
                     bool = is_object<T>::value>
struct _is_nt_destructible_impl: false_type {};
template<typename T>
struct _is_nt_destructible_impl<T, false, true>: bool_constant<noexcept(declval<T>().~T())> {};
template<typename T, bool B>
struct _is_nt_destructible_impl<T, true, B>: true_type {};

template<typename T, bool = is_destructible<T>::value>
struct _is_nt_destructible_helper  : false_type {};
template<typename T>
struct _is_nt_destructible_helper<T, true>: _is_nt_destructible_impl<remove_all_extents_t<T>> {}; // can not use _and_

template<typename T>
struct is_nothrow_destructible: _is_nt_destructible_helper<T> {};

// constructible

template<typename T, typename Arg, typename = void>
struct _is_a_constructible_impl: false_type {};
template<typename T, typename Arg>
struct _is_a_constructible_impl<T, Arg, void_t<decltype(new T(declval<Arg>()))>>: true_type {};
// use new to avoid explicit cast semantics

template<typename T, typename Arg, bool = is_reference<T>::value>
struct _is_a_constructible: is_convertible<Arg, T> {};
template<typename T, typename Arg>
struct _is_a_constructible<T, Arg, false>: _and_<is_destructible<T>,
                                                 _is_a_constructible_impl<T, Arg>> {};

template<typename, typename T, typename... Args>
struct _is_va_constructible_impl: false_type {};
template<typename T, typename... Args>
struct _is_va_constructible_impl<void_t<decltype(T(declval<Args>()...))>, T, Args...>: true_type {};

template<typename T, typename... Args>
struct _is_va_constructible: _is_va_constructible_impl<void, T, Args...> {};

template<typename, typename = void>
struct _is_def_constructible_impl: false_type {};
template<typename T>
struct _is_def_constructible_impl<T, void_t<decltype(T())>>: true_type {};

template<typename T, bool = is_array<T>::value>
struct _is_def_constructible: _and_<_not_<is_void<T>>,
                                    _is_def_constructible_impl<T>> {};
template<typename T>
struct _is_def_constructible<T, true>: _and_<is_bounded_array<T>,
                                             _is_def_constructible<remove_extent_t<T>>> {};

template<typename T, typename... Args>
struct _is_constructible: _is_va_constructible<T, Args...> {};

template<typename T, typename Arg>
struct _is_constructible<T, Arg>: _is_a_constructible<T, Arg> {};

template<typename T>
struct _is_constructible<T>: _is_def_constructible<T> {};

#if (defined(_ALA_CLANG) && !ALA_IS_IDENTIFIER(__is_constructible)) || \
    (defined(_ALA_GCC) && _ALA_GCC_VER >= 80100) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1900)
template<typename T, typename... Args>
struct is_constructible: bool_constant<__is_constructible(T, Args...)> {};
#else
template<typename T, typename... Args>
struct is_constructible: _is_constructible<T, Args...> {};
#endif

// template<typename T, typename... Args>
// struct is_trivially_constructible: bool_constant<__is_trivially_constructible(T, Args...)> {};

// template<typename T, typename... Args>
// struct is_nothrow_constructible: bool_constant<__is_nothrow_constructible(T, Args...)> {};

template<typename T, typename... Args>
struct is_trivially_constructible: _and_<is_constructible<T, Args...>,
                                         bool_constant<__is_trivially_constructible(remove_all_extents_t<T>, Args...)>> {};
// Add remove_all_extent in bif to avoid gcc crash

template<typename T, typename... Args>
struct _is_nt_va_constructible_impl: bool_constant<noexcept(T(declval<Args>()...))> {};

template<typename T, typename Arg>
struct _is_nt_a_constructible_impl: conditional_t<is_reference<T>::value,
                                                  is_nothrow_convertible<Arg, T>,
                                                  bool_constant<noexcept(T(declval<Arg>()))>> {};

template<typename T>
struct _is_nt_def_constructible_impl: bool_constant<noexcept(T())> {};

template<typename T, bool = is_array<T>::value>
struct _is_nt_def_constructible_helper: _is_nt_def_constructible_impl<T> {};

template<typename T>
struct _is_nt_def_constructible_helper<T, true>: _and_<is_bounded_array<T>,
                                                 _is_nt_def_constructible_helper<remove_extent_t<T>>> {};

template<typename T, typename... Args>
struct _is_nt_constructible: _is_nt_va_constructible_impl<T, Args...> {};

template<typename T, typename Arg>
struct _is_nt_constructible<T, Arg>: _is_nt_a_constructible_impl<T, Arg> {};

template<typename T>
struct _is_nt_constructible<T>: _is_nt_def_constructible_helper<T> {};

template<typename T, typename... Args>
struct is_nothrow_constructible: _and_<is_constructible<T, Args...>,
                                       _is_nt_constructible<T, Args...>> {};

template<typename T, typename U, typename = void>
struct _is_assignable_impl: false_type {};
template<typename T, typename U>
struct _is_assignable_impl<T, U, void_t<decltype(declval<T>() = declval<U>())>>: true_type {};

template<typename T, typename U, bool = _or_<is_void<T>, is_void<U>>::value>
struct _is_assignable_helper: _is_assignable_impl<T, U> {};
template<typename T, typename U>
struct _is_assignable_helper<T, U, true>: false_type {};

template<typename T, typename U>
struct is_assignable: _is_assignable_helper<T, U> {};

template<typename T, typename U>
struct is_trivially_assignable: _and_<is_assignable<T, U>, bool_constant<__is_trivially_assignable(T, U)>>  {};

template<typename T, typename U, bool = is_assignable<T, U>::value>
struct _is_nt_assignable_helper: false_type {};
template<typename T, typename U>
struct _is_nt_assignable_helper<T, U, true>: bool_constant<noexcept(declval<T>() = declval<U>())> {};

template<typename T, typename U>
struct is_nothrow_assignable: _is_nt_assignable_helper<T, U> {};

template<typename From, typename To>
struct _is_convertible_impl {
    template<typename To1>
    static void _convert(To1) noexcept;

    template<typename From1, typename To1,
             typename = decltype(_convert<To1>(declval<From1>()))>
    static true_type _test(int);

    template<typename, typename>
    static false_type _test(...);

    using type = decltype(_test<From, To>(0));
};

template<typename T>
struct _is_returnable_impl {
    template<typename T1, typename = T1()>
    static true_type _test(int);

    template<typename>
    static false_type _test(...);

    using type = decltype(_test<T>(0));
};

template<typename From, typename To,
          bool = _or_<is_void<From>, is_function<To>, is_array<To>>::value>
struct _is_convertible_helper: is_void<To> {};

template<typename From, typename To>
struct _is_convertible_helper<From, To, false>: _and_<typename _is_convertible_impl<From, To>::type,
                                                      typename _is_returnable_impl<To>::type> {};

template<typename From, typename To>
struct is_convertible: _is_convertible_helper<From, To> {};

template<typename From, typename To, typename = void>
struct is_explicit_convertible_impl: false_type {};
template<typename From, typename To>
struct is_explicit_convertible_impl<From, To, void_t<decltype(static_cast<To>(declval<From>()))>>: true_type {};

template<typename From, typename To>
struct is_explicit_convertible: is_explicit_convertible_impl<From, To> {};

template<typename T> struct is_default_constructible: is_constructible<T> {};
template<typename T> struct is_trivially_default_constructible: is_trivially_constructible<T> {};
template<typename T> struct is_nothrow_default_constructible: is_nothrow_constructible<T> {};

template<typename T> struct is_copy_constructible: is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_trivially_copy_constructible: is_trivially_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_nothrow_copy_constructible: is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

template<typename T> struct is_move_constructible: is_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_trivially_move_constructible: is_trivially_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_nothrow_move_constructible: is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};

template<typename T> struct is_copy_assignable: is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};
template<typename T> struct is_trivially_copy_assignable: is_trivially_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};
template<typename T> struct is_nothrow_copy_assignable: is_nothrow_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template<typename T> struct is_move_assignable: is_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};
template<typename T> struct is_trivially_move_assignable: is_trivially_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};
template<typename T> struct is_nothrow_move_assignable: is_nothrow_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};

template<typename T> struct has_virtual_destructor: bool_constant<__has_virtual_destructor(T)> {};

template<typename T>
struct _is_referenceable_impl {
    template<typename T1, typename = T1&>
    static true_type _test(int);

    template<typename>
    static false_type _test(...);

    using type = decltype(_test<T>(0));
};

template<typename T>
struct _is_referenceable: _is_referenceable_impl<T>::type {};

template<typename T, typename U>
struct _is_swappable_with_impl {
    template<typename T1, typename U1,
             typename = decltype(swap(declval<T1>(), declval<U1>()))>
    static true_type _test(int);

    template<typename, typename>
    static false_type _test(...);

    using type = _and_<decltype((_test<T, U>(0))), decltype((_test<U, T>(0)))>;
};

template<typename T, typename U, bool NotVoid = _or_<is_void<T>, is_void<U>>::value>
struct _is_swappable_with_helper: _is_swappable_with_impl<T, U>::type {};

template<typename T, typename U>
struct _is_swappable_with_helper<T, U, true>: false_type {};

template<typename T, typename U>
struct is_swappable_with: _is_swappable_with_helper<T, U> {};

template<typename T>
struct is_swappable: conditional_t<_is_referenceable<T>::value,
                                   is_swappable_with<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>,
                                   false_type> {};

template<typename T, typename U, bool>
struct _is_nothrow_swappable_helper: bool_constant<noexcept(swap(declval<T>(), declval<U>())) &&
                                                   noexcept(swap(declval<U>(), declval<T>()))> {};

template<typename T, typename U>
struct _is_nothrow_swappable_helper<T, U, false>: false_type {};

template<typename T, typename U>
struct is_nothrow_swappable_with: _is_nothrow_swappable_helper<T, U, is_swappable_with<T, U>::value> {};

template<typename T>
struct is_nothrow_swappable: conditional_t<_is_referenceable<T>::value,
                                            is_nothrow_swappable_with<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>,
                                            false_type> {};

template<typename T> struct alignment_of: integral_constant<size_t, alignof(T)> {};

template<typename T>           struct rank:       integral_constant<size_t, 0> {};
template<typename T>           struct rank<T[]>:  integral_constant<size_t, rank<T>::value + 1> {};
template<typename T, size_t N> struct rank<T[N]>: integral_constant<size_t, rank<T>::value + 1> {};

template<typename T, unsigned N = 0>       struct extent:          integral_constant<size_t, 0> {};
template<typename T>                       struct extent<T[], 0>:  integral_constant<size_t, 0> {};
template<typename T, unsigned N>           struct extent<T[], N>:  extent<T, N - 1> {};
template<typename T, size_t I>             struct extent<T[I], 0>: integral_constant<size_t, I> {};
template<typename T, size_t I, unsigned N> struct extent<T[I], N>: extent<T, N - 1> {};

template<typename T, typename U> struct is_same:      false_type {};
template<typename T>             struct is_same<T, T>: true_type {};

template<typename Base, typename Derived> struct is_base_of: bool_constant<__is_base_of(Base, Derived)> {};

template<typename T> struct remove_const             { using type = T; };
template<typename T> struct remove_const<const T>    { using type = T; };

template<typename T> struct remove_volatile                { using type = T; };
template<typename T> struct remove_volatile<volatile T>    { using type = T; };

template<typename T> struct remove_cv { using type = remove_volatile_t<remove_const_t<T>>; };

template<typename T> struct add_cv { using type = const volatile T; };
template<typename T> struct add_const { using type = const T; };
template<typename T> struct add_volatile { using type = volatile T; };

template<typename T> struct remove_reference      { using type = T; };
template<typename T> struct remove_reference<T&>  { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };

template<typename T, typename = void> struct _add_lvalue_reference_helper { using type = T ; };
template<typename T> struct _add_lvalue_reference_helper<T, void_t<T&>>   { using type = T&; };
template<typename T> struct add_lvalue_reference: _add_lvalue_reference_helper<T> {};

template<typename T, typename = void> struct _add_rvalue_reference_helper { using type = T  ; };
template<typename T> struct _add_rvalue_reference_helper<T, void_t<T&&>>  { using type = T&&; };
template<typename T> struct add_rvalue_reference: _add_rvalue_reference_helper<T> {};

template<typename T> struct _make_signed_i           { using type = T               ; };
template<> struct _make_signed_i<unsigned char>      { using type = signed char     ; };
template<> struct _make_signed_i<unsigned short>     { using type = signed short    ; };
template<> struct _make_signed_i<unsigned int>       { using type = signed int      ; };
template<> struct _make_signed_i<unsigned long>      { using type = signed long     ; };
template<> struct _make_signed_i<unsigned long long> { using type = signed long long; };
template<> struct _make_signed_i<char>               { using type = signed char     ; };
template<> struct _make_signed_i<wchar_t>            { using type = conditional_t<sizeof(wchar_t) == 2, short, int>; };
#if _ALA_ENABLE_INT128T
template<> struct _make_signed_i<__uint128_t>        { using type = __int128_t      ; };
#endif

template<typename T, bool = is_enum<T>::value> struct _make_signed_helper
{ using type = _copy_cv_t<T, make_signed_t<underlying_type_t<T>>>; };

template<typename T> struct _make_signed_helper<T, false>
{ using type = _copy_cv_t<T, typename _make_signed_i<remove_cv_t<T>>::type>; };

template<typename T> struct make_signed: _make_signed_helper<T> {};

template<typename T> struct _make_unsigned_i         { using type = T                 ; };
template<> struct _make_unsigned_i<signed char>      { using type = unsigned char     ; };
template<> struct _make_unsigned_i<signed short>     { using type = unsigned short    ; };
template<> struct _make_unsigned_i<signed int>       { using type = unsigned int      ; };
template<> struct _make_unsigned_i<signed long>      { using type = unsigned long     ; };
template<> struct _make_unsigned_i<signed long long> { using type = unsigned long long; };
template<> struct _make_unsigned_i<char>             { using type = unsigned char     ; };
template<> struct _make_unsigned_i<wchar_t>          { using type = conditional_t<sizeof(wchar_t) == 2, unsigned short, unsigned int>; };
#if _ALA_ENABLE_INT128T
template<> struct _make_unsigned_i<__int128_t>       { using type = __uint128_t       ; };
#endif

template<typename T, bool = is_enum<T>::value> struct _make_unsigned_helper
{ using type = _copy_cv_t<T, make_unsigned_t<underlying_type_t<T>>>; };

template<typename T> struct _make_unsigned_helper<T, false>
{ using type = _copy_cv_t<T, typename _make_unsigned_i<remove_cv_t<T>>::type>; };

template<typename T> struct make_unsigned: _make_unsigned_helper<T> {};

template<typename T>           struct remove_extent       { using type = T; };
template<typename T>           struct remove_extent<T[]>  { using type = T; };
template<typename T, size_t N> struct remove_extent<T[N]> { using type = T; };

template<typename T>           struct remove_all_extents       { using type = T                      ; };
template<typename T>           struct remove_all_extents<T[]>  { using type = remove_all_extents_t<T>; };
template<typename T, size_t N> struct remove_all_extents<T[N]> { using type = remove_all_extents_t<T>; };

template<typename T> struct remove_pointer                    { using type = T; };
template<typename T> struct remove_pointer<T*>                { using type = T; };
template<typename T> struct remove_pointer<T* const>          { using type = T; };
template<typename T> struct remove_pointer<T* volatile>       { using type = T; };
template<typename T> struct remove_pointer<T* const volatile> { using type = T; };

template<typename T, typename = void> struct _add_pointer_impl                 { using type = T;   };
template<typename T>                  struct _add_pointer_impl<T, void_t<T *>> { using type = T *; };
template<typename T> struct add_pointer: _add_pointer_impl<remove_reference_t<T>> {};

template<size_t Size, size_t Align = 1, bool = (Size < Align << 1)> struct _alignshl;
template<size_t Size, size_t Align, bool> struct _alignshl: _alignshl<Size, Align << 1> {};
template<size_t Size, size_t Align> struct _alignshl<Size, Align, true>: integral_constant<size_t, Align> {};

template<size_t Size>
struct _maxalign: _min_<_alignshl<Size>,
                        integral_constant<size_t, alignof(max_align_t)>> {};

template<size_t Size, size_t Align = _maxalign<Size>::value>
struct aligned_storage {
    struct type {
        alignas(Align) unsigned char _data[Size];
    };
};

template<size_t Size, typename... Ts>
struct aligned_union {
    static_assert(sizeof...(Ts) != 0, "Undefined behaviour");
    constexpr static size_t alignment_value = _maximal_<integral_constant<size_t, alignof(Ts)>...>::value;
    using type = aligned_storage_t<Size, alignment_value>;
};

template<typename T>
struct _decay_helper: conditional_t<
                          is_array<T>::value,
                          add_pointer<remove_extent_t<T>>,
                          conditional_t<
                              is_function<T>::value,
                              add_pointer<T>,
                              remove_cv<T>>> {};

template<typename T>
struct decay: _decay_helper<remove_reference_t<T>> {};

template< typename T >
struct remove_cvref { using type = remove_cv_t<remove_reference_t<T>>; };

template<bool B, typename T> struct enable_if {};
template<typename T> struct enable_if<true, T> { using type = T; };

template<typename T1, typename T2>
using _cond_tp_t = decay_t<decltype(false ? declval<T1>(): declval<T2>())>;

template<typename T1, typename T2>
using _cond_ref_t = decltype(false ? declval<T1 (&)()>()(): declval<T2 (&)()>()());

template<typename T>
using _clref_t = add_lvalue_reference_t<const remove_reference_t<T>>;

template<typename T1, typename T2, typename = void>
struct _common_type_2_impl {};

template<typename T1, typename T2>
struct _common_type_2_impl<T1, T2, void_t<_cond_ref_t<_clref_t<T1>, _clref_t<T2>>>>
{ using type = decay_t<_cond_ref_t<_clref_t<T1>, _clref_t<T2>>>; };

template<typename T1, typename T2, typename = void>
struct _common_type_2_sfinae: _common_type_2_impl<T1, T2> {};

template<typename T1, typename T2>
struct _common_type_2_sfinae<T1, T2, void_t<_cond_tp_t<T1, T2>>>
{ using type = _cond_tp_t<T1, T2>; };

template<typename T1, typename T2, typename = void>
struct _common_type_2_std: _common_type_2_sfinae<T1, T2> {};

template<typename T1, typename T2>
struct _common_type_2_std<T1, T2, void_t<std::common_type<T1, T2>>>
    : std::common_type<T1, T2> {};

template<typename T1, typename T2>
struct _common_type_2: conditional_t<is_same<T1, decay_t<T1>>::value &&
                                     is_same<T2, decay_t<T2>>::value,
                                     _common_type_2_std<T1, T2>,
                                     common_type<decay_t<T1>, decay_t<T2>>> {};

template<typename, typename T1, typename T2, typename... Ts>
struct _common_type_n_sfinae {};

template<typename T1, typename T2, typename... Ts>
struct _common_type_n_sfinae<void_t<common_type_t<T1, T2>>, T1, T2, Ts...>
    : common_type<common_type_t<T1, T2>, Ts...> {};

template<typename T1, typename T2, typename... Ts>
struct _common_type_n: _common_type_n_sfinae<void, T1, T2, Ts...> {};

template<typename... Ts> struct common_type;
template<> struct common_type<> {};
template<typename T> struct common_type<T>: common_type<T, T> {};
template<typename T1, typename T2> struct common_type<T1, T2>: _common_type_2<T1, T2> {};
template<typename T1, typename T2, typename... Ts>
struct common_type<T1, T2, Ts...>: _common_type_n<T1, T2, Ts...> {};

template<typename T, bool = is_enum<T>::value> struct _underlying_type_helper {};
template<typename T> struct _underlying_type_helper<T, true> { using type = __underlying_type(T); };

template<typename T> struct underlying_type: _underlying_type_helper<T> {};

template<typename Void, typename, typename...>
struct _invoke_result_sfinae {};

template<typename Fn, typename... Args>
struct _invoke_result_sfinae<
    void_t<decltype(ala::invoke(declval<Fn>(), declval<Args>()...))>, Fn, Args...> {
    using type = decltype(ala::invoke(declval<Fn>(), declval<Args>()...));
};

template<typename Fn, typename... Args>
struct invoke_result: _invoke_result_sfinae<void, Fn, Args...> {};

template<typename>
struct result_of;

template<typename Fn, typename... Args>
struct result_of<Fn(Args...)>: invoke_result<Fn, Args...> {};

template<typename Result, typename Ret, typename = void>
struct _is_invocable_r_impl: false_type {};

template<typename Result, typename Ret>
struct _is_invocable_r_impl<Result, Ret, void_t<typename Result::type>>
    : _or_<is_void<Ret>, is_convertible<typename Result::type, Ret>> {};

template<typename Fn, typename... Args>
struct is_invocable: _is_invocable_r_impl<invoke_result<Fn, Args...>, void> {};

template<typename Ret, typename Fn, typename... Args>
struct is_invocable_r: _is_invocable_r_impl<invoke_result<Fn, Args...>, Ret> {};

template<typename Fn, typename... Args>
struct _is_nt_invocable: bool_constant<noexcept(ala::invoke(declval<Fn>(), declval<Args>()...))> {};

template<typename Result, typename Ret, typename = void>
struct _is_nt_invocable_r_impl: false_type {};

template<typename Result, typename Ret>
struct _is_nt_invocable_r_impl<Result, Ret, void_t<typename Result::type>>
    : _or_<is_void<Ret>,
           _and_<is_convertible<typename Result::type, Ret>,
                 is_nothrow_constructible<Ret, typename Result::type>>> {};

template<typename Fn, typename... Args>
struct is_nothrow_invocable: _and_<is_invocable<Fn, Args...>, _is_nt_invocable<Fn, Args...>> {};

template<typename Ret, typename Fn, typename... Args>
struct is_nothrow_invocable_r: _and_<is_invocable_r<Ret, Fn, Args...>,
                                     _is_nt_invocable<Fn, Args...>,
                                     _is_nt_invocable_r_impl<invoke_result<Fn, Args...>, Ret>> {};

template<typename... B> struct conjunction: _and_<B...> {};
template<typename... B> struct disjunction: _or_<B...> {};
template<typename B>    struct negation   : _not_<B> {};

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

// C++20

template<typename From, typename To,
        bool = _or_<is_void<From>, is_function<To>, is_array<To>>::value,
        bool = is_convertible<From, To>::value>
struct _is_nt_convertible_helper: false_type {};

template<typename From, typename To>
struct _is_nt_convertible_helper<From, To, true, true>: true_type {};

template<typename From, typename To>
struct _is_nt_convertible_helper<From, To, false, true>
    : bool_constant<noexcept(_is_convertible_impl<From, To>::template _convert<To>(declval<From>()))> {};

template<typename From, typename To>
struct is_nothrow_convertible: _is_nt_convertible_helper<From, To> {};

template<typename T> struct type_identity { using type = T; };

template<typename T> struct unwrap_reference { using type = T; };
template<typename T> struct unwrap_reference<reference_wrapper<T>> { using type = T&; };

template<typename T> struct unwrap_ref_decay: unwrap_reference<decay_t<T>> {};

template<typename T>           struct is_bounded_array:      false_type {};
template<typename T>           struct is_bounded_array<T[]>: false_type {};
template<typename T, size_t N> struct is_bounded_array<T[N]>: true_type {};

template<typename T>           struct is_unbounded_array:       false_type {};
template<typename T>           struct is_unbounded_array<T[]>:   true_type {};
template<typename T, size_t N> struct is_unbounded_array<T[N]>: false_type {};

template<typename T>
using _rm_ref_t = remove_reference_t<T>;

template<typename T1, typename T2,
         typename Y1 = _rm_ref_t<T1>, typename Y2 = _rm_ref_t<T2>,
         typename = void>
struct _sc_lref {};

template<typename T1, typename T2, typename Y1, typename Y2>
struct _sc_lref<T1, T2, Y1, Y2, 
                void_t<_cond_ref_t<_copy_cv_t<Y1, Y2>&, _copy_cv_t<Y2, Y1>&>>>
{ using type = _cond_ref_t<_copy_cv_t<Y1, Y2>&, _copy_cv_t<Y2, Y1>&>; };

template<typename T1, typename T2, typename C,
          bool = is_convertible<T1, C>::value &&
                 is_convertible<T2, C>::value>
struct _sc_rref_helper {};

template<typename T1, typename T2, typename C>
struct _sc_rref_helper<T1, T2, C, true> { using type = C; };

template<typename T1, typename T2,
         typename Y1 = _rm_ref_t<T1>, typename Y2 = _rm_ref_t<T2>,
         typename = void>
struct _sc_rref {};

template<typename T1, typename T2, typename Y1, typename Y2>
struct _sc_rref<T1, T2, Y1, Y2, void_t<typename _sc_lref<Y1&, Y2&>::type>>
    : _sc_rref_helper<T1, T2, _rm_ref_t<typename _sc_lref<Y1&, Y2&>::type>&&> {};

template<typename T1, typename T2, typename C,
         bool = is_convertible<T1, C>::value &&
                is_convertible<T2, C>::value>
struct _sc_rlref_helper {};

template<typename T1, typename T2, typename C>
struct _sc_rlref_helper<T1, T2, C, true> { using type = C; };

template<typename T1, typename T2,
         typename Y1 = _rm_ref_t<T1>, typename Y2 = _rm_ref_t<T2>,
         typename = void>
struct _sc_rlref {};

template<typename T1, typename T2, typename Y1, typename Y2>
struct _sc_rlref<T1, T2, Y1, Y2,
                 void_t<typename _sc_lref<const Y1&, Y2&>::type>>
    : _sc_rlref_helper<T1, T2, typename _sc_lref<const Y1&, Y2&>::type> {};

template<typename T1, typename T2,
         bool = is_rvalue_reference<T1>::value &&
                is_lvalue_reference<T2>::value>
struct _sc_ref_mix: _sc_ref_mix<T2, T1> {};

template<typename T1, typename T2>
struct _sc_ref_mix<T1, T2, true>: _sc_rlref<T1, T2> {};

template<typename T1, typename T2,
         bool = is_reference<T1>::value &&
                is_reference<T2>::value>
struct _sc_reference {};

template<typename T1, typename T2>
struct _sc_reference<T1, T2, true>
    : conditional_t<
          is_lvalue_reference<T1>::value && is_lvalue_reference<T2>::value,
          _sc_lref<T1, T2>,
          conditional_t<
              is_rvalue_reference<T1>::value && is_rvalue_reference<T2>::value,
              _sc_rref<T1, T2>,
              _sc_ref_mix<T1, T2>>> {};

template<typename T1, typename T2, typename = void>
struct _common_reference_2_helper: common_type<T1, T2> {};

template<typename T1, typename T2>
struct _common_reference_2_helper<T1, T2, void_t<_cond_ref_t<T1, T2>>>
{ using type = _cond_ref_t<T1, T2>; };

template<typename, typename,
         template<typename> class, template<typename> class>
struct basic_common_reference {};

template<typename T1, typename T2>
using _basic_common_ref_t = typename basic_common_reference<
    remove_cvref_t<T1>, remove_cvref_t<T2>,
    _get_cvref<T1>::template rebind, _get_cvref<T2>::template rebind>::type;

template<typename T1, typename T2, typename = void>
struct _common_reference_2_sfinae: _common_reference_2_helper<T1, T2> {};

template<typename T1, typename T2>
struct _common_reference_2_sfinae<T1, T2, void_t<_basic_common_ref_t<T1, T2>>>
{ using type = _basic_common_ref_t<T1, T2>; };

template<typename T1, typename T2,
         bool = is_reference<T1>::value &&
                is_reference<T2>::value,
         typename = void>
struct _common_reference_2: _common_reference_2_sfinae<T1, T2> {};

template<typename T1, typename T2>
struct _common_reference_2<T1, T2, true, void_t<typename _sc_reference<T1, T2>::type>>
{ using type = typename _sc_reference<T1, T2>::type; };

template<typename, typename T1, typename T2, typename... Ts>
struct _common_reference_n_sfinae {};

template<typename T1, typename T2, typename... Ts>
struct _common_reference_n_sfinae<void_t<common_reference_t<T1, T2>>, T1, T2, Ts...>
    : common_reference<common_reference_t<T1, T2>, Ts...> {};

template<typename T1, typename T2, typename... Ts>
struct _common_reference_n: _common_reference_n_sfinae<void, T1, T2, Ts...> {};

template<typename... Ts> struct common_reference;
template<> struct common_reference<> {};
template<typename T> struct common_reference<T> { using type = T; };
template<typename T1, typename T2> struct common_reference<T1, T2>: _common_reference_2<T1, T2> {};
template<typename T1, typename T2, typename... Ts>
struct common_reference<T1, T2, Ts...>: _common_reference_n<T1, T2, Ts...> {};

#if _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED
inline constexpr bool is_constant_evaluated() noexcept {
    return __builtin_is_constant_evaluated();
}
#endif

// Extra

template<typename T>
struct _is_implicitly_default_constructible_impl {
    template<typename T1>
    static void _help(const T1 &);

    template<typename T1, typename = decltype(_help<T1>({}))>
    static true_type _test(int);

    template<typename T1>
    static false_type _test(...);

    using type = decltype(_test<T>(0));
};

template<typename T, bool = is_default_constructible<T>::value>
struct _is_implicitly_default_constructible_helper: false_type {};

template<typename T>
struct _is_implicitly_default_constructible_helper<T, true>
    : _is_implicitly_default_constructible_impl<T>::type {};

template<typename T>
struct is_implicitly_default_constructible
    : _is_implicitly_default_constructible_helper<T> {};

template<typename T>
struct _is_reference_wrapper_helper: false_type {};

template<typename T>
struct _is_reference_wrapper_helper<reference_wrapper<T>>: true_type {};

template<typename T>
struct is_reference_wrapper: _is_reference_wrapper_helper<remove_cvref_t<T>> {};

template<typename T, template<typename...> class>
struct is_specification: false_type {};

template<template<typename...> class Templt, typename... Args>
struct is_specification<Templt<Args...>, Templt>: true_type {};

// template<typename T, template<typename..., auto...> class>
// struct is_specification_with_varg: false_type {};

// template<template<typename..., auto...> class Templt, class... TArgs, auto... Args>
// struct is_specification_with_varg<Templt<TArgs..., Args...>, Templt>: true_type {};

} // namespace ala

// clang-format on

#ifdef _ALA_MSVC
    #pragma warning(pop)
#endif

#ifdef _ALA_CLANG
    #pragma clang diagnostic pop
#endif

#endif // HEAD