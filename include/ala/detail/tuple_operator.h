#ifndef _ALA_DETAIL_TUPLE_OPERATOR_H
#define _ALA_DETAIL_TUPLE_OPERATOR_H

#include <ala/type_traits.h>

namespace ala {

template<typename... Ts>
struct _type_;

template<typename T>
struct tuple_size;

template<typename Tuple, typename = void>
struct _tuple_size_helper {};

template<typename Tuple>
struct _tuple_size_helper<Tuple, void_t<decltype(tuple_size<Tuple>::value)>>
    : tuple_size<Tuple> {};

template<typename T>
struct tuple_size<const T>: _tuple_size_helper<T> {};

template<typename T>
struct tuple_size<volatile T>: _tuple_size_helper<T> {};

template<typename T>
struct tuple_size<const volatile T>: _tuple_size_helper<T> {};

template<size_t I, typename T>
struct tuple_element;

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

template<typename...>
struct tuple;

template<typename, typename>
struct pair;

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

#if defined(_LIBCPP_ABI_NAMESPACE)
}
#endif

#define ALA_MAKE_STRUCT_BIND(_type_) \
    template<size_t I, typename... Ts> \
    struct tuple_element<I, ala::_type_<Ts...>> \
        : ala::tuple_element<I, ala::_type_<Ts...>> {}; \
\
    template<size_t I, typename... Ts> \
    struct tuple_element<I, const ala::_type_<Ts...>> \
        : ala::tuple_element<I, const ala::_type_<Ts...>> {}; \
\
    template<size_t I, typename... Ts> \
    struct tuple_element<I, volatile ala::_type_<Ts...>> \
        : ala::tuple_element<I, volatile ala::_type_<Ts...>> {}; \
\
    template<size_t I, typename... Ts> \
    struct tuple_element<I, const volatile ala::_type_<Ts...>> \
        : ala::tuple_element<I, const volatile ala::_type_<Ts...>> {}; \
\
    template<typename... Ts> \
    struct tuple_size<ala::_type_<Ts...>> \
        : ala::tuple_size<ala::_type_<Ts...>> {}; \
\
    template<typename... Ts> \
    struct tuple_size<const ala::_type_<Ts...>> \
        : ala::tuple_size<ala::_type_<Ts...>> {}; \
\
    template<typename... Ts> \
    struct tuple_size<volatile ala::_type_<Ts...>> \
        : ala::tuple_size<ala::_type_<Ts...>> {}; \
\
    template<typename... Ts> \
    struct tuple_size<const volatile ala::_type_<Ts...>> \
        : ala::tuple_size<ala::_type_<Ts...>> {};

ALA_MAKE_STRUCT_BIND(tuple)
ALA_MAKE_STRUCT_BIND(pair)

} // namespace std
#endif

#endif