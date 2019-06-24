#ifndef _ALA_DETAIL_TYPE_PACK_H
#define _ALA_DETAIL_TYPE_PACK_H

#include <ala/detail/integer_sequence.h>

namespace ala {

template<size_t I, typename T>
struct _type_pack_element_base {
    using type = T;
};

template<typename IntSeq, typename... Ts>
struct _type_pack_element_index;

template<size_t... I, typename... Ts>
struct _type_pack_element_index<index_sequence<I...>, Ts...>
    : _type_pack_element_base<I, Ts>... {};

template<size_t I, typename T>
_type_pack_element_base<I, T>
_type_pack_element_cast(_type_pack_element_base<I, T> &&);

template<size_t I, typename... Ts>
using type_pack_element_t = typename decltype(_type_pack_element_cast<I>(
    _type_pack_element_index<index_sequence_for<Ts...>, Ts...>{}))::type;

template<typename Void, size_t I, typename... Ts>
struct _type_pack_element_helper {};

template<size_t I, typename... Ts>
struct _type_pack_element_helper<void_t<type_pack_element_t<I, Ts...>>, I, Ts...> {
    using type = type_pack_element_t<I, Ts...>;
};

template<size_t I, typename... Ts>
struct type_pack_element : _type_pack_element_helper<void, I, Ts...> {};

} // namespace ala

#endif // HEAD