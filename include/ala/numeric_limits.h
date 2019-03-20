#include <ala/config.h>
#include <ala/external/climits.h>

#define MAKE_numeric_limits(_type_, _minV_, _maxV_, _lowestV_, _epsilonV_, \
                            _round_errorV_, _infinityV_, _quiet_NaNV_, \
                            _signaling_NaNV_, _denorm_minV_) \
    template<> \
    struct numeric_limits< _type_ > { \
        static constexpr _type_ min() noexcept { return _minV_ ; } \
        static constexpr _type_ max() noexcept { return _maxV_ ; } \
        static constexpr _type_ lowest() noexcept { return _lowestV_ ; } \
        static constexpr _type_ epsilon() noexcept { return _epsilonV_ ; } \
        static constexpr _type_ round_error() noexcept { \
            return _round_errorV_ ; \
        } \
        static constexpr _type_ infinity() noexcept { return _infinityV_ ; } \
        static constexpr _type_ quiet_NaN() noexcept { return _quiet_NaNV_ ; } \
        static constexpr _type_ signaling_NaN() noexcept { \
            return _signaling_NaNV_ ; \
        } \
        static constexpr _type_ denorm_min() noexcept { \
            return _denorm_minV_ ; \
        } \
    };

template<typename T>
struct numeric_limits;
// clang-format off

#if defined(__FLT_DENORM_MIN__)
#define _ALA_DENORM_MIN __FLT_DENORM_MIN__
#elif defined(FLT_TRUE_MIN)
#define _ALA_DENORM_MIN FLT_TRUE_MIN
#else
#error "no denorm min"
#endif

// #define
MAKE_numeric_limits(bool, false, true, false, false, false, false, false, false, false)
MAKE_numeric_limits(char, CHAR_MIN, CHAR_MAX, CHAR_MIN, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(signed char, SCHAR_MIN, SCHAR_MAX, SCHAR_MIN, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(unsigned char, 0, UCHAR_MAX, 0, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(wchar_t, WCHAR_MIN, WCHAR_MAX, WCHAR_MIN, 0, 0, 0, 0, 0, 0)
// MAKE_numeric_limits(char8_t, 0, UCHAR_MAX, 0, 0, 0, 0, 0, 0, 0)
// MAKE_numeric_limits(char16_t, 0, UINT_LEAST16_MAX, 0, 0, 0, 0, 0, 0, 0)
// MAKE_numeric_limits(char32_t, 0, UINT_LEAST32_MAX, 0, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(short, SHRT_MIN, SHRT_MAX, SHRT_MIN, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(unsigned short, 0, USHRT_MAX, 0, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(int, INT_MIN, INT_MAX, INT_MIN, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(unsigned int, 0, UINT_MAX, 0, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(long, LONG_MIN, LONG_MAX, LONG_MIN, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(unsigned long, 0, ULONG_MAX, 0, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(long long, LLONG_MIN, LLONG_MAX, LLONG_MIN, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(unsigned long long, 0, ULLONG_MAX, 0, 0, 0, 0, 0, 0, 0)
MAKE_numeric_limits(float, FLT_MIN, FLT_MAX, FLT_MAX, FLT_EPSILON, 5, __builtin_huge_valf(), __builtin_nanf(""), __builtin_nansf(""), _ALA_DENORM_MIN)
MAKE_numeric_limits(double, DBL_MIN, DBL_MAX, DBL_MAX, DBL_EPSILON, 5, __builtin_huge_val(), __builtin_nan(""), __builtin_nans(""), _ALA_DENORM_MIN)
MAKE_numeric_limits(long double, LDBL_MIN, LDBL_MAX, LDBL_MAX, LDBL_EPSILON, 5L, __builtin_huge_val(), __builtin_nan(""), __builtin_nans(""), _ALA_DENORM_MIN)

#undef MAKE_numeric_limits