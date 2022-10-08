#ifndef _ALA_BIT_H
#define _ALA_BIT_H

#include <ala/type_traits.h>
#include <ala/intrin/bit.h>

#include <limits>

namespace ala {

using ::std::numeric_limits;

// bit_cast
template<class To, class From>
ALA_NODISCARD constexpr enable_if_t<
    _and_<bool_constant<sizeof(To) == sizeof(From)>, is_trivially_copyable<To>,
          is_trivially_copyable<From>>::value,
    To>
bit_cast(const From &x) noexcept {
#if ALA_HAS_BUILTIN(__builtin_bit_cast) || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1927)
    return __builtin_bit_cast(To, x);
#else
    static_assert(is_trivially_constructible_v<To>,
                  "This implementation additionally requires destination type "
                  "to be trivially constructible");
    To t;
    ala::memcpy(&t, &x, sizeof(To));
    return t;
#endif
}

// byteswap
template<class T>
constexpr enable_if_t<is_integral<T>::value, T> byteswap(T x) noexcept {
    if (sizeof(T) == 1) {
        return x;
    } else if (sizeof(T) == 2) {
        return ala::intrin::bswap16(x);
    } else if (sizeof(T) == 4) {
        return ala::intrin::bswap32(x);
    } else if (sizeof(T) == 8) {
        return ala::intrin::bswap64(x);
    } else {
        assert(false);
    }
}

template<class T>
struct _is_uint
    : _and_<is_integral<T>, _not_<is_same<T, bool>>, _not_<is_same<T, char>>,
#if _ALA_ENABLE_CHAR8T
            _not_<is_same<T, char8_t>>,
#endif
            _not_<is_same<T, char16_t>>, _not_<is_same<T, char32_t>>,
            _not_<is_same<T, wchar_t>>, is_unsigned<T>> {
};

// rotating
template<class T>
ALA_NODISCARD constexpr enable_if_t<_is_uint<T>::value, T> rotl(T x,
                                                                int s) noexcept {
    constexpr unsigned int digits = numeric_limits<T>::digits;
    if ((s % digits) == 0)
        return x;
    return (x << (s % digits)) | (x >> (digits - (s % digits)));
}

template<class T>
ALA_NODISCARD constexpr enable_if_t<_is_uint<T>::value, T> rotr(T x,
                                                                int s) noexcept {
    constexpr unsigned int digits = numeric_limits<T>::digits;
    if ((s % digits) == 0)
        return x;
    return (x >> (s % digits)) | (x << (digits - (s % digits)));
}

// counting
template<class T>
constexpr enable_if_t<_is_uint<T>::value, int> countl_zero(T x) noexcept {
    if (x == 0)
        return numeric_limits<T>::digits;

    if (sizeof(T) <= sizeof(unsigned int))
        return ala::intrin::clz(static_cast<unsigned int>(x)) -
               (numeric_limits<unsigned int>::digits - numeric_limits<T>::digits);
    else if (sizeof(T) <= sizeof(unsigned long))
        return ala::intrin::clz(static_cast<unsigned long>(x)) -
               (numeric_limits<unsigned long>::digits - numeric_limits<T>::digits);
    else if (sizeof(T) <= sizeof(unsigned long long))
        return ala::intrin::clz(static_cast<unsigned long long>(x)) -
               (numeric_limits<unsigned long long>::digits -
                numeric_limits<T>::digits);
    else {
        int r = 0;
        int t = 0;
        constexpr unsigned int digits = numeric_limits<unsigned long long>::digits;
        while (true) {
            x = ala::rotr(x, digits);
            if ((t = ala::intrin::clz(static_cast<unsigned long long>(x))) !=
                digits)
                break;
            r += t;
        }
        return r + t;
    }
}

template<class T>
constexpr enable_if_t<_is_uint<T>::value, int> countl_one(T x) noexcept {
    return x != numeric_limits<T>::max() ? countl_zero(static_cast<T>(~x)) :
                                           numeric_limits<T>::digits;
}

template<class T>
constexpr enable_if_t<_is_uint<T>::value, int> countr_zero(T x) noexcept {
    if (x == 0)
        return numeric_limits<T>::digits;

    if (sizeof(T) <= sizeof(unsigned int))
        return ala::intrin::ctz(static_cast<unsigned int>(x));
    else if (sizeof(T) <= sizeof(unsigned long))
        return ala::intrin::ctz(static_cast<unsigned long>(x));
    else if (sizeof(T) <= sizeof(unsigned long long))
        return ala::intrin::ctz(static_cast<unsigned long long>(x));
    else {
        int r = 0;
        constexpr unsigned int digits = numeric_limits<unsigned long long>::digits;
        while (static_cast<unsigned long long>(x) == 0uLL) {
            r += digits;
            x >>= digits;
        }
        return r + ala::intrin::ctz(static_cast<unsigned long long>(x));
    }
}

template<class T>
constexpr enable_if_t<_is_uint<T>::value, int> countr_one(T x) noexcept {
    return x != numeric_limits<T>::max() ? countr_zero(static_cast<T>(~x)) :
                                           numeric_limits<T>::digits;
}

template<class T>
constexpr enable_if_t<_is_uint<T>::value, int> popcount(T x) noexcept {
    if (sizeof(T) <= sizeof(unsigned int))
        return ala::intrin::popcount(static_cast<unsigned int>(x));
    else if (sizeof(T) <= sizeof(unsigned long))
        return ala::intrin::popcount(static_cast<unsigned long>(x));
    else if (sizeof(T) <= sizeof(unsigned long long))
        return ala::intrin::popcount(static_cast<unsigned long long>(x));
    else {
        int r = 0;
        while (x != 0) {
            r += ala::intrin::popcount(static_cast<unsigned long long>(x));
            x >>= numeric_limits<unsigned long long>::digits;
        }
        return r;
    }
}

// integral powers of 2
template<class T>
constexpr enable_if_t<_is_uint<T>::value, bool> has_single_bit(T x) noexcept {
    return x != 0 && (x & (x - 1)) == 0;
}

template<class T>
constexpr T bit_width(T x) noexcept {
    return numeric_limits<T>::digits - ala::countl_zero(x);
}

template<class T>
constexpr enable_if_t<_is_uint<T>::value, T> bit_ceil(T x) noexcept {
    if (x < 2u)
        return T(1);
    if (is_same<T, decltype(+x)>::value)
        return T(1) << ala::bit_width(T(x - 1));
    else {
        constexpr int offset_for_ub = numeric_limits<unsigned>::digits -
                                      numeric_limits<T>::digits;
        return T(1u << (ala::bit_width(T(x - 1)) + offset_for_ub) >>
                 offset_for_ub);
    }
}

template<class T>
constexpr enable_if_t<_is_uint<T>::value, T> bit_floor(T x) noexcept {
    if (x != 0)
        return T{1} << (ala::bit_width(x) - 1);
    return 0;
}

} // namespace ala
#endif