#ifndef _ALA_DETAIL_FUNCTIONAL_BASE_H
#define _ALA_DETAIL_FUNCTIONAL_BASE_H

#include <ala/config.h>

namespace ala {

template<class T = void>
struct less {
    using result_type = bool;
    using first_argument_type = T;
    using second_argument_type = T;
    constexpr bool operator()(const T &lhs, const T &rhs) const {
        return lhs < rhs;
    }
};

template<>
struct less<void> {
    using is_transparent = void;
    template<class T, class U>
    constexpr decltype(auto) operator()(T &&lhs, U &&rhs) const {
        return ala::forward<T>(lhs) < ala::forward<U>(rhs);
    }
};

} // namespace ala

#endif // HEAD