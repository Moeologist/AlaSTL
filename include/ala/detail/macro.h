#ifndef _ALA_DETAIL_MACRO_H
#define _ALA_DETAIL_MACRO_H

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

template<class...>
struct Fucker;

template<auto...>
struct FuckerV;

#define CONCAT1(x, y) x##y
#define CONCAT2(x, y) CONCAT1(x, y)
#define MAKE_COUNTER(x) CONCAT2(x, __COUNTER__)

#define FUCK(...) Fucker<decltype(__VA_ARGS__)> MAKE_COUNTER(fuck)
#define FUCKT(...) Fucker<__VA_ARGS__> MAKE_COUNTER(fuck)
#define FUCKV(...) FuckerV<__VA_ARGS__> MAKE_COUNTER(fuck)

#endif