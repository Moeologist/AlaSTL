#ifndef _ALA_CONFIG_STDDEF_H
#define _ALA_CONFIG_STDDEF_H

#include <ala/type_traits.h>

namespace ala {

enum class byte : unsigned char {};

template<class IntegerType>
constexpr enable_if_t<is_integral<IntegerType>::value, IntegerType>
to_integer(byte b) noexcept {
    return IntegerType(b);
}

template<class IntegerType>
constexpr enable_if_t<is_integral<IntegerType>::value, byte> &
operator<<=(byte &b, IntegerType shift) noexcept {
    return b = b << shift;
}

template<class IntegerType>
constexpr enable_if_t<is_integral<IntegerType>::value, byte> &
operator>>=(byte &b, IntegerType shift) noexcept {
    return b = b >> shift;
}

template<class IntegerType>
constexpr enable_if_t<is_integral<IntegerType>::value, byte>
operator<<(byte b, IntegerType shift) noexcept {
    return ala::byte(static_cast<unsigned int>(b) << shift);
}

template<class IntegerType>
constexpr enable_if_t<is_integral<IntegerType>::value, byte>
operator>>(byte b, IntegerType shift) noexcept {
    return ala::byte(static_cast<unsigned int>(b) >> shift);
}

constexpr byte operator|(byte l, byte r) noexcept {
    return ala::byte(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
}

constexpr byte operator&(byte l, byte r) noexcept {
    return ala::byte(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
}

constexpr byte operator^(byte l, byte r) noexcept {
    return ala::byte(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
}

constexpr byte operator~(byte b) noexcept {
    return ala::byte(~static_cast<unsigned int>(b));
}

constexpr byte &operator|=(byte &l, byte r) noexcept {
    return l = l | r;
}

constexpr byte &operator&=(byte &l, byte r) noexcept {
    return l = l & r;
}

constexpr byte &operator^=(byte &l, byte r) noexcept {
    return l = l ^ r;
}

} // namespace ala
#endif // _ALA_CONFIG_STDDEF_H