#ifndef _ALA_DETAIL_TUPLE_OPERATOR_H
#define _ALA_DETAIL_TUPLE_OPERATOR_H

#include <ala/type_traits.h>

namespace ala {

template<typename... Ts>
struct tuple;

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

}

#endif