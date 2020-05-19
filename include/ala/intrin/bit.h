#ifndef _ALA_INTRIN_CPUID_H
#define _ALA_INTRIN_CPUID_H

#include <ala/type_traits.h>

#ifdef _ALA_MSVC
unsigned char _BitScanForward(unsigned long *, unsigned long);
unsigned char _BitScanReverse(unsigned long *, unsigned long);

#if defined(_ALA_X64) || defined(_ALA_ARM64)
unsigned char _BitScanForward64(unsigned long *, unsigned long long);
unsigned char _BitScanReverse64(unsigned long *, unsigned long long);
#endif

#endif

namespace ala {
namespace intrin {

#ifndef _ALA_MSVC
// clang-format off

inline int _ctz(unsigned x)             noexcept { return __builtin_ctz(x); }
inline int _ctzl(unsigned long x)       noexcept { return __builtin_ctzl(x); }
inline int _ctzll(unsigned long long x) noexcept { return __builtin_ctzll(x); }

inline int _clz(unsigned x)             noexcept { return __builtin_clz(x); }
inline int _clzl(unsigned long x)       noexcept { return __builtin_clzl(x); }
inline int _clzll(unsigned long long x) noexcept { return __builtin_clzll(x); }

    // clang-format on

#else

// libc++ ctz/clz implement
inline int _ctz(unsigned x) {
    static_assert(sizeof(unsigned) == sizeof(unsigned long), "");
    static_assert(sizeof(unsigned long) == 4, "");
    unsigned long result;
    if (_BitScanForward(&result, x))
        return static_cast<int>(result);
    return 32;
}

inline int _ctzl(unsigned long x) {
    static_assert(sizeof(unsigned long) == sizeof(unsigned), "");
    return _ctz(static_cast<unsigned>(x));
}

inline int _ctzll(unsigned long long x) {
    unsigned long result;
    #if defined(_ALA_ARM64) || defined(_ALA_X64)
    if (_BitScanForward64(&result, x))
        return static_cast<int>(result);
    #else
    // Win32 doesn't have _BitScanForward64 so emulate it with two 32 bit calls.
    if (_BitScanForward(&result, static_cast<unsigned long>(x)))
        return static_cast<int>(result);
    if (_BitScanForward(&result, static_cast<unsigned long>(x >> 32)))
        return static_cast<int>(result + 32);
    #endif
    return 64;
}

// Precondition:  x != 0
inline int _clz(unsigned x) {
    static_assert(sizeof(unsigned) == sizeof(unsigned long), "");
    static_assert(sizeof(unsigned long) == 4, "");
    unsigned long result;
    if (_BitScanReverse(&result, x))
        return static_cast<int>(31 - result);
    return 32; // Undefined Behavior.
}

inline int _clzl(unsigned long x) {
    static_assert(sizeof(unsigned) == sizeof(unsigned long), "");
    return _clz(static_cast<unsigned>(x));
}

inline int _clzll(unsigned long long x) {
    unsigned long result;
    #if defined(_ALA_ARM64) || defined(_ALA_X64)
    if (_BitScanReverse64(&result, x))
        return static_cast<int>(63 - result);
    #else
    // Win32 doesn't have _BitScanReverse64 so emulate it with two 32 bit calls.
    if (_BitScanReverse(&result, static_cast<unsigned long>(x >> 32)))
        return static_cast<int>(63 - (result + 32));
    if (_BitScanReverse(&result, static_cast<unsigned long>(x)))
        return static_cast<int>(63 - result);
    #endif
    return 64; // Undefined Behavior.
}
#endif

template<class UInt>
enable_if_t<sizeof(UInt) == 4, int> ctz(UInt i) {
    static_assert(sizeof(unsigned) == 4, "Internal error");
    return ala::intrin::_ctz(i);
}

template<class UInt>
enable_if_t<sizeof(UInt) == 8, int> ctz(UInt i) {
    static_assert(sizeof(unsigned long long) == 8, "Internal error");
    return ala::intrin::_ctzll(i);
}

template<class UInt>
enable_if_t<sizeof(UInt) == 4, int> clz(UInt i) {
    static_assert(sizeof(unsigned) == 4, "Internal error");
    return ala::intrin::_clz(i);
}

template<class UInt>
enable_if_t<sizeof(UInt) == 8, int> clz(UInt i) {
    static_assert(sizeof(unsigned long long) == 8, "Internal error");
    return ala::intrin::_clzll(i);
}

} // namespace intrin
} // namespace ala
#endif