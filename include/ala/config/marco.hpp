#ifndef CONFIG_MARCO_HPP
#define CONFIG_MARCO_HPP

#ifndef CONFIG_HPP
#error "Never use this head directly; include config.hpp instead."
#endif

#if defined(_MSC_VER) &&_MSC_VER >= 1900 // msvc or clang msvc target

#if defined(__clang__) && (__clang_major__ == 3 && __clang_minor__ < 4 || __clang_major__ < 3)
#error "Unsupported compiler; ala need c++14 supported compiler at least."
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define ALA_FORCEINLINE __forceinline
#define ALA_NOINLINE __declspec(noinline)

#ifdef _M_AMD64
#else
#endif // Architectures

#elif defined(__GNUC__) // gcc or clang gnu target

#if (!defined(__clang__) && __GNUC__ < 5) || defined(__clang__) && (__clang_major__ == 3 && __clang_minor__ < 4 || __clang_major__ < 3)
#error "Unsupported compiler; ala need c++14 supported compiler at least."
#endif

#define ALA_FORCEINLINE __attribute__((always_inline))
#define ALA_NOINLINE __attribute__((noinline))

#ifdef __x86_64__
#else
#endif // Architectures

#else
#error "Unsupported compiler; ala need c++14 supported compiler at least."

#endif // Compilers

#if __cpluscplus < 201703L
#define ALA_VARIABLE_INLINE
#else
#define ALA_VARIABLE_INLINE inline
#endif // language standard

#endif // MARCO_HPP