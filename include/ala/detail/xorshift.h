// http://xoshiro.di.unimi.it/
#ifndef _ALA_DETAIL_XORSHIFT_H
#define _ALA_DETAIL_XORSHIFT_H

#include <ala/config.h>
#include <ala/type_traits.h>

namespace ala {
namespace detail {

template<typename UInt>
constexpr UInt rotl(const UInt x, int k) {
    return (x << k) | (x >> (sizeof(UInt) * 8 - k));
}

template<typename>
struct _xoshiro_jump;

template<>
struct _xoshiro_jump<uint64_t> {
    static constexpr uint64_t jmp[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c,
                                       0xa9582618e03fc9aa, 0x39abdc4529b1661c};
    // equivalent to 2^128 calls to next()

    /*
  static constexpr uint64_t jmp[] = {0x76e15d3efefdcbbf,
                                      0xc5004e441c522fb3,
                                      0x77710069854ee241,
                                      0x39109bb02acbe635};
  equivalent to 2^192 calls to next()
  */
};

template<>
struct _xoshiro_jump<uint32_t> {
    static constexpr uint32_t jmp[] = {0x8764000b, 0xf542d2d3, 0x6fa035c3,
                                       0x77f2db5b};
    // equivalent to 2^64 calls to next()
};

/*
A    state xor-lshift
B    state rotate left
Star use ** scrambler (+ scrambler by default)
I    index of element used for starstar output
*/

template<typename UInt, UInt A, UInt B, bool Star, size_t I>
struct xoshiro {
    typedef UInt result_type;
    result_type s[4];

    constexpr result_type plus() {
        return s[0] + s[3];
    }

    constexpr result_type starstar() {
        return rotl(s[I] * 5, 7) * 9;
    }

    constexpr void next() {
        const result_type t = s[1] << A;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], B);
    }

    constexpr result_type operator()() {
        next();
        return Star ? starstar() : plus();
    }

    constexpr void jump() {
        // jump available only UInt is uint32_t or uint64_t
        constexpr const result_type(&jmp)[4] = _xoshiro_jump<result_type>::jmp;
        result_type s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        for (int i = 0; i < sizeof(jmp) / sizeof(*jmp); ++i)
            for (int b = 0; b < 8 * sizeof(result_type); ++b) {
                if (jmp[i] & (result_type(1) << b)) {
                    s0 ^= s[0];
                    s1 ^= s[1];
                    s2 ^= s[2];
                    s3 ^= s[3];
                }
                next();
            }
        s[0] = s0;
        s[1] = s1;
        s[2] = s2;
        s[3] = s3;
    }
};

using xoshiro256ss = xoshiro<uint64_t, 17, 45, true, 1>;
using xoshiro256p = xoshiro<uint64_t, 17, 45, false, 1>;
using xoshiro128ss = xoshiro<uint32_t, 9, 11, true, 0>;
using xoshiro128p = xoshiro<uint32_t, 9, 11, false, 0>;

double rand_double(uint64_t s) {
    s = (s >> (64 - 52)) | ((1ull << 10) - 1) << 52;
    return *(double *)(&s) - 1.0;
};

float rand_float(uint32_t s) {
    s = (s >> (32 - 23)) | ((1u << 7) - 1) << 23;
    return *(float *)(&s) - 1.0f;
}

} // namespace detail
} // namespace ala

#endif // HEAD