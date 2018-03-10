// surfix: _impl - aux class, _helper - aux base class, test - aux function, tester - auxfun's aux struct
#ifndef TYPETRAITS_HPP
#define TYPETRAITS_HPP

#include "ala/config.hpp"
#include "ala/detail/type_traits_declare.hpp"

namespace ala {

template <class T> add_rvalue_reference_t<T> declval() noexcept;

template <typename T, T Value>
struct integral_constant {
	static const T value = Value;
	typedef T value_type;
	typedef integral_constant<T, Value> type;

	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; }
};

template <bool B>
using bool_constant = bool_constant<B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename...> struct _or_;
template <> struct _or_<> : false_type {};
template <typename B1> struct _or_<B1> : B1 {};
template <typename B1, typename B2> struct _or_<B1, B2> : conditional_t<B1::value, B1, B2> {};
template <typename B1, typename B2, typename B3, typename... Bn>
struct _or_<B1, B2, B3, Bn...> : conditional_t<B1::value, B1, _or_<B2, B3, Bn...>> {};

template <typename...> struct _and_;
template <> struct _and_<> : true_type {};
template <typename B1> struct _and_<B1> : B1 {};
template <typename B1, typename B2> struct _and_<B1, B2> : conditional_t<B1::value, B2, B1>::type {};
template <typename B1, typename B2, typename B3, typename... Bn>
struct _and_<B1, B2, B3, Bn...> : conditional_t<B1::value, _and_<B2, B3, Bn...>, B1> {};

template <typename B>
struct _not_ : bool_constant<!bool(B::value)> {};

template <class T> struct is_void : is_same<void, remove_cv_t<T>> {};

template <class T> struct is_null_pointer : is_same<nullptr_t, remove_cv_t<T>> {};

template <typename> struct _is_integral_helper :            false_type {};
template <> struct _is_integral_helper<bool> :               true_type {};
template <> struct _is_integral_helper<char> :               true_type {};
template <> struct _is_integral_helper<signed char> :        true_type {};
template <> struct _is_integral_helper<unsigned char> :      true_type {};
template <> struct _is_integral_helper<wchar_t> :            true_type {};
template <> struct _is_integral_helper<char16_t> :           true_type {};
template <> struct _is_integral_helper<char32_t> :           true_type {};
template <> struct _is_integral_helper<short> :              true_type {};
template <> struct _is_integral_helper<unsigned short> :     true_type {};
template <> struct _is_integral_helper<int> :                true_type {};
template <> struct _is_integral_helper<unsigned int> :       true_type {};
template <> struct _is_integral_helper<long> :               true_type {};
template <> struct _is_integral_helper<unsigned long> :      true_type {};
template <> struct _is_integral_helper<long long> :          true_type {};
template <> struct _is_integral_helper<unsigned long long> : true_type {};
template <typename T> struct is_integral : _is_integral_helper<remove_cv_t<T>> {};

template <typename> struct _is_floating_point_helper :     false_type {};
template <> struct _is_floating_point_helper<float> :       true_type {};
template <> struct _is_floating_point_helper<double> :      true_type {};
template <> struct _is_floating_point_helper<long double> : true_type {};
template <typename T> struct is_floating_point : _is_floating_point_helper<remove_cv_t<T>> {};

template <typename T> struct is_array :                      false_type {};
template <typename T> struct is_array<T[]> :                  true_type {};
template <typename T, size_t Size> struct is_array<T[Size]> : true_type {};

template <typename T> struct is_enum : bool_constant<__is_enum(T)> {};

template <typename T> struct is_union : bool_constant<__is_union(T)> {};

template <typename T> struct is_class : bool_constant<__is_class(T)> {};

template <class> struct is_function : false_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...)> :                     true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...)> :                true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) const> :               true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) volatile> :            true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) const volatile> :      true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) const> :          true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) volatile> :       true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) const volatile> : true_type {};

template <class Ret, class... Args> struct is_function<Ret(Args...) &> :                      true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) const &> :                true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) volatile &> :             true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) const volatile &> :       true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) &> :                 true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) const &> :           true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) volatile &> :        true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) const volatile &> :  true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) &&> :                     true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) const &&> :               true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) volatile &&> :            true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args...) const volatile &&> :      true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) &&> :                true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) const &&> :          true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) volatile &&> :       true_type {};
template <class Ret, class... Args> struct is_function<Ret(Args..., ...) const volatile &&> : true_type {};

#if __cplusplus >= 201703L
template<class Ret, class... Args> struct is_function<Ret(Args...) noexcept> :                      true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) noexcept> :                   true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const noexcept> :                true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile noexcept> :             true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile noexcept> :       true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) const noexcept> :             true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) volatile noexcept> :          true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) const volatile noexcept> :    true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) & noexcept> :                    true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const & noexcept> :              true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile & noexcept> :           true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile & noexcept> :     true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) & noexcept> :                 true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) const & noexcept> :           true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) volatile & noexcept> :        true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) const volatile & noexcept> :  true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) && noexcept> :                   true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const && noexcept> :             true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile && noexcept> :          true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile && noexcept> :    true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) && noexcept> :                true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) const && noexcept> :          true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) volatile && noexcept> :       true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args..., ...) const volatile && noexcept> : true_type {};
#endif

template <class T> struct _is_pointer_helper :     false_type {};
template <class T> struct _is_pointer_helper<T *> : true_type {};
template <class T> struct is_pointer : _is_pointer_helper<remove_cv_t<T>> {};

template <class T> struct is_lvalue_reference :     false_type {};
template <class T> struct is_lvalue_reference<T&> : true_type {};

template <class T> struct is_rvalue_reference :      false_type {};
template <class T> struct is_rvalue_reference<T&&> : true_type {};

template <typename>                   struct _is_member_object_pointer_helper : false_type {};
template <typename T, typename Class> struct _is_member_object_pointer_helper<T Class::*> : _not_<is_function<T>> {};
template <typename T> struct is_member_object_pointer : _is_member_object_pointer_helper<remove_cv_t<T>> {};

template <typename>                   struct _is_member_function_pointer_helper : false_type {};
template <typename T, typename Class> struct _is_member_function_pointer_helper<T Class::*> : bool_constant<is_function_v<T>> {};
template <typename T> struct is_member_function_pointer : _is_member_function_pointer_helper<remove_cv_t<T>> {};

template <typename T> struct is_reference : _or_<is_lvalue_reference<T>, is_rvalue_reference<T>> {};

template <typename T> struct is_arithmetic : _or_<is_integral<T>, is_floating_point<T>> {};

template <typename T> struct is_fundamental : _or_<is_arithmetic<T>, is_void<T>, is_null_pointer<T>> {};

template <typename T> struct is_object : _not_<_or_<is_function<T>, is_reference<T>, is_void<T>>> {};

template <typename T> struct is_scalar : _or_<is_arithmetic<T>, is_enum<T>, is_pointer<T>, is_member_pointer<T>, is_null_pointer<T>> {};

template <typename T> struct is_compound : _not_<is_fundamental<T>> {};

template <typename T>                 struct _is_member_pointer_helper : false_type {};
template <typename T, typename Class> struct _is_member_pointer_helper<T Class::*> : true_type {};
template <typename T> struct is_member_pointer : _is_member_pointer_helper<remove_cv_t<T>> {};

template <class T> struct is_const          : false_type {};
template <class T> struct is_const<const T> : true_type {};

template <class T> struct is_volatile             : false_type {};
template <class T> struct is_volatile<volatile T> : true_type {};

template <typename T> struct is_trivial : bool_constant<__is_trivial(T)> {};

template <typename T> struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};

template <typename T> struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};

template <typename T> struct is_pod : bool_constant<__is_pod(T)> {};

#if _cplusplus < 201703L
template <typename T> struct is_literal_type : bool_constant<__is_literal_type(T)> {};
#endif

#if _cplusplus >= 201703L
#ifdef __clang__
template <class T> struct is_aggregate : bool_constant<__is_aggregate(T)> {};

#elif defined __GNUC__
template <typename T> struct has_unique_object_representations : bool_constant<__has_unique_object_representations(remove_cv_t<remove_all_extents_t<T>>)> {};

template <typename T> struct is_aggregate : bool_constant<__is_aggregate(remove_cv_t<T>)> {};

#elif defined _MSC_VER
template <typename T> struct has_unique_object_representations : bool_constant<__has_unique_object_representations(T)> {};

#endif
#endif

template <typename T> struct is_empty : bool_constant<__is_empty(T)> {};

template <typename T> struct is_polymorphic : bool_constant<__is_polymorphic(T)> {};

template <typename T> struct is_abstract : bool_constant<__is_abstract(T)> {};

template <typename T> struct is_final : bool_constant<__is_final(T)> {};

template <typename T,bool = is_arithmetic_v<T>> struct _is_unsigned_helper : bool_constant<T(0) < T(-1)> {};
template <typename T> 							struct _is_unsigned_helper<T, false> : false_type {};
template <typename T> struct is_unsigned : _is_unsigned_helper<T> {};

template <typename T,bool = is_arithmetic_v<T>> struct _is_signed_helper : bool_constant<T(-1) < T(0)> {};
template <typename T>                           struct _is_signed_helper<T,false> : false_type {};
template <typename T> struct is_signed : _is_signed_helper<T> {};

#if defined(__GNUC__) || defined(__clang__) // Use GCC
#include "ala/detail/type_traits_detail.hpp"
#else // Use MSVC or clang
template <class T, class... _Args> struct is_constructible
 : bool_constant<__is_constructible(T, _Args...)> {};

template <class T, class... _Args> struct is_trivially_constructible
 : bool_constant<__is_trivially_constructible(T, _Args...)> {};

template <class T, class... _Args> struct is_nothrow_constructible
 : bool_constant<__is_nothrow_constructible(T, _Args...)> {};

template<class _To, class _From> struct is_assignable
 : bool_constant<__is_assignable(_To, _From)> {};

template<class _To, class _From> struct is_trivially_assignable
 : bool_constant<__is_trivially_assignable(_To, _From)> {};

template<class _To, class _From> struct is_nothrow_assignable
 : bool_constant<__is_nothrow_assignable(_To, _From)> {};

template <typename T> struct is_destructible
 : bool_constant<__is_destructible(T)> {};

template <typename T> struct is_trivially_destructible
 : bool_constant<__is_trivially_destructible(T)> {};

template <typename T> struct is_nothrow_destructible
 : bool_constant<__is_nothrow_destructible(T)> {};

#endif
#if !defined(__clang__) && defined(_MSC_VER)
template <typename T> struct is_default_constructible : is_constructible<T> {};
template <typename T> struct is_nothrow_default_constructible : is_nothrow_constructible<T> {};
#endif
template <typename T> struct is_trivially_default_constructible : is_trivially_constructible<T> {};

template <typename T> struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <typename T> struct is_trivially_copy_constructible : is_trivially_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <typename T> struct is_nothrow_copy_constructible : is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

template <typename T> struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};
template <typename T> struct is_trivially_move_constructible : is_trivially_constructible<T, add_rvalue_reference_t<T>> {};
template <typename T> struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};

template <class T> struct is_copy_assignable : is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};
template <class T> struct is_trivially_copy_assignable : is_trivially_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};
template <class T> struct is_nothrow_copy_assignable : is_nothrow_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template <class T> struct is_move_assignable : is_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};
template <class T> struct is_trivially_move_assignable : is_trivially_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};
template <class T> struct is_nothrow_move_assignable : is_nothrow_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};

template<typename T> struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};

template <typename T>
ALA_FORCEINLINE void swap(T &a, T &b) noexcept;

struct unused {};

template <typename T, typename U, bool IsVoid = (is_void_v<T> || is_void_v<U>)>
struct _is_swappable_with_helper {
	static const bool value =
	    !is_same_v<decltype(swap(declval<T>(), declval<U>())), unused> &&
	    !is_same_v<decltype(swap(declval<U>(), declval<T>())), unused>;
};

template <typename T, typename U>
struct _is_swappable_with_helper<T, U, true> { static const bool value = false; };

template <typename T, typename U>
struct is_swappable_with : bool_constant<_is_swappable_with_helper<T, U>::value> {};

template <class T>
struct alignment_of : integral_constant<size_t, alignof(T)> {};

template <class T>
struct rank : integral_constant<size_t, 0> {};

template <class T>
struct rank<T[]> : integral_constant<size_t, rank_v<T> + 1> {};

template <class T, size_t N>
struct rank<T[N]> : integral_constant<size_t, rank_v<T> + 1> {};

template <class T, unsigned N = 0>
struct extent : integral_constant<size_t, 0> {};

template <class T>
struct extent<T[], 0> : integral_constant<size_t, 0> {};

template <class T, unsigned N>
struct extent<T[], N> : extent<T, N-1> {};

template <class T, size_t I>
struct extent<T[I], 0> : integral_constant<size_t, I> {};

template <class T, size_t I, unsigned N>
struct extent<T[I], N> : extent<T, N-1> {};

template <class T, class U> struct is_same : false_type {};
template <class T> struct is_same<T, T> : true_type {};

template <typename Base, typename Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};

template <typename From, typename To, bool = _or_<is_void<From>, is_function<To>, is_array<To>>::value>
struct _is_convertible_impl { typedef is_void<To> type; };

template <typename From, typename To>
struct _is_convertible_impl<From, To, false> {
  private:
    template <typename To1>
	static void _test_aux(To1);

    template <typename From1, typename To1, typename = decltype(_test_aux<To1>(declval<From1>()))>
	static true_type _test(int);

    template <typename, typename>
	static false_type _test(...);
  public:
    typedef decltype(_test<From, To>(0)) type;
};

template <typename From, typename To>
struct is_convertible : _is_convertible_impl<From, To>::type {};

template <typename T>           struct remove_const             { typedef T type;    };
template <typename T>           struct remove_const<const T>    { typedef T type;    };
template <typename T>           struct remove_const<const T[]>  { typedef T type[];  };
template <typename T, size_t N> struct remove_const<const T[N]> { typedef T type[N]; };

template <typename T>           struct remove_volatile                { typedef T type;    };
template <typename T>           struct remove_volatile<volatile T>    { typedef T type;    };
template <typename T>           struct remove_volatile<volatile T[]>  { typedef T type[];  };
template <typename T, size_t N> struct remove_volatile<volatile T[N]> { typedef T type[N]; };

template <typename T> struct remove_cv { typedef remove_volatile_t<remove_const_t<T>> type; };

template <typename T> struct add_cv { typedef const volatile T type; };

template <typename T> struct add_const { typedef const T type; };

template <typename T> struct add_volatile { typedef volatile T type; };

template <typename T> struct remove_reference      { typedef T type; };
template <typename T> struct remove_reference<T&>  { typedef T type; };
template <typename T> struct remove_reference<T&&> { typedef T type; };

template <class T, class = void>
struct _add_reference_impl { typedef T _l; typedef T _r; };
template <class T>
struct _add_reference_impl<T, void_t<T&>> { typedef T& _l; typedef T&& _r; };

template <class T>
struct add_lvalue_reference { typedef typename _add_reference_impl<T>::_l type; };
template <class T>
struct add_rvalue_reference { typedef typename _add_reference_impl<T>::_r type; };

template <typename T> struct make_signed                 { typedef T                      type; };
template <> struct make_signed<unsigned char>            { typedef signed char            type; };
template <> struct make_signed<const unsigned char>      { typedef const signed char      type; };
template <> struct make_signed<unsigned short>           { typedef signed short           type; };
template <> struct make_signed<const unsigned short>     { typedef const signed short     type; };
template <> struct make_signed<unsigned int>             { typedef signed int             type; };
template <> struct make_signed<const unsigned int>       { typedef const signed int       type; };
template <> struct make_signed<unsigned long>            { typedef signed long            type; };
template <> struct make_signed<const unsigned long>      { typedef const signed long      type; };
template <> struct make_signed<unsigned long long>       { typedef signed long long       type; };
template <> struct make_signed<const unsigned long long> { typedef const signed long long type; };
template <> struct make_signed<char>                     { typedef signed char            type; };
template <> struct make_signed<const char>               { typedef signed char            type; };

#if (defined(__WCHAR_MAX__) && (__WCHAR_MAX__ == 4294967295U))
template <> struct make_signed<wchar_t> { typedef int32_t type; };
#elif (defined(__WCHAR_MAX__) && (__WCHAR_MAX__ == 65535))
template <> struct make_signed<wchar_t> { typedef int16_t type; };
#endif

template <typename T> struct make_unsigned               { typedef T                        type; };
template <> struct make_unsigned<signed char>            { typedef unsigned char            type; };
template <> struct make_unsigned<const signed char>      { typedef const unsigned char      type; };
template <> struct make_unsigned<signed short>           { typedef unsigned short           type; };
template <> struct make_unsigned<const signed short>     { typedef const unsigned short     type; };
template <> struct make_unsigned<signed int>             { typedef unsigned int             type; };
template <> struct make_unsigned<const signed int>       { typedef const unsigned int       type; };
template <> struct make_unsigned<signed long>            { typedef unsigned long            type; };
template <> struct make_unsigned<const signed long>      { typedef const unsigned long      type; };
template <> struct make_unsigned<signed long long>       { typedef unsigned long long       type; };
template <> struct make_unsigned<const signed long long> { typedef const unsigned long long type; };
template <> struct make_unsigned<char>                   { typedef unsigned char            type; };
template <> struct make_unsigned<const char>             { typedef unsigned char            type; };

#if defined(__WCHAR_MAX__) && (__WCHAR_MAX__ != 4294967295U)
template <> struct make_unsigned<wchar_t> { typedef uint32_t type; };
#elif defined(__WCHAR_MAX__) && (__WCHAR_MAX__ != 65535)
template <> struct make_unsigned<wchar_t> { typedef uint16_t type; };
#endif

template <class T>           struct remove_extent       { typedef T type; };
template <class T>           struct remove_extent<T[]>  { typedef T type; };
template <class T, size_t N> struct remove_extent<T[N]> { typedef T type; };

template <class T>           struct remove_all_extents       { typedef T                       type; };
template <class T>           struct remove_all_extents<T[]>  { typedef remove_all_extents_t<T> type; };
template <class T, size_t N> struct remove_all_extents<T[N]> { typedef remove_all_extents_t<T> type; };

template <class T> struct remove_pointer                    {typedef T type;};
template <class T> struct remove_pointer<T*>                {typedef T type;};
template <class T> struct remove_pointer<T* const>          {typedef T type;};
template <class T> struct remove_pointer<T* volatile>       {typedef T type;};
template <class T> struct remove_pointer<T* const volatile> {typedef T type;};

template <class T, bool is_fun = false> struct _add_pointer_helper             { typedef remove_reference_t<T>* type; };
template <class T>                struct _add_pointer_helper<T, true>               { typedef T type;                 };
template <class T, class... Args> struct _add_pointer_helper<T(Args...), true>      { typedef T(*type)(Args...);      };
template <class T, class... Args> struct _add_pointer_helper<T(Args..., ...), true> { typedef T(*type)(Args..., ...); };
template <class T> struct add_pointer : _add_pointer_helper<T, is_function_v<T>> {};

template <size_t Len, size_t Align = 8>
struct aligned_storage {
	struct type {
		alignas(Align) unsigned char data[Len];
	};
};

template <size_t... Vals> struct _maximum;
template <>               struct _maximum<> : integral_constant<size_t, 0> {};
template <size_t Val>     struct _maximum<Val> : integral_constant<size_t, Val> {};
template <size_t First, size_t Second, size_t... Rest> struct _maximum<First, Second, Rest...> : _maximum<(First < Second ? Second : First), Rest...> {};

template <size_t Len, class... Types>
struct aligned_union {
	static constexpr size_t alignment_value = _maximum<alignof(Types)...>::value;
	struct type {
		alignas(alignment_value) char _s[_maximum<Len, sizeof(Types)...>::value];
	};
};

template <class T>
struct decay {
  private:
	typedef remove_reference_t<T> U;
  public:
	typedef conditional_t<is_array_v<U>, remove_extent_t<U> *,
			conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>> type;
};

template < typename T >
struct remove_cvref { typedef remove_cv_t<remove_reference_t<T>> type; };

template <bool B, class T = void> struct enable_if {};
template <class T> struct enable_if<true, T> { typedef T type; };

template <bool B, typename T, typename F> struct conditional      { typedef T type; };
template <typename T, typename F> struct conditional<false, T, F> { typedef F type; };

template <typename... T> struct common_type;
template <typename T> struct common_type<T> { typedef decay_t<T> type; };
template <typename T, typename U> struct common_type<T, U> { typedef decay_t<decltype(true ? declval<T>() : declval<U>())> type; };
template <typename T, typename U, typename... V> struct common_type<T, U, V...> { typedef common_type_t<common_type_t<T, U>, V...> type; };

template <typename T> struct underlying_type { typedef __underlying_type(T) type; };

template <class...> struct conjunction : true_type {};
template <class Bool> struct conjunction<Bool> : Bool {};
template <class Bool, class... Bools>
struct conjunction<Bool, Bools...> : conditional_t<bool(Bool::value), conjunction<Bools...>, Bool> {};

template <class...> struct disjunction : false_type {};
template <class Bool> struct disjunction<Bool> : Bool {};
template <class Bool, class... Bools>
struct disjunction<Bool, Bools...> : conditional_t<bool(Bool::value), Bool, disjunction<Bools...>> {};


template <class B>
struct negation : bool_constant<!bool(B::value)> {};

enum class endian {
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

} // namespace ala

#endif // TYPETRAITS_HPP