template <typename T>
struct _is_array_known_bounds : bool_constant<(extent_v<T> > 0)> {};

template <typename T>
struct _is_array_unknown_bounds : _and_<is_array<T>, _not_<extent<T, 0>>> {};

template <typename T>
struct _is_destructible_impl {
	template <typename T1, typename = decltype(declval<T1 &>().~T1())>
	static true_type _test(int);
	template <typename>
	static false_type _test(...);
	typedef decltype(_test<T>(0)) type;
};

template <typename T, bool = _or_<is_void<T>, _is_array_unknown_bounds<T>, is_function<T>>::value,
          bool = _or_<is_reference<T>, is_scalar<T>>::value>
struct _is_destructible_helper : false_type {};
template <typename T>
struct _is_destructible_helper<T, false, false> : _is_destructible_impl<remove_all_extents_t<T>>::type {};
template <typename T>
struct _is_destructible_helper<T, false, true> : true_type {};

template <typename T>
struct is_destructible : _is_destructible_helper<T> {};

template <typename T>
struct is_trivially_destructible : _and_<is_destructible<T>, bool_constant<__has_trivial_destructor(T)>> {};

template <class _Tp, bool = is_destructible_v<_Tp>>
struct _is_nt_destructible_helper  : false_type {};
template <class _Tp>
struct _is_nt_destructible_helper<_Tp, true> : bool_constant<noexcept(declval<_Tp>().~_Tp())> {};

template <class _Tp>
struct is_nothrow_destructible : _is_nt_destructible_helper<_Tp> {};
template <class _Tp, size_t _Ns>
struct is_nothrow_destructible<_Tp[_Ns]> : is_nothrow_destructible<_Tp> {};
template <class _Tp>
struct is_nothrow_destructible<_Tp &> : true_type {};
template <class _Tp>
struct is_nothrow_destructible<_Tp &&> : true_type {};

template <typename T>
struct _is_default_constructible_impl {
	template <typename T1, typename = decltype(T1())>
	static true_type _test(int);
	template <typename>
	static false_type _test(...);
	typedef decltype(_test<T>(0)) type;
};

template <typename T, bool = is_array_v<T>>
struct _is_default_constructible_helper;
template <typename T>
struct _is_default_constructible_helper<T, true> : _and_<_is_array_known_bounds<T>,
_not_<is_void<T>>, typename _is_default_constructible_impl<remove_all_extents_t<T>>::type> {};
template <typename T>
struct _is_default_constructible_helper<T, false> : _and_<_not_<is_void<T>>,
typename _is_default_constructible_impl<T>::type> {};

template <typename T>
struct is_default_constructible : _is_default_constructible_helper<T> {};

template <typename T, bool = is_array_v<T>>
struct _is_nt_default_constructible_helper;
template <typename T>
struct _is_nt_default_constructible_helper<T, true> : _and_<_is_array_known_bounds<T>,
bool_constant<noexcept(remove_all_extents_t<T>())>> {};
template <typename T>
struct _is_nt_default_constructible_helper<T, false> : bool_constant<noexcept(T())> {};

template <typename T>
struct is_nothrow_default_constructible : _and_<is_default_constructible<T>, _is_nt_default_constructible_helper<T>> {};

template <typename From, typename To>
struct _is_static_castable_impl {
	template <typename From1, typename To1, typename = decltype(static_cast<To1>(declval<From1>()))>
	static true_type _test(int);
	template <typename, typename>
	static false_type _test(...);
	typedef decltype(_test<From, To>(0)) type;
};

template <typename From, typename To>
struct _is_static_castable : _is_static_castable_impl<From, To>::type {};

template <typename T, typename Arg>
struct _is_direct_constructible_impl {
	template <typename T1, typename Arg1, typename = decltype(::new T1(declval<Arg1>()))>
	static true_type _test(int);
	template <typename, typename>
	static false_type _test(...);
	typedef decltype(_test<T, Arg>(0)) type;
};

template <typename T, typename _Arg>
struct _is_direct_constructible_helper : _and_<is_destructible<T>,typename _is_direct_constructible_impl<T, _Arg>::type> {};

template <typename From, typename To, bool = _not_<_or_<is_void<From>, is_function<From>>>::value>
struct _is_baseTo_derived_ref;

template <typename From, typename To>
struct _is_baseTo_derived_ref<From, To, true> {
	typedef remove_cv_t<remove_reference_t<From>> _src_t;
	typedef remove_cv_t<remove_reference_t<To>> _dst_t;
	typedef _and_<_not_<is_same<_src_t, _dst_t>>, is_base_of<_src_t, _dst_t>,
						  _not_<is_constructible<_dst_t, From>>> type;
	static constexpr bool value = type::value;
};

template <typename From, typename To>
struct _is_baseTo_derived_ref<From, To, false> : false_type {};

template <typename From, typename To,
          bool = _and_<is_lvalue_reference<From>, is_rvalue_reference<To>>::value>
struct _is_lvalueTo_rvalue_ref;

template <typename From, typename To>
struct _is_lvalueTo_rvalue_ref<From, To, true> {
	typedef remove_cv_t<remove_reference_t<From>> _src_t;
	typedef remove_cv_t<remove_reference_t<To>> _dst_t;
	typedef _and_<_not_<is_function<_src_t>>, _or_<is_same<_src_t, _dst_t>, is_base_of<_dst_t, _src_t>>> type;
	static constexpr bool value = type::value;
};

template <typename From, typename To>
struct _is_lvalueTo_rvalue_ref<From, To, false> : false_type {};

template <typename T, typename _Arg>
struct _is_direct_constructible_ref_cast :
_and_<_is_static_castable<_Arg, T>, _not_<_or_<_is_baseTo_derived_ref<_Arg, T>, _is_lvalueTo_rvalue_ref<_Arg, T>>>> {};

template <typename T, typename _Arg>
struct _is_direct_constructible : conditional_t<
is_reference_v<T>, _is_direct_constructible_ref_cast<T, _Arg>,
                   _is_direct_constructible_helper<T, _Arg>> {};

template <typename T, typename... Args>
struct _is_nary_constructible_impl {
	template <typename T1, typename... Args1, typename = decltype(T1(declval<Args1>()...))>
	static true_type _test(int);
	template <typename, typename...>
	static false_type _test(...);
	typedef decltype(_test<T, Args...>(0)) type;
};

template <typename T, typename... _Args>
struct _is_nary_constructible : _is_nary_constructible_impl<T, _Args...>::type {
	static_assert(sizeof...(_Args) > 1, "Only useful for > 1 arguments");
};

template <typename T, typename... _Args>
struct _is_constructible_helper : _is_nary_constructible<T, _Args...> {};

template <typename T, typename _Arg>
struct _is_constructible_helper<T, _Arg> : _is_direct_constructible<T, _Arg> {};

template <typename T>
struct _is_constructible_helper<T> : is_default_constructible<T> {};

template <typename T, typename... _Args>
struct is_constructible : _is_constructible_helper<T, _Args...> {};

template <typename T, typename... _Args>
struct is_trivially_constructible : _and_<is_constructible<T, _Args...>,
bool_constant<__is_trivially_constructible(remove_extent_t<T>, _Args...)>> {}; // Add remove_extent in bif to avoid crash

template <typename T, typename... _Args>
struct _is_nothrow_constructible_helper : bool_constant<noexcept(T(declval<_Args>()...))> {};
template <typename T, typename _Arg>
struct _is_nothrow_constructible_helper<T, _Arg> : bool_constant<noexcept(static_cast<T>(declval<_Arg>()))> {};
template <typename T>
struct _is_nothrow_constructible_helper<T> : is_nothrow_default_constructible<T> {};

template <typename T, typename... _Args>
struct is_nothrow_constructible : _and_<is_constructible<T, _Args...>, _is_nothrow_constructible_helper<T, _Args...>> {};

template <typename T, typename U>
struct _is_assignable_impl {
	template <typename T1, typename U1,
	          typename = decltype(declval<T1>() = declval<U1>())>
	static true_type _test(int);
	template <typename, typename>
	static false_type _test(...);
	typedef decltype(_test<T, U>(0)) type;
};

template <typename T, typename U, bool = _or_<is_void<T>, is_void<U>>::value>
struct _is_assignable_helper : _is_assignable_impl<T, U>::type {};
template <typename T, typename U>
struct _is_assignable_helper<T, U, true> : false_type {};

template <typename T, typename U>
struct is_assignable : _is_assignable_helper<T, U> {};

template<typename T, typename U>
struct is_trivially_assignable : _and_<is_assignable<T, U>, bool_constant<__is_trivially_assignable(T, U)>>  {};

template <typename T, typename U, bool = is_assignable_v<T, U>>
struct _is_nt_assignable_helper : false_type {};
template <typename T, typename U>
struct _is_nt_assignable_helper<T, U, true> : bool_constant<noexcept(declval<T>() = declval<U>())> {};

template <typename T, typename U>
struct is_nothrow_assignable : _is_nt_assignable_helper<T, U> {};

template <typename From, typename To>
struct _is_convertible_impl {
	template <typename To1>
	static void _test_aux(To1);
	template <typename From1, typename To1,
	          typename = decltype(_test_aux<To1>(declval<From1>()))>
	static true_type _test(int);
	template <typename, typename>
	static false_type _test(...);
	typedef decltype(_test<From, To>(0)) type;
};

template <typename From, typename To, 
		  bool = _or_<is_void<From>, is_function<To>, is_array<To>>::value>
struct _is_convertible_helper : is_void<To> {};

template <typename From, typename To>
struct _is_convertible_helper<From, To, false> : _is_convertible_impl<From, To> {};

template <typename From, typename To>
struct is_convertible : _is_convertible_helper<From, To>::type {};