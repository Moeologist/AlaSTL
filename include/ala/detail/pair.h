#ifndef _ALA_DETAIL_PAIR_H
#define _ALA_DETAIL_PAIR_H

#include <ala/detail/tuple_operator.h>

namespace ala {

template<typename... T>
struct tuple;

struct piecewise_construct_t { explicit piecewise_construct_t() = default; };

#if _ALA_ENABLE_INLINE_VAR
inline constexpr piecewise_construct_t piecewise_construct{};
#endif

template<typename T1, typename T2>
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    // make template not specialization immediately

    template<typename U1 = T1, typename U2 = T2,
             typename = enable_if_t<is_default_constructible<U1>::value &&
                                    is_default_constructible<U2>::value &&
                                    is_implicitly_default_constructible<U1>::value &&
                                    is_implicitly_default_constructible<U2>::value>>
    constexpr pair(): first(), second() {}

    template<typename U1 = T1, typename U2 = T2, typename = void,
             typename = enable_if_t<is_default_constructible<U1>::value &&
                                    is_default_constructible<U2>::value &&
                                    !(is_implicitly_default_constructible<U1>::value &&
                                      is_implicitly_default_constructible<U2>::value)>>
    explicit constexpr pair(): first(), second() {}

    template<typename U1 = T1, typename U2 = T2,
             typename = enable_if_t<is_copy_constructible<U1>::value &&
                                    is_copy_constructible<U2>::value &&
                                    is_convertible<const U1 &, U1>::value &&
                                    is_convertible<const U2 &, U2>::value>>
    constexpr pair(const T1 &a, const T2 &b): first(a), second(b) {}

    template<typename U1 = T1, typename U2 = T2, typename = void,
             typename = enable_if_t<is_copy_constructible<U1>::value &&
                                    is_copy_constructible<U2>::value &&
                                    !(is_convertible<const U1 &, U1>::value &&
                                      is_convertible<const U2 &, U2>::value)>>
    explicit constexpr pair(const T1 &a, const T2 &b): first(a), second(b) {}

    template<typename U1, typename U2,
             typename = enable_if_t<is_constructible<first_type, U1 &&>::value &&
                                    is_constructible<second_type, U2 &&>::value &&
                                    is_convertible<U1 &&, first_type>::value &&
                                    is_convertible<U2 &&, second_type>::value>>
    constexpr pair(U1 &&a, U2 &&b)
        : first(ala::forward<U1>(a)), second(ala::forward<U2>(b)) {}

    template<typename U1, typename U2, typename = void,
             typename = enable_if_t<is_constructible<first_type, U1 &&>::value &&
                                    is_constructible<second_type, U2 &&>::value &&
                                    !(is_convertible<U1 &&, first_type>::value &&
                                      is_convertible<U2 &&, second_type>::value)>>
    explicit constexpr pair(U1 &&a, U2 &&b)
        : first(ala::forward<U1>(a)), second(ala::forward<U2>(b)) {}

    template<typename U1, typename U2,
             typename = enable_if_t<is_constructible<first_type, const U1 &>::value &&
                                    is_constructible<second_type, const U2 &>::value &&
                                    is_convertible<const U1 &, first_type>::value &&
                                    is_convertible<const U2 &, second_type>::value>>
    constexpr pair(const pair<U1, U2> &p): first(p.first), second(p.second) {}

    template<typename U1, typename U2, typename = void,
             typename =
                 enable_if_t<is_constructible<first_type, const U1 &>::value &&
                             is_constructible<second_type, const U2 &>::value &&
                             !(is_convertible<const U1 &, first_type>::value &&
                               is_convertible<const U2 &, second_type>::value)>>
    explicit constexpr pair(const pair<U1, U2> &p)
        : first(p.first), second(p.second) {}

    template<typename U1, typename U2,
             typename = enable_if_t<is_constructible<first_type, U1 &&>::value &&
                                    is_constructible<second_type, U2 &&>::value &&
                                    is_convertible<U1 &&, first_type>::value &&
                                    is_convertible<U2 &&, second_type>::value>>
    constexpr pair(pair<U1, U2> &&p)
        : first(ala::forward<U1>(p.first)), second(ala::forward<U2>(p.second)) {}

    template<typename U1, typename U2, typename = void,
             typename = enable_if_t<is_constructible<first_type, U1 &&>::value &&
                                    is_constructible<second_type, U2 &&>::value &&
                                    !(is_convertible<U1 &&, first_type>::value &&
                                      is_convertible<U2 &&, second_type>::value)>>
    explicit constexpr pair(pair<U1, U2> &&p)
        : first(ala::forward<U1>(p.first)), second(ala::forward<U2>(p.second)) {}

    template<typename... Args1, typename... Args2>
    constexpr pair(piecewise_construct_t pct, tuple<Args1...> first_args,
                   tuple<Args2...> second_args)
        : pair(pct, first_args, second_args, index_sequence_for<Args1...>(),
               index_sequence_for<Args2...>()) {}

    template<class... Args1, class... Args2, size_t... I1, size_t... I2>
    constexpr pair(piecewise_construct_t, tuple<Args1...> &first_args,
                   tuple<Args2...> &second_args, index_sequence<I1...>,
                   index_sequence<I2...>); // define in tuple.h

    pair(const pair &) = default;
    pair(pair &&) = default;

    template<int>
    struct _dummy {};

    constexpr pair &operator=(
        conditional_t<is_copy_assignable<T1>::value && is_copy_assignable<T2>::value,
                      const pair &, _dummy<0>>
            p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    template<typename U1, typename U2>
    constexpr enable_if_t<is_assignable<first_type &, const U1 &>::value &&
                              is_assignable<second_type &, const U2 &>::value,
                          pair &>
    operator=(const pair<U1, U2> &p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    constexpr pair &operator=(
        conditional_t<is_move_assignable<T1>::value && is_move_assignable<T2>::value,
                      pair &&, _dummy<1>>
            p) noexcept(is_nothrow_move_assignable<first_type>::value
                            &&is_nothrow_move_assignable<second_type>::value) {
        first = ala::forward<first_type>(p.first);
        second = ala::forward<second_type>(p.second);
        return *this;
    }

    template<typename U1, typename U2>
    constexpr enable_if_t<is_assignable<first_type &, U1 &&>::value &&
                              is_assignable<second_type &, U2 &&>::value,
                          pair &>
    operator=(pair<U1, U2> &&p) {
        first = ala::forward<U1>(p.first);
        second = ala::forward<U2>(p.second);
        return *this;
    }

    constexpr void
    swap(pair &p) noexcept(is_nothrow_swappable<first_type>::value
                               &&is_nothrow_swappable<second_type>::value) {
        ala::swap(first, p.first);
        ala::swap(second, p.second);
    }
};

template<typename T1, typename T2>
constexpr bool operator==(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename T1, typename T2>
constexpr bool operator<(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return lhs.first < rhs.first ||
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template<typename T1, typename T2>
constexpr bool operator!=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return !(lhs == rhs);
}

template<typename T1, typename T2>
constexpr bool operator>(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return rhs < lhs;
}

template<typename T1, typename T2>
constexpr bool operator<=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return !(rhs < lhs);
}

template<typename T1, typename T2>
constexpr bool operator>=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return !(lhs < rhs);
}

template<class T1, class T2>
constexpr auto make_pair(T1 &&t1, T2 &&t2) -> pair<decay_t<T1>, decay_t<T2>> {
    return pair<decay_t<T1>, decay_t<T2>>(ala::forward<T1>(t1),
                                          ala::forward<T2>(t2));
}

template<class T1, class T2>
constexpr enable_if_t<is_swappable<T1>::value && is_swappable<T2>::value>
swap(pair<T1, T2> &lhs, pair<T1, T2> &rhs) noexcept(
    is_nothrow_swappable<T1>::value &&is_nothrow_swappable<T2>::value) {
    lhs.swap(rhs);
}

template<class Return, class Pair>
constexpr Return _pair_get_helper(Pair &pr,
                                  integral_constant<size_t, 0>) noexcept {
    return pr.first;
}

template<class Return, class Pair>
constexpr Return _pair_get_helper(Pair &pr,
                                  integral_constant<size_t, 1>) noexcept {
    return pr.second;
}

template<size_t I, class T1, class T2>
constexpr tuple_element_t<I, pair<T1, T2>> &get(pair<T1, T2> &pr) noexcept {
    static_assert(I == 0 || I == 1, "ala::pair index out of range");
    typedef tuple_element_t<I, pair<T1, T2>> &type;
    typedef integral_constant<size_t, I> idx;
    return _pair_get_helper<type>(pr, idx());
}

template<size_t I, class T1, class T2>
constexpr const tuple_element_t<I, pair<T1, T2>> &
get(const pair<T1, T2> &pr) noexcept {
    static_assert(I == 0 || I == 1, "ala::pair index out of range");
    typedef const tuple_element_t<I, pair<T1, T2>> &type;
    typedef integral_constant<size_t, I> idx;
    return _pair_get_helper<type>(pr, idx());
}

template<size_t I, class T1, class T2>
constexpr tuple_element_t<I, pair<T1, T2>> &&get(pair<T1, T2> &&pr) noexcept {
    static_assert(I == 0 || I == 1, "ala::pair index out of range");
    typedef tuple_element_t<I, pair<T1, T2>> &&type;
    typedef integral_constant<size_t, I> idx;
    return ala::forward<type>(ala::get<I>(pr));
}

template<size_t I, class T1, class T2>
constexpr const tuple_element_t<I, pair<T1, T2>> &&
get(const pair<T1, T2> &&pr) noexcept {
    static_assert(I == 0 || I == 1, "ala::pair index out of range");
    typedef const tuple_element_t<I, pair<T1, T2>> &&type;
    typedef integral_constant<size_t, I> idx;
    return ala::forward<type>(ala::get<I>(pr));
}

template<class T, class U>
constexpr T &get(pair<type_identity_t<T>, U> &p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<T &>(p.first);
}

template<class T, class U>
constexpr const T &get(const pair<type_identity_t<T>, U> &p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<const T &>(p.first);
}

template<class T, class U>
constexpr T &&get(pair<type_identity_t<T>, U> &&p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<T &&>(p.first);
}

template<class T, class U>
constexpr const T &&get(const pair<type_identity_t<T>, U> &&p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<const T &&>(p.first);
}

template<class T, class U>
constexpr T &get(pair<U, type_identity_t<T>> &p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<T &>(p.second);
}

template<class T, class U>
constexpr const T &get(const pair<U, type_identity_t<T>> &p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<const T &>(p.second);
}

template<class T, class U>
constexpr T &&get(pair<U, type_identity_t<T>> &&p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<T &&>(p.second);
}

template<class T, class U>
constexpr const T &&get(const pair<U, type_identity_t<T>> &&p) noexcept {
    static_assert(!is_same<T, U>::value,
                  "No specified type or more than one type");
    return static_cast<const T &&>(p.second);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<typename T1, typename T2>
pair(T1, T2) -> pair<T1, T2>;
#endif

} // namespace ala

#endif