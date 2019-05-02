#ifndef _ALA_TUPLE_H
#define _ALA_TUPLE_H

#include <ala/detail/integer_sequence.h>
#include <ala/detail/pair.h>
#include <ala/detail/tuple_operator.h>
#include <ala/type_traits.h>

namespace ala {

template<size_t I, typename T, bool = is_empty<T>::value && !is_final<T>::value>
struct _tuple_base;

template<size_t I, typename T>
struct _tuple_base<I, T, false> {
    T value;

    template<typename U,
             typename = enable_if_t<_and_<_not_<is_same<remove_cvref_t<U>, _tuple_base>>,
                                          is_constructible<T, U>>::value>>
    explicit constexpr _tuple_base(U &&u) noexcept(
        is_nothrow_constructible<T, U>::value)
        : value(ala::forward<U>(u)) {}

    template<typename U>
    _tuple_base &operator=(U &&u) noexcept(is_nothrow_assignable<T &, U>::value) {
        value = ala::forward<U>(u);
        return *this;
    }

    constexpr _tuple_base() = default;

    constexpr const T &get() const noexcept {
        return value;
    }

    void swap(_tuple_base &tb) noexcept(is_nothrow_swappable<_tuple_base>::value) {
        ala::swap(value, tb.value);
    }
};

template<size_t I, typename T>
struct _tuple_base<I, T, true>: private T {
    template<typename U,
             typename = enable_if_t<_and_<_not_<is_same<remove_cvref_t<U>, _tuple_base>>,
                                          is_constructible<T, U>>::value>>
    explicit constexpr _tuple_base(U &&u) noexcept(
        is_nothrow_constructible<T, U>::value)
        : T(ala::forward<U>(u)) {}

    template<typename U>
    _tuple_base &operator=(U &&u) noexcept(is_nothrow_assignable<T &, U>::value) {
        T::operator=(ala::forward<U>(u));
        return *this;
    }

    constexpr _tuple_base() = default;

    constexpr const T &get() const noexcept {
        return static_cast<const T &>(*this);
    }

    void swap(_tuple_base &tb) noexcept(is_nothrow_swappable<_tuple_base>::value) {
        ala::swap(get(), tb.get());
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
    return const_cast<type &>(static_cast<_tuple_base<I, type> &>(tp._impl).get());
}

template<size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>> &
get(const tuple<Ts...> &tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return const_cast<const type &>(
        static_cast<const _tuple_base<I, type> &>(tp._impl).get());
}

template<size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>> &&get(tuple<Ts...> &&tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return const_cast<type &&>(
        static_cast<_tuple_base<I, type> &&>(tp._impl).get());
}

template<size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>> &&
get(const tuple<Ts...> &&tp) noexcept {
    typedef tuple_element_t<I, tuple<Ts...>> type;
    return const_cast<const type &&>(
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
    template<typename... Us,
             typename = enable_if_t<(sizeof...(Us) > 1) ||
                                    !_and_<_is_tuple<remove_cvref_t<Us>>...>::value>>
    explicit constexpr _tuple_impl(Us &&... us) noexcept(
        _and_<is_nothrow_constructible<Ts, Us>...>::value)
        : _tuple_base<Ids, Ts>(ala::forward<Us>(us))... {}

    template<typename Tuple,
             typename = enable_if_t<_is_tuple<remove_cvref_t<Tuple>>::value>>
    constexpr _tuple_impl(Tuple &&tp) noexcept(
        _and_<is_nothrow_constructible<
            Ts, conditional_t<is_lvalue_reference<Tuple>::value,
                              tuple_element_t<Ids, remove_reference_t<Tuple>> &,
                              tuple_element_t<Ids, remove_reference_t<Tuple>> &&>>...>::value)
        : _tuple_base<Ids, Ts>(ala::get<Ids>(ala::forward<Tuple>(tp)))... {}

    template<typename... Dummy>
    constexpr void _packer(Dummy...) {}

    template<typename Tuple,
             typename = enable_if_t<_is_tuple<remove_cvref_t<Tuple>>::value>>
    constexpr _tuple_impl &operator=(Tuple &&tp) noexcept(
        _and_<is_nothrow_assignable<
            Ts &,
            conditional_t<is_lvalue_reference<Tuple>::value,
                          tuple_element_t<Ids, remove_reference_t<Tuple>> &,
                          tuple_element_t<Ids, remove_reference_t<Tuple>> &&>>...>::value) {
        _packer(_tuple_base<Ids, Ts>::operator=(
            ala::get<Ids>(ala::forward<Tuple>(tp)))...);
        return *this;
    }

    constexpr _tuple_impl() = default;

    constexpr void
    swap(_tuple_impl &tpl) noexcept(_and_<is_nothrow_swappable<Ts>...>::value) {
        _packer(_tuple_base<Ids, Ts>::swap(
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

    // Dummy make template not specialization immediately (msvc not need this)
    template<typename Dummy = true_type,
             typename = enable_if_t<
                 _and_<Dummy, _and_<is_default_constructible<Ts>...>,
                       _and_<is_implicitly_default_constructible<Ts>...>>::value>>
    constexpr tuple() noexcept(_and_<is_nothrow_default_constructible<Ts>...>::value)
        : _impl() {}

    template<typename Dummy = true_type, typename = void,
             typename = enable_if_t<
                 _and_<Dummy, _and_<is_default_constructible<Ts>...>,
                       _not_<_and_<is_implicitly_default_constructible<Ts>...>>>::value>>
    explicit constexpr tuple() noexcept(
        _and_<is_nothrow_default_constructible<Ts>...>::value)
        : _impl() {}

    template<typename Dummy = true_type, bool = true,
             typename = enable_if_t<_and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                                          _and_<is_copy_constructible<Ts>...>,
                                          _and_<is_convertible<const Ts &, Ts>...>>::value>>
    constexpr tuple(const Ts &... ts) noexcept(
        _and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(ts...) {}

    template<typename Dummy = true_type, bool = true, typename = void,
             typename = enable_if_t<
                 _and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                       _and_<is_copy_constructible<Ts>...>,
                       _not_<_and_<is_convertible<const Ts &, Ts>...>>>::value>>
    explicit constexpr tuple(const Ts &... ts) noexcept(
        _and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(ts...) {}

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
    struct _copy_ctor_check {
        static constexpr bool imp = false;
        static constexpr bool exp = false;
    };

    template<template<typename...> class TupleTemplt, typename... Us>
    struct _copy_ctor_check<TupleTemplt<Us...>> {
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
    struct _move_ctor_check {
        static constexpr bool imp = false;
        static constexpr bool exp = false;
    };

    template<template<typename...> class TupleTemplt, typename... Us>
    struct _move_ctor_check<TupleTemplt<Us...>> {
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

    template<typename Tuple, typename T1 = remove_cvref_t<Tuple>,
             typename = enable_if_t<!is_same<T1, tuple>::value>,
             typename = enable_if_t<_is_tuple<T1>::value>,
             typename = enable_if_t<
                 conditional_t<is_lvalue_reference<Tuple>::value,
                               _copy_ctor_check<T1>, _move_ctor_check<T1>>::imp>>
    constexpr tuple(Tuple &&tp) noexcept(
        is_nothrow_constructible<impl_type, Tuple &&>::value)
        : _impl(ala::forward<Tuple>(tp)) {}

    template<typename Tuple, typename T1 = remove_cvref_t<Tuple>,
             typename = enable_if_t<!is_same<T1, tuple>::value>,
             typename = enable_if_t<_is_tuple<T1>::value>,
             typename = enable_if_t<
                 conditional_t<is_lvalue_reference<Tuple>::value,
                               _copy_ctor_check<T1>, _move_ctor_check<T1>>::exp>,
             typename = void>
    explicit constexpr tuple(Tuple &&tp) noexcept(
        is_nothrow_constructible<impl_type, Tuple &&>::value)
        : _impl(ala::forward<Tuple>(tp)) {}

    template<typename Tuple>
    struct _copy_asgn_check {
        static constexpr bool ok = false;
    };

    template<template<typename...> class TupleTemplt, typename... Us>
    struct _copy_asgn_check<TupleTemplt<Us...>> {
        static constexpr bool ok =
            _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                  _and_<is_assignable<Ts &, const Us &>...>>::value;
    };

    template<typename Tuple>
    struct _move_asgn_check {
        static constexpr bool ok = false;
    };

    template<template<typename...> class TupleTemplt, typename... Us>
    struct _move_asgn_check<TupleTemplt<Us...>> {
        static constexpr bool ok =
            _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                  _and_<is_assignable<Ts &, Us &&>...>>::value;
    };

    template<typename Tuple, typename T1 = remove_cvref_t<Tuple>,
             typename = enable_if_t<!is_same<T1, tuple>::value>,
             typename = enable_if_t<_is_tuple<T1>::value>,
             typename = enable_if_t<
                 conditional_t<is_lvalue_reference<Tuple>::value,
                               _copy_asgn_check<T1>, _move_asgn_check<T1>>::ok>>
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

template<typename EmptyTuple, typename... Tuples>
struct _tuple_cat_type;

template<typename... Ts, typename... T1Ts, typename... Tuples>
struct _tuple_cat_type<tuple<Ts...>, tuple<T1Ts...>, Tuples...> {
    typedef typename _tuple_cat_type<tuple<Ts..., T1Ts...>, Tuples...>::type type;
};

template<typename... Ts>
struct _tuple_cat_type<tuple<Ts...>> {
    typedef tuple<Ts...> type;
};

template<typename... Tuples>
using _tuple_cat_t =
    typename _tuple_cat_type<tuple<>, remove_cvref_t<Tuples>...>::type;

template<typename CatTuple, typename TmpTuple, typename TmpSeq, typename T1Is>
struct _tuple_cat_impl;

template<typename CatTuple, typename... TmpTs, size_t... TmpIs, size_t... T1Is>
struct _tuple_cat_impl<CatTuple, tuple<TmpTs...>, index_sequence<TmpIs...>,
                       index_sequence<T1Is...>> {
    template<typename Tuple1>
    static constexpr CatTuple _cat(tuple<TmpTs...> &&tmp, Tuple1 &&t1) {
        return CatTuple(static_cast<TmpTs>(ala::get<TmpIs>(tmp))...,
                        ala::get<T1Is>(ala::forward<Tuple1>(t1))...);
    }

    template<typename Tuple1, typename Tuple2, typename... Tuples>
    static constexpr CatTuple _cat(tuple<TmpTs...> &&tmp, Tuple1 &&t1,
                                   Tuple2 &&t2, Tuples &&... ts) {
        typedef remove_reference_t<Tuple1> T1;
        typedef remove_reference_t<Tuple2> T2;

        typedef _tuple_cat_impl<
            CatTuple, tuple<TmpTs..., tuple_element_t<T1Is, T1> &&...>,
            make_index_sequence<sizeof...(TmpTs) + tuple_size<T1>::value>,
            make_index_sequence<tuple_size<T2>::value>>
            next_t;

        return next_t::_cat(
            forward_as_tuple(static_cast<TmpTs>(ala::get<TmpIs>(tmp))...,
                             ala::get<T1Is>(ala::forward<Tuple1>(t1))...),
            ala::forward<Tuple2>(t2), ala::forward<Tuples>(ts)...);
    }
};

template<typename Tuple1, typename... Tuples>
constexpr _tuple_cat_t<Tuple1, Tuples...> tuple_cat(Tuple1 &&t1, Tuples &&... ts) {
    typedef _tuple_cat_impl<
        _tuple_cat_t<Tuple1, Tuples...>, tuple<>, index_sequence<>,
        make_index_sequence<tuple_size<remove_reference_t<Tuple1>>::value>>
        impl_t;
    return impl_t::_cat(tuple<>(), ala::forward<Tuple1>(t1),
                        ala::forward<Tuples>(ts)...);
}

constexpr tuple<> tuple_cat() {
    return tuple<>();
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

#if _ALA_ENABLE_INLINE_VAR
inline constexpr _ignore_t ignore{};
#endif

template<typename F, typename Tuple, size_t... I>
constexpr invoke_result_t<F, Tuple> _apply_impl(F &&f, Tuple &&tpl,
                                                index_sequence<I...>) {
    return ala::invoke(ala::forward<F>(f),
                       ala::get<I>(ala::forward<Tuple>(tpl))...);
}

template<typename F, typename Tuple>
constexpr invoke_result_t<F, Tuple> apply(F &&f, Tuple &&tpl) {
    return _apply_impl(
        ala::forward<F>(f), ala::forward<Tuple>(tpl),
        make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>());
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

template<typename T>
struct _is_same_unary {
    template<typename U>
    struct _impl: is_same<T, U> {};
};

template<typename T, typename... Ts>
using _type_pick_same_t = typename _type_pick_impl<
    _is_same_unary<T>::template _impl, 0, index_sequence<>, Ts...>::type;

template<typename T, typename... Ts>
constexpr T &get(tuple<Ts...> &t) noexcept {
    typedef _type_pick_same_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(t);
}

template<typename T, typename... Ts>
constexpr T &&get(tuple<Ts...> &&t) noexcept {
    typedef _type_pick_same_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(ala::move(t));
}

template<typename T, typename... Ts>
constexpr const T &get(const tuple<Ts...> &t) noexcept {
    typedef _type_pick_same_t<T, Ts...> Index;
    static_assert(Index::size() == 1, "no type or more than one type");
    return get<get_integer_sequence<0, Index>::value>(t);
}

template<typename T, typename... Ts>
constexpr const T &&get(const tuple<Ts...> &&t) noexcept {
    typedef _type_pick_same_t<T, Ts...> Index;
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

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(tuple<Ts...> &t, index_sequence<Ids...>)
    -> decltype(forward_as_tuple(get<Ids>(t)...)) {
    return forward_as_tuple(get<Ids>(t)...);
}

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(tuple<Ts...> &&t, index_sequence<Ids...>)
    -> decltype(forward_as_tuple(get<Ids>(ala::move(t))...)) {
    return forward_as_tuple(get<Ids>(ala::move(t))...);
}

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(const tuple<Ts...> &t, index_sequence<Ids...>)
    -> decltype(forward_as_tuple(get<Ids>(t)...)) {
    return forward_as_tuple(get<Ids>(t)...);
}

template<typename... Ts, size_t... Ids>
constexpr auto _choice_helper(const tuple<Ts...> &&t, index_sequence<Ids...>)
    -> decltype(forward_as_tuple(get<Ids>(ala::move(t))...)) {
    return forward_as_tuple(get<Ids>(ala::move(t))...);
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(tuple<Ts...> &t)
    -> decltype(_choice_helper(t, _type_pick_t<Templt, Ts...>())) {
    return _choice_helper(t, _type_pick_t<Templt, Ts...>());
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(tuple<Ts...> &&t)
    -> decltype(_choice_helper(ala::move(t), _type_pick_t<Templt, Ts...>())) {
    return _choice_helper(ala::move(t), _type_pick_t<Templt, Ts...>());
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(const tuple<Ts...> &t)
    -> decltype(_choice_helper(t, _type_pick_t<Templt, Ts...>())) {
    return _choice_helper(t, _type_pick_t<Templt, Ts...>());
}

template<template<typename> class Templt, typename... Ts>
constexpr auto choice(const tuple<Ts...> &&t)
    -> decltype(_choice_helper(ala::move(t), _type_pick_t<Templt, Ts...>())) {
    return _choice_helper(ala::move(t), _type_pick_t<Templt, Ts...>());
}

}; // namespace ala

#endif // HEAD