#ifndef _ALA_LA_VECTOR_H
#define _ALA_LA_VECTOR_H

#define _ALALIB_USE_ALA
#ifdef _ALALIB_USE_ALA
#include <ala/type_traits.h>
#include <ala/La/external.h>
#else
#include <type_traits>
#include <cmath>
#include <initializer_list>
#endif

namespace ala {
namespace La {

#ifdef _ALALIB_USE_ALA
using ala::size_t;
using ala::common_type_t;
#else
using std::size_t;
using std::common_type_t;
#endif

template<class T, size_t Size>
struct Vector {
    typedef T value_type;
    typedef size_t size_type;
    constexpr static size_t size = Size;
    T _m[Size];

    static_assert(Size >= 2, "Vector size must >= 2");

    template<class Cast>
    constexpr Vector<Cast, Size> cast() noexcept {
        Vector<Cast, Size> tmp;
        for (int i = 0; i < Size; ++i)
            tmp._m[i] = _m[i];
        return tmp;
    }

    constexpr Vector &fill(T value) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] = value;
        return *this;
    }

    constexpr T sum() const noexcept {
        T ret{};
        for (int i = 0; i < Size; ++i)
            ret += _m[i];
        return ret;
    }

    constexpr T prod() const noexcept {
        T sum = 1;
        for (int i = 0; i < Size; ++i)
            sum *= _m[i];
        return;
    }

    constexpr T l1norm() const noexcept {
        T sum{};
        for (int i = 0; i < Size; ++i)
            sum += abs(_m[i]);
        return sum;
    }

    constexpr T l2norm() const noexcept {
        T sum{};
        for (int i = 0; i < Size; ++i)
            sum += _m[i] * _m[i];
        return sqrt(sum);
    }

    constexpr Vector &normlize() noexcept {
        T l2 = l2norm();
        return l2 == 0 ? *this : *this = *this / l2;
    }

    constexpr Vector normlized() const noexcept {
        Vector tmp(*this);
        return tmp.normlize();
    }

    constexpr bool operator==(const Vector &rhs) const noexcept {
        for (int i = 0; i < Size; ++i)
            if (!(_m[i] == rhs._m[i]))
                return false;
        return true;
    }

    constexpr bool operator!=(const Vector &rhs) const noexcept {
        return !(*this == rhs);
    }

    constexpr Vector &operator+=(const Vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] += rhs._m[i];
        return *this;
    }

    constexpr Vector operator+(const Vector &rhs) const noexcept {
        Vector tmp(*this);
        return tmp += rhs;
    }

    constexpr Vector &operator+=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] += rhs;
        return *this;
    }

    constexpr Vector operator+(const T &rhs) const noexcept {
        Vector tmp(*this);
        return tmp += rhs;
    }

    constexpr Vector &operator-=(const Vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] -= rhs._m[i];
        return *this;
    }

    constexpr Vector operator-(const Vector &rhs) const noexcept {
        Vector tmp(*this);
        return tmp -= rhs;
    }

    constexpr Vector &operator-=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] -= rhs;
        return *this;
    }

    constexpr Vector operator-(const T &rhs) const noexcept {
        Vector tmp(*this);
        return tmp -= rhs;
    }

    constexpr Vector &operator*=(const Vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] *= rhs._m[i];
        return *this;
    }

    constexpr Vector operator*(const Vector &rhs) const noexcept {
        Vector tmp(*this);
        return tmp *= rhs;
    }

    constexpr Vector &operator*=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] *= rhs;
        return *this;
    }

    constexpr Vector const operator*(const T &rhs) const noexcept {
        Vector tmp(*this);
        return tmp *= rhs;
    }

    constexpr Vector &operator/=(const Vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] /= rhs._m[i];
        return *this;
    }

    constexpr Vector operator/(const Vector &rhs) const noexcept {
        Vector tmp(*this);
        return tmp /= rhs;
    }

    constexpr Vector &operator/=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] /= rhs;
        return *this;
    }

    constexpr Vector operator/(const T &rhs) const noexcept {
        Vector tmp(*this);
        return tmp /= rhs;
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
    constexpr T *const data() const noexcept {
        return _m;
    }
};

template<class T, size_t Size, class U>
constexpr Vector<T, Size> operator+(const U &lhs, const Vector<T, Size> &rhs) {
    return rhs + lhs;
}

template<class T, size_t Size, class U>
constexpr Vector<T, Size> operator-(const U &lhs, const Vector<T, Size> &rhs) {
    Vector<T, Size> tmp{};
    return tmp.fill(lhs) -= rhs;
}

template<class T, size_t Size, class U>
constexpr Vector<T, Size> operator*(const U &lhs, const Vector<T, Size> &rhs) {
    return rhs * lhs;
}

template<class T, size_t Size, class U>
constexpr Vector<T, Size> operator/(const U &lhs, const Vector<T, Size> &rhs) {
    Vector<T, Size> tmp{};
    return tmp.fill(lhs) /= rhs;
}

template<class T, size_t Size>
constexpr T dot(const Vector<T, Size> &lhs, const Vector<T, Size> &rhs) {
    return (lhs * rhs).sum();
}

template<class T>
constexpr Vector<T, 3> cross(const Vector<T, 3> &lhs, const Vector<T, 3> &rhs) {
    return Vector<T, 3>{lhs[1] * rhs[2] - lhs[2] * rhs[1],
                        lhs[2] * rhs[0] - lhs[0] * rhs[2],
                        lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

template<class T>
constexpr T cross(const Vector<T, 2> &lhs, const Vector<T, 2> &rhs) {
    return abs(lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<typename... Ts>
Vector(Ts...)->Vector<common_type_t<Ts...>, sizeof...(Ts)>;
#endif

} // namespace La
} // namespace ala

#endif // HEAD