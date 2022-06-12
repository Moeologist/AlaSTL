#ifndef _ALA_DETAIL_TYPE_PACK_H
#define _ALA_DETAIL_TYPE_PACK_H

#include <ala/detail/integer_sequence.h>

namespace ala {

template<size_t I, typename T>
struct _type_pack_element_base {
    using type = T;
    using index = integral_constant<size_t, I>;
};

template<typename IntSeq, typename... Ts>
struct _type_pack_element_index;

template<size_t... I, typename... Ts>
struct _type_pack_element_index<index_sequence<I...>, Ts...>
    : _type_pack_element_base<I, Ts>... {};

template<size_t I, typename T>
_type_pack_element_base<I, T>
_type_pack_element_cast(_type_pack_element_base<I, T> &&);

template<typename T, size_t I>
_type_pack_element_base<I, T>
_type_pack_index_cast(_type_pack_element_base<I, T> &&);

template<typename T, typename... Ts>
using _type_pack_index_t = typename decltype(_type_pack_index_cast<T>(
    _type_pack_element_index<index_sequence_for<Ts...>, Ts...>{}))::index;


#if ALA_HAS_BUILTIN(__type_pack_element)

template<size_t I, typename... Ts>
using type_pack_element_t = __type_pack_element<I, Ts...>;

#else

template<size_t I, typename... Ts>
using type_pack_element_t = typename decltype(_type_pack_element_cast<I>(
    _type_pack_element_index<index_sequence_for<Ts...>, Ts...>{}))::type;

#endif

template<typename Void, size_t I, typename... Ts>
struct _type_pack_element_helper {};

template<size_t I, typename... Ts>
struct _type_pack_element_helper<void_t<type_pack_element_t<I, Ts...>>, I, Ts...> {
    using type = type_pack_element_t<I, Ts...>;
};

template<size_t I, typename... Ts>
struct type_pack_element: _type_pack_element_helper<void, I, Ts...> {};

template<typename Void, typename T, typename... Ts>
struct _type_pack_index_helper {};

template<typename T, typename... Ts>
struct _type_pack_index_helper<void_t<_type_pack_index_t<T, Ts...>>, T, Ts...>
    : _type_pack_index_t<T, Ts...> {};

template<typename T, typename... Ts>
struct type_pack_index: _type_pack_index_helper<void, T, Ts...> {};

#if _ALA_ENABLE_INLINE_VAR
template<typename T, typename... Ts>
inline constexpr size_t type_pack_index_v = type_pack_index<T, Ts...>::value;
#endif

} // namespace ala

#endif // HEAD