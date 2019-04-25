#ifndef _ALA_RANDOM_H
#define _ALA_RANDOM_H

#include <ala/type_traits.h>

#if defined(_ALA_X86) && \
    ((defined(__RDSEED__) && defined(__RDRND__)) || defined(_ALA_MSVC))
#include <ala/detail/intrin.h>
#endif

namespace ala {

#if defined(_ALA_X86) && \
    ((defined(__RDSEED__) && defined(__RDRND__)) || defined(_ALA_MSVC))

template<class UInt>
constexpr enable_if_t<sizeof(UInt) == 2, int> rdseed(UInt *p) {
    return _rdseed16_step(p);
}

template<class UInt>
constexpr enable_if_t<sizeof(UInt) == 4, int> rdseed(UInt *p) {
    return _rdseed32_step(p);
}

template<class UInt>
constexpr enable_if_t<sizeof(UInt) == 8, int> rdseed(UInt *p) {
    return _rdseed64_step(p);
}

template<class UInt>
constexpr enable_if_t<sizeof(UInt) == 2, int> rdrand(UInt *p) {
    return _rdrand16_step(p);
}

template<class UInt>
constexpr enable_if_t<sizeof(UInt) == 4, int> rdrand(UInt *p) {
    return _rdrand32_step(p);
}

template<class UInt>
constexpr enable_if_t<sizeof(UInt) == 8, int> rdrand(UInt *p) {
    return _rdrand64_step(p);
}

template<class UInt, bool RdSeed = true>
struct random_device_adaptor {
    typedef UInt result_type;
    result_type s;
    random_device_adaptor() {}
    explicit random_device_adaptor(const std::string &token);
    random_device_adaptor(const random_device_adaptor &) = delete;
    random_device_adaptor &operator=(const random_device_adaptor &) = delete;
    static constexpr result_type min() {
        return numeric_limits<result_type>::min();
    }
    static constexpr result_type max() {
        return numeric_limits<result_type>::max();
    }
    result_type operator()() {
        int r = RdSeed ? rdseed(&s) : rdrand(&s);
        if (r)
            return s;
        throw exception{};
    }
};

using random_device = random_device_adaptor<uint_fast32_t>;

#endif

// see http://xoshiro.di.unimi.it/

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
Star use ** scrambler
I    index of element used for starstar output
*/

template<typename UInt, UInt A, UInt B, bool Star, size_t I>
struct xoshiro {
    typedef UInt result_type;
    result_type s[4];

    constexpr result_type rotl(result_type x, result_type k) {
        return (x << k) | (x >> (sizeof(UInt) * 8 - k));
    }

    template<bool Dummy = Star>
    constexpr enable_if_t<!Dummy, result_type> scramber() {
        return s[0] + s[3];
    }

    template<bool Dummy = Star>
    constexpr enable_if_t<Dummy, result_type> scramber() {
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
        return scramber();
    }

    constexpr void discard(unsigned long long k) {
        for (; k > 0; --k)
            next();
    }

    constexpr void jump() {
        // jump need UInt is uint32_t or uint64_t
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

template<typename UInt, UInt A, UInt B, UInt C>
struct xorshift {
    typedef UInt result_type;
    result_type s;

    constexpr void next() {
        s ^= s << A;
        s ^= s >> B;
        s ^= s << C;
    }
    constexpr void discard(unsigned long long k) {
        for (; k > 0; --k)
            next();
    }

    constexpr result_type operator()() {
        next();
        return s;
    }
};

template<typename UInt, UInt A, UInt C, UInt M>
struct linear_congruential_engine {
    typedef UInt result_type;
    result_type s;

    constexpr void next() {
        s = (s * A + C) % M;
    }

    constexpr void discard(unsigned long long k) {
        for (; k > 0; --k)
            next();
    }

    constexpr result_type operator()() {
        next();
        return s;
    }
};

using xoshiro256ss = xoshiro<uint_fast64_t, 17, 45, true, 1>;
using xoshiro256p = xoshiro<uint_fast64_t, 17, 45, false, 1>;
using xoshiro128ss = xoshiro<uint_fast32_t, 9, 11, true, 0>;
using xoshiro128p = xoshiro<uint_fast32_t, 9, 11, false, 0>;

using xorshift32 = xorshift<uint_fast32_t, 13, 17, 5>;
using xorshift64 = xorshift<uint_fast64_t, 13, 7, 17>;

using minstd_rand = linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647>;

template<typename T, size_t Sz = sizeof(T)>
struct _generate_real_traits;

template<typename T>
struct _generate_real_traits<T, 4> {
    typedef float value_type;
    static_assert(numeric_limits<value_type>::is_iec559,
                  "IEC 559 (IEEE 754) check failed");
    static constexpr size_t sign = 1;
    static constexpr size_t exponent = 8;
    static constexpr size_t fraction = 23;
};

template<typename T>
struct _generate_real_traits<T, 8> {
    typedef double value_type;
    static_assert(numeric_limits<value_type>::is_iec559,
                  "IEC 559 (IEEE 754) check failed");
    static constexpr size_t sign = 1;
    static constexpr size_t exponent = 11;
    static constexpr size_t fraction = 52;
};

template<class URBG, class UInt = typename remove_cvref_t<URBG>::result_type>
constexpr typename _generate_real_traits<UInt>::value_type generate_real(URBG &&g) {
    typedef _generate_real_traits<UInt> _traits;
    typedef typename _traits::value_type Real;
    constexpr UInt I = sizeof(UInt) * 8;
    constexpr UInt Mask = ((UInt)1 << (_traits::exponent - 1)) - 1;
    constexpr UInt M = _traits::fraction;
    UInt s = (g() >> (I - M)) | (Mask << M);
    return *(Real *)(&s) - (Real)1.0;
}

} // namespace ala
#endif // HEAD