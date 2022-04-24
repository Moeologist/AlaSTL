#ifndef _ALA_INTRIN_CPUID_H
#define _ALA_INTRIN_CPUID_H

#ifdef _ALA_MSVC
extern unsigned char _BitScanForward(unsigned long *, unsigned long);
extern unsigned char _BitScanReverse(unsigned long *, unsigned long);
extern unsigned int __popcnt(unsigned int);

    #if defined(_ALA_X64) || defined(_ALA_ARM64)
extern unsigned char _BitScanForward64(unsigned long *, __int64);
extern unsigned char _BitScanReverse64(unsigned long *, __int64);
extern unsigned __int64 __popcnt64(unsigned __int64);
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

inline int _popcount(unsigned x)             noexcept { return __builtin_popcount(x); }
inline int _popcountl(unsigned long x)       noexcept { return __builtin_popcountl(x); }
inline int _popcountll(unsigned long long x) noexcept { return __builtin_popcountll(x); }

    // clang-format on

#else

// libc++ ctz/clz implement
inline int _ctz(unsigned x) {
    static_assert(sizeof(unsigned) == sizeof(unsigned long), "Internal error");
    unsigned long result;
    if (_BitScanForward(&result, x))
        return static_cast<int>(result);
    return 8 * sizeof(unsigned); // Undefined Behavior.
}

inline int _ctzl(unsigned long x) {
    unsigned long result;
    if (_BitScanForward(&result, x))
        return static_cast<int>(result);
    return 8 * sizeof(unsigned long); // Undefined Behavior.
}

inline int _ctzll(unsigned long long x) {
    static_assert(sizeof(unsigned long long) * 8 == 64, "Internal error");
    static_assert(sizeof(unsigned long) * 8 == 32, "Internal error");
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
    static_assert(sizeof(unsigned) == sizeof(unsigned long), "Internal error");
    unsigned long result;
    if (_BitScanForward(&result, x))
        return static_cast<int>(result);
    return 8 * sizeof(unsigned); // Undefined Behavior.
}

inline int _clzl(unsigned long x) {
    unsigned long result;
    if (_BitScanReverse(&result, x))
        return static_cast<int>(31 - result);
    return 8 * sizeof(unsigned long); // Undefined Behavior.
}

inline int _clzll(unsigned long long x) {
    static_assert(sizeof(unsigned long long) * 8 == 64, "Internal error");
    static_assert(sizeof(unsigned long) * 8 == 32, "Internal error");
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

inline int _popcount(unsigned x) noexcept {
    return __popcnt(x);
}

inline int _popcountl(unsigned long x) noexcept {
    static_assert(sizeof(unsigned long) == sizeof(unsigned int),
                  "Internal error");
    return __popcnt(x);
}

inline int _popcountll(unsigned long long x) noexcept {
    static_assert(sizeof(unsigned long long) * 8 == 64, "Internal error");
    static_assert(sizeof(unsigned int) * 8 == 32, "Internal error");
    #if defined(_ALA_ARM64) || defined(_ALA_X64)
    return __popcnt64(static_cast<unsigned __int64>(x));
    #else
    return __popcnt(static_cast<unsigned int>(x)) +
           __popcnt(static_cast<unsigned int>(x >> 32));
    #endif
}
#endif

// clang-format off
inline int ctz(unsigned i)           noexcept { return ala::intrin::_ctz(i);  }
inline int ctz(unsigned long i)      noexcept { return ala::intrin::_ctzl(i); }
inline int ctz(unsigned long long i) noexcept { return ala::intrin::_ctzll(i);}
inline int clz(unsigned i)           noexcept { return ala::intrin::_clz(i);  }
inline int clz(unsigned long i)      noexcept { return ala::intrin::_clzl(i); }
inline int clz(unsigned long long i) noexcept { return ala::intrin::_clzll(i);}
inline int popcount(unsigned i)           noexcept { return ala::intrin::_popcount(i); }
inline int popcount(unsigned long i)      noexcept { return ala::intrin::_popcountl(i); }
inline int popcount(unsigned long long i) noexcept { return ala::intrin::_popcountll(i); }
// clang-format on

} // namespace intrin
} // namespace ala
#endif