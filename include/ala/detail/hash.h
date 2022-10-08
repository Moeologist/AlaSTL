#ifndef _ALA_DETAIL_HASH_H
#define _ALA_DETAIL_HASH_H

#include <ala/detail/memory_base.h>
#include <ala/detail/farmhash.h>
#include <ala/detail/city.h>

ALA_BEGIN_NAMESPACE_STD

template<class>
struct hash;

ALA_END_NAMESPACE_STD

namespace ala {

template<class T>
struct hash;

struct _disabled_hash {
    constexpr _disabled_hash() = delete;
    constexpr _disabled_hash(const _disabled_hash &) = delete;
    constexpr _disabled_hash(_disabled_hash &&) = delete;
    constexpr _disabled_hash &operator=(const _disabled_hash &) = delete;
    constexpr _disabled_hash &operator=(_disabled_hash &&) = delete;
};

template<class T, bool = is_invocable_r<size_t, std::hash<T>, T>::value>
struct _std_hash: std::hash<T> {};

template<class T>
struct _std_hash<T, false>: _disabled_hash {};

template<class T, bool = is_enum<T>::value>
struct _enum_hash {
    using argument_type = T;
    using result_type = size_t;
    constexpr size_t operator()(T v) const noexcept {
        return hash<underlying_type_t<T>>{}(static_cast<underlying_type_t<T>>(v));
    }
};

template<class T>
struct _enum_hash<T, false>: _std_hash<T> {};

template<class T>
struct _farm_hash {
    using argument_type = T;
    using result_type = size_t;

    size_t _call(true_type, const char *p) const noexcept {
        return farmhash::Hash64(p, sizeof(T));
    }

    size_t _call(false_type, const char *p) const noexcept {
        return farmhash::Hash32(p, sizeof(T));
    }

    size_t operator()(T v) const noexcept {
        const char *p = reinterpret_cast<const char *>(ala::addressof(v));
        return _farm_hash::_call(bool_constant<sizeof(size_t) == 8>{}, p);
    }
};

template<class T>
struct _city_hash {
    using argument_type = T;
    using result_type = size_t;

    size_t _call(true_type, const char *p) const noexcept {
        return cityhash::CityHash64(p, sizeof(T));
    }

    size_t _call(false_type, const char *p) const noexcept {
        return cityhash::CityHash32(p, sizeof(T));
    }

    size_t operator()(T v) const noexcept {
        const char *p = reinterpret_cast<const char *>(ala::addressof(v));
        return _city_hash::_call(bool_constant<sizeof(size_t) == 8>{}, p);
    }
};

template<class T, bool = (sizeof(T) > sizeof(size_t))>
struct _identity_hash {
    using argument_type = T;
    using result_type = size_t;
    constexpr size_t operator()(T v) const noexcept {
        return static_cast<size_t>(v);
    }
};

template<class T>
struct _identity_hash<T, true>: _farm_hash<T> {};

template<class T>
struct _float_hash {
    using argument_type = T;
    using result_type = size_t;
    constexpr size_t operator()(T v) const noexcept {
        if (v == static_cast<T>(0.))
            return _farm_hash<T>{}(0.);
        return _farm_hash<T>{}(v);
    }
};

// clang-format off

template<class T> struct hash: _enum_hash<T> {};

#if ALA_USE_IDENTITY_FOR_INTEGRAL
template<class T> using _integral_hash = _identity_hash<T>;
#else
template<class T> using _integral_hash = _farm_hash<T>;
#endif

template<> struct hash<bool>:           _integral_hash<bool> {};
template<> struct hash<char>:           _integral_hash<char> {};
template<> struct hash<signed char>:    _integral_hash<signed char> {};
template<> struct hash<unsigned char>:  _integral_hash<unsigned char> {};
template<> struct hash<char16_t>:       _integral_hash<char16_t> {};
template<> struct hash<char32_t>:       _integral_hash<char32_t> {};
template<> struct hash<wchar_t>:        _integral_hash<wchar_t> {};
template<> struct hash<short>:          _integral_hash<short> {};
template<> struct hash<unsigned short>: _integral_hash<unsigned short> {};
template<> struct hash<int>:            _integral_hash<int> {};
template<> struct hash<unsigned int>:   _integral_hash<unsigned int> {};
template<> struct hash<long>:           _integral_hash<long> {};
template<> struct hash<unsigned long>:  _integral_hash<unsigned long> {};

template<> struct hash<long long> :         _integral_hash<long long> {};
template<> struct hash<unsigned long long>: _integral_hash<unsigned long long> {};



#if _ALA_ENABLE_CHAR8T
template<> struct hash<char8_t>: _identity_hash<char8_t> {};
#endif

#if _ALA_ENABLE_INT128T
template<> struct hash<__int128_t>:  _farm_hash<__int128_t> {};
template<> struct hash<__uint128_t>: _farm_hash<__uint128_t> {};
#endif

template<> struct hash<float>:       _float_hash<float> {};
template<> struct hash<double>:      _float_hash<double> {};
template<> struct hash<long double>: _float_hash<long double> {};

// clang-format on

template<class T>
struct hash<T *> {
    using argument_type = T *;
    using result_type = size_t;

    constexpr size_t operator()(T *v) const noexcept {
        static_assert(!(sizeof(size_t) < sizeof(T *)), "Unsupported platform");
        return hash<size_t>{}(reinterpret_cast<size_t>(v));
    }
};

template<>
struct hash<nullptr_t> {
    using argument_type = nullptr_t;
    using result_type = size_t;

    constexpr size_t operator()(nullptr_t) const noexcept {
        return 0;
    }
};

template<typename T, typename = void>
struct _is_hashable: false_type {};

template<typename Hashable>
struct _is_hashable<
    Hashable, enable_if_t<is_invocable_r<size_t, hash<Hashable>, Hashable>::value>>
    : true_type {};

} // namespace ala
#endif