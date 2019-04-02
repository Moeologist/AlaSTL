#ifndef _ALA_DETAIL_CONFIG_SUPPORT_H
#define _ALA_DETAIL_CONFIG_SUPPORT_H

#if !defined(_ALA_CLANG) && !defined(__is_identifier)
#define __is_identifier(x) 0
#endif

#if defined(_ALA_MSVC)

#if !defined(__has_cpp_attribute)
#define __has_cpp_attribute(x) 0
#endif
#define _ALA_ENABLE_CPP_ATTR_MACRO 0

#if _MSC_VER >= 1915
#define _ALA_ENABLE_CPP_MACRO 1
#else
#define _ALA_ENABLE_CPP_MACRO 0
#endif

#else
#if !defined(__has_cpp_attribute)
#error "unsupported compiler; ala need __has_cpp_attribute in non-MSVC."
#endif
#if !defined(__cpp_lambdas)
#error "unsupported compiler; ala need __cpp_[feature] in non-MSVC."
#endif
#define _ALA_ENABLE_CPP_ATTR_MACRO 1
#define _ALA_ENABLE_CPP_MACRO 1
#endif

#if _ALA_ENABLE_CPP_MACRO && __cpp_deduction_guides >= 201606 || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1914 && _ALA_CPP_STD >= 17)
#define _ALA_ENABLE_DEDUCTION_GUIDES 1
#else
#define _ALA_ENABLE_DEDUCTION_GUIDES 0
#endif

#if _ALA_ENABLE_CPP_MACRO && __cpp_if_constexpr	>= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_CPP_STD >= 17)
#define ALA_CONST_IF if constexpr
#else
#define ALA_CONST_IF if
#endif

// compatibility macro
#if _ALA_ENABLE_CPP_ATTR_MACRO && __has_cpp_attribute(nodiscard) || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_CPP_STD >= 17)
#define ALA_NODISCARD [[nodiscard]]
#else
#define ALA_NODISCARD
#endif

#if _ALA_ENABLE_CPP_MACRO && __cpp_inline_variables >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && _ALA_CPP_STD >= 17)
#define ALA_VAR_INLINE inline
#else
#define ALA_VAR_INLINE
#endif

#if ALA_ENABLE_CPP_MACRO && __cpp_noexcept_function_type >= 201510L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1912 && _ALA_CPP_STD >= 17)
#define _ALA_ENABLE_NOEXCEPT_TYPE 1
#else
#define _ALA_ENABLE_NOEXCEPT_TYPE 0
#endif

#if _ALA_ENABLE_CPP_MACRO && __cpp_structured_bindings >= 201606L || \
    (defined(_ALA_MSVC) && _MSC_VER >= 1911 && _ALA_CPP_STD >= 17)
#define _ALA_ENABLE_STRUCT_BIND 1
#else
#define _ALA_ENABLE_STRUCT_BIND 0
#endif

#ifdef _ALA_DEBUG
#define ALA_FORCEINLINE
#define ALA_NOINLINE

#else // _ALA_DEBUG

#if defined(_ALA_MSVC) || defined(_ALA_CLANG_MSVC)
#define ALA_FORCEINLINE __forceinline
#define ALA_NOINLINE __declspec(noinline)
#else
#define ALA_FORCEINLINE __attribute__((always_inline))
#define ALA_NOINLINE __attribute__((noinline))
#endif

#endif // _ALA_DEBUG

#endif // HEAD