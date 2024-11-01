#ifndef _ALA_UTILITY_H
#define _ALA_UTILITY_H

#include <ala/detail/pair.h>

namespace ala {

template<class Enum>
constexpr underlying_type_t<Enum> to_underlying(Enum e) noexcept {
    return static_cast<underlying_type_t<Enum>>(e);
}

template<class T, class U = T>
constexpr T exchange(T &obj, U &&new_value) {
    T old_value = ala::move(obj);
    obj = ala::forward<U>(new_value);
    return old_value;
}

template<class T>
constexpr conditional_t<!is_nothrow_move_constructible<T>::value &&
                            is_copy_constructible<T>::value,
                        const T &, T &&>
move_if_noexcept(T &x) noexcept {
    return ala::move(x);
}

template<class T>
constexpr add_const_t<T> &as_const(T &t) noexcept {
    return t;
}

template<class T>
void as_const(const T &&) = delete;

namespace rel_ops {
template<class T>
bool operator!=(const T &lhs, const T &rhs) {
    return !(lhs == rhs);
}

template<class T>
bool operator>(const T &lhs, const T &rhs) {
    return rhs < lhs;
}

template<class T>
bool operator<=(const T &lhs, const T &rhs) {
    return !(rhs < lhs);
}

template<class T>
bool operator>=(const T &lhs, const T &rhs) {
    return !(lhs < rhs);
}
} // namespace rel_ops

struct in_place_t {
    explicit in_place_t() = default;
};

template<class T>
struct in_place_type_t {
    explicit in_place_type_t() = default;
};

template<size_t I>
struct in_place_index_t {
    explicit in_place_index_t() = default;
};

ALA_INLINE_CONSTEXPR_V in_place_t in_place{};

template<class T>
ALA_INLINE_CONSTEXPR_V in_place_type_t<T> in_place_type{};

template<size_t I>
ALA_INLINE_CONSTEXPR_V in_place_index_t<I> in_place_index{};

} // namespace ala
#endif