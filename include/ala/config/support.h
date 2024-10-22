#ifndef _ALA_CONFIG_SUPPORT_H
#define _ALA_CONFIG_SUPPORT_H

#ifndef ALA_LANG
    #ifdef _MSVC_LANG
        #define ALA_LANG _MSVC_LANG
    #else
        #define ALA_LANG __cplusplus
    #endif
#endif

#ifdef ALA_API_VER
    #if ALA_API_VER != 17 && ALA_API_VER != 20
        #error ala only support c++17 stdlib version
    #endif
#else
    #if ALA_LANG > 201703L
        #define ALA_API_VER 20
    #else
        #define ALA_API_VER 17
    #endif
#endif

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

#if ALA_HAS_BUILTIN(__builtin_is_constant_evaluated) || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1925)
    #define _ALA_ENABLE_BUILTIN_IS_CONSTANT_EVALUATED 1
#endif

#if __cpp_aligned_new >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_ALIGNED_NEW 1
#endif

// full support in _MSC_VER=1930
#if __cpp_concepts >= 201907L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1923 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_CONCEPTS 1
#endif

#if __cpp_deduction_guides >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1914 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_DEDUCTION_GUIDES 1
#endif

#if __cpp_if_constexpr >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_IF_CONSTEXPR 1
#endif

#if (__cpp_inline_variables >= 201606L || \
     (defined(_ALA_MSVC) && _MSC_VER >= 1912)) && \
    ALA_LANG >= 201703L
    #define _ALA_ENABLE_INLINE_VAR 1
#endif

#if __cpp_noexcept_function_type >= 201510L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_NOEXCEPT_TYPE 1
#endif

#if __cpp_structured_bindings >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_STRUCT_BIND 1
#endif

#if __cpp_variable_templates >= 201304L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1910 && ALA_LANG >= 201103L)
    #define _ALA_ENABLE_TEMPLATE_VAR 1
#endif

#if __cpp_fold_expressions >= 201603L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && ALA_LANG >= 201703L)
    #define _ALA_ENABLE_FOLD_EXPRESSIONS 1
#endif

#if __cpp_char8_t >= 201811L
    #define _ALA_ENABLE_CHAR8T 1
#endif

#ifdef __SIZEOF_INT128__
    #define _ALA_ENABLE_INT128T 1
#endif

#if __cpp_constexpr >= 201907L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1929 && ALA_LANG >= 202002L)
    #define _ALA_CONSTEXPR_VER 20
    // no compiler implement all C++20 constexpr features
    #define ALA_CONSTEXPR20 constexpr
    #define ALA_CONSTEXPR17 constexpr
#elif __cpp_constexpr >= 201603L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && ALA_LANG >= 201703L)
    #define _ALA_CONSTEXPR_VER 17 // constexpr lambda
    #define ALA_CONSTEXPR20
    #define ALA_CONSTEXPR17 constexpr
#elif __cpp_constexpr >= 201304L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1910 && ALA_LANG >= 201103L)
    #define _ALA_CONSTEXPR_VER 14
    #define ALA_CONSTEXPR20
    #define ALA_CONSTEXPR17
#else
    #define _ALA_CONSTEXPR_VER 11
    #define ALA_CONSTEXPR20
    #define ALA_CONSTEXPR17
#endif

#if ALA_HAS_CPP_ATTRIBUTE(nodiscard) || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && ALA_LANG >= 202002L)
    #define ALA_NODISCARD [[nodiscard]]
#else
    #define ALA_NODISCARD
#endif

#if !defined(ALA_EXPECT)
    #if ALA_HAS_BUILTIN(__builtin_expect)
        #define ALA_EXPECT(x) __builtin_expect(!!(x), 1)
        #define ALA_UNEXPECT(x) __builtin_expect(!!(x), 0)
    #else
        #define ALA_EXPECT(x) (x)
        #define ALA_UNEXPECT(x) (x)
    #endif
#endif

#if !defined(ALA_UNREACHABLE)
    #if ALA_HAS_BUILTIN(__builtin_unreachable)
        #define ALA_UNREACHABLE() __builtin_unreachable()
    #else
        #define ALA_UNREACHABLE()
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

// configurable
#ifndef ALA_INSERTION_THRESHOLD
    #define ALA_INSERTION_THRESHOLD 20
#endif

#ifndef ALA_NINTHER_THRESHOLD
    #define ALA_NINTHER_THRESHOLD 128
#endif

#ifndef ALA_INSERTION_LIMIT
    #define ALA_INSERTION_LIMIT 8
#endif

#ifndef ALA_USE_ALLOC_REBIND
    #define ALA_USE_ALLOC_REBIND 0
#endif

#ifndef ALA_TEMPLATE_RECURSIVE_DEPTH
    #define ALA_TEMPLATE_RECURSIVE_DEPTH 512
#endif

#if (__cpp_inline_variables >= 201606L || \
     (defined(_ALA_MSVC) && _MSC_VER >= 1912)) && \
    ALA_LANG >= 201703L
    #define ALA_INLINE_CONSTEXPR_V inline constexpr
#else
    #define ALA_INLINE_CONSTEXPR_V constexpr
#endif

#define ALA_CONCEPT ALA_INLINE_CONSTEXPR_V bool

#if defined(__cpp_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI)
    #define ALA_ENABLE_RTTI 1
#else
    #define ALA_ENABLE_RTTI 0
#endif

#ifndef ALA_USE_RTTI
    #ifdef ALA_ENABLE_RTTI
        #define ALA_USE_RTTI 1
    #else
        #define ALA_USE_RTTI 0
    #endif
#else
    #if ALA_USE_RTTI && !defined(ALA_ENABLE_RTTI)
        #warning your compiler or cflags not support rtti
        #define ALA_USE_RTTI 0
    #endif
#endif

#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
    #define ALA_ENABLE_EXCEPTION 1
#else
    #define ALA_ENABLE_EXCEPTION 0
#endif

#ifndef ALA_USE_EXCEPTION
    #ifdef ALA_ENABLE_EXCEPTION
        #define ALA_USE_EXCEPTION 1
    #else
        #define ALA_USE_EXCEPTION 0
    #endif
#else
    #if ALA_USE_EXCEPTION && !defined(ALA_ENABLE_EXCEPTION)
        #warning your compiler or cflags not support exception
        #define ALA_USE_EXCEPTION 0
    #endif
#endif

#ifndef ALA_USE_STD_ITER_TAG
    #define ALA_USE_STD_ITER_TAG 0
#endif

#ifndef ALA_USE_CONCEPTS
    #ifdef _ALA_ENABLE_CONCEPTS
        #define ALA_USE_CONCEPTS 1
    #else
        #define ALA_USE_CONCEPTS 0
    #endif
#else
    #if ALA_USE_CONCEPTS && !defined(_ALA_ENABLE_CONCEPTS)
        #warning your compiler or cflags not support concepts
        #define ALA_USE_CONCEPTS 0
    #endif
#endif

#ifndef ALA_USE_IDENTITY_FOR_INTEGRAL
    #define ALA_USE_IDENTITY_FOR_INTEGRAL 0
#endif

#if defined(_LIBCPP_VERSION)
    #define ALA_BEGIN_NAMESPACE_STD _LIBCPP_BEGIN_NAMESPACE_STD
    #define ALA_END_NAMESPACE_STD _LIBCPP_END_NAMESPACE_STD
#elif defined(_LIBCPP_ABI_NAMESPACE)
    #define ALA_BEGIN_NAMESPACE_STD \
        namespace std { \
        inline namespace _LIBCPP_ABI_NAMESPACE {
    #define ALA_END_NAMESPACE_STD \
        } \
        }
#elif defined(_MSVC_STL_VERSION)
    #define ALA_BEGIN_NAMESPACE_STD _STD_BEGIN
    #define ALA_END_NAMESPACE_STD _STD_END
#else
    #define ALA_BEGIN_NAMESPACE_STD namespace std {
    #define ALA_END_NAMESPACE_STD }
#endif

#endif // HEAD