#ifndef _ALA_DETAIL_INTRIN_H
#define _ALA_DETAIL_INTRIN_H

#ifdef _ALA_X86

#if defined(_ALA_MSVC)

#ifdef __cplusplus
extern "C" {
#endif

extern int _rdrand16_step(unsigned short *);
extern int _rdrand32_step(unsigned int *);
extern int _rdseed16_step(unsigned short *);
extern int _rdseed32_step(unsigned int *);

#ifdef _ALA_X86_64
extern int _rdrand64_step(unsigned long long *);
extern int _rdseed64_step(unsigned long long *);
#endif

#ifdef __cplusplus
}
#endif

#elif defined(_ALA_CLANG)

#define _rdrand16_step(p) __builtin_ia32_rdrand16_step(p)
#define _rdrand32_step(p) __builtin_ia32_rdrand32_step(p)
#define _rdseed16_step(p) __builtin_ia32_rdseed16_step(p)
#define _rdseed32_step(p) __builtin_ia32_rdseed32_step(p)

#ifdef _ALA_X86_64
#define _rdrand64_step(p) __builtin_ia32_rdrand64_step(p)
#define _rdseed64_step(p) __builtin_ia32_rdseed64_step(p)
#endif

#elif defined(_ALA_GCC)

#define _rdrand16_step(p) __builtin_ia32_rdrand16_step(p)
#define _rdrand32_step(p) __builtin_ia32_rdrand32_step(p)
#define _rdseed16_step(p) __builtin_ia32_rdseed_hi_step(p)
#define _rdseed32_step(p) __builtin_ia32_rdseed_si_step(p)

#ifdef _ALA_X86_64
#define _rdrand64_step(p) __builtin_ia32_rdrand64_step(p)
#define _rdseed64_step(p) __builtin_ia32_rdseed_di_step(p)
#endif

#endif // COMPILER

#endif // X86

#endif // HEAD