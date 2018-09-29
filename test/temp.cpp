template <typename Result, typename Ret, typename = void>
struct _is_nt_invocable_r_impl : false_type {};

template <typename Result, typename Ret>
struct _is_nt_invocable_r_impl<Result, Ret, void_t<typename Result::type>> : _or_<is_void<Ret>,
_and_<is_convertible<typename Result::type, Ret>,is_nothrow_constructible<Ret, typename Result::type>>> {};

template <typename Ret, typename Fn, typename... Args>
struct is_nothrow_invocable_r : _and_<_is_nt_invocable_r_impl<_invoke_result<Fn, Args...>, Ret>,
_call_is_nothrow_<Fn, Args...>>::type {};