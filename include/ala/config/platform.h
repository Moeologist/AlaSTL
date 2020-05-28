#ifndef _ALA_CONFIG_PLATFORM_H
#define _ALA_CONFIG_PLATFORM_H

#if defined(_MSC_VER) // msvc or clang (msvc target)
    #if defined(__clang__)

        #define _ALA_CLANG
        #define _ALA_CLANG_MSVC

        #ifdef __clang_patchlevel__
            #define _ALA_CLANG_VER \
                (__clang_major__ * 10000 + __clang_minor * 100 + \
                 __clang_patchlevel__)
        #else
            #define _ALA_CLANG_VER \
                (__clang_major__ * 10000 + __clang_minor * 100)
        #endif

        #if _ALA_CLANG_VER < 30500
            #error Unsupported compiler; ala needs clang 3.5 at least
        #endif

    #else // __clang__
        #define _ALA_MSVC

        #if _MSC_VER < 1910
            #error Unsupported compiler; ala needs visual studio 2017 at least
        #endif

    #endif // __clang__

#elif defined(__GNUC__) // gcc or clang (gnu target)
    #if defined(__clang__)

        #define _ALA_CLANG
        #define _ALA_CLANG_GNU

        #ifdef __clang_patchlevel__
            #define _ALA_CLANG_VER \
                (__clang_major__ * 10000 + __clang_minor * 100 + \
                 __clang_patchlevel__)
        #else
            #define _ALA_CLANG_VER \
                (__clang_major__ * 10000 + __clang_minor * 100)
        #endif

        #if _ALA_CLANG_VER < 30500
            #error Unsupported compiler; ala needs clang 3.5 at least
        #endif

    #else // __clang__

        #define _ALA_GCC

        #ifdef __GNUC_PATCHLEVEL__
            #define _ALA_GCC_VER \
                (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
        #else
            #define _ALA_GCC_VER (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
        #endif

        #if _ALA_GCC_VER < 50000
            #error Unsupported compiler; ala needs gcc 5 at least
        #endif

    #endif // __clang__

#else // other compiler
    #error Unsupported compiler.
#endif // Compilers

#if defined(_M_IX86) || defined(__i386__) || defined(_M_AMD64) || \
    defined(__amd64__)
    #define _ALA_X86
    #if defined(_M_AMD64) || defined(__amd64__)
        #define _ALA_X64
    #endif
#elif defined(_M_ARM) || defined(__arm__) || defined(_M_ARM64) || \
    defined(__aarch64__)
    #define _ALA_ARM
    #if defined(_M_ARM64) || defined(__aarch64__)
        #define _ALA_ARM64
    #endif
#else
    #warning Unsupported architecture, some features are disabled
#endif // Architectures

#ifdef _WIN32
    #define _ALA_WIN32
#elif defined(__linux__)
    #define _ALA_LINUX
#elif defined(__APPLE__)
    #define _ALA_APPLE
#endif

#if defined(__unix__) || defined(__unix)
    #define _ALA_UNIX
#endif

#ifdef _MSVC_LANG
    #define _ALA_LANG _MSVC_LANG
#else
    #define _ALA_LANG __cplusplus
#endif

#ifdef __SIZEOF_INT128__
    #define _ALA_INT128
#endif

#endif // HEAD