#ifndef _ALA_DETAIL_FUNCTIONAL_BASE_H
#define _ALA_DETAIL_FUNCTIONAL_BASE_H

#include <ala/config.h>
#include <ala/type_traits.h>

#define ALA_MAKE_BASE_FUNCTION_UNARY(_name_, _op_) \
    template<class T = void> \
    struct _name_ { \
        using result_type = bool; \
        using first_argument_type = T; \
        using second_argument_type = T; \
        constexpr bool operator()(const T &rhs) const { \
            return _op_ rhs; \
        } \
    }; \
\
    template<> \
    struct _name_<void> { \
        using is_transparent = void; \
        template<class U> \
        constexpr decltype(auto) operator()(U &&rhs) const { \
            return _op_ ala::forward<U>(rhs); \
        } \
    };

#define ALA_MAKE_BASE_FUNCTION(_name_, _op_) \
    template<class T = void> \
    struct _name_ { \
        using result_type = bool; \
        using first_argument_type = T; \
        using second_argument_type = T; \
        constexpr bool operator()(const T &lhs, const T &rhs) const { \
            return lhs _op_ rhs; \
        } \
    }; \
\
    template<> \
    struct _name_<void> { \
        using is_transparent = void; \
        template<class T, class U> \
        constexpr decltype(auto) operator()(T &&lhs, U &&rhs) const { \
            return ala::forward<T>(lhs) _op_ ala::forward<U>(rhs); \
        } \
    };

namespace ala {
ALA_MAKE_BASE_FUNCTION(plus, +)
ALA_MAKE_BASE_FUNCTION(minus, -)
ALA_MAKE_BASE_FUNCTION(multiplies, *)
ALA_MAKE_BASE_FUNCTION(divides, /)
ALA_MAKE_BASE_FUNCTION(modulus, %)
ALA_MAKE_BASE_FUNCTION(negate, -)

ALA_MAKE_BASE_FUNCTION(equal_to, ==)
ALA_MAKE_BASE_FUNCTION(not_equal_to, !=)
ALA_MAKE_BASE_FUNCTION(less, <)
ALA_MAKE_BASE_FUNCTION(less_equal, <=)
ALA_MAKE_BASE_FUNCTION(greater, >)
ALA_MAKE_BASE_FUNCTION(greater_equal, >=)

ALA_MAKE_BASE_FUNCTION(logical_and, &&)
ALA_MAKE_BASE_FUNCTION(logical_or, ||)
ALA_MAKE_BASE_FUNCTION_UNARY(logical_not, !)

ALA_MAKE_BASE_FUNCTION(bit_and, &)
ALA_MAKE_BASE_FUNCTION(bit_or, |)
ALA_MAKE_BASE_FUNCTION(bit_xor, ^)
ALA_MAKE_BASE_FUNCTION_UNARY(bit_not, ~)
} // namespace ala

#undef ALALA_MAKE_BASE_FUNCTION
#undef ALA_MAKE_BASE_FUNCTION_UNARY
#endif // HEAD