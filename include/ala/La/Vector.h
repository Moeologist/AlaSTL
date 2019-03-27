#ifndef _ALA_LA_VECTOR_H
#define _ALA_LA_VECTOR_H

#define _ALALIB_USE_ALA
#ifdef _ALALIB_USE_ALA
#include <ala/type_traits.h>
#include <ala/external/assert.h>
#include <ala/external/math.h>
#include <ala/external/initializer_list.h>
namespace astd = ala;
#else
#include <type_traits>
#include <cassert>
#include <cmath>
#include <initializer_list>
namespace astd = std;
#endif

namespace ala {
namespace La {

template<class T, size_t Size>
struct vector {
    typedef T value_type;
    typedef size_t size_type;
    template<class Cast>
    constexpr vector<Cast, Size> cast() noexcept {
        vector<Cast, Size> tmp;
        for (int i = 0; i < Size; ++i)
            tmp._m[i] = _m[i];
        return tmp;
    }

    constexpr vector &fill(T value) noexcept {
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

    constexpr vector &normlize() noexcept {
        T l2 = l2norm();
        return l2 == 0 ? *this : *this = *this / l2;
    }

    constexpr vector normlized() const noexcept {
        vector tmp(*this);
        return tmp.normlize();
    }

    constexpr bool operator==(const vector &rhs) const noexcept {
        for (int i = 0; i < Size; ++i)
            if (!(_m[i] == rhs._m[i]))
                return false;
        return true;
    }

    constexpr bool operator!=(const vector &rhs) const noexcept {
        return !(*this == rhs);
    }

    constexpr vector &operator+=(const vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] += rhs._m[i];
        return *this;
    }

    constexpr vector operator+(const vector &rhs) const noexcept {
        vector tmp(*this);
        return tmp += rhs;
    }

    constexpr vector &operator+=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] += rhs;
        return *this;
    }

    constexpr vector operator+(const T &rhs) const noexcept {
        vector tmp(*this);
        return tmp += rhs;
    }

    constexpr vector &operator-=(const vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] -= rhs._m[i];
        return *this;
    }

    constexpr vector operator-(const vector &rhs) const noexcept {
        vector tmp(*this);
        return tmp -= rhs;
    }

    constexpr vector &operator-=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] -= rhs;
        return *this;
    }

    constexpr vector operator-(const T &rhs) const noexcept {
        vector tmp(*this);
        return tmp -= rhs;
    }

    constexpr vector &operator*=(const vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] *= rhs._m[i];
        return *this;
    }

    constexpr vector operator*(const vector &rhs) const noexcept {
        vector tmp(*this);
        return tmp *= rhs;
    }

    constexpr vector &operator*=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] *= rhs;
        return *this;
    }

    constexpr vector const operator*(const T &rhs) const noexcept {
        vector tmp(*this);
        return tmp *= rhs;
    }

    constexpr vector &operator/=(const vector &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] /= rhs._m[i];
        return *this;
    }

    constexpr vector operator/(const vector &rhs) const noexcept {
        vector tmp(*this);
        return tmp /= rhs;
    }

    constexpr vector &operator/=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] /= rhs;
        return *this;
    }

    constexpr vector operator/(const T &rhs) const noexcept {
        vector tmp(*this);
        return tmp /= rhs;
    }

    constexpr T &operator[](int index) noexcept {
        if (index < 0)
            index = Size + index;
        assert(index > 0 && index < Size) return _m[index];
    }

    constexpr const T &operator[](int index) const noexcept {
        if (index < 0)
            index = Size + index;
        assert(index > 0 && index < Size) return _m[index];
    }

    constexpr T *data() noexcept {
        return _m;
    }
    constexpr T *const data() const noexcept {
        return _m;
    }

    static_assert(Size >= 2, "vector size must >= 2");
    using size_type = size_t;
    using value_type = T;
    constexpr static size_t size = Size;
    T _m[Size];
};

template<class T, size_t Size, class U>
constexpr enable_if_t<is_assignable<T &, U>::value, vector<T, Size>>
operator+(const U &lhs, const vector<T, Size> &rhs) {
    return rhs + lhs;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_assignable<T &, U>::value, vector<T, Size>>
operator-(const U &lhs, const vector<T, Size> &rhs) {
    vector<T, Size> tmp;
    return tmp.fill(lhs) -= rhs;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_assignable<T &, U>::value, vector<T, Size>>
operator*(const U &lhs, const vector<T, Size> &rhs) {
    return rhs * lhs;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_assignable<T &, U>::value, vector<T, Size>>
operator/(const U &lhs, const vector<T, Size> &rhs) {
    vector<T, Size> tmp;
    return tmp.fill(lhs) /= rhs;
}

template<class T, size_t Size>
constexpr T dot(const vector<T, Size> &lhs, const vector<T, Size> &rhs) {
    return (lhs * rhs).sum();
}

template<class T>
constexpr vector<T, 3> cross(const vector<T, 3> &lhs, const vector<T, 3> &rhs) {
    return vector<T, 3>{lhs[1] * rhs[2] - lhs[2] * rhs[1],
                        lhs[2] * rhs[0] - lhs[0] * rhs[2],
                        lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

template<class T>
constexpr T cross(const vector<T, 2> &lhs, const vector<T, 2> &rhs) {
    return abs(lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<typename... Ts>
vector(Ts...)->vector<common_type_t<Ts...>, sizeof...(Ts)>;
#endif

} // namespace La
} // namespace ala

#endif // HEAD