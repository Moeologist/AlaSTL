#ifndef _ALA_DETAIL_FUNCTIONAL_BASE
#define _ALA_DETAIL_FUNCTIONAL_BASE

#include <ala/config.h>

namespace ala {

template<class T = void>
struct less {
    constexpr bool operator()(const T &lhs, const T &rhs) const {
        return lhs < rhs;
    }
};

} // namespace ala

#endif // HEAD