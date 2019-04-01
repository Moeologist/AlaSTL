#ifndef _ALA_DETAIL_INTRIN_H
#define _ALA_DETAIL_INTRIN_H

#ifdef _ALA_X86

#if defined(_ALA_MSVC) || defined(_ALA_CLANG_MSVC)

extern int _rdrand16_step(unsigned short *);
extern int _rdrand32_step(unsigned int *);
extern int _rdseed16_step(unsigned short *);
extern int _rdseed32_step(unsigned int *);

#ifdef _ALA_X86_64
extern int _rdrand64_step(unsigned long long *);
extern int _rdseed64_step(unsigned __int64 *);
#endif

#else

inline int _rdrand16_step(unsigned short *__p) {
    return __builtin_ia32_rdrand16_step(__p);
}
inline int _rdrand32_step(unsigned int *__p) {
    return __builtin_ia32_rdrand32_step(__p);
}
inline int _rdseed16_step(unsigned short *__p) {
    return __builtin_ia32_rdseed_hi_step(__p);
}
inline int _rdseed32_step(unsigned int *__p) {
    return __builtin_ia32_rdseed_si_step(__p);
}

#ifdef _ALA_X86_64
inline int _rdrand64_step(unsigned long long *__p) {
    return __builtin_ia32_rdrand64_step(__p);
}
inline int _rdseed64_step(unsigned long long *__p) {
    return __builtin_ia32_rdseed_di_step(__p);
}
#endif

#endif

#endif // X86

#endif // HEAD