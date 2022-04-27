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

#elif defined(__GNUC__) && !defined(__apple_build_version__) // gcc or clang (gnu target)
    #if defined(__clang__) && !defined(__apple_build_version__)

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

#elif defined(__clang__) && defined(__apple_build_version__) // AppleClang
    #define _ALA_CLANG
    #define _ALA_CLANG_APPLE

    #ifdef __clang_patchlevel__
        #define _ALA_APPLE_CLANG_VER \
            (__clang_major__ * 10000 + __clang_minor * 100 + \
             __clang_patchlevel__)
    #else
        #define _ALA_APPLE_CLANG_VER \
            (__clang_major__ * 10000 + __clang_minor * 100)
    #endif

    // https://en.wikipedia.org/wiki/Xcode#Version_comparison_table
    #if 0
    #elif _ALA_APPLE_CLANG_VER >= 130106
        #define _ALA_CLANG_VER 130000
    #elif _ALA_APPLE_CLANG_VER >= 130000
        #define _ALA_CLANG_VER 120000
    #elif _ALA_APPLE_CLANG_VER >= 120005
        #define _ALA_CLANG_VER 110100
    #elif _ALA_APPLE_CLANG_VER >= 120000
        #define _ALA_CLANG_VER 100000
    #elif _ALA_APPLE_CLANG_VER >= 110003
        #define _ALA_CLANG_VER 90000
    #elif _ALA_APPLE_CLANG_VER >= 110000
        #define _ALA_CLANG_VER 80000
    #elif _ALA_APPLE_CLANG_VER >= 100001
        #define _ALA_CLANG_VER 70000
    #elif _ALA_APPLE_CLANG_VER >= 100000
        #define _ALA_CLANG_VER 60001
    #elif _ALA_APPLE_CLANG_VER >= 90100
        #define _ALA_CLANG_VER 50002
    #elif _ALA_APPLE_CLANG_VER >= 90000
        #define _ALA_CLANG_VER 40000
    #elif _ALA_APPLE_CLANG_VER >= 80100
        #define _ALA_CLANG_VER 30900
    #elif _ALA_APPLE_CLANG_VER >= 80000
        #define _ALA_CLANG_VER 30900
    #elif _ALA_APPLE_CLANG_VER >= 70300
        #define _ALA_CLANG_VER 30800
    #elif _ALA_APPLE_CLANG_VER >= 70002
        #define _ALA_CLANG_VER 30700
    #elif _ALA_APPLE_CLANG_VER >= 70000
        #define _ALA_CLANG_VER 30700
    #elif _ALA_APPLE_CLANG_VER >= 60100
        #define _ALA_CLANG_VER 30600
    #elif _ALA_APPLE_CLANG_VER >= 60000
        #define _ALA_CLANG_VER 30500
    #elif _ALA_APPLE_CLANG_VER >= 50100
        #define _ALA_CLANG_VER 30400
    #elif _ALA_APPLE_CLANG_VER >= 50000
        #define _ALA_CLANG_VER 30300
    #elif _ALA_APPLE_CLANG_VER >= 40200
        #define _ALA_CLANG_VER 30200
    #elif _ALA_APPLE_CLANG_VER >= 40100
        #define _ALA_CLANG_VER 30100
    #elif _ALA_APPLE_CLANG_VER >= 40000
        #define _ALA_CLANG_VER 30100
    #elif _ALA_APPLE_CLANG_VER >= 30100
        #define _ALA_CLANG_VER 30100
    #elif _ALA_APPLE_CLANG_VER >= 30000
        #define _ALA_CLANG_VER 30000
    #elif _ALA_APPLE_CLANG_VER >= 20100
        #define _ALA_CLANG_VER 30000
    #elif _ALA_APPLE_CLANG_VER >= 20000
        #define _ALA_CLANG_VER 20900
    #elif _ALA_APPLE_CLANG_VER >= 10700
        #define _ALA_CLANG_VER 20900
    #endif

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

#if defined(__clang__) && defined(__c2__)
    #define _ALA_CLANG_C2
#endif

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

#endif // HEAD