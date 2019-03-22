#ifndef _ALA_TUPLE_H
#define _ALA_TUPLE_H

#include <ala/type_traits.h>
#include <ala/detail/integer_sequence.h>

namespace ala {

template<size_t I, typename T, bool IsEmpty = is_empty<T>::value && !is_final<T>::value>
struct _tuple_base;

template<size_t I, typename T, bool IsEmpty>
struct _tuple_base {
    T value;
    _tuple_base &operator=(const _tuple_base &) = delete;

public:
    _tuple_base() noexcept(is_nothrow_constructible<T>::value)
        : value() {
        static_assert(
            !is_reference<T>::value,
            "Attempt to default construct a reference element in a tuple");
    }

    template<typename T1,
             typename = enable_if_t<
                 _and_<_not_<is_same<remove_cvref_t<T1>, _tuple_base>>,
                       is_constructible<T, T1>>::value>>
    constexpr explicit _tuple_base(T1 &&t) noexcept(
        is_nothrow_constructible<T1, T>::value)
        : value(ala::forward<T1>(t)) {}

    _tuple_base(const _tuple_base &tb) = default;
    _tuple_base(_tuple_base &&tb) = default;

    template<typename T1>
    _tuple_base &operator=(T1 &&t) noexcept(
        is_nothrow_assignable<T &, T1>::value) {
        value = ala::forward<T1>(t);
        return *this;
    }

    void swap(_tuple_base &tb) noexcept(
        is_nothrow_swappable<_tuple_base>::value) {
        ala::swap(value, tb.value);
    }

    constexpr T &get() noexcept { return value; }
    constexpr const T &get() const noexcept { return value; }
};

template<size_t I, typename T>
struct _tuple_base<I, T, true>: private T {
    _tuple_base &operator=(const _tuple_base &) = delete;

public:
    _tuple_base() noexcept(is_nothrow_constructible<T>::value) {}

    template<typename T1,
             typename = enable_if_t<
                 _and_<_not_<is_same<remove_cvref_t<T1>, _tuple_base>>,
                       is_constructible<T, T1>>::value>>
    constexpr explicit _tuple_base(T1 &&t) noexcept(
        is_nothrow_constructible<T, T1>::value)
        : T(ala::forward<T1>(t)) {}

    _tuple_base(const _tuple_base &t) = default;
    _tuple_base(_tuple_base &&t) = default;

    template<typename T1>
    _tuple_base &operator=(T1 &&t) noexcept(
        is_nothrow_assignable<T &, T1>::value) {
        T::operator=(ala::forward<T1>(t));
        return *this;
    }

    void swap(_tuple_base &tb) noexcept(
        is_nothrow_swappable<_tuple_base>::value) {
        ala::swap(get(), tb.get());
    }

    constexpr T &get() { return static_cast<T &>(*this); }
    constexpr const T &get() const { return static_cast<const T &>(*this); }
};

template<typename... Ts>
struct tuple;

template<typename T>
struct tuple_size;

template<typename... Ts>
struct tuple_size<tuple<Ts...>>
    : integral_constant<size_t, sizeof...(Ts)> {};

template<typename T>
struct tuple_size<const T>: integral_constant<size_t, tuple_size<T>::value> {};

template<typename T>
struct tuple_size<volatile T>: integral_constant<size_t, tuple_size<T>::value> {
};

template<typename T>
struct tuple_size<const volatile T>
    : integral_constant<size_t, tuple_size<T>::value> {};

template<size_t I, typename T>
struct tuple_element;

template<size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>>
    : tuple_element<I - 1, tuple<Tail...>> {
    static_assert(I <= sizeof...(Tail), "out of range");
};

template<typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    typedef Head type;
};

template<size_t I, typename T>
struct tuple_element<I, const T> {
    typedef add_const_t<typename tuple_element<I, T>::type> type;
};

template<size_t I, typename T>
struct tuple_element<I, volatile T> {
    typedef add_volatile_t<typename tuple_element<I, T>::type> type;
};

template<size_t I, typename T>
struct tuple_element<I, const volatile T> {
    typedef add_cv_t<typename tuple_element<I, T>::type> type;
};

template<typename T>
ALA_VAR_INLINE constexpr size_t tuple_size_v = tuple_size<T>::value;

template<size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template<typename Tuple>
struct _is_tuple_helper: false_type {};

template<typename... Ts>
struct _is_tuple_helper<tuple<Ts...>>: true_type {};

template<typename Tuple>
struct is_tuple: _is_tuple_helper<remove_cvref_t<Tuple>> {};

template<size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>> &
get(tuple<Ts...> &tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return static_cast<_tuple_base<I, type> &>(tp._impl).get();
}

template<size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>> &
get(const tuple<Ts...> &tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return static_cast<const _tuple_base<I, type> &>(tp._impl).get();
}

template<size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>> &&
get(tuple<Ts...> &&tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return static_cast<type &&>(
        static_cast<_tuple_base<I, type> &&>(tp._impl).get());
}

template<size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>> &&
get(const tuple<Ts...> &&tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return static_cast<const type &&>(
        static_cast<const _tuple_base<I, type> &&>(tp._impl).get());
}

// _tuple_impl

template<typename Seq, typename... Ts>
struct _tuple_impl;

template<size_t... Idx, typename... Ts>
struct _tuple_impl<index_sequence<Idx...>, Ts...>: _tuple_base<Idx, Ts>... {
    constexpr _tuple_impl() noexcept(
        _and_<is_nothrow_default_constructible<Ts>...>::value) {}

    template<typename... Us>
    explicit constexpr _tuple_impl(Us &&... us) noexcept(
        _and_<is_nothrow_constructible<Ts, Us>...>::value)
        : _tuple_base<Idx, Ts>(ala::forward<Us>(us))... {}

    template<typename Tuple>
    constexpr _tuple_impl(Tuple &&tp) noexcept(
        _and_<is_nothrow_constructible<Ts, tuple_element_t<Idx, remove_reference_t<Tuple>>>...>::value)
        : _tuple_base<Idx, Ts>(ala::forward<tuple_element_t<Idx, remove_reference_t<Tuple>>>(ala::get<Idx>(tp)))... {}

    template<typename... Dummy>
    constexpr void _va_packer(Dummy...) {}

    template<typename Tuple>
    constexpr _tuple_impl &operator=(Tuple &&tp) noexcept(
        _and_<is_nothrow_assignable<Ts &, tuple_element_t<Idx, remove_reference_t<Tuple>>>...>::value) {
        _va_packer(_tuple_base<Idx, Ts>::operator=(
            ala::forward<tuple_element_t<Idx, remove_reference_t<Tuple>>>(ala::get<Idx>(tp)))...);
        return *this;
    }

    _tuple_impl(const _tuple_impl &) = default;
    _tuple_impl(_tuple_impl &&) = default;

    constexpr _tuple_impl &operator=(const _tuple_impl &tpl) noexcept(
        _and_<is_nothrow_copy_assignable<Ts>...>::value) {
        _va_packer(_tuple_base<Idx, Ts>::operator=(
            static_cast<const _tuple_base<Idx, Ts> &>(tpl).get())...);
        return *this;
    }

    constexpr _tuple_impl &operator=(_tuple_impl &&tpl) noexcept(
        _and_<is_nothrow_move_assignable<Ts>...>::value) {
        _va_packer(_tuple_base<Idx, Ts>::operator=(ala::forward<Ts>(
            static_cast<_tuple_base<Idx, Ts> &>(tpl).get()))...);
        return *this;
    }

    constexpr void swap(_tuple_impl &tpl) noexcept(
        _and_<is_nothrow_swappable<Ts>...>::value) {
        _va_packer(_tuple_base<Idx, Ts>::swap(
            static_cast<_tuple_base<Idx, Ts> &>(tpl))...);
    }
};

template<typename... Ts>
struct tuple {
private:
    typedef _tuple_impl<make_index_sequence<sizeof...(Ts)>, Ts...> impl_type;
    impl_type _impl;

    template<size_t Sz, typename... Us>
    friend constexpr typename tuple_element<Sz, tuple<Us...>>::type &
    get(tuple<Us...> &) noexcept;
    template<size_t Sz, typename... Us>
    friend constexpr const typename tuple_element<Sz, tuple<Us...>>::type &
    get(const tuple<Us...> &) noexcept;
    template<size_t Sz, typename... Us>
    friend constexpr typename tuple_element<Sz, tuple<Us...>>::type &&
    get(tuple<Us...> &&) noexcept;
    template<size_t Sz, typename... Us>
    friend constexpr const typename tuple_element<Sz, tuple<Us...>>::type &&
    get(const tuple<Us...> &&) noexcept;

public:
    tuple(tuple const &) = default;
    tuple(tuple &&) = default;

    // make template not specialization immediately
    template<typename Dummy = true_type,
             typename = enable_if_t<
                 _and_<Dummy, _and_<is_default_constructible<Ts>...>,
                       _and_<is_implicitly_default_constructible<Ts>...>>::value>>
    constexpr tuple() noexcept(_and_<is_nothrow_default_constructible<Ts>...>::value) {}

    template<typename Dummy = true_type,
             typename = enable_if_t<
                 _and_<Dummy, _and_<is_default_constructible<Ts>...>,
                       _not_<_and_<is_implicitly_default_constructible<Ts>...>>>::value>,
             bool = true>
    explicit constexpr tuple() noexcept(_and_<is_nothrow_default_constructible<Ts>...>::value) {}

    template<typename Dummy = true_type,
             typename = enable_if_t<
                 _and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                       _and_<is_copy_constructible<Ts>...>,
                       _and_<is_convertible<const Ts &, Ts>...>>::value>,
             typename = void>
    constexpr tuple(const Ts &... vs) noexcept(_and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(vs...) {}

    template<typename Dummy = true_type,
             typename = enable_if_t<
                 _and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                       _and_<is_copy_constructible<Ts>...>,
                       _not_<_and_<is_convertible<const Ts &, Ts>...>>>::value>,
             typename = void,
             bool = true>
    explicit constexpr tuple(const Ts &... vs) noexcept(_and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(vs...) {}

    template<typename... Us, typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) >= 1>,
                                                          bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                                                          _and_<is_constructible<Ts, Us &&>...>,
                                                          _and_<is_convertible<Ts, Us &&>...>>::value>>
    constexpr tuple(Us &&... us) noexcept(
        is_nothrow_constructible<impl_type, Us...>::value)
        : _impl(ala::forward<Us>(us)...) {}

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) >= 1>,
                       bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_constructible<Ts, Us &&>...>,
                       _not_<_and_<is_convertible<Ts, Us &&>...>>>::value>,
             bool = true>
    explicit constexpr tuple(Us &&... us) noexcept(
        is_nothrow_constructible<impl_type, Us...>::value)
        : _impl(ala::forward<Us>(us)...) {}

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_constructible<Ts, const Us &>...>,
                       _or_<bool_constant<sizeof...(Ts) != 1>,
                            _and_<_not_<is_convertible<const tuple<Us> &, Ts>...>,
                                  _not_<is_constructible<Ts, const tuple<Us> &>...>,
                                  _not_<is_same<Ts, Us>...>>>>::value>,
             typename = enable_if_t<
                 _and_<is_convertible<const Us &, Ts>...>::value>>
    constexpr tuple(const tuple<Us...> &tp) noexcept(
        is_nothrow_constructible<impl_type, const tuple<Us...> &>::value)
        : _impl(tp) {}

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_constructible<Ts, const Us &>...>,
                       _or_<bool_constant<sizeof...(Ts) != 1>,
                            _and_<_not_<is_convertible<const tuple<Us> &, Ts>...>,
                                  _not_<is_constructible<Ts, const tuple<Us> &>...>,
                                  _not_<is_same<Ts, Us>...>>>>::value>,
             typename = enable_if_t<
                 !_and_<is_convertible<const Us &, Ts>...>::value>,
             bool = true>
    explicit constexpr tuple(const tuple<Us...> &tp) noexcept(
        is_nothrow_constructible<impl_type, const tuple<Us...> &>::value)
        : _impl(tp) {}

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_constructible<Ts, Us &&>...>,
                       _or_<bool_constant<sizeof...(Ts) != 1>,
                            _and_<_not_<is_convertible<tuple<Us>, Ts>...>,
                                  _not_<is_constructible<Ts, tuple<Us>>...>,
                                  _not_<is_same<Ts, Us>...>>>>::value>,
             typename = enable_if_t<
                 _and_<is_convertible<Us &&, Ts>...>::value>>
    constexpr tuple(tuple<Us...> &&tp) noexcept(
        is_nothrow_constructible<impl_type, tuple<Us...> &&>::value)
        : _impl(ala::move(tp)) {}

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_constructible<Ts, Us &&>...>,
                       _or_<bool_constant<sizeof...(Ts) != 1>,
                            _and_<_not_<is_convertible<tuple<Us>, Ts>...>,
                                  _not_<is_constructible<Ts, tuple<Us>>...>,
                                  _not_<is_same<Ts, Us>...>>>>::value>,
             typename = enable_if_t<
                 !_and_<is_convertible<Us &&, Ts>...>::value>,
             bool = true>
    explicit constexpr tuple(tuple<Us...> &&tp) noexcept(
        is_nothrow_constructible<impl_type, tuple<Us...> &&>::value)
        : _impl(ala::move(tp)) {}

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_assignable<Ts &, const Us &>...>>::value>>
    constexpr tuple &operator=(const tuple<Us...> &tp) noexcept(
        is_nothrow_assignable<impl_type &, const tuple<Us...> &>::value) {
        _impl.operator=(tp);
        return *this;
    }

    template<typename... Us,
             typename = enable_if_t<
                 _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                       _and_<is_assignable<Ts &, Us &&>...>>::value>>
    constexpr tuple &operator=(tuple<Us...> &&tp) noexcept(
        is_nothrow_assignable<impl_type &, tuple<Us...> &&>::value) {
        _impl.operator=(ala::move(tp));
        return *this;
    }

    enable_if_t<_and_<is_copy_assignable<Ts>...>::value, tuple &> constexpr
    operator=(const tuple &tp) noexcept(
        _and_<is_nothrow_copy_assignable<Ts>...>::value) {
        _impl.operator=(tp._impl);
        return *this;
    }

    enable_if_t<_and_<is_move_assignable<Ts>...>::value, tuple &> constexpr
    operator=(tuple &&tp) {
        _impl.operator=(ala::move(tp._impl));
        return *this;
    }

    constexpr void
    swap(tuple &tp) noexcept(_and_<is_nothrow_swappable<Ts>...>::value) {
        _impl.swap(tp._impl);
    }
};

template<size_t I, bool>
struct _tuple_equal_impl {
    template<typename T, typename U>
    constexpr bool operator()(const T &lhs, const U &rhs) {
        constexpr size_t idx = tuple_size<T>::value - I;
        return ala::get<idx>(lhs) == ala::get<idx>(rhs) &&
               _tuple_equal_impl<I - 1, true>()(lhs, rhs);
    }
};

template<>
struct _tuple_equal_impl<0, true> {
    template<typename T, typename U>
    constexpr bool operator()(const T &, const U &) {
        return true;
    }
};

template<size_t Size>
struct _tuple_equal_impl<Size, false> {
    template<typename T, typename U>
    constexpr bool operator()(const T &lhs, const U &rhs) {
        return false;
    }
};

template<size_t I, bool>
struct _tuple_less_impl {
    template<typename T, typename U>
    constexpr bool operator()(const T &lhs, const U &rhs) {
        constexpr size_t idx = tuple_size<T>::value - I;
        if (ala::get<idx>(lhs) < ala::get<idx>(rhs))
            return true;
        if (ala::get<idx>(rhs) < ala::get<idx>(lhs))
            return false;
        return _tuple_less_impl<I - 1, true>()(lhs, rhs);
    }
};

template<>
struct _tuple_less_impl<0, true> {
    template<typename T, typename U>
    constexpr bool operator()(const T &, const U &) {
        return false;
    }
    template<typename... Ts, typename... Us, typename = enable_if_t<(sizeof...(Ts) < sizeof...(Us))>>
    constexpr bool operator()(const tuple<Ts...> &, const tuple<Us...> &) {
        return true;
    }
};

template<size_t Size>
struct _tuple_less_impl<Size, false> {
    template<typename T, typename U>
    constexpr bool operator()(const T &lhs, const U &rhs) {
        return _tuple_less_impl<Size, true>()(rhs, lhs);
    }
};

template<typename... Ts, typename... Us>
constexpr bool operator==(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    return _tuple_equal_impl<sizeof...(Ts), sizeof...(Ts) == sizeof...(Us)>()(lhs, rhs);
}

template<typename... Ts, typename... Us>
constexpr bool operator!=(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    return !(lhs == rhs);
}

template<typename... Ts, typename... Us>
constexpr bool operator<(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    constexpr size_t Size = sizeof...(Ts) < sizeof...(Us) ? sizeof...(Ts) : sizeof...(Us);
    return _tuple_less_impl<Size, sizeof...(Ts) < sizeof...(Us)>()(lhs, rhs);
}

template<typename... Ts, typename... Us>
constexpr bool operator>(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    return rhs < lhs;
}

template<typename... Ts, typename... Us>
constexpr bool operator<=(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    return !(rhs < lhs);
}

template<typename... Ts, typename... Us>
constexpr bool operator>=(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    return !(lhs < rhs);
}

template<typename... Ts>
constexpr tuple<unwrap_ref_decay_t<Ts>...>
make_tuple(Ts &&... args) {
    return tuple<unwrap_ref_decay_t<Ts>...>(ala::forward<Ts>(args)...);
}

template<typename... Ts>
constexpr tuple<Ts &&...> forward_as_tuple(Ts &&... args) noexcept {
    return tuple<Ts &&...>(ala::forward<Ts>(args)...);
}

template<typename EmptyTuple, typename EmptySeq, typename FirstTpSeq>
struct _tuple_cat_impl;

template<typename... Ts, size_t... Is, size_t... Js>
struct _tuple_cat_impl<tuple<Ts...>, index_sequence<Is...>, index_sequence<Js...>> {
    template<typename Tuple0>
    constexpr decltype(auto) operator()(tuple<Ts...> t, Tuple0 &&t0) {
        return forward_as_tuple(ala::forward<Ts>(ala::get<Is>(t))...,
                                ala::get<Js>(ala::forward<Tuple0>(t0))...);
    }

    template<typename Tuple0, typename Tuple1, typename... Tuples>
    constexpr decltype(auto) operator()(tuple<Ts...> t, Tuple0 &&t0, Tuple1 &&t1, Tuples &&... tpls) {
        typedef remove_reference_t<Tuple0> T0;
        typedef remove_reference_t<Tuple1> T1;
        typedef _tuple_cat_impl<tuple<Ts..., tuple_element_t<Js, T0> &&...>,
                                make_index_sequence<sizeof...(Ts) + tuple_size<T0>::value>,
                                make_index_sequence<tuple_size<T1>::value>>
            next;
        return next()(forward_as_tuple(
                          ala::forward<Ts>(ala::get<Is>(t))...,
                          ala::get<Js>(ala::forward<Tuple0>(t0))...),
                      ala::forward<Tuple1>(t1),
                      ala::forward<Tuples>(tpls)...);
    }
};

template<typename Tuple0, typename... Tuples>
constexpr decltype(auto) tuple_cat(Tuple0 &&t0, Tuples &&... tpls) {
    typedef typename remove_reference<Tuple0>::type T0;
    return _tuple_cat_impl<tuple<>, index_sequence<>,
                           make_index_sequence<tuple_size<T0>::value>>()(
        tuple<>(), ala::forward<Tuple0>(t0),
        ala::forward<Tuples>(tpls)...);
}

constexpr decltype(auto) tuple_cat() {
    return tuple<>{};
}

template<typename... Args>
constexpr tuple<Args &...> tie(Args &... args) noexcept {
    return tuple<Args &...>(args...);
}

struct _ignore_t {
    template<typename T>
    constexpr const _ignore_t &operator=(T &&) const { return *this; }
};

ALA_VAR_INLINE constexpr _ignore_t ignore{};

template<typename F, typename Tuple, size_t... I>
constexpr decltype(auto) _apply_impl(F &&f, Tuple &&tpl, index_sequence<I...>) {
    return ala::invoke(ala::forward<F>(f), ala::get<I>(ala::forward<Tuple>(tpl))...);
}

template<typename F, typename Tuple>
constexpr decltype(auto) _apply(F &&f, Tuple &&tpl) {
    return _apply_impl(ala::forward<F>(f), ala::forward<Tuple>(tpl),
                       make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{});
}

template<typename T, typename Tuple, size_t... I>
constexpr T _make_from_tuple_impl(Tuple &&tpl, index_sequence<I...>) {
    return T(ala::get<I>(ala::forward<Tuple>(tpl))...);
}

template<typename T, typename Tuple>
constexpr T make_from_tuple(Tuple &&tpl) {
    return _make_from_tuple_impl<T>(ala::forward<Tuple>(tpl),
                                    make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{});
}

template<typename T, size_t Cur, typename IntSeq, typename... Ts>
struct _type_to_index_impl;

template<typename T, size_t Cur, typename T1, typename... Ts, size_t... SIs>
struct _type_to_index_impl<T, Cur, index_sequence<SIs...>, T1, Ts...> {
    typedef conditional_t<is_same<T, T1>::value,
                          typename _type_to_index_impl<T, Cur + 1, index_sequence<SIs..., Cur>, Ts...>::type,
                          typename _type_to_index_impl<T, Cur + 1, index_sequence<SIs...>, Ts...>::type>
        type;
};

template<typename T, size_t Cur, size_t... SIs>
struct _type_to_index_impl<T, Cur, index_sequence<SIs...>> {
    typedef index_sequence<SIs...> type;
};

template<typename T, typename... Ts>
using type_to_index_t = typename _type_to_index_impl<T, 0, index_sequence<>, Ts...>::type;

template<typename T, typename... Ts>
constexpr T &get(tuple<Ts...> &t) noexcept {
    typedef type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(t);
}

template<typename T, typename... Ts>
constexpr T &&get(tuple<Ts...> &&t) noexcept {
    typedef type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(ala::move(t));
}

template<typename T, typename... Ts>
constexpr const T &get(const tuple<Ts...> &t) noexcept {
    typedef type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(t);
}

template<typename T, typename... Ts>
constexpr const T &&get(const tuple<Ts...> &&t) noexcept {
    typedef type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(ala::move(t));
}

// extra
// template<template<typename> class Tplt,
//          size_t Cur, typename IntSeq, typename... Ts>
// struct _type_pick_impl;

// template<template<typename> class Tplt,
//          size_t Cur, typename T1, typename... Ts, size_t... SIs>
// struct _type_pick_impl<Tplt, Cur, index_sequence<SIs...>, T1, Ts...> {
//     typedef conditional_t<Tplt<T1>::value,
//                           typename _type_to_index_impl<template Tplt, Cur + 1, index_sequence<SIs..., Cur>, Ts...>::type,
//                           typename _type_to_index_impl<template Tplt, Cur + 1, index_sequence<SIs...>, Ts...>::type>
//         type;
// };

// template<template<typename> class Tplt,
//          size_t Cur, size_t... SIs>
// struct _type_pick_impl<Tplt, Cur, index_sequence<SIs...>> {
//     typedef index_sequence<SIs...> type;
// };

// template<template<typename> class Tplt, typename... Ts>
// using type_pick_t = typename _type_pick_impl<Tplt, 0, index_sequence<>, Ts...>::type;

template<size_t... Is, typename... Ts>
constexpr auto choice(const tuple<Ts...> &t) {
    return make_tuple(get<Is>(t)...);
}

template<size_t... Is, typename... Ts>
constexpr auto choice(const tuple<Ts...> &&t) {
    return make_tuple(get<Is>(ala::move(t))...);  // move more than one time ?
}

// template<template<typename> class Tplt, typename... Ts>
// constexpr auto choice(tuple<Ts...> &t) {
//     typedef type_pick_t<Tplt, Ts...> Index;
//     return get<get_integer_sequence<0, Index>::value>(t);
// }

// template<template<typename> class Tplt, typename... Ts>
// constexpr auto choice(tuple<Ts...> &&t) {
//     typedef type_pick_t<Tplt, Ts...> Index;
//     return get<get_integer_sequence<0, Index>::value>(ala::move(t));
// }

// template<template<typename> class Tplt, typename... Ts>
// constexpr auto choice(const tuple<Ts...> &t) {
//     typedef type_pick_t<Tplt, Ts...> Index;
//     return get<get_integer_sequence<0, Index>::value>(t);
// }

// template<template<typename> class Tplt, typename... Ts>
// constexpr auto choice(const tuple<Ts...> &&t) {
//     typedef type_pick_t<Tplt, Ts...> Index;
//     return get<get_integer_sequence<0, Index>::value>(ala::move(t));
// }


#if ALA_ENABLE_CPP_MACRO && __cpp_deduction_guides >= 201606 || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1914)
template<typename... Ts>
tuple(Ts...)->tuple<Ts...>;
#endif

} // namespace ala

#endif // HEAD