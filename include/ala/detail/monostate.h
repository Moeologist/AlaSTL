#ifndef _ALA_DETAIL_MONOSTATE_H
#define _ALA_DETAIL_MONOSTATE_H

#include <ala/detail/hash.h>

namespace ala {

struct monostate {};

template<>
struct hash<monostate> {
    using argument_type = monostate;
    using result_type = size_t;

    constexpr size_t operator()(monostate) const noexcept {
        return 10;
    }
};

// monostate relational operators
constexpr bool operator<(monostate, monostate) noexcept {
    return false;
}

constexpr bool operator>(monostate, monostate) noexcept {
    return false;
}

constexpr bool operator<=(monostate, monostate) noexcept {
    return true;
}

constexpr bool operator>=(monostate, monostate) noexcept {
    return true;
}

constexpr bool operator==(monostate, monostate) noexcept {
    return true;
}

constexpr bool operator!=(monostate, monostate) noexcept {
    return false;
}

}

#endif