// clang-format off
#ifndef _ALA_INTRIN_RDRAND_H
#define _ALA_INTRIN_RDRAND_H

#ifdef _ALA_X86

#if defined(_ALA_MSVC)

#ifdef __cplusplus
extern "C" {
#endif

extern int _rdrand16_step(unsigned short *);
extern int _rdrand32_step(unsigned int *);

#ifdef _ALA_X64
extern int _rdrand64_step(unsigned long long *);
#endif

#ifdef __cplusplus
}
#endif

#endif // defined(_ALA_MSVC)

namespace ala {
namespace intrin {

        // clang-format off
    #if defined(_ALA_MSVC)

        static inline int _rdrand16(unsigned short *p)         { return _rdrand16_step(p); }
        static inline int _rdrand32(unsigned int *p)           { return _rdrand32_step(p); }

        #ifdef _ALA_X64
        static inline int _rdrand64(unsigned long long *p)     { return _rdrand64_step(p); }
        #endif

    #elif defined(_ALA_CLANG)

        #ifndef __RDRAND__
        #pragma clang attribute push (__attribute__((target("rdrnd"))), apply_to=function)
        #define __POP_RDRAND__
        #endif // __RDRAND__

        static inline int _rdrand16(unsigned short *p)     { return __builtin_ia32_rdrand16_step(p); }
        static inline int _rdrand32(unsigned int *p)       { return __builtin_ia32_rdrand32_step(p); }
        #ifdef _ALA_X64
        static inline int _rdrand64(unsigned long long *p) { return __builtin_ia32_rdrand64_step(p); }
        #endif

        #ifdef __POP_RDRAND__
        #pragma clang attribute pop
        #undef __POP_RDRAND__
        #endif // __POP_RDRAND__

    #elif defined(_ALA_GCC)

        #ifndef __RDRAND__
        #pragma GCC push_options
        #pragma GCC target("rdrnd")
        #define __POP_RDRAND__
        #endif // __RDRAND__

        static inline int _rdrand16(unsigned short *p)     { return __builtin_ia32_rdrand16_step(p); }
        static inline int _rdrand32(unsigned int *p)       { return __builtin_ia32_rdrand32_step(p); }
        #ifdef _ALA_X64
        static inline int _rdrand64(unsigned long long *p) { return __builtin_ia32_rdrand64_step(p); }
        #endif

        #ifdef __POP_RDRAND__
        #pragma GCC push_options
        #undef __POP_RDRAND__
        #endif // __POP_RDRAND__

    #endif // COMPILER

    #ifndef _ALA_X64
        inline int _rdrand64(unsigned long long *p) {
            unsigned int *up = (unsigned int *)p;
            return _rdrand32(up) && _rdrand32(up + 1);
        }
    #endif

    inline int rdrand(unsigned short *p)      { return ala::intrin::_rdrand16(p); }
    inline int rdrand(unsigned int *p)        { return ala::intrin::_rdrand32(p); }
    inline int rdrand(unsigned long long *p)  { return ala::intrin::_rdrand64(p); }

} // namespace intrin
} // namespace ala

#endif // X86

#endif // HEAD