// http://xoshiro.di.unimi.it/
#ifndef _ALA_DETAIL_XORSHIFT_HPP
#define _ALA_DETAIL_XORSHIFT_HPP

#include <ala/config.h>

#ifdef _ALA_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitwise-op-parentheses"
#pragma clang diagnostic ignored "-Wshift-op-parentheses"
#elif defined _ALA_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#elif defined _ALA_MSVC
#pragma warning(push)
#pragma warning(disable : 4554)
#endif

#define rotl(x, k) (x << k | x >> 8 * sizeof(x) - k)

namespace ala {
namespace detail {

template<typename T>
struct args;

template<>
struct args<uint32_t> {
    constexpr static int shi = 9;
    constexpr static int rot = 11;
    constexpr static uint32_t jmp[4] = {0x8764000b, 0xf542d2d3, 0x6fa035c3,
                                        0x77f2db5b};
};

template<>
struct args<uint64_t> {
    constexpr static int shi = 17;
    constexpr static int rot = 45;
    constexpr static uint64_t jmp[4] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c,
                                        0xa9582618e03fc9aa, 0x39abdc4529b1661c};
};

template<typename UInt>
struct xoshiro {
    UInt s[4];

    xoshiro(UInt const seed) { s[0] = seed; }

    UInt plus() { return s[0] + s[3]; }

    UInt starstar() { return rotl(s[1] * 5, 7) * 9; }

    void next() {
        UInt const t = s[1] << args<UInt>::shi;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], args<UInt>::rot);
    }

    UInt operator()() {
        this->next();
        return this->plus();
    }

    void jump() {
        UInt s0 = 0;
        UInt s1 = 0;
        UInt s2 = 0;
        UInt s3 = 0;
        for (int i = 0; i < 4; ++i)
            for (int b = 0; b < 8 * sizeof(UInt); ++b) {
                if (args<UInt>::jmp[i] & UInt(1) << b) {
                    s0 ^= s[0];
                    s1 ^= s[1];
                    s2 ^= s[2];
                    s3 ^= s[3];
                }
                this->next();
            }
        s[0] = s0;
        s[1] = s1;
        s[2] = s2;
        s[3] = s3;
    }
};

double rand_double(uint64_t s) {
    s = (s >> 64 - 52) | (1ull << 10) - 1 << 52;
    return *(double *)(&s) - 1.0;
};

float rand_float(uint32_t s) {
    s = (s >> 32 - 23) | (1u << 7) - 1 << 23;
    return *(float *)(&s) - 1.0f;
}

} // namespace detail
} // namespace ala

#ifdef _ALA_CLANG
#pragma clang diagnostic pop
#elif defined _ALA_GCC
#pragma GCC diagnostic pop
#elif defined _ALA_MSVC
#pragma warning(pop)
#endif

#endif // XORSHIFT_HPP