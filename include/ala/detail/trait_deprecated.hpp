template <typename Base> true_type _is_base_of_test(const volatile Base*);
template <typename Base> false_type _is_base_of_test(const volatile void*);
template <typename Base, typename Derived>
using _is_base_of_tester = decltype(_is_base_of_test<Base>(declval<Derived*>()));

template <typename Base, typename Derived, typename = void>
struct _is_base_helper : true_type {};
template <typename Base, typename Derived>
struct _is_base_helper<Base, Derived, void_t<decltype(_is_base_of_tester<Base, Derived>>>
 : _is_base_of_tester<Base, Derived> { };

template <typename Base, typename Derived>
struct is_base_of : conditional_t<is_class_v<Base> && is_class_v<Derived>,
					_is_base_helper<Base, Derived>, false_type> {};

template <typename T> struct add_lvalue_reference                      { typedef T&                  type; };
template <typename T> struct add_lvalue_reference<T&>                  { typedef T&                  type; };
template <>           struct add_lvalue_reference<void>                { typedef void                type; };
template <>           struct add_lvalue_reference<const void>          { typedef const void          type; };
template <>           struct add_lvalue_reference<volatile void>       { typedef volatile void       type; };
template <>           struct add_lvalue_reference<const volatile void> { typedef const volatile void type; };

template <typename T> struct add_rvalue_reference                      { typedef T&&                 type; };
template <typename T> struct add_rvalue_reference<T&>                  { typedef T&                  type; };
template <>           struct add_rvalue_reference<void>                { typedef void                type; };
template <>           struct add_rvalue_reference<const void>          { typedef const void          type; };
template <>           struct add_rvalue_reference<volatile void>       { typedef volatile void       type; };
template <>           struct add_rvalue_reference<const volatile void> { typedef const volatile void type; };

template <typename, typename T, typename... Args>
struct _is_constructible_helper : false_type {};
template <typename T, typename... Args>
struct _is_constructible_helper<void_t<decltype(T(declval<Args>()...))>, T, Args...> : true_type {};
template <typename T, typename... Args>
struct is_constructible : _is_constructible_helper<void_t<>, T, Args...> {};
template <typename T, typename... Args>
struct is_constructible<T&, Args...> : _is_constructible_helper<void_t<>, T, Args...> {};
template <typename T, typename... Args>
struct is_constructible<T&&, Args...> : _is_constructible_helper<void_t<>, T, Args...> {};
template <typename... Args> struct is_constructible<void, Args...> : false_type {};
template <typename... Args> struct is_constructible<void const, Args...> : false_type {};
template <typename... Args> struct is_constructible<void const volatile, Args...> : false_type {};
template <typename... Args> struct is_constructible<void volatile, Args...> : false_type {};

template<typename T, typename... _Args>
struct is_trivially_constructible
 : bool_constant<(is_constructible_v<T, _Args...> && __is_trivially_constructible(remove_extent_t<T>, _Args...))> {}; // remove extent in bif to avoid crash

template <bool, typename T, typename... Args>
struct _is_nothrow_constructible_helper : false_type {};;
template <typename T, typename... Args>
struct _is_nothrow_constructible_helper<true, T, Args...>
 : bool_constant<noexcept(T(declval<Args>()...))> {};
template <typename T, typename... Args>
struct is_nothrow_constructible
 : _is_nothrow_constructible_helper<is_constructible_v<T, Args...>, remove_extent_t<T>, Args...> {};

template<class, class = void_t<>>
struct _is_destructible_helper : false_type {};
template< class T >
struct _is_destructible_helper<T, void_t<decltype(~T())>> : true_type {};
template <typename T>
struct is_destructible : _is_destructible_helper<void_t<>, T> {};

template <class _Tp>
void _implicit_conversion_to(_Tp) noexcept {}

template <bool, bool, class _Tp, class... _Args>
struct _is_nothrow_constructible_helper;
template <class _Tp, class... _Args>
struct _is_nothrow_constructible_helper<true, false, _Tp, _Args...>  : integral_constant<bool, noexcept(_Tp(declval<_Args>()...))> {};
template <class _Tp, class _Arg>
struct _is_nothrow_constructible_helper<true, true, _Tp, _Arg> : integral_constant<bool, noexcept(_implicit_conversion_to<_Tp>(declval<_Arg>()))> {};
template <class _Tp, bool _IsReference, class... _Args>
struct _is_nothrow_constructible_helper<false, _IsReference, _Tp, _Args...> : false_type {};

template <class _Tp, class... _Args>
struct is_nothrow_constructible : 
_is_nothrow_constructible_helper<is_constructible<_Tp, _Args...>::value, is_reference<_Tp>::value, _Tp, _Args...> {};
template <class _Tp, size_t _Ns>
struct is_nothrow_constructible<_Tp[_Ns]> : 
_is_nothrow_constructible_helper<is_constructible<_Tp>::value, is_reference<_Tp>::value, _Tp> {};


Wrong Code Because of No-Truncation
/*
template <typename T, typename... _Args>
struct _is_nothrow_constructible_impl : bool_constant<noexcept(T(declval<_Args>()...))> {};
template <typename T, typename _Arg>
struct _is_nothrow_constructible_impl<T, _Arg> : bool_constant<noexcept(static_cast<T>(declval<_Arg>()))> {};
template <typename T>
struct _is_nothrow_constructible_impl<T> : is_nothrow_default_constructible<T> {};

template <typename T, typename... _Args>
struct is_nothrow_constructible : bool_constant<
is_constructible_v<T, _Args...> && _is_nothrow_constructible_impl<T, _Args...>::value> {};


template <typename _Tp, bool = is_array_v<_Tp>>
struct _is_nt_default_constructible_impl;
template <typename _Tp>
struct _is_nt_default_constructible_impl<_Tp, true> : bool_constant<
_is_array_known_bounds<_Tp>::value && noexcept(remove_all_extents_t<_Tp>())> {};
template <typename _Tp>
struct _is_nt_default_constructible_impl<_Tp, false> : bool_constant<noexcept(_Tp())> {}; // Why ?

template <typename _Tp>
struct is_nothrow_default_constructible : bool_constant<
is_default_constructible_v<_Tp> && _is_nt_default_constructible_impl<remove_reference_t<_Tp>>::value> {};
*/