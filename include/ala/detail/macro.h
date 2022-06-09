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
        using type = def; \
    }; \
    template<typename _type_> \
    struct _get_##mem<_type_, ala::void_t<typename _type_::mem>> { \
        using type = typename _type_::mem; \
    }; \
    using mem = typename _get_##mem<cls>::type;

namespace ala {

template<class...>
struct TypeCheck;

template<auto...>
struct ValueCheck;

} // namespace ala

#define ALA_STRINGIFY0(x) #x
#define ALA_STRINGIFY(x) ALA_STRINGIFY0(x)
#define ALA_CONCAT0(x, y) x##y
#define ALA_CONCAT(x, y) ALA_CONCAT0(x, y)

#define ALA_COUNTER_ID ALA_CONCAT(_ala_id_, __COUNTER__)

#define ALA_TYPE(...) ala::TypeCheck<__VA_ARGS__> ALA_COUNTER_ID
#define ALA_VALUE(...) ala::ValueCheck<__VA_ARGS__> ALA_COUNTER_ID
#define ALA_TYPEOF(...) ALA_TYPE(decltype(__VA_ARGS__))

#endif