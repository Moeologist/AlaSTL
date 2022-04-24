#ifndef _ALA_CONFIG_SUPPORT_H
#define _ALA_CONFIG_SUPPORT_H

#if defined(__is_identifier)
    #define ALA_IS_IDENTIFIER(x) __is_identifier(x)
#else
    #define ALA_IS_IDENTIFIER(x) 0
#endif

#if defined(__has_builtin)
    #define ALA_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define ALA_HAS_BUILTIN(x) 0
#endif

#if defined(__has_cpp_attribute)
    #define ALA_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define ALA_HAS_CPP_ATTRIBUTE(x) 0
#endif

#if !defined(_ALA_MSVC)
    #if !defined(__has_cpp_attribute) || !defined(__cpp_lambdas)
        #warning Unsupported compiler; ala need cpp feature test macro in non-MSVC.
    #endif
#endif

#if defined(_ALA_CLANG) && ALA_HAS_BUILTIN(__make_integer_seq) || defined(_ALA_MSVC)
    #define _ALA_ENABLE_MAKE_INTEGER_SEQ 1
#else
    #define _ALA_ENABLE_MAKE_INTEGER_SEQ 0
#endif

#if defined(_ALA_CLANG) && ALA_HAS_BUILTIN(__type_pack_element)
    #define _ALA_ENABLE_TYPE_PACK_ELEMENT 1
#else
    #define _ALA_ENABLE_TYPE_PACK_ELEMENT 0
#endif

#if ALA_HAS_BUILTIN(__builtin_bit_cast) || (defined(_ALA_MSVC) && _MSC_VER >= 1927)
    #define _ALA_ENABLE_BUILTIN_BIT_CAST 1
#else
    #define _ALA_ENABLE_BUILTIN_BIT_CAST 0
#endif

#if ALA_HAS_CPP_ATTRIBUTE(nodiscard) || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_LANG >= 201703L)
    #define ALA_NODISCARD [[nodiscard]]
#else
    #define ALA_NODISCARD
#endif

#if __cpp_aligned_new >= 201606L
    #define _ALA_ENABLE_ALIGNED_NEW 1
#else
    #define _ALA_ENABLE_ALIGNED_NEW 0
#endif

#if __cpp_deduction_guides >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1914 && _ALA_LANG >= 201703L)
    #define _ALA_ENABLE_DEDUCTION_GUIDES 1
#else
    #define _ALA_ENABLE_DEDUCTION_GUIDES 0
#endif

#if __cpp_if_constexpr >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_LANG >= 201703L)
    #define _ALA_ENABLE_IF_CONSTEXPR 1
#else
    #define _ALA_ENABLE_IF_CONSTEXPR 0
#endif

#if __cpp_inline_variables >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && _ALA_LANG >= 201703L)
    #define _ALA_ENABLE_INLINE_VAR 1
#else
    #define _ALA_ENABLE_INLINE_VAR 0
#endif

#if __cpp_noexcept_function_type >= 201510L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && _ALA_LANG >= 201703L)
    #define _ALA_ENABLE_NOEXCEPT_TYPE 1
#else
    #define _ALA_ENABLE_NOEXCEPT_TYPE 0
#endif

#if __cpp_structured_bindings >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_LANG >= 201703L)
    #define _ALA_ENABLE_STRUCT_BIND 1
#else
    #define _ALA_ENABLE_STRUCT_BIND 0
#endif

#if __cpp_variable_templates >= 201304L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1910 && _ALA_LANG >= 201103L)
    #define _ALA_ENABLE_TEMPLATE_VAR 1
#else
    #define _ALA_ENABLE_TEMPLATE_VAR 0
#endif

#if __cpp_fold_expressions >= 201603L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && _ALA_LANG >= 201703L)
    #define _ALA_ENABLE_FOLD_EXPRESSIONS 1
#else
    #define _ALA_ENABLE_FOLD_EXPRESSIONS 0
#endif

#if __cpp_constexpr >= 201907L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1930 && _ALA_LANG >= 202002L)
    #define _ALA_CONSTEXPR 20  // no compiler implement all C++20 constexpr features
#elif __cpp_constexpr >= 201603L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_LANG >= 201703L)
    #define _ALA_CONSTEXPR 17 // constexpr lambda
#elif __cpp_constexpr >= 201304L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1910 && _ALA_LANG >= 201103L)
    #define _ALA_CONSTEXPR 14
#else
    #define _ALA_CONSTEXPR 11
#endif

#if __cpp_char8_t >= 201811L
    #define _ALA_ENABLE_CHAR8T 1
#else
    #define _ALA_ENABLE_CHAR8T 0
#endif

#ifdef __SIZEOF_INT128__
    #define _ALA_ENABLE_INT128T 1
#else
    #define _ALA_ENABLE_INT128T 0
#endif

#if !defined(ALA_EXPECT)
    #if ALA_HAS_BUILTIN(__builtin_expect)
        #define ALA_EXPECT(x) (__builtin_expect(!!(x), 1))
    #else
        #define ALA_EXPECT(x) (x)
    #endif
#endif

#ifndef ALA_LANG
    #ifdef _MSVC_LANG
        #define ALA_LANG _MSVC_LANG
    #else
        #define ALA_LANG __cplusplus
    #endif
#endif

#if defined(_ALA_MSVC) || defined(_ALA_CLANG_MSVC)
    #define ALA_INLINE __inline
    #define ALA_FORCEINLINE __forceinline
    #define ALA_NOINLINE __declspec(noinline)
#else
    #define ALA_INLINE __inline
    #define ALA_FORCEINLINE __attribute__((always_inline))
    #define ALA_NOINLINE __attribute__((noinline))
#endif

#ifndef ALA_INSERTION_THRESHOLD
    #define ALA_INSERTION_THRESHOLD 28
#endif

#ifndef ALA_USE_ALLOC_REBIND
    #define ALA_USE_ALLOC_REBIND 0
#endif

#ifndef ALA_TEMPLATE_RECURSIVE_DEPTH
    #define ALA_TEMPLATE_RECURSIVE_DEPTH 512
#endif

#endif // HEAD