#ifndef _ALA_DETAIL_MACRO_H
#define _ALA_DETAIL_MACRO_H

#define ALA_HAS_MEM(mem) \
    template<typename _type, typename = ala::void_t<>> \
    struct _has_##mem: ala::false_type {}; \
    template<typename _type> \
    struct _has_##mem<_type, ala::void_t<typename _type::mem>>: ala::true_type {};

#define ALA_HAS_MEM_VAL(memval) \
    template<typename _type, typename = ala::void_t<>> \
    struct _has_##memval: ala::false_type {}; \
    template<typename _type> \
    struct _has_##memval<_type, ala::void_t<decltype(&ala::declval<_type>().memval)>>: ala::true_type {};

#define ALA_HAS_MEM_TYPEDEF(cls, mem, def) \
    template<typename _type, typename = ala::void_t<>> \
    struct _get_##mem { \
        typedef def type; \
    }; \
    template<typename _type> \
    struct _get_##mem<_type, ala::void_t<typename _type::mem>> { \
        typedef typename _type::mem type; \
    }; \
    typedef typename _get_##mem<cls>::type mem;

#endif