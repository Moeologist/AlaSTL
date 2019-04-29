#ifndef _ALA_CONFIG_PLATFORM_H
#define _ALA_CONFIG_PLATFORM_H

#if defined(_MSC_VER) // msvc or clang (msvc target)
#if defined(__clang__) && defined(__clang_major__) && defined(__clang_patchlevel__)

#define _ALA_CLANG
#define _ALA_CLANG_MSVC

#if (__clang_major__ * 100 + __clang_minor * 10 + __clang_patchlevel__) < 350
#error "unsupported compiler; ala needs clang 3.5 at least."
#endif

#else // __clang__
#define _ALA_MSVC

#if _MSC_VER < 1910
#error "unsupported compiler; ala needs visual studio 2017 at least."
#endif

#endif // __clang__

#elif defined(__GNUC__) // gcc or clang (gnu target)
#if defined(__clang__) && defined(__clang_major__) && defined(__clang_patchlevel__)

#define _ALA_CLANG
#define _ALA_CLANG_GNU

#if (__clang_major__ * 100 + __clang_minor * 10 + __clang_patchlevel__) < 350
#error "unsupported compiler; ala needs clang 3.5 at least."
#endif

#else // __clang__

#define _ALA_GCC

#if (__GNUC__ * 1000 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__) < 5000
#error "unsupported compiler; ala needs gcc 5 at least."
#endif

#endif // __clang__

#else // other compiler
#error "unsupported compiler."
#endif // Compilers

#if defined(_M_IX86) || defined(__i386__) || defined(_M_AMD64) || \
    defined(__amd64__)
#define _ALA_X86
#if defined(_M_AMD64) || defined(__amd64__)
#define _ALA_X86_64
#else
#define _ALA_X86_32
#endif
#elif defined(_M_ARM) || defined(__arm__) || defined(_M_ARM64) || \
    defined(__aarch64__)
#define _ALA_ARM
#if defined(_M_ARM64) || defined(__aarch64__)
#define _ALA_ARM64
#else
#define _ALA_ARM32
#endif
#else
// #error "unsupported arch; ala support arm and X86 (both 32bit and 64bit)."
#endif // Architectures

#ifdef _WIN32
#define _ALA_WIN
#elif defined(__linux__)
#define _ALA_LINUX
#elif defined(__APPLE__)
#define _ALA_MAC
#endif

#if defined(_DEBUG) && !defined(NDEBUG)
#define _ALA_DEBUG
#endif

#if !defined(_DEBUG) && defined(NDEBUG)
#define _ALA_RELEASE
#endif

#if !defined(_ALA_DEBUG) && !defined(_ALA_RELEASE)
#define _ALA_DEBUG
#endif

#if __cplusplus >= 201703L
#define _ALA_CPP_STD 17
#elif __cplusplus >= 201402L
#define _ALA_CPP_STD 14
#elif __cplusplus >= 201103L
#define _ALA_CPP_STD 11
#else
#define _ALA_CPP_STD 14
#endif // language standard

#endif // HEAD