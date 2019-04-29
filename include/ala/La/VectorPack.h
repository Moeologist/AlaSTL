#ifndef _ALA_LA_VECTOR_H
#define _ALA_LA_VECTOR_H

#include <ala/La/external.h>
#include <ala/type_traits.h>
#include <ala/utility.h>

namespace ala {
namespace La {

using ala::size_t;
using ala::common_type_t;
using ala::type_identity_t;
using ala::index_sequence;
using ala::make_index_sequence;

template<class T, class Index>
struct VectorBase;

template<class T, size_t... Is>
struct VectorBase<T, index_sequence<Is...>> {
    typedef T value_type;
    typedef size_t size_type;
    typedef index_sequence<Is...> index_type;

    constexpr static size_t Size = sizeof...(Is);

    T _m[Size] = {};

    template<class... Args>
    void __(Args...) const {}

    template<class U>
    constexpr VectorBase<U, index_type> to_type() noexcept {
        return {U(_m[Is])...};
    }

    constexpr VectorBase &fill(T value) noexcept {
        __((_m[Is] = value)...);
        return *this;
    }

    constexpr T sum() const noexcept {
        T res = 0;
        __((res += _m[Is])...);
        return res;
    }

    constexpr T prod() const noexcept {
        T res = 1;
        __((res *= _m[Is])...);
        return;
    }

    constexpr T l1norm() const noexcept {
        T res = 0;
        __((res += ala::La::abs(_m[Is]))...);
        return res;
    }

    constexpr T l2norm() const noexcept {
        T res = 0;
        __((res += _m[Is] * _m[Is])...);
        return ala::La::sqrt(res);
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
        __((_m[Is] = rhs._m[Is])...);
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
        __((_m[Is] += rhs._m[Is])...);
        return *this;
    }

    constexpr VectorBase &operator+=(const T &rhs) noexcept {
        return *this += VectorBase{(Is, rhs)...};
    }

    constexpr VectorBase &operator-=(const VectorBase &rhs) noexcept {
        __((_m[Is] -= rhs._m[Is])...);
        return *this;
    }

    constexpr VectorBase &operator-=(const T &rhs) noexcept {
        return *this -= VectorBase{(Is, rhs)...};
    }

    constexpr VectorBase &operator*=(const VectorBase &rhs) noexcept {
        __((_m[Is] *= rhs._m[Is])...);
        return *this;
    }

    constexpr VectorBase &operator*=(const T &rhs) noexcept {
        return *this *= VectorBase{(Is, rhs)...};
    }

    constexpr VectorBase &operator/=(const VectorBase &rhs) noexcept {
        __((_m[Is] /= rhs._m[Is])...);
        return *this;
    }

    constexpr VectorBase &operator/=(const T &rhs) noexcept {
        return *this /= VectorBase{(Is, rhs)...};
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
constexpr T dot(const VectorBase<T, Index> &lhs, const VectorBase<T, Index> &rhs) {
    return (lhs * rhs).sum();
}

using Index3 = make_index_sequence<3>;
using Index2 = make_index_sequence<2>;

template<class T>
constexpr VectorBase<T, Index3> cross(const VectorBase<T, Index3> &lhs,
                                      const VectorBase<T, Index3> &rhs) {
    return VectorBase<T, Index3>{lhs[1] * rhs[2] - lhs[2] * rhs[1],
                                 lhs[2] * rhs[0] - lhs[0] * rhs[2],
                                 lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

template<class T>
constexpr T cross(const VectorBase<T, Index2> &lhs,
                  const VectorBase<T, Index2> &rhs) {
    return lhs[0] * rhs[1] - lhs[1] * rhs[0];
}

template<class T, size_t Size>
using Vector = VectorBase<T, make_index_sequence<Size>>;

using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;
using Vector6d = Vector<double, 6>;
using Vector8d = Vector<double, 8>;
using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;
using Vector6f = Vector<float, 6>;
using Vector8f = Vector<float, 8>;
using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;
using Vector6i = Vector<int, 6>;
using Vector8i = Vector<int, 8>;

} // namespace La
} // namespace ala

#endif // HEAD