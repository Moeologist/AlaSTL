#ifndef EASTL_BIND_H

#include <ala/tuple.h>

namespace ala {

template<class T>
struct reference_wrapper {
    using type = T;
    void _test(T &) noexcept;
    void _test(T &&) = delete;
    template<class U, class = decltype(_test(declval<U>())),
             class = enable_if_t<!is_same<reference_wrapper, remove_cvref_t<U>>::value>>
    reference_wrapper(U &&u) noexcept(noexcept(_test(declval<U>())))
        : _ptr(ala::addressof(_test(ala::forward<U>(u)))) {}
    reference_wrapper(const reference_wrapper &other) noexcept = default;
    reference_wrapper &operator=(const reference_wrapper &x) = default;
    operator T &() const noexcept {
        return *_ptr;
    }
    T &get() const noexcept {
        return *_ptr;
    }
    template<class... Args>
    enable_if_t<is_invocable<T &, Args...>::value, invoke_result_t<T &, Args...>>
    operator()(Args &&... args) const {
        return ala::invoke(get(), ala::forward<Args>(args)...);
    }
};

template<int N>
struct _placeholder_t {};

namespace placeholders {
_placeholder_t<1> _1;
_placeholder_t<2> _2;
_placeholder_t<3> _3;
_placeholder_t<4> _4;
_placeholder_t<5> _5;
_placeholder_t<6> _6;
_placeholder_t<7> _7;
_placeholder_t<8> _8;
_placeholder_t<9> _9;
} // namespace placeholders

template<class T>
struct is_placeholder: integral_constant<int, 0> {};

struct is_placeholder<_placeholder_t<0>>: integral_constant<int, 0> {};

template<int N>
struct is_placeholder<_placeholder_t<N>>: integral_constant<int, N> {};

#if _ALA_ENABLE_INLINE_VAR
template<class T>
inline constexpr int is_placeholder_v = is_placeholder<T>::value;
#endif

template<class = void, class F, class... Args>
struct _get_result_type {
    typedef invoke_result_t<F, Args...> type;
};

template<class F, class... Args>
struct _get_result_type<void_t<typename T::result_type>, F, Args...> {
    typedef typename T::result_type type;
};

template<class R, class F, class... Args, class>
struct _bind_t;

template<class T>
struct is_bind_expression: false_type {};

template<class... Ts>
struct is_bind_expression<_bind_t<Ts...>>: true_type {};

template<class T>
inline constexpr bool is_bind_expression_v = is_bind_expression<T>::value;

template<class R, class F, class... Args>
struct _bind_t {
    typedef R result_type;
    typedef decay_t<F> _fn_t;
    typedef tuple<decay_t<Args>...> _tuple_t;
    _fn_t _fn;
    _tuple_t _tuple;

    template<class Tuple, class T,
             class = enable_if_t<!is_bind_expression<remove_cvref_t<T>>::value>>
    decltype(auto) _select(Tuple &&, T &&val) {
        return ala::forward<T>(val);
    }

    template<class Tuple, class Bind,
             class = enable_if_t<is_bind_expression<remove_cvref_t<Bind>>::value>>
    decltype(auto) _select(Tuple &&tp, Bind &&inner_bind) {
        return inner_bind._call(ala::forward<Tuple>(tp));
    }

    template<class Tuple, int N>
    decltype(auto) _select(Tuple &&tp, placeholder<N>) {
        return ala::get<N - 1>(ala::forward<Tuple>(tp));
    }

    template<class Tuple, class T>
    decltype(auto) _select(Tuple &&tp, reference_wrapper<T> refwarp) {
        return refwarp.get();
    }

    template<class Tuple, size_t... N>
    decltype(auto) _call_helper(Tuple &&tp, index_sequence<N...>) {
        return ala::invoke(f, _select(ala::forward<Tuple>(tp),
                                      ala::get<N>(_tuple))...);
    }

    template<class Tuple>
    decltype(auto) _call(Tuple &&tp) {
        return this->_call_helper(f, ala::index_sequence_for<Args...>());
    }

public:
    _bind_t(F &&f, Args &&... args)
        : _fn(ala::forward<F>(f)),
          _tuple(ala::forward_as_tuple(ala::forward<Args>(args)...)) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value &&
                                 is_copy_constructible<_fn_t>::value>>
    _bind_t(const _bind_t &other): _fn(other._fn), _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value &&
                                 is_move_constructible<_fn_t>::value>>
    _bind_t(_bind_t &&other)
        : _fn(ala::move(other._fn)), _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    decltype(auto) operator()(Args1 &&... args) {
        return _call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }
};

template<class F, class... Args>
inline _bind_t<F, Args...> bind(F &&f, Args &&... args) {
    return _bind_t<F, Args...>(ala::forward<F>(f), ala::forward<Args>(args)...);
}

} // namespace ala

#endif // EASTL_BIND_H