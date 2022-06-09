#ifndef _ALA_DETAIL_TUPLE_OPERATOR_H
#define _ALA_DETAIL_TUPLE_OPERATOR_H

#include <ala/type_traits.h>
#include <ala/detail/type_pack.h>

namespace ala {

// llvm D28222

template<typename T>
struct tuple_size;

template<typename Tuple, typename = void>
struct _tuple_size_sfinae {};

template<typename Tuple>
struct _tuple_size_sfinae<Tuple, void_t<decltype(tuple_size<Tuple>::value)>>
    : integral_constant<size_t, tuple_size<Tuple>::value> {};

template<typename T>
struct tuple_size<_sfinae_checker<const T, decltype(sizeof(tuple_size<T>))>>
    : _tuple_size_sfinae<T> {};

template<typename T>
struct tuple_size<_sfinae_checker<volatile T, decltype(sizeof(tuple_size<T>))>>
    : _tuple_size_sfinae<T> {};

template<typename T>
struct tuple_size<_sfinae_checker<const volatile T, decltype(sizeof(tuple_size<T>))>>
    : _tuple_size_sfinae<T> {};

template<size_t I, typename T>
struct tuple_element;

template<size_t I, typename T>
struct tuple_element<I, const T> {
    using type = add_const_t<typename tuple_element<I, T>::type>;
};

template<size_t I, typename T>
struct tuple_element<I, volatile T> {
    using type = add_volatile_t<typename tuple_element<I, T>::type>;
};

template<size_t I, typename T>
struct tuple_element<I, const volatile T> {
    using type = add_cv_t<typename tuple_element<I, T>::type>;
};

#if _ALA_ENABLE_INLINE_VAR
template<typename T>
inline constexpr size_t tuple_size_v = tuple_size<T>::value;
#endif

template<size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template<typename, size_t>
struct array;

template<typename, typename>
struct pair;

template<typename...>
struct tuple;

template<class T, size_t N>
struct tuple_size<array<T, N>>: integral_constant<size_t, N> {};

template<size_t I, class T, size_t N>
struct tuple_element<I, array<T, N>> {
    static_assert(I < N, "ala::array index out of range");
    using type = T;
};

template<class T1, class T2>
struct tuple_size<pair<T1, T2>>: integral_constant<size_t, 2> {};

template<size_t I, class T1, class T2>
struct tuple_element<I, pair<T1, T2>>: type_pack_element<I, T1, T2> {};

template<typename... Ts>
struct tuple_size<tuple<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

template<size_t I, typename... Ts>
struct tuple_element<I, tuple<Ts...>>: type_pack_element<I, Ts...> {};

template<size_t I, class T, size_t N>
constexpr T &get(array<T, N> &a) noexcept;

template<size_t I, class T, size_t N>
constexpr T &&get(array<T, N> &&a) noexcept;

template<size_t I, class T, size_t N>
constexpr const T &get(const array<T, N> &a) noexcept;

template<size_t I, class T, size_t N>
constexpr const T &&get(const array<T, N> &&a) noexcept;

template<size_t I, class T1, class T2>
constexpr tuple_element_t<I, pair<T1, T2>> &get(pair<T1, T2> &pr) noexcept;

template<size_t I, class T1, class T2>
constexpr const tuple_element_t<I, pair<T1, T2>> &
get(const pair<T1, T2> &pr) noexcept;

template<size_t I, class T1, class T2>
constexpr tuple_element_t<I, pair<T1, T2>> &&get(pair<T1, T2> &&pr) noexcept;

template<size_t I, class T1, class T2>
constexpr const tuple_element_t<I, pair<T1, T2>> &&
get(const pair<T1, T2> &&pr) noexcept;

template<size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>> &get(tuple<Ts...> &tp) noexcept;

template<size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>> &
get(const tuple<Ts...> &tp) noexcept;

template<size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>> &&get(tuple<Ts...> &&tp) noexcept;

template<size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>> &&
get(const tuple<Ts...> &&tp) noexcept;

} // namespace ala

#if _ALA_ENABLE_STRUCT_BIND
namespace std {

    #if defined(_LIBCPP_ABI_NAMESPACE)
inline namespace _LIBCPP_ABI_NAMESPACE {
    #endif

template<size_t, typename>
struct tuple_element;

template<typename>
struct tuple_size;

template<size_t I, typename T, ala::size_t N>
struct tuple_element<I, ala::array<T, N>>
    : ala::tuple_element<I, ala::array<T, N>> {};

template<size_t I, typename T, ala::size_t N>
struct tuple_element<I, const ala::array<T, N>>
    : ala::tuple_element<I, const ala::array<T, N>> {};

template<size_t I, typename T, ala::size_t N>
struct tuple_element<I, volatile ala::array<T, N>>
    : ala::tuple_element<I, volatile ala::array<T, N>> {};

template<size_t I, typename T, ala::size_t N>
struct tuple_element<I, const volatile ala::array<T, N>>
    : ala::tuple_element<I, const volatile ala::array<T, N>> {};

template<typename T, ala::size_t N>
struct tuple_size<ala::array<T, N>>: ala::tuple_size<ala::array<T, N>> {};

template<typename T, ala::size_t N>
struct tuple_size<const ala::array<T, N>>
    : ala::tuple_size<const ala::array<T, N>> {};

template<typename T, ala::size_t N>
struct tuple_size<volatile ala::array<T, N>>
    : ala::tuple_size<volatile ala::array<T, N>> {};

template<typename T, ala::size_t N>
struct tuple_size<const volatile ala::array<T, N>>
    : ala::tuple_size<const volatile ala::array<T, N>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, ala::pair<Ts...>>
    : ala::tuple_element<I, ala::pair<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, const ala::pair<Ts...>>
    : ala::tuple_element<I, const ala::pair<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, volatile ala::pair<Ts...>>
    : ala::tuple_element<I, volatile ala::pair<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, const volatile ala::pair<Ts...>>
    : ala::tuple_element<I, const volatile ala::pair<Ts...>> {};

template<typename... Ts>
struct tuple_size<ala::pair<Ts...>>: ala::tuple_size<ala::pair<Ts...>> {};

template<typename... Ts>
struct tuple_size<const ala::pair<Ts...>>
    : ala::tuple_size<const ala::pair<Ts...>> {};

template<typename... Ts>
struct tuple_size<volatile ala::pair<Ts...>>
    : ala::tuple_size<volatile ala::pair<Ts...>> {};

template<typename... Ts>
struct tuple_size<const volatile ala::pair<Ts...>>
    : ala::tuple_size<ala::pair<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, ala::tuple<Ts...>>
    : ala::tuple_element<I, ala::tuple<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, const ala::tuple<Ts...>>
    : ala::tuple_element<I, const ala::tuple<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, volatile ala::tuple<Ts...>>
    : ala::tuple_element<I, volatile ala::tuple<Ts...>> {};

template<size_t I, typename... Ts>
struct tuple_element<I, const volatile ala::tuple<Ts...>>
    : ala::tuple_element<I, const volatile ala::tuple<Ts...>> {};

template<typename... Ts>
struct tuple_size<ala::tuple<Ts...>>: ala::tuple_size<ala::tuple<Ts...>> {};

template<typename... Ts>
struct tuple_size<const ala::tuple<Ts...>>
    : ala::tuple_size<const ala::tuple<Ts...>> {};

template<typename... Ts>
struct tuple_size<volatile ala::tuple<Ts...>>
    : ala::tuple_size<volatile ala::tuple<Ts...>> {};

template<typename... Ts>
struct tuple_size<const volatile ala::tuple<Ts...>>
    : ala::tuple_size<const volatile ala::tuple<Ts...>> {};

    #if defined(_LIBCPP_ABI_NAMESPACE)
}
    #endif

} // namespace std
#endif

#endif