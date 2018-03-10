#ifndef XORSHIFT_HPP
#define XORSHIFT_HPP

#include <ala/config.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitwise-op-parentheses"
#pragma clang diagnostic ignored "-Wshift-op-parentheses"
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#elif defined _MSC_VER
#pragma  warning(push) 
#pragma  warning(disable: 4554)
#endif

namespace ala {

class xoroshiro128plus {
	uint64_t s[2] = {0, 0xabcd};

  public:
	xoroshiro128plus(const uint64_t seed) { s[0] = seed; }
	ALA_FORCEINLINE uint64_t operator()() {
		const uint64_t s0 = s[0];
		uint64_t s1 = s[1];
		s1 ^= s0;
		s[0] = (s0 << 55 | s0 >> 64 - 55) ^ s1 ^ s1 << 14;
		s[1] = (s1 << 36 | s1 >> 64 - 36);
		return s[0] + s[1];
	}
};

class xorshift1024star {
	uint64_t s[16] = {0, 1, 2, 11, 12, 22, 111, 112, 122, 222, 1111, 1112, 1122, 1222, 2222, 11111};
	size_t p;

  public:
	xorshift1024star(const uint64_t seed) { s[0] = seed; }
	ALA_FORCEINLINE uint64_t operator()() {
		const uint64_t s0 = s[p];
		uint64_t s1 = s[p = (p + 1) & 15];
		s1 ^= s1 << 31;
		s[p] = s1 ^ s0 ^ s1 >> 11 ^ s0 >> 30;
		return s[p] * 0x9e3779b97f4a7c13;
	}
};

ALA_FORCEINLINE double rand_double(uint64_t s) {
	s = s & (1ull << 52) - 1 | (1ull << 10) - 1 << 52;
	return *(double *)(&s) - 1.0;
};

ALA_FORCEINLINE float rand_float(uint32_t s) {
	s = s & (1 << 23) - 1 | (1 << 7) - 1 << 23;
	return *(float *)(&s) - 1.0f;
}

} // namespace ala

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined __GNUC__
#pragma GCC diagnostic pop
#elif defined _MSC_VER
#pragma  warning(pop)
#endif

#endif // XORSHIFT_HPP