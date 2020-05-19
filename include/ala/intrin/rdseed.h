// clang-format off
#ifndef _ALA_INTRIN_RDSEED_H
#define _ALA_INTRIN_RDSEED_H

#ifdef _ALA_X86

#if defined(_ALA_MSVC)

#ifdef __cplusplus
extern "C" {
#endif

extern int _rdseed16_step(unsigned short *);
extern int _rdseed32_step(unsigned int *);

#ifdef _ALA_X86_64
extern int _rdseed64_step(unsigned long long *);
#endif

#ifdef __cplusplus
}
#endif

#endif // defined(_ALA_MSVC)

namespace ala {
namespace intrin {

    #if defined(_ALA_MSVC)

        inline int _rdseed16(unsigned short *p)         { return _rdseed16_step(p); }
        inline int _rdseed32(unsigned int *p)           { return _rdseed32_step(p); }

        #ifdef _ALA_X64
        inline int _rdseed64(unsigned long long *p)     { return _rdseed64_step(p); }
        #endif

    #elif defined(_ALA_CLANG)

        inline int _rdseed16(unsigned short *p)     { return __builtin_ia32_rdseed16_step(p); }
        inline int _rdseed32(unsigned int *p)       { return __builtin_ia32_rdseed32_step(p); }
        #ifdef _ALA_X64
        inline int _rdseed64(unsigned long long *p) { return __builtin_ia32_rdseed64_step(p); }
        #endif

    #elif defined(_ALA_GCC)

        inline int _rdseed16(unsigned short *p)     { return __builtin_ia32_rdseed_hi_step(p); }
        inline int _rdseed32(unsigned int *p)       { return __builtin_ia32_rdseed_si_step(p); }
        #ifdef _ALA_X64
        inline int _rdseed64(unsigned long long *p) { return __builtin_ia32_rdseed_di_step(p); }
        #endif

    #endif // COMPILER

    #ifndef _ALA_X64
        inline int _rdseed64(unsigned long long *p) {
            unsigned int *up = (unsigned int *)p;
            return _rdseed32(up) && _rdseed32(up + 1);
        }
    #endif

    inline int rdseed(unsigned short *p)      { return ala::intrin::_rdseed16(p); }
    inline int rdseed(unsigned int *p)        { return ala::intrin::_rdseed32(p); }
    inline int rdseed(unsigned long long *p)  { return ala::intrin::_rdseed64(p); }

} // namespace intrin
} // namespace ala

#endif // X86

#endif // HEAD