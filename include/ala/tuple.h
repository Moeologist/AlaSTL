#ifndef _ALA_TUPLE_H
#define _ALA_TUPLE_H

#include <ala/detail/integer_sequence.h>
#include <ala/detail/pair.h>
#include <ala/detail/tuple_operator.h>
#include <ala/type_traits.h>

namespace ala {

template<size_t I, typename T, bool IsEmpty = is_empty<T>::value && !is_final<T>::value>
struct _tuple_base;

template<size_t I, typename T, bool IsEmpty>
struct _tuple_base {
    T value;
    _tuple_base &operator=(const _tuple_base &) = delete;

public:
    _tuple_base() noexcept(is_nothrow_constructible<T>::value): value() {
        static_assert(
            !is_reference<T>::value,
            "Attempt to default construct a reference element in a tuple");
    }

    template<typename T1,
             typename = enable_if_t<_and_<_not_<is_same<remove_cvref_t<T1>, _tuple_base>>,
                                          is_constructible<T, T1>>::value>>
    constexpr explicit _tuple_base(T1 &&t) noexcept(
        is_nothrow_constructible<T1, T>::value)
        : value(ala::forward<T1>(t)) {}

    _tuple_base(const _tuple_base &tb) = default;
    _tuple_base(_tuple_base &&tb) = default;

    template<typename T1>
    _tuple_base &operator=(T1 &&t) noexcept(is_nothrow_assignable<T &, T1>::value) {
        value = ala::forward<T1>(t);
        return *this;
    }

    void swap(_tuple_base &tb) noexcept(is_nothrow_swappable<_tuple_base>::value) {
        ala::swap(value, tb.value);
    }

    constexpr T &get() noexcept {
        return value;
    }

    constexpr const T &get() const noexcept {
        return value;
    }
};

// empty class optimize in libc++

template<size_t I, typename T>
struct _tuple_base<I, T, true>: private T {
    _tuple_base &operator=(const _tuple_base &) = delete;

public:
    _tuple_base() noexcept(is_nothrow_constructible<T>::value) {}

    template<typename T1,
             typename = enable_if_t<_and_<_not_<is_same<remove_cvref_t<T1>, _tuple_base>>,
                                          is_constructible<T, T1>>::value>>
    constexpr explicit _tuple_base(T1 &&t) noexcept(
        is_nothrow_constructible<T, T1>::value)
        : T(ala::forward<T1>(t)) {}

    _tuple_base(const _tuple_base &t) = default;
    _tuple_base(_tuple_base &&t) = default;

    template<typename T1>
    _tuple_base &operator=(T1 &&t) noexcept(is_nothrow_assignable<T &, T1>::value) {
        T::operator=(ala::forward<T1>(t));
        return *this;
    }

    void swap(_tuple_base &tb) noexcept(is_nothrow_swappable<_tuple_base>::value) {
        ala::swap(get(), tb.get());
    }

    constexpr T &get() {
        return static_cast<T &>(*this);
    }

    constexpr const T &get() const {
        return static_cast<const T &>(*this);
    }
};

template<typename... Ts>
struct tuple_size<tuple<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

template<size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>>
    : tuple_element<I - 1, tuple<Tail...>> {
    static_assert(I <= sizeof...(Tail), "out of range");
};

template<typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    typedef Head type;
};

template<size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>> &get(tuple<Ts...> &tp) noexcept {
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
constexpr tuple_element_t<I, tuple<Ts...>> &&get(tuple<Ts...> &&tp) noexcept {
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

template<typename Tuple>
struct _is_tuple: false_type {};
template<typename... TArgs>
struct _is_tuple<tuple<TArgs...>>: true_type {};
template<typename First, typename Second>
struct _is_tuple<pair<First, Second>>: true_type {};

template<typename Seq, typename... Ts>
struct _tuple_impl;

template<size_t... Ids, typename... Ts>
struct _tuple_impl<index_sequence<Ids...>, Ts...>: _tuple_base<Ids, Ts>... {
    constexpr _tuple_impl() noexcept(
        _and_<is_nothrow_default_constructible<Ts>...>::value) {}

    template<typename... Us>
    explicit constexpr _tuple_impl(Us &&... us) noexcept(
        _and_<is_nothrow_constructible<Ts, Us>...>::value)
        : _tuple_base<Ids, Ts>(ala::forward<Us>(us))... {}

    template<typename Tuple,
             typename = enable_if_t<_is_tuple<remove_cvref_t<Tuple>>::value>>
    constexpr _tuple_impl(Tuple &&tp) noexcept(
        _and_<is_nothrow_constructible<
            Ts, tuple_element_t<Ids, remove_reference_t<Tuple>>>...>::value)
        : _tuple_base<Ids, Ts>(
              ala::forward<tuple_element_t<Ids, remove_reference_t<Tuple>>>(
                  ala::get<Ids>(tp)))... {}

    template<typename... Dummy>
    constexpr void _va_packer(Dummy...) {}

    template<typename Tuple,
             typename = enable_if_t<_is_tuple<remove_cvref_t<Tuple>>::value>>
    constexpr _tuple_impl &operator=(Tuple &&tp) noexcept(
        _and_<is_nothrow_assignable<
            Ts &, tuple_element_t<Ids, remove_reference_t<Tuple>>>...>::value) {
        _va_packer(_tuple_base<Ids, Ts>::operator=(
            ala::forward<tuple_element_t<Ids, remove_reference_t<Tuple>>>(
                ala::get<Ids>(tp)))...);
        return *this;
    }

    _tuple_impl(const _tuple_impl &) = default;
    _tuple_impl(_tuple_impl &&) = default;

    constexpr _tuple_impl &operator=(const _tuple_impl &tpl) noexcept(
        _and_<is_nothrow_copy_assignable<Ts>...>::value) {
        _va_packer(_tuple_base<Ids, Ts>::operator=(
            static_cast<const _tuple_base<Ids, Ts> &>(tpl).get())...);
        return *this;
    }

    constexpr _tuple_impl &operator=(_tuple_impl &&tpl) noexcept(
        _and_<is_nothrow_move_assignable<Ts>...>::value) {
        _va_packer(_tuple_base<Ids, Ts>::operator=(ala::forward<Ts>(
            static_cast<_tuple_base<Ids, Ts> &>(tpl).get()))...);
        return *this;
    }

    constexpr void
    swap(_tuple_impl &tpl) noexcept(_and_<is_nothrow_swappable<Ts>...>::value) {
        _va_packer(_tuple_base<Ids, Ts>::swap(
            static_cast<_tuple_base<Ids, Ts> &>(tpl))...);
    }
};

template<typename... Ts>
struct tuple {
private:
    typedef _tuple_impl<index_sequence_for<Ts...>, Ts...> impl_type;
    impl_type _impl;
    template<size_t Id, typename... Us>
    friend constexpr typename tuple_element<Id, tuple<Us...>>::type &
    get(tuple<Us...> &) noexcept;
    template<size_t Id, typename... Us>
    friend constexpr const typename tuple_element<Id, tuple<Us...>>::type &
    get(const tuple<Us...> &) noexcept;
    template<size_t Id, typename... Us>
    friend constexpr typename tuple_element<Id, tuple<Us...>>::type &&
    get(tuple<Us...> &&) noexcept;
    template<size_t Id, typename... Us>
    friend constexpr const typename tuple_element<Id, tuple<Us...>>::type &&
    get(const tuple<Us...> &&) noexcept;

public:
    tuple(tuple const &) = default;
    tuple(tuple &&) = default;

    // make template not specialization immediately (msvc not need this)
    template<typename Dummy = true_type,
             typename = enable_if_t<
                 _and_<Dummy, _and_<is_default_constructible<Ts>...>,
                       _and_<is_implicitly_default_constructible<Ts>...>>::value>>
    constexpr tuple() noexcept(
        _and_<is_nothrow_default_constructible<Ts>...>::value) {}

    template<typename Dummy = true_type, typename = void,
             typename = enable_if_t<
                 _and_<Dummy, _and_<is_default_constructible<Ts>...>,
                       _not_<_and_<is_implicitly_default_constructible<Ts>...>>>::value>>
    explicit constexpr tuple() noexcept(
        _and_<is_nothrow_default_constructible<Ts>...>::value) {}

    template<typename Dummy = true_type, bool = true,
             typename = enable_if_t<_and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                                          _and_<is_copy_constructible<Ts>...>,
                                          _and_<is_convertible<const Ts &, Ts>...>>::value>>
    constexpr tuple(const Ts &... vs) noexcept(
        _and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(vs...) {}

    template<typename Dummy = true_type, bool = true, typename = void,
             typename = enable_if_t<
                 _and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                       _and_<is_copy_constructible<Ts>...>,
                       _not_<_and_<is_convertible<const Ts &, Ts>...>>>::value>>
    explicit constexpr tuple(const Ts &... vs) noexcept(
        _and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(vs...) {}

    template<typename... Us,
             typename =
                 enable_if_t<_and_<bool_constant<sizeof...(Ts) >= 1>,
                                   bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                                   _and_<is_constructible<Ts, Us &&>...>,
                                   _and_<is_convertible<Ts, Us &&>...>>::value>>
    constexpr tuple(Us &&... us) noexcept(
        is_nothrow_constructible<impl_type, Us...>::value)
        : _impl(ala::forward<Us>(us)...) {}

    template<typename... Us, typename = void,
             typename =
                 enable_if_t<_and_<bool_constant<sizeof...(Ts) >= 1>,
                                   bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                                   _and_<is_constructible<Ts, Us &&>...>,
                                   _not_<_and_<is_convertible<Ts, Us &&>...>>>::value>>
    explicit constexpr tuple(Us &&... us) noexcept(
        is_nothrow_constructible<impl_type, Us...>::value)
        : _impl(ala::forward<Us>(us)...) {}

    template<typename Tuple>
    struct _copy_cons_check;

    template<template<typename...> class TupleTemplt, typename... Us>
    struct _copy_cons_check<TupleTemplt<Us...>> {
        static constexpr bool ok =
            _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                  _and_<is_constructible<Ts, const Us &>...>,
                  _or_<bool_constant<sizeof...(Ts) != 1>,
                       _not_<_or_<is_convertible<const tuple<Us> &, Ts>...,
                                  is_constructible<Ts, const tuple<Us> &>...,
                                  is_same<Ts, Us>...>>>>::value;
        static constexpr bool imp = ok &&
                                    _and_<is_convertible<const Us &, Ts>...>::value;
        static constexpr bool exp = ok &&
                                    !_and_<is_convertible<const Us &, Ts>...>::value;
    };

    template<typename Tuple>
    struct _move_cons_check;

    template<template<typename...> class TupleTemplt, typename... Us>
    struct _move_cons_check<TupleTemplt<Us...>> {
        static constexpr bool ok = _and_<
            bool_constant<sizeof...(Ts) == sizeof...(Us)>,
            _and_<is_constructible<Ts, Us &&>...>,
            _or_<bool_constant<sizeof...(Ts) != 1>,
                 _not_<_or_<is_convertible<tuple<Us>, Ts>...,
                            is_constructible<Ts, tuple<Us>>..., is_same<Ts, Us>...>>>>::value;
        static constexpr bool imp = ok &&
                                    _and_<is_convertible<Us &&, Ts>...>::value;
        static constexpr bool exp = ok &&
                                    !_and_<is_convertible<Us &&, Ts>...>::value;
    };

    template<typename Tuple, typename Rmcvref = remove_cvref_t<Tuple>,
             typename = enable_if_t<_is_tuple<Rmcvref>::value>,
             typename = enable_if_t<conditional_t<
                 is_lvalue_reference<Tuple>::value, _copy_cons_check<Rmcvref>,
                 _move_cons_check<Rmcvref>>::imp>>
    constexpr tuple(Tuple &&tp) noexcept(
        is_nothrow_constructible<impl_type, Tuple &&>::value)
        : _impl(ala::forward<Tuple>(tp)) {}

    template<typename Tuple, typename Rmcvref = remove_cvref_t<Tuple>,
             typename = enable_if_t<_is_tuple<Rmcvref>::value>, typename = void,
             typename = enable_if_t<conditional_t<
                 is_lvalue_reference<Tuple>::value, _copy_cons_check<Rmcvref>,
                 _move_cons_check<Rmcvref>>::exp>>
    explicit constexpr tuple(Tuple &&tp) noexcept(
        is_nothrow_constructible<impl_type, Tuple &&>::value)
        : _impl(ala::forward<Tuple>(tp)) {}

    /*
    template<typename U1, typename U2>
    using _pair_helper = _and_<is_constructible<tuple_element_t<0, tuple>, U1>,
                               is_constructible<tuple_element_t<1, tuple>, U2>>;

    template<typename U1, typename U2>
    using _pair_helper_conv = _and_<is_convertible<U1, tuple_element_t<0, tuple>>,
                                    is_convertible<U2, tuple_element_t<1, tuple>>>;

    template<typename U1, typename U2,
             typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) == 2>,
                                          _pair_helper<const U1 &, const U2 &>>::value>,
             typename = enable_if_t<_pair_helper_conv<const U1 &, const U2 &>::value>>
    tuple(const pair<U1, U2> &p) noexcept(
        is_nothrow_copy_constructible<U1>::value
            &&is_nothrow_copy_constructible<U2>::value)
        : _impl(p.first, p.second) {}

    template<typename U1, typename U2, typename = void,
             typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) == 2>,
                                          _pair_helper<const U1 &, const U2 &>>::value>,
             typename = enable_if_t<!_pair_helper_conv<const U1 &, const U2 &>::value>>
    explicit tuple(const pair<U1, U2> &p) noexcept(
        is_nothrow_copy_constructible<U1>::value
            &&is_nothrow_copy_constructible<U2>::value)
        : _impl(p.first, p.second) {}

    template<typename U1, typename U2,
             typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) == 2>,
                                          _pair_helper<U1 &&, U2 &&>>::value>,
             typename = enable_if_t<_pair_helper_conv<U1 &&, U2 &&>::value>>
    tuple(pair<U1, U2> &&p) noexcept(is_nothrow_copy_constructible<U1>::value &&
                                         is_nothrow_copy_constructible<U2>::value)
        : _impl(ala::forward<U1>(p.first), ala::forward<U2>(p.second)) {}

    template<typename U1, typename U2, typename = void,
             typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) == 2>,
                                          _pair_helper<U1 &&, U2 &&>>::value>,
             typename = enable_if_t<!_pair_helper_conv<U1 &&, U2 &&>::value>>
    explicit tuple(pair<U1, U2> &&p) noexcept(
        is_nothrow_copy_constructible<U1>::value
            &&is_nothrow_copy_constructible<U2>::value)
        : _impl(ala::forward<U1>(p.first), ala::forward<U2>(p.second)) {}
*/
    template<typename Tuple>
    struct _copy_asgn_check;

    template<typename... Us>
    struct _copy_asgn_check<tuple<Us...>> {
        static constexpr bool ok =
            _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                  _and_<is_assignable<Ts &, const Us &>...>>::value;
    };

    template<typename Tuple>
    struct _move_asgn_check;

    template<typename... Us>
    struct _move_asgn_check<tuple<Us...>> {
        static constexpr bool ok =
            _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                  _and_<is_assignable<Ts &, Us &&>...>>::value;
    };

    template<typename Tuple, typename Rmcvref = remove_cvref_t<Tuple>,
             typename = enable_if_t<_is_tuple<Rmcvref>::value>,
             typename = enable_if_t<conditional_t<is_lvalue_reference<Tuple>::value,
                                                  _copy_asgn_check<Rmcvref>,
                                                  _move_asgn_check<Rmcvref>>::ok>>
    constexpr tuple &operator=(Tuple &&tp) noexcept(
        is_nothrow_assignable<impl_type &, Tuple &&>::value) {
        _impl.operator=(ala::forward<Tuple>(tp));
        return *this;
    }

    template<typename Dummy = true_type,
             typename = enable_if_t<_and_<Dummy, is_copy_assignable<Ts>...>::value>>
    constexpr tuple &operator=(const tuple &tp) noexcept(
        _and_<is_nothrow_copy_assignable<Ts>...>::value) {
        _impl.operator=(tp._impl);
        return *this;
    }

    template<typename Dummy = true_type,
             typename = enable_if_t<_and_<Dummy, is_move_assignable<Ts>...>::value>>
    constexpr tuple &operator=(tuple &&tp) {
        _impl.operator=(ala::move(tp._impl));
        return *this;
    }
    /*
    template<typename U1, typename U2>
    using _pair_helper_asgn =
        _and_<is_assignable<tuple_element_t<0, tuple> &, U1>,
              is_assignable<tuple_element_t<1, tuple> &, U2>>;

    template<typename U1, typename U2,
             typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) == 2>,
                                          _pair_helper_asgn<const U1 &, const U2 &>>::value>>
    constexpr tuple &operator=(const pair<U1, U2> &p) noexcept(
        is_nothrow_assignable<tuple_element_t<0, tuple> &, const U1 &>::value &&
            is_nothrow_assignable<tuple_element_t<1, tuple> &, const U2 &>::value) {
        ala::get<0>(*this) = p.first;
        ala::get<1>(*this) = p.second;
    }

    template<typename U1, typename U2,
             typename = enable_if_t<_and_<bool_constant<sizeof...(Ts) == 2>,
                                          _pair_helper_asgn<U1 &&, U2 &&>>::value>>
    constexpr tuple &operator=(pair<U1, U2> &&p) noexcept(
        is_nothrow_assignable<tuple_element_t<0, tuple> &, U1 &&>::value
            &&is_nothrow_assignable<tuple_element_t<1, tuple> &, U2 &&>::value) {
        ala::get<0>(*this) = ala::move(p.first);
        ala::get<1>(*this) = ala::move(p.second);
    }
*/
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
    template<typename... Ts, typename... Us,
             typename = enable_if_t<(sizeof...(Ts) < sizeof...(Us))>>
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
    return _tuple_equal_impl<sizeof...(Ts), sizeof...(Ts) == sizeof...(Us)>()(lhs,
                                                                              rhs);
}

template<typename... Ts, typename... Us>
constexpr bool operator!=(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    return !(lhs == rhs);
}

template<typename... Ts, typename... Us>
constexpr bool operator<(const tuple<Ts...> &lhs, const tuple<Us...> &rhs) {
    constexpr size_t Size = sizeof...(Ts) < sizeof...(Us) ? sizeof...(Ts) :
                                                            sizeof...(Us);
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
constexpr tuple<unwrap_ref_decay_t<Ts>...> make_tuple(Ts &&... args) {
    return tuple<unwrap_ref_decay_t<Ts>...>(ala::forward<Ts>(args)...);
}

template<typename... Ts>
constexpr tuple<Ts &&...> forward_as_tuple(Ts &&... args) noexcept {
    return tuple<Ts &&...>(ala::forward<Ts>(args)...);
}

template<typename EmptyTuple, typename EmptySeq, typename ArgSeq>
struct _tuple_cat_impl;

template<typename... RetTs, size_t... RetIs, size_t... Tuple1Is>
struct _tuple_cat_impl<tuple<RetTs...>, index_sequence<RetIs...>,
                       index_sequence<Tuple1Is...>> {
    template<typename Tuple1>
    constexpr decltype(auto) operator()(tuple<RetTs...> t, Tuple1 &&t0) {
        return make_tuple(ala::forward<RetTs>(ala::get<RetIs>(t))...,
                          ala::get<Tuple1Is>(ala::forward<Tuple1>(t0))...);
    }

    template<typename Tuple1, typename Tuple2, typename... Tuples>
    constexpr decltype(auto) operator()(tuple<RetTs...> t, Tuple1 &&t0,
                                        Tuple2 &&t1, Tuples &&... tpls) {
        typedef remove_reference_t<Tuple1> T1;
        typedef remove_reference_t<Tuple2> T2;

        typedef _tuple_cat_impl<
            tuple<RetTs..., tuple_element_t<Tuple1Is, T1> &&...>,
            make_index_sequence<sizeof...(RetTs) + tuple_size<T1>::value>,
            make_index_sequence<tuple_size<T2>::value>>
            next;

        return next()(forward_as_tuple(ala::forward<RetTs>(ala::get<RetIs>(t))...,
                                       ala::get<Tuple1Is>(
                                           ala::forward<Tuple1>(t0))...),
                      ala::forward<Tuple2>(t1), ala::forward<Tuples>(tpls)...);
    }
};

template<typename Tuple1, typename... Tuples>
constexpr decltype(auto) tuple_cat(Tuple1 &&t0, Tuples &&... tpls) {
    typedef typename remove_reference<Tuple1>::type T1;
    return _tuple_cat_impl<tuple<>, index_sequence<>,
                           make_index_sequence<tuple_size<T1>::value>>()(
        tuple<>(), ala::forward<Tuple1>(t0), ala::forward<Tuples>(tpls)...);
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
    constexpr const _ignore_t &operator=(T &&) const {
        return *this;
    }
};

ALA_VAR_INLINE constexpr _ignore_t ignore{};

template<typename F, typename Tuple, size_t... I>
constexpr decltype(auto) _apply_impl(F &&f, Tuple &&tpl, index_sequence<I...>) {
    return ala::invoke(ala::forward<F>(f),
                       ala::get<I>(ala::forward<Tuple>(tpl))...);
}

template<typename F, typename Tuple>
constexpr decltype(auto) apply(F &&f, Tuple &&tpl) {
    return _apply_impl(
        ala::forward<F>(f), ala::forward<Tuple>(tpl),
        make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{});
}

template<typename T, typename Tuple, size_t... I>
constexpr T _make_from_tuple_impl(Tuple &&tpl, index_sequence<I...>) {
    return T(ala::get<I>(ala::forward<Tuple>(tpl))...);
}

template<typename T, typename Tuple>
constexpr T make_from_tuple(Tuple &&tpl) {
    return _make_from_tuple_impl<T>(
        ala::forward<Tuple>(tpl),
        make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{});
}

template<typename T, size_t Cur, typename IntSeq, typename... Ts>
struct _type_to_index_impl;

template<typename T, size_t Cur, typename T1, typename... Ts, size_t... SIs>
struct _type_to_index_impl<T, Cur, index_sequence<SIs...>, T1, Ts...> {
    typedef conditional_t<
        is_same<T, T1>::value,
        typename _type_to_index_impl<T, Cur + 1, index_sequence<SIs..., Cur>, Ts...>::type,
        typename _type_to_index_impl<T, Cur + 1, index_sequence<SIs...>, Ts...>::type>
        type;
};

template<typename T, size_t Cur, size_t... SIs>
struct _type_to_index_impl<T, Cur, index_sequence<SIs...>> {
    typedef index_sequence<SIs...> type;
};

template<typename T, typename... Ts>
using _type_to_index_t =
    typename _type_to_index_impl<T, 0, index_sequence<>, Ts...>::type;

template<typename T, typename... Ts>
constexpr T &get(tuple<Ts...> &t) noexcept {
    typedef _type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(t);
}

template<typename T, typename... Ts>
constexpr T &&get(tuple<Ts...> &&t) noexcept {
    typedef _type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(ala::move(t));
}

template<typename T, typename... Ts>
constexpr const T &get(const tuple<Ts...> &t) noexcept {
    typedef _type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(t);
}

template<typename T, typename... Ts>
constexpr const T &&get(const tuple<Ts...> &&t) noexcept {
    typedef _type_to_index_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(ala::move(t));
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<typename... Ts>
tuple(Ts...)->tuple<Ts...>;

template<typename T1, typename T2>
tuple(pair<T1, T2>)->tuple<T1, T2>;
#endif

template<typename T1, typename T2>
template<typename... Args1, typename... Args2, size_t... I1, size_t... I2>
constexpr pair<T1, T2>::pair(piecewise_construct_t, tuple<Args1...> &first_args,
                             tuple<Args2...> &second_args,
                             index_sequence<I1...>, index_sequence<I2...>)
    : first(ala::forward<Args1>(ala::get<I1>(first_args))...),
      second(ala::forward<Args2>(ala::get<I2>(second_args))...) {}

// extra features

template<template<typename> class Templt, size_t Cur, typename IntSeq, typename... Ts>
struct _type_pick_impl;

template<template<typename> class Templt, size_t Cur, typename T1,
         typename... Ts, size_t... SIs>
struct _type_pick_impl<Templt, Cur, index_sequence<SIs...>, T1, Ts...> {
    typedef conditional_t<
        Templt<T1>::value,
        typename _type_pick_impl<Templt, Cur + 1, index_sequence<SIs..., Cur>, Ts...>::type,
        typename _type_pick_impl<Templt, Cur + 1, index_sequence<SIs...>, Ts...>::type>
        type;
};

template<template<typename> class Templt, size_t Cur, size_t... SIs>
struct _type_pick_impl<Templt, Cur, index_sequence<SIs...>> {
    typedef index_sequence<SIs...> type;
};

template<template<typename> class Templt, typename... Ts>
using _type_pick_t =
    typename _type_pick_impl<Templt, 0, index_sequence<>, Ts...>::type;

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(tuple<Ts...> &t, index_sequence<Ids...>) {
    return forward_as_tuple(get<Ids>(t)...);
}

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(tuple<Ts...> &&t, index_sequence<Ids...>) {
    return forward_as_tuple(get<Ids>(ala::move(t))...); // move many times?
}

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(const tuple<Ts...> &t, index_sequence<Ids...>) {
    return forward_as_tuple(get<Ids>(t)...);
}

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(const tuple<Ts...> &&t, index_sequence<Ids...>) {
    return forward_as_tuple(get<Ids>(ala::move(t))...);
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(tuple<Ts...> &t) {
    typedef _type_pick_t<Templt, Ts...> Index;
    return _choice_helper(t, Index());
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(tuple<Ts...> &&t) {
    typedef _type_pick_t<Templt, Ts...> Index;
    return _choice_helper(ala::move(t), Index());
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(const tuple<Ts...> &t) {
    typedef _type_pick_t<Templt, Ts...> Index;
    return _choice_helper(t, Index());
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(const tuple<Ts...> &&t) {
    typedef _type_pick_t<Templt, Ts...> Index;
    return _choice_helper(ala::move(t), Index());
}

}; // namespace ala

#endif // HEAD