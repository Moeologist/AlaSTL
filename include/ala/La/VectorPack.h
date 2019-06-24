#ifndef _ALA_LA_VECTOR_H
#define _ALA_LA_VECTOR_H

#include <ala/La/external.h>
#include <ala/type_traits.h>
#include <ala/utility.h>

#ifdef _ALA_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
#endif

namespace ala {
namespace La {

using ala::size_t;
using ala::common_type_t;
using ala::type_identity_t;
using ala::enable_if_t;
using ala::index_sequence;
using ala::make_index_sequence;

template<class T>
constexpr T _static_sqrt(T x) {
    T xhalf = T(0.5) * x;
    x = xhalf;
    for (int i = 0; i < 8; ++i)
        x = x * (T(1.5) - xhalf * x * x);
    return (T(1 / x);
}

template<class T>
struct _Dummy {
    T _v;
    _Dummy(const T &t):_v(t) {}
};

template<class T>
constexpr auto static_sqrt(T x) -> decltype(_static_sqrt(x)) {
    return _static_sqrt(x);
}

template<class T>
constexpr auto static_sqrt(_Dummy<T> x) -> decltype(::sqrt(x)) {
    return ::sqrt(x._v);
}

template<class T, size_t Size>
struct Vector;

template<class T, class Index>
struct VectorBase;

template<class T, size_t... Is>
struct VectorBase<T, index_sequence<Is...>> {
    typedef T value_type;
    typedef size_t size_type;
    typedef index_sequence<Is...> index_type;

    constexpr static size_t Size = sizeof...(Is);

    T _m[Size] = {};

    using impl = Vector<T, sizeof...(Is)>;
    constexpr operator impl() {
        return *this;
    }

    template<class U>
    constexpr VectorBase<U, index_type> to_type() noexcept {
        return {U(_m[Is])...};
    }

    constexpr VectorBase &fill(const T &value) noexcept {
        return *this = VectorBase{(Is, value)...};
    }

    constexpr VectorBase &clamp(const T &lo, const T &hi) noexcept {
        return *this = VectorBase{
                   (_m[Is] < lo ? lo : (hi < _m[Is] ? hi : _m[Is]))...};
    }

    constexpr T max() const noexcept {
        T res = _m[0];
        auto tmp = {(res = _m[Is] > res ? _m[Is] : res)...};
        return res;
    }

    constexpr T min() const noexcept {
        T res = _m[0];
        auto tmp = {(res = _m[Is] < res ? _m[Is] : res)...};
        return res;
    }

    constexpr T sum() const noexcept {
        T res = 0;
        auto tmp = {(res += _m[Is])...};
        return res;
    }

    constexpr T prod() const noexcept {
        T res = 1;
        auto tmp = {(res *= _m[Is])...};
        return res;
    }

    constexpr T l1norm() const noexcept {
        return abs().sum();
    }

    constexpr T l2norm() const noexcept {
        return ala::La::sqrt((*this * *this).sum());
    }

    constexpr VectorBase abs() const noexcept {
        return {ala::La::abs(_m[Is])...};
    }

    constexpr VectorBase sqrt() const noexcept {
        return {ala::La::sqrt(_m[Is])...};
    }

    constexpr VectorBase &normlize() noexcept {
        T l2 = l2norm();
        return l2 == 0 ? *this : *this = *this / l2;
    }

    constexpr VectorBase normlized() const noexcept {
        VectorBase tmp(*this);
        return tmp.normlize();
    }

    constexpr VectorBase &operator=(const VectorBase &rhs) noexcept {
        auto tmp = {(_m[Is] = rhs._m[Is])...};
        return *this;
    }

    constexpr bool operator==(const VectorBase &rhs) const noexcept {
        for (int i = 0; i < Size; ++i)
            if (!(_m[i] == rhs._m[i]))
                return false;
        return true;
    }

    constexpr bool operator!=(const VectorBase &rhs) const noexcept {
        return !(*this == rhs);
    }

    constexpr VectorBase &operator+=(const VectorBase &rhs) noexcept {
        auto tmp = {(_m[Is] += rhs._m[Is])...};
        return *this;
    }

    constexpr VectorBase &operator+=(const T &rhs) noexcept {
        return *this += VectorBase{(Is, rhs)...};
    }

    constexpr VectorBase &operator-=(const VectorBase &rhs) noexcept {
        auto tmp = {(_m[Is] -= rhs._m[Is])...};
        return *this;
    }

    constexpr VectorBase &operator-=(const T &rhs) noexcept {
        return *this -= VectorBase{(Is, rhs)...};
    }

    constexpr VectorBase &operator*=(const VectorBase &rhs) noexcept {
        auto tmp = {(_m[Is] *= rhs._m[Is])...};
        return *this;
    }

    constexpr VectorBase &operator*=(const T &rhs) noexcept {
        return *this *= VectorBase{(Is, rhs)...};
    }

    constexpr VectorBase &operator/=(const VectorBase &rhs) noexcept {
        auto tmp = {(_m[Is] /= rhs._m[Is])...};
        return *this;
    }

    constexpr VectorBase &operator/=(const T &rhs) noexcept {
        return *this /= VectorBase{(Is, rhs)...};
    }

    constexpr T dot(const VectorBase &rhs) const noexcept {
        return (*this * rhs).sum();
    }

    template<size_t Dummy = Size, class = enable_if_t<Dummy == 2>>
    constexpr VectorBase cross(const VectorBase &rhs) const noexcept {
        return _m[0] * rhs._m[1] - _m[1] * rhs._m[0];
    }

    template<size_t Dummy = Size, class = void, class = enable_if_t<Dummy == 3>>
    constexpr VectorBase cross(const VectorBase &rhs) const noexcept {
        return {_m[1] * rhs._m[2] - _m[2] * rhs._m[1],
                _m[2] * rhs._m[0] - _m[0] * rhs._m[2],
                _m[0] * rhs._m[1] - _m[1] * rhs._m[0]};
    }

    constexpr T &operator[](size_t index) noexcept {
        return _m[index];
    }

    constexpr const T &operator[](size_t index) const noexcept {
        return _m[index];
    }

    constexpr T *data() noexcept {
        return _m;
    }

    constexpr const T *data() const noexcept {
        return _m;
    }
};

template<class T, class Index>
constexpr VectorBase<T, Index> operator+(const VectorBase<T, Index> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp += rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator+(const VectorBase<T, Index> &lhs,
                                         const type_identity_t<T> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp += rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator+(const type_identity_t<T> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    return rhs + lhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator-(const VectorBase<T, Index> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp -= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator-(const VectorBase<T, Index> &lhs,
                                         const type_identity_t<T> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp -= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator-(const type_identity_t<T> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    VectorBase<T, Index> tmp{};
    return tmp.fill(lhs) -= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator*(const VectorBase<T, Index> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp *= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator*(const VectorBase<T, Index> &lhs,
                                         const type_identity_t<T> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp *= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator*(const type_identity_t<T> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    return rhs * lhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator/(const VectorBase<T, Index> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp /= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator/(const VectorBase<T, Index> &lhs,
                                         const type_identity_t<T> &rhs) {
    VectorBase<T, Index> tmp{lhs};
    return tmp /= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> operator/(const type_identity_t<T> &lhs,
                                         const VectorBase<T, Index> &rhs) {
    VectorBase<T, Index> tmp{};
    return tmp.fill(lhs) /= rhs;
}

template<class T, class Index>
constexpr VectorBase<T, Index> sqrt(const VectorBase<T, Index> &v) {
    return v.sqrt();
}

template<class T, class Index>
constexpr T dot(const VectorBase<T, Index> &lhs, const VectorBase<T, Index> &rhs) {
    return lhs.dot(rhs);
}

template<class T, class Index>
constexpr auto cross(const VectorBase<T, Index> &lhs,
                     const VectorBase<T, Index> &rhs)
    -> decltype(lhs.cross(rhs)) {
    return lhs.cross(rhs);
}

// template<class T, size_t Size>
// using Vector = VectorBase<T, make_index_sequence<Size>>;

template<class T, size_t Size>
struct Vector: VectorBase<T, make_index_sequence<Size>> {};

using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;
using Vector8d = Vector<double, 8>;

using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;
using Vector8f = Vector<float, 8>;

using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;
using Vector8i = Vector<int, 8>;

} // namespace La
} // namespace ala

#ifdef _ALA_CLANG
#pragma clang diagnostic pop
#endif

#endif // HEAD