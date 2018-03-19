#ifndef _ALA_CONFIG_STDDEF_H
#define _ALA_CONFIG_STDDEF_H

namespace ala {

typedef decltype(nullptr) nullptr_t;

#if defined(_ALA_MSVC) || defined(_ALA_CLANG_MSVC)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

#ifdef _ALA_X64
typedef unsigned __int64 size_t;
typedef __int64 ptrdiff_t;
typedef __int64 intptr_t;

#else
typedef unsigned int size_t;
typedef int ptrdiff_t;
typedef int intptr_t;

#endif // Architectures

#elif defined(_ALA_GCC) || defined(_ALA_CLANG_GNU)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned uint32_t;
__extension__ typedef long long int64_t;
__extension__ typedef unsigned long long uint64_t;

#ifdef _ALA_X64
__extension__ typedef unsigned long long size_t;
typedef long long ptrdiff_t;
typedef long long intptr_t;

#else
typedef unsigned int size_t;
typedef int ptrdiff_t;
typedef int intptr_t;

#endif // Architectures

#endif // Conpilers

} // namespace ala

#endif // HEAD