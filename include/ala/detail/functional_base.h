#ifndef _ALA_DETAIL_FUNCTIONAL_BASE_H
#define _ALA_DETAIL_FUNCTIONAL_BASE_H

#include <ala/type_traits.h>

#define ALA_MAKE_BASE_FUNCTION_UNARY(_name_, _op_, _res_) \
    template<class T = void> \
    struct _name_ { \
        using result_type = _res_; \
        using argument_type = T; \
        constexpr _res_ operator()(const T &rhs) const { \
            return _op_ rhs; \
        } \
    }; \
\
    template<> \
    struct _name_<void> { \
        using is_transparent = int; \
        template<class U> \
        constexpr auto operator()(U &&rhs) const \
            -> decltype(_op_ ala::forward<U>(rhs)) { \
            return _op_ ala::forward<U>(rhs); \
        } \
    };

#define ALA_MAKE_BASE_FUNCTION(_name_, _op_, _res_) \
    template<class T = void> \
    struct _name_ { \
        using result_type = _res_; \
        using first_argument_type = T; \
        using second_argument_type = T; \
        constexpr _res_ operator()(const T &lhs, const T &rhs) const { \
            return lhs _op_ rhs; \
        } \
    }; \
\
    template<> \
    struct _name_<void> { \
        using is_transparent = int; \
        template<class T, class U> \
        constexpr auto operator()(T &&lhs, U &&rhs) const \
            -> decltype(ala::forward<T>(lhs) _op_ ala::forward<U>(rhs)) { \
            return ala::forward<T>(lhs) _op_ ala::forward<U>(rhs); \
        } \
    };

namespace ala {

ALA_MAKE_BASE_FUNCTION(plus, +, T)
ALA_MAKE_BASE_FUNCTION(minus, -, T)
ALA_MAKE_BASE_FUNCTION(multiplies, *, T)
ALA_MAKE_BASE_FUNCTION(divides, /, T)
ALA_MAKE_BASE_FUNCTION(modulus, %, T)
ALA_MAKE_BASE_FUNCTION_UNARY(negate, -, T)

ALA_MAKE_BASE_FUNCTION(equal_to, ==, bool)
ALA_MAKE_BASE_FUNCTION(not_equal_to, !=, bool)
ALA_MAKE_BASE_FUNCTION(less, <, bool)
ALA_MAKE_BASE_FUNCTION(less_equal, <=, bool)
ALA_MAKE_BASE_FUNCTION(greater, >, bool)
ALA_MAKE_BASE_FUNCTION(greater_equal, >=, bool)

ALA_MAKE_BASE_FUNCTION(logical_and, &&, bool)
ALA_MAKE_BASE_FUNCTION(logical_or, ||, bool)
ALA_MAKE_BASE_FUNCTION_UNARY(logical_not, !, bool)

ALA_MAKE_BASE_FUNCTION(bit_and, &, T)
ALA_MAKE_BASE_FUNCTION(bit_or, |, T)
ALA_MAKE_BASE_FUNCTION(bit_xor, ^, T)
ALA_MAKE_BASE_FUNCTION_UNARY(bit_not, ~, T)

} // namespace ala

#undef ALALA_MAKE_BASE_FUNCTION
#undef ALA_MAKE_BASE_FUNCTION_UNARY
#endif // HEAD