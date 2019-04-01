#ifndef _ALA_DETAIL_MACRO_H
#define _ALA_DETAIL_MACRO_H

#define ALA_HAS_MEM(mem) \
    template<typename _type, typename = void_t<>> \
    struct _has_##mem: false_type {}; \
    template<typename _type> \
    struct _has_##mem<_type, void_t<typename _type::mem>>: true_type {};

#define ALA_HAS_MEM_VAL(memval) \
    template<typename _type, typename = void_t<>> \
    struct _has_##memval: false_type {}; \
    template<typename _type> \
    struct _has_##memval<_type, void_t<decltype(declval<_type>().memval)>>: true_type {};

#define ALA_HAS_MEM_TYPEDEF(cls, mem, def) \
    template<typename _type, typename = void_t<>> \
    struct _get_##mem { \
        typedef def type; \
    }; \
    template<typename _type> \
    struct _get_##mem<_type, void_t<typename _type::mem>> { \
        typedef typename _type::mem type; \
    }; \
    typedef typename _get_##mem<cls>::type mem;

#endif