#ifndef _ALA_DETAIL_MACRO_H
#define _ALA_DETAIL_MACRO_H

#ifdef _ALA_MSVC
#define ALA_HAS_MEM(mem) \
    template<typename _type_, typename = void> \
    struct _has_##mem: ala::false_type {}; \
    template<typename _type_> \
    struct _has_##mem<_type_, ala::void_t<decltype(ala::declval<_type_>().mem)>> \
        : ala::true_type {};

#else
#define ALA_HAS_MEM(mem) \
    template<typename _type_, typename = void> \
    struct _has_##mem: ala::false_type {}; \
    template<typename _type_> \
    struct _has_##mem<_type_, ala::void_t<decltype(&_type_::mem)>> \
        : ala::true_type {};

#endif

#define ALA_HAS_MEM_TYPE(mem) \
    template<typename _type_, typename = void> \
    struct _has_##mem: ala::false_type {}; \
    template<typename _type_> \
    struct _has_##mem<_type_, ala::void_t<typename _type_::mem>> \
        : ala::true_type {};

#define ALA_HAS_MEM_TYPEDEF(cls, mem, def) \
    template<typename _type_, typename = void> \
    struct _get_##mem { \
        typedef def type; \
    }; \
    template<typename _type_> \
    struct _get_##mem<_type_, ala::void_t<typename _type_::mem>> { \
        typedef typename _type_::mem type; \
    }; \
    typedef typename _get_##mem<cls>::type mem;

#endif