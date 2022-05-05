#ifndef _ALA_TUPLE_H
#define _ALA_TUPLE_H

#include <ala/detail/pair.h>

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

    constexpr _tuple_base(): value() {}

    constexpr const T &get() const noexcept {
        return value;
    }

    void swap(_tuple_base &tb) noexcept(is_nothrow_swappable<_tuple_base>::value) {
        ala::_swap_adl(value, tb.value);
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

    constexpr _tuple_base(){};

    constexpr const T &get() const noexcept {
        return static_cast<const T &>(*this);
    }

    void swap(_tuple_base &tb) noexcept(is_nothrow_swappable<_tuple_base>::value) {
        ala::_swap_adl(get(), tb.get());
    }
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

template<typename Seq, typename... Ts>
struct _tuple_impl;

template<typename... Ts>
struct tuple;

template<size_t... Ids, typename... Ts>
struct _tuple_impl<index_sequence<Ids...>, Ts...>: _tuple_base<Ids, Ts>... {
    template<typename... Us>
    explicit constexpr _tuple_impl(true_type, Us &&... us) noexcept(
        _and_<is_nothrow_constructible<Ts, Us>...>::value)
        : _tuple_base<Ids, Ts>(ala::forward<Us>(us))... {}

    template<typename Tuple>
    constexpr _tuple_impl(false_type, Tuple &&tp) noexcept(
        _and_<is_nothrow_constructible<
            Ts, decltype(ala::get<Ids>(declval<Tuple>()))>...>::value)
        : _tuple_base<Ids, Ts>(ala::get<Ids>(ala::forward<Tuple>(tp)))... {}

    template<typename... Dummy>
    constexpr void _packer(Dummy &&...) {}

    template<typename Tuple>
    constexpr _tuple_impl &operator=(Tuple &&tp) noexcept(
        _and_<is_nothrow_assignable<
            Ts &, decltype(ala::get<Ids>(declval<Tuple>()))>...>::value) {
        _packer(_tuple_base<Ids, Ts>::operator=(
            ala::get<Ids>(ala::forward<Tuple>(tp)))...);
        return *this;
    }

    constexpr _tuple_impl() = default;

    constexpr void
    swap(_tuple_impl &tpl) noexcept(_and_<is_nothrow_swappable<Ts>...>::value) {
        _packer(
            (_tuple_base<Ids, Ts>::swap(static_cast<_tuple_base<Ids, Ts> &>(tpl)),
             0)...);
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
    // Fix libc++ PR23256
    template<class Tuple, class... Args>
    struct _is_copy_move_ctor: false_type {};

    template<class Tuple, class Args1>
    struct _is_copy_move_ctor<Tuple, Args1>
        : is_same<Tuple, remove_cvref_t<Args1>> {};

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
        : _impl(true_type{}, ts...) {}

    template<typename Dummy = true_type, bool = true, typename = void,
             typename = enable_if_t<
                 _and_<Dummy, bool_constant<sizeof...(Ts) >= 1>,
                       _and_<is_copy_constructible<Ts>...>,
                       _not_<_and_<is_convertible<const Ts &, Ts>...>>>::value>>
    explicit constexpr tuple(const Ts &... ts) noexcept(
        _and_<is_nothrow_copy_constructible<Ts>...>::value)
        : _impl(true_type{}, ts...) {}

    template<typename... Us,
             typename =
                 enable_if_t<_and_<bool_constant<sizeof...(Ts) >= 1>,
                                   bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                                   _not_<_is_copy_move_ctor<tuple, Us...>>,
                                   _and_<is_constructible<Ts, Us>...>,
                                   _and_<is_convertible<Us, Ts>...>>::value>>
    constexpr tuple(Us &&... us) noexcept(
        is_nothrow_constructible<impl_type, Us...>::value)
        : _impl(true_type{}, ala::forward<Us>(us)...) {}

    template<typename... Us, typename = void,
             typename =
                 enable_if_t<_and_<bool_constant<sizeof...(Ts) >= 1>,
                                   bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                                   _not_<_is_copy_move_ctor<tuple, Us...>>,
                                   _and_<is_constructible<Ts, Us>...>,
                                   _not_<_and_<is_convertible<Us, Ts>...>>>::value>>
    explicit constexpr tuple(Us &&... us) noexcept(
        is_nothrow_constructible<impl_type, Us...>::value)
        : _impl(true_type{}, ala::forward<Us>(us)...) {}

    template<typename Tcvref, typename Tuple>
    struct _ctor_check {
        static constexpr bool imp = false;
        static constexpr bool exp = false;
    };

    template<typename Tcvref, template<typename...> class TupleTemplt, typename... Us>
    struct _ctor_check<Tcvref, TupleTemplt<Us...>> {
        static constexpr bool ok = _and_<
            bool_constant<sizeof...(Ts) == sizeof...(Us)>,
            _and_<is_constructible<Ts, _copy_cvref_t<Tcvref, Us>>...>,
            _or_<bool_constant<sizeof...(Ts) != 1>,
                 _not_<_or_<is_convertible<_copy_cvref_t<Tcvref, tuple<Us>>, Ts>...,
                            is_constructible<Ts, _copy_cvref_t<Tcvref, tuple<Us>>>...,
                            is_same<Ts, Us>...>>>>::value;
        static constexpr bool imp =
            ok && _and_<is_convertible<_copy_cvref_t<Tcvref, Us>, Ts>...>::value;
        static constexpr bool exp =
            ok && !_and_<is_convertible<_copy_cvref_t<Tcvref, Us>, Ts>...>::value;
    };

    // Fix PR31384
    template<class... Us,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<tuple<Us...> &, tuple<Us...>>::imp>>
    constexpr tuple(tuple<Us...> &other): _impl(false_type{}, other) {}

    template<class... Us,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<const tuple<Us...> &, tuple<Us...>>::imp>>
    constexpr tuple(const tuple<Us...> &other): _impl(false_type{}, other) {}

    template<class... Us,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<tuple<Us...> &&, tuple<Us...>>::imp>>
    constexpr tuple(tuple<Us...> &&other)
        : _impl(false_type{}, ala::move(other)) {}

    template<class... Us,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<const tuple<Us...> &&, tuple<Us...>>::imp>>
    constexpr tuple(const tuple<Us...> &&other)
        : _impl(false_type{}, ala::move(other)) {}

    template<class U1, class U2, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<pair<U1, U2> &, pair<U1, U2>>::imp>>
    constexpr tuple(pair<U1, U2> &p): _impl(false_type{}, p) {}

    template<class U1, class U2, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<const pair<U1, U2> &, pair<U1, U2>>::imp>>
    constexpr tuple(const pair<U1, U2> &p): _impl(false_type{}, p) {}

    template<class U1, class U2, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<pair<U1, U2> &&, pair<U1, U2>>::imp>>
    constexpr tuple(pair<U1, U2> &&p): _impl(false_type{}, ala::move(p)) {}

    template<class U1, class U2, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<const pair<U1, U2> &&, pair<U1, U2>>::imp>>
    constexpr tuple(const pair<U1, U2> &&p)
        : _impl(false_type{}, ala::move(p)) {}

    template<class... Us, class = void,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<tuple<Us...> &, tuple<Us...>>::exp>>
    explicit constexpr tuple(tuple<Us...> &other): _impl(false_type{}, other) {}

    template<class... Us, class = void,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<const tuple<Us...> &, tuple<Us...>>::exp>>
    explicit constexpr tuple(const tuple<Us...> &other)
        : _impl(false_type{}, other) {}

    template<class... Us, class = void,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<tuple<Us...> &&, tuple<Us...>>::exp>>
    explicit constexpr tuple(tuple<Us...> &&other)
        : _impl(false_type{}, ala::move(other)) {}

    template<class... Us, class = void,
             class = enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                                 !is_same<tuple, tuple<Us...>>::value>,
             typename = enable_if_t<_ctor_check<const tuple<Us...> &&, tuple<Us...>>::exp>>
    explicit constexpr tuple(const tuple<Us...> &&other)
        : _impl(false_type{}, ala::move(other)) {}

    template<class U1, class U2, class = void, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<pair<U1, U2> &, pair<U1, U2>>::exp>>
    explicit constexpr tuple(pair<U1, U2> &p): _impl(false_type{}, p) {}

    template<class U1, class U2, class = void, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<const pair<U1, U2> &, pair<U1, U2>>::exp>>
    explicit constexpr tuple(const pair<U1, U2> &p): _impl(false_type{}, p) {}

    template<class U1, class U2, class = void, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<pair<U1, U2> &&, pair<U1, U2>>::exp>>
    explicit constexpr tuple(pair<U1, U2> &&p)
        : _impl(false_type{}, ala::move(p)) {}

    template<class U1, class U2, class = void, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             typename = enable_if_t<_ctor_check<const pair<U1, U2> &&, pair<U1, U2>>::exp>>
    explicit constexpr tuple(const pair<U1, U2> &&p)
        : _impl(false_type{}, ala::move(p)) {}

    template<typename Tcvref, typename Tuple>
    struct _asgn_check {
        static constexpr bool ok = false;
    };

    template<typename Tcvref, template<typename...> class TupleTemplt, typename... Us>
    struct _asgn_check<Tcvref, TupleTemplt<Us...>> {
        static constexpr bool ok =
            _and_<bool_constant<sizeof...(Ts) == sizeof...(Us)>,
                  _and_<is_assignable<Ts &, _copy_cvref_t<Tcvref, Us>>...>>::value;
    };

    template<typename Tuple>
    constexpr tuple &_operator_eq(Tuple &&tp) noexcept(
        is_nothrow_assignable<impl_type &, Tuple>::value) {
        _impl.operator=(ala::forward<Tuple>(tp));
        return *this;
    }

    template<class... Us, class = enable_if_t<sizeof...(Us) == sizeof...(Ts)>,
             class = enable_if_t<_asgn_check<const tuple<Us...> &, tuple<Us...>>::ok>>
    constexpr tuple &operator=(const tuple<Us...> &other) noexcept(
        noexcept(tuple::_operator_eq(other))) {
        return _operator_eq(other);
    }

    template<class... Us, class = enable_if_t<sizeof...(Us) == sizeof...(Ts)>,
             class = enable_if_t<_asgn_check<tuple<Us...> &&, tuple<Us...>>::ok>>
    constexpr tuple &operator=(tuple<Us...> &&other) noexcept(
        noexcept(tuple::_operator_eq(ala::move(other)))) {
        return _operator_eq(ala::move(other));
    }

    template<class U1, class U2, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             class = enable_if_t<_asgn_check<const pair<U1, U2> &, pair<U1, U2>>::ok>>
    constexpr tuple &
    operator=(const pair<U1, U2> &p) noexcept(noexcept(tuple::_operator_eq(p))) {
        return _operator_eq(p);
    }

    template<class U1, class U2, class Dummy = true_type,
             class = enable_if_t<Dummy::value && 2 == sizeof...(Ts)>,
             class = enable_if_t<_asgn_check<pair<U1, U2> &&, pair<U1, U2>>::ok>>
    constexpr tuple &operator=(pair<U1, U2> &&p) noexcept(
        noexcept(tuple::_operator_eq(ala::move(p)))) {
        return _operator_eq(ala::move(p));
    }

    // C++23
    // template<class... Us>
    // constexpr const tuple &operator=(const tuple<Us...> &other) const;
    // template<class... Us>
    // constexpr const tuple &operator=(tuple<Us...> &&other) const;
    // template<class U1, class U2>
    // constexpr const tuple &operator=(const pair<U1, U2> &p) const;
    // template<class U1, class U2>
    // constexpr const tuple &operator=(pair<U1, U2> &&p) const;

    template<int>
    struct _dummy {};

    tuple(tuple const &) = default;
    tuple(tuple &&) = default;

    constexpr tuple &
    operator=(conditional_t<_and_<is_copy_assignable<Ts>...>::value,
                            const tuple &, _dummy<0>>
                  tp) noexcept(_and_<is_nothrow_copy_assignable<Ts>...>::value) {
        _impl.operator=(tp);
        return *this;
    }

    constexpr tuple &operator=(
        conditional_t<_and_<is_move_assignable<Ts>...>::value, tuple &&, _dummy<1>>
            tp) noexcept(_and_<is_nothrow_move_assignable<Ts>...>::value) {
        _impl.operator=(ala::move(tp));
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

template<size_t N, size_t I>
struct _tuple_less_impl {
    template<typename T, typename U>
    constexpr bool operator()(const T &lhs, const U &rhs) {
        if (ala::get<I>(lhs) < ala::get<I>(rhs))
            return true;
        if (ala::get<I>(rhs) < ala::get<I>(lhs))
            return false;
        return _tuple_less_impl<N, I + 1>()(lhs, rhs);
    }
};

template<size_t N>
struct _tuple_less_impl<N, N> {
    template<typename T, typename U>
    constexpr bool operator()(const T &, const U &) {
        return tuple_size<remove_reference_t<T>>::value <
               tuple_size<remove_reference_t<U>>::value;
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
    return _tuple_less_impl<Size, 0>()(lhs, rhs);
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

template<typename Tuple>
struct _to_true_tuple {};

template<typename T, size_t N>
struct _to_true_tuple<array<T, N>> {
    template<typename Seq>
    struct _repeat {};

    template<size_t... Id>
    struct _repeat<index_sequence<Id...>> {
        template<size_t>
        using _id_t = T;
        using type = tuple<_id_t<Id>...>;
    };
    using type = typename _repeat<make_index_sequence<N>>::type;
};

template<typename... Ts>
struct _to_true_tuple<tuple<Ts...>> {
    using type = tuple<Ts...>;
};

template<typename T1, typename T2>
struct _to_true_tuple<pair<T1, T2>> {
    using type = tuple<T1, T2>;
};

template<typename T>
using _to_true_tuple_t = typename _to_true_tuple<T>::type;

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
    typename _tuple_cat_type<tuple<>, _to_true_tuple_t<remove_cvref_t<Tuples>>...>::type;

template<typename IdxSeq, typename Ret>
struct _tuple_cat_impl;

template<size_t... Is, typename Ret>
struct _tuple_cat_impl<index_sequence<Is...>, Ret> {
    template<size_t Idx, typename Tuple1, typename... Tuples>
    static constexpr decltype(auto) _deep_get(true_type, Tuple1 &&t1,
                                              Tuples &&... ts) {
        return ala::get<Idx>(ala::forward<Tuple1>(t1));
    }

    template<size_t Idx, typename Tuple1, typename... Tuples>
    static constexpr decltype(auto) _deep_get(false_type, Tuple1 &&t1,
                                              Tuples &&... ts) {
        using T1 = remove_reference_t<Tuple1>;
        return _tuple_cat_impl::_deep_get_helper<Idx - tuple_size<T1>::value>(
            ala::forward<Tuples>(ts)...);
    }

    template<size_t Idx, typename Tuple1, typename... Tuples>
    static constexpr decltype(auto) _deep_get_helper(Tuple1 &&t1,
                                                     Tuples &&... ts) {
        using T1 = remove_reference_t<Tuple1>;
        return _tuple_cat_impl::_deep_get<Idx>(
            bool_constant<(Idx < tuple_size<T1>::value)>{},
            ala::forward<Tuple1>(t1), ala::forward<Tuples>(ts)...);
    }

    template<typename... Tuples>
    static constexpr Ret _cat(Tuples &&... ts) {
        return Ret(_tuple_cat_impl::_deep_get_helper<Is>(
            ala::forward<Tuples>(ts)...)...);
    }
};

template<typename...>
struct _sum_;
template<>
struct _sum_<>: integral_constant<size_t, 0> {};
template<typename I>
struct _sum_<I>: integral_constant<size_t, I::value> {};
template<typename I, typename... Is>
struct _sum_<I, Is...>
    : integral_constant<size_t, I::value + _sum_<Is...>::value> {};

template<typename... Tuples>
constexpr _tuple_cat_t<Tuples...> tuple_cat(Tuples &&... ts) {
    using Size = _sum_<tuple_size<remove_reference_t<Tuples>>...>;
    using Idx = make_index_sequence<Size::value>;
    using Ret = _tuple_cat_t<remove_cvref_t<Tuples>...>;
    using impl_t = _tuple_cat_impl<Idx, Ret>;
    return impl_t::_cat(ala::forward<Tuples>(ts)...);
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
inline constexpr _ignore_t ignore;
#else
constexpr _ignore_t ignore;
#endif

template<typename F, typename Tuple, size_t... I>
constexpr decltype(auto) _apply_impl(F &&f, Tuple &&tpl, index_sequence<I...>) {
    return ala::invoke(ala::forward<F>(f),
                       ala::get<I>(ala::forward<Tuple>(tpl))...);
}

template<typename F, typename Tuple>
constexpr decltype(auto) apply(F &&f, Tuple &&tpl) {
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

template<typename T, typename... Ts>
constexpr T &get(tuple<Ts...> &t) noexcept {
    return get<type_pack_index<T, Ts...>::value>(t);
}

template<typename T, typename... Ts>
constexpr T &&get(tuple<Ts...> &&t) noexcept {
    return get<type_pack_index<T, Ts...>::value>(ala::move(t));
}

template<typename T, typename... Ts>
constexpr const T &get(const tuple<Ts...> &t) noexcept {
    return get<type_pack_index<T, Ts...>::value>(t);
}

template<typename T, typename... Ts>
constexpr const T &&get(const tuple<Ts...> &&t) noexcept {
    return get<type_pack_index<T, Ts...>::value>(ala::move(t));
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<typename... Ts>
tuple(Ts...) -> tuple<Ts...>;

template<typename T1, typename T2>
tuple(pair<T1, T2>) -> tuple<T1, T2>;
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