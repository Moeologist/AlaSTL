#ifndef _ALA_RANDOM_H
#define _ALA_RANDOM_H

#include <ala/intrin/bit.h>

#include <ala/intrin/rdrand.h>
#include <ala/intrin/rdseed.h>

namespace ala {

struct bad_random_device: exception {
    explicit bad_random_device() {}
};

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
        int r = this->rd(&s);
        if (r)
            return s;
        throw bad_random_device{};
    }

protected:
    template<bool Dummy = RdSeed>
    enable_if_t<Dummy, int> rd(UInt *p) {
        return ala::intrin::rdseed(&s);
    }

    template<bool Dummy = RdSeed>
    enable_if_t<!Dummy, int> rd(UInt *p) {
        return ala::intrin::rdrand(&s);
    }
};

using random_device = random_device_adaptor<uint_fast32_t>;
using random_device_64 = random_device_adaptor<uint_fast64_t>;

// see http://xoshiro.di.unimi.it/

template<typename>
struct _xoshiro_jump;

template<>
struct _xoshiro_jump<uint_fast64_t> {
    static constexpr uint_fast64_t jmp[] = {0x180ec6d33cfd0aba,
                                            0xd5a61266f0c9392c,
                                            0xa9582618e03fc9aa,
                                            0x39abdc4529b1661c};
    // equivalent to 2^128 calls to next()

    /*
  static constexpr uint_fast64_t jmp[] = {0x76e15d3efefdcbbf,
                                      0xc5004e441c522fb3,
                                      0x77710069854ee241,
                                      0x39109bb02acbe635};
  equivalent to 2^192 calls to next()
  */
};

#if UINT_FAST32_MAX != UINT_FAST64_MAX
template<>
struct _xoshiro_jump<uint_fast32_t> {
    static constexpr uint_fast32_t jmp[] = {0x8764000b, 0xf542d2d3, 0x6fa035c3,
                                            0x77f2db5b};
    // equivalent to 2^64 calls to next()
};
#endif

/*
A        state xor-lshift
B        state rotate left
Scramber Scramber type
*/

enum xoshiro_scramber { ScramberPlus, ScramberStarStar, ScramberPlusPlus };

template<typename UInt, UInt A, UInt B, xoshiro_scramber Scramber>
struct xoshiro {
    static_assert(sizeof(UInt) == 4 || sizeof(UInt) == 8,
                  "xoshiro only support 32bit|64bit integral");
    typedef UInt result_type;
    result_type s[4];

    constexpr result_type rotl(result_type x, result_type k) {
        return (x << k) | (x >> (sizeof(UInt) * 8 - k));
    }

    template<xoshiro_scramber Dummy = Scramber>
    constexpr enable_if_t<sizeof(UInt) == 8 && Dummy == ScramberPlus, result_type>
    scramber() {
        return s[0] + s[3];
    }

    template<xoshiro_scramber Dummy = Scramber>
    constexpr enable_if_t<sizeof(UInt) == 8 && Dummy == ScramberStarStar, result_type>
    scramber() {
        return rotl(s[1] * 5, 7) * 9;
    }

    template<xoshiro_scramber Dummy = Scramber>
    constexpr enable_if_t<sizeof(UInt) == 8 && Dummy == ScramberPlusPlus, result_type>
    scramber() {
        return rotl(s[0] + s[3], 23) + s[0];
    }

    template<xoshiro_scramber Dummy = Scramber>
    constexpr enable_if_t<sizeof(UInt) == 4 && Dummy == ScramberPlus, result_type>
    scramber() {
        return s[0] + s[3];
    }

    template<xoshiro_scramber Dummy = Scramber>
    constexpr enable_if_t<sizeof(UInt) == 4 && Dummy == ScramberStarStar, result_type>
    scramber() {
        return rotl(s[0] * 5, 7) * 9;
    }

    template<xoshiro_scramber Dummy = Scramber>
    constexpr enable_if_t<sizeof(UInt) == 4 && Dummy == ScramberPlusPlus, result_type>
    scramber() {
        return rotl(s[0] + s[3], 7) + s[0];
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
        // jump need UInt is uint_fast32_t or uint_fast64_t
        constexpr const result_type(&jmp)[4] = _xoshiro_jump<result_type>::jmp;
        result_type s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        for (int i = 0; i < sizeof(jmp) / sizeof(*jmp); ++i)
            for (int b = 0; b < sizeof(result_type) * 8; ++b) {
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

using xoshiro256pp = xoshiro<uint_fast64_t, 17, 45, ScramberPlusPlus>;
using xoshiro256ss = xoshiro<uint_fast64_t, 17, 45, ScramberStarStar>;
using xoshiro256p = xoshiro<uint_fast64_t, 17, 45, ScramberPlus>;
using xoshiro128pp = xoshiro<uint_fast32_t, 9, 11, ScramberPlusPlus>;
using xoshiro128ss = xoshiro<uint_fast32_t, 9, 11, ScramberStarStar>;
using xoshiro128p = xoshiro<uint_fast32_t, 9, 11, ScramberPlus>;

using xorshift32 = xorshift<uint_fast32_t, 13, 17, 5>;
using xorshift64 = xorshift<uint_fast64_t, 13, 7, 17>;

using minstd_rand = linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647>;

template<class URBG>
conditional_t<(sizeof(typename remove_cvref_t<URBG>::result_type) * 8 >= 52),
              double, float>
generate_real(URBG &&g) {
    using UInt = typename remove_cvref_t<URBG>::result_type;
    using Real =
        conditional_t<(sizeof(typename remove_cvref_t<URBG>::result_type) * 8 >= 52),
                      double, float>;
    static_assert(numeric_limits<Real>::is_iec559,
                  "IEC 559 (IEEE 754) check failed");
    static_assert(sizeof(UInt) >= sizeof(Real), "The size of uint too small");
    constexpr size_t exponent = is_same<double, Real>::value ? 11 : 8;
    constexpr size_t fraction = is_same<double, Real>::value ? 52 : 23;
    constexpr size_t bits = sizeof(UInt) * 8;
    constexpr UInt e = ((UInt)1 << (exponent - 1)) - 1;
    UInt s = (g() >> (bits - fraction)) | (e << fraction);
    return *(Real *)(&s) - (Real)1.0;
}

template<class Int = int>
struct uniform_int_distribution {
    using result_type = Int;
    struct param_type {
        result_type _a;
        result_type _b;

        typedef uniform_int_distribution distribution_type;

        explicit param_type(result_type a = 0,
                            result_type b = numeric_limits<result_type>::max())
            : _a(a), _b(b) {}

        result_type a() const {
            return _a;
        }
        result_type b() const {
            return _b;
        }

        friend bool operator==(const param_type &lhs, const param_type &rhs) {
            return lhs._a == rhs._a && lhs._b == rhs._b;
        }

        friend bool operator!=(const param_type &lhs, const param_type &rhs) {
            return !(lhs == rhs);
        }
    };

protected:
    param_type _p;

public:
    // constructors and reset functions
    explicit uniform_int_distribution(
        result_type a = 0, result_type b = numeric_limits<result_type>::max())
        : _p(a, b) {}
    explicit uniform_int_distribution(const param_type &p): _p(p) {}
    void reset() {}

    // generating functions

    template<class URNG>
    result_type operator()(URNG &g) {
        return (*this)(g, _p);
    }

    template<class URNG>
    result_type operator()(URNG &g, const param_type &p) {
        result_type a = p.a();
        result_type b = p.b();
        result_type l = b - a;
        if (l == 0)
            return a;
        int z = ala::intrin::clz(make_unsigned_t<Int>(l));
        result_type mask = numeric_limits<result_type>::max() >> z;
        result_type r = g() & mask;
        for (; l < r;)
            r = g() & mask;
        return a + r;
    }

    // property functions
    result_type a() const {
        return _p.a();
    }
    result_type b() const {
        return _p.b();
    }

    param_type param() const {
        return _p;
    }

    void param(const param_type &p) {
        _p = p;
    }

    result_type min() const {
        return a();
    }

    result_type max() const {
        return b();
    }

    friend bool operator==(const uniform_int_distribution &lhs,
                           const uniform_int_distribution &rhs) {
        return lhs._p == rhs._p;
    }

    friend bool operator!=(const uniform_int_distribution &lhs,
                           const uniform_int_distribution &rhs) {
        return !(lhs == rhs);
    }
};

} // namespace ala
#endif // HEAD