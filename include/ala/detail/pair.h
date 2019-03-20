#ifndef _ALA_DETAIL_PAIR_H
#define _ALA_DETAIL_PAIR_H

#include <ala/config.h>
#include <ala/type_traits.h>
#include <type_traits>

namespace ala {

template<typename T>
struct _is_implicitly_default_constructible_impl {
    template<typename T1>
    static void _help(const T1 &);

    template<typename T1, typename = decltype(_help<const T1 &>({}))>
    static true_type _test(const T1 &);

    static false_type _test(...);

    typedef decltype(_test(declval<T>())) type;
};

template<typename T>
struct _is_implicitly_default_constructible_helper
    : _is_implicitly_default_constructible_impl<T>::type {};

template<typename T>
struct is_implicitly_default_constructible
    : _and_<is_default_constructible<T>,
            _is_implicitly_default_constructible_helper<T>> {};

template<typename T>
constexpr ALA_VAR_INLINE bool is_implicitly_default_constructible_v =
    is_implicitly_default_constructible<T>::value;

template<typename T1, typename T2>
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    // clang-format off
    // make template not specialization
    template<typename  U1 = T1, typename U2 = T2, typename = enable_if_t<
                 is_default_constructible<U1>::value &&
                 is_default_constructible<U2>::value &&
                 !(is_implicitly_default_constructible<U1>::value &&
                 is_implicitly_default_constructible<U2>::value)>, bool = true>
    explicit constexpr pair()
        : first(), second() {}

    template<typename  U1 = T1, typename U2 = T2, typename = enable_if_t<
                 is_default_constructible<U1>::value &&
                 is_default_constructible<U2>::value &&
                 is_implicitly_default_constructible<U1>::value &&
                 is_implicitly_default_constructible<U2>::value>>
    constexpr pair()
        : first(), second() {}

    template<typename  U1 = T1, typename U2 = T2, typename = enable_if_t<
                 is_copy_constructible<U1>::value &&
                 is_copy_constructible<U2>::value &&
                 !(is_convertible<const U1&, U1>::value &&
                 is_convertible<const U2&, U2>::value)>, bool = true>
    explicit constexpr pair(const T1 &a, const T2 &b)
        : first(a), second(b) {}

    template<typename  U1 = T1, typename U2 = T2, typename = enable_if_t<
                 is_copy_constructible<U1>::value &&
                 is_copy_constructible<U2>::value &&
                 is_convertible<const U1&, U1>::value &&
                 is_convertible<const U2&, U2>::value>>
    constexpr pair(const T1 &a, const T2 &b)
        : first(a), second(b) {}

    template<typename U1, typename U2, typename = enable_if_t<
                                           is_constructible<first_type, U1&&>::value &&
                                           is_constructible<second_type, U2&&>::value &&
                                           !(is_convertible<U1&&, first_type>::value &&
                                           is_convertible<U2&&, second_type>::value)>>
    explicit constexpr pair(U1 &&a, U2 &&b)
        : first(forward<U1>(a)), second(forward<U2>(b)) {}

    template<typename U1, typename U2, typename = void, typename = enable_if_t<
                                           is_constructible<first_type, U1&&>::value &&
                                           is_constructible<second_type, U2&&>::value &&
                                           is_convertible<U1&&, first_type>::value &&
                                           is_convertible<U2&&, second_type>::value>>
    constexpr pair(U1 &&a, U2 &&b)
        : first(forward<U1>(a)), second(forward<U2>(b)) {}

    template<typename U1, typename U2, typename = enable_if_t<
                                           is_constructible<first_type, const U1&>::value &&
                                           is_constructible<second_type, const U2&>::value &&
                                           !(is_convertible<const U1&, first_type>::value &&
                                           is_convertible<const U2&, second_type>::value)>>
    explicit constexpr pair(const pair<U1, U2> &p)
        : first(p.first), second(p.second) {}

    template<typename U1, typename U2, typename = void, typename = enable_if_t<
                                           is_constructible<first_type, const U1&>::value &&
                                           is_constructible<second_type, const U2&>::value &&
                                           is_convertible<const U1&, first_type>::value &&
                                           is_convertible<const U2&, second_type>::value>>
    constexpr pair(const pair<U1, U2> &p)
        : first(p.first), second(p.second) {}

    template<typename U1, typename U2, typename = enable_if_t<
                                           is_constructible<first_type, U1&&>::value &&
                                           is_constructible<second_type, U2&&>::value &&
                                           !(is_convertible<U1&&, first_type>::value &&
                                           is_convertible<U2&&, second_type>::value)>>
    explicit constexpr pair(pair<U1, U2> &&p)
        : first(forward<U1>(p.first)), second(forward<U2>(p.second)) {}

    template<typename U1, typename U2, typename = void, typename = enable_if_t<
                                           is_constructible<first_type, U1&&>::value &&
                                           is_constructible<second_type, U2&&>::value &&
                                           is_convertible<U1&&, first_type>::value &&
                                           is_convertible<U2&&, second_type>::value>>
    constexpr pair(pair<U1, U2> &&p)
        : first(forward<U1>(p.first)), second(forward<U2>(p.second)) {}

/*
    template<class... Args1, class... Args2>
    constexpr pair(std::piecewise_construct_t, std::tuple<Args1...> first_args,
                   std::tuple<Args2...> second_args);
*/
    pair(const pair &) = default;
    pair(pair &&) = default;

    template<typename  U1 = T1, typename U2 = T2>
    constexpr enable_if_t<is_copy_assignable<U1>::value &&
                          is_copy_assignable<U2>::value, pair>
    &operator=(const pair &p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    template<typename U1, typename U2>
    constexpr enable_if_t<is_assignable<first_type&, const U1&>::value &&
                          is_assignable<second_type&, const U2&>::value, pair>
    &operator=(const pair<U1, U2> &p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    template <typename U1 = T1, typename U2 = T2>
    constexpr enable_if_t<is_move_assignable<U1>::value &&
                          is_move_assignable<U2>::value, pair>
    &operator=(pair &&p) noexcept(
            is_nothrow_move_assignable<first_type>::value &&
            is_nothrow_move_assignable<second_type>::value) {
        first = forward<first_type>(p.first);
        second = forward<second_type>(p.second);
        return *this;
    }

    template<typename U1, typename U2>
    constexpr enable_if_t<is_assignable<first_type&, U1&&>::value &&
                          is_assignable<second_type&, U2&&>::value, pair>
    &operator=(pair<U1, U2> &&p) {
        first = forward<U1>(p.first);
        second = forward<U2>(p.second);
        return *this;
    }

    constexpr void swap(pair &p) noexcept(
        is_nothrow_swappable<first_type>::value &&
        is_nothrow_swappable<second_type>::value) {
        swap(first, p.first);
        swap(second, p.second);
    }
    // clang-format on
};

template<typename T1, typename T2>
constexpr bool operator==(const pair<T1, T2> &x, const pair<T1, T2> &y) {
    return x.first == y.first && x.second == y.second;
}

template<typename T1, typename T2>
constexpr bool operator<(const pair<T1, T2> &x, const pair<T1, T2> &y) {
    return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}

template<typename T1, typename T2>
constexpr bool operator!=(const pair<T1, T2> &x, const pair<T1, T2> &y) {
    return !(x == y);
}

template<typename T1, typename T2>
constexpr bool operator>(const pair<T1, T2> &x, const pair<T1, T2> &y) {
    return y < x;
}

template<typename T1, typename T2>
constexpr bool operator<=(const pair<T1, T2> &x, const pair<T1, T2> &y) {
    return !(y < x);
}

template<typename T1, typename T2>
constexpr bool operator>=(const pair<T1, T2> &x, const pair<T1, T2> &y) {
    return !(x < y);
}

template<class T1, class T2>
constexpr decltype(auto) make_pair(T1 &&t1, T2 &&t2) {
    return pair<decay_t<T1>, decay_t<T2>>(forward<T1>(t1), forward<T2>(t2));
}

template<class T1, class T2>
constexpr enable_if_t<is_swappable<T1>::value && is_swappable<T2>::value>
swap(pair<T1, T2> &x, pair<T1, T2> &y) noexcept(
    is_nothrow_swappable<T1>::value &&is_nothrow_swappable<T2>::value) {
    x.swap(y);
}

#if ALA_ENABLE_CPP_MACRO && __cpp_deduction_guides >= 201606 || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1914)
template<typename T1, typename T2>
pair(T1, T2)->pair<T1, T2>;
#endif

} // namespace ala

#endif