#ifndef _ALA_LA_VECTOR_H
#define _ALA_LA_VECTOR_H

#ifdef _ALALIB_USE_ALA
#include "ala/config.h"
#include "ala/type_traits.h"
#include <cassert>
#include <cmath>
#include <initializer_list>

namespace ala {
namespace La {

template<class... T>
struct all_is_convertible;

template<class T, class... Types>
struct all_is_convertible<T, Types...> : conjunction<is_convertible<Types, T>...> {};

template<class T, size_t Size>
class vector {
    typedef T value_type;
    typedef size_t size_type;

public:
    constexpr vector() noexcept {}

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector(const std::initializer_list<U> &ilist) noexcept {
        assert(ilist.size() == Size);
        typename std::initializer_list<U>::iterator it = ilist.begin();
        for (int i = 0; i < Size; ++i, ++it)
            _m[i] = static_cast<T>(*it);
    }

    template<class... Args,
             class = enable_if_t<all_is_convertible<T, Args...>::value &&
                                 sizeof...(Args) == Size>>
    constexpr vector(Args... args) noexcept {
        _va_helper(args...);
    }

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector(const vector<U, Size> &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] = static_cast<T>(rhs._m[i]);
    }

    // move operator is not recommended
    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector(vector<U, Size> &&rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] = static_cast<T>(rhs._m[i]);
    }

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector<T, Size> &operator=(const vector<U, Size> &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] = static_cast<T>(rhs._m[i]);
        return *this;
    }

    // move operator is not recommended
    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector<T, Size> &operator=(vector<U, Size> &&rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] = static_cast<T>(rhs._m[i]);
        return *this;
    }

    constexpr const T sum() const noexcept {
        T ret = _m[0];
        for (int i = 1; i < Size; ++i)
            ret += _m[i];
        return ret;
    }

    constexpr const T prod() const noexcept {
        T sum = _m[0];
        for (int i = 1; i < Size; ++i)
            sum *= _m[i];
        return;
    }

    constexpr const T l1norm() const noexcept {
        T sum = abs(_m[0]);
        for (int i = 1; i < Size; ++i)
            sum += jit(_m[i]);
        return sum;
    }

    constexpr const T l2norm() const noexcept {
        T sum = _m[0] * _m[0];
        for (int i = 1; i < Size; ++i)
            sum += _m[i] * _m[i];
        return sqrt(sum);
    }

    constexpr vector<T, Size> &normlize() noexcept {
        T l2 = l2norm();
        return l2 == 0 ? *this : *this = *this / l2;
    }

    constexpr vector<T, Size> normlized() const noexcept {
        vector<T, Size> tmp(*this);
        return tmp.normlize();
    }

    template<class U, class = enable_if_t<is_convertible<
                          decltype(declval<T>() == declval<U>()), bool>::value>>
    constexpr const bool operator==(const vector<U, Size> &rhs) const noexcept {
        for (int i = 0; i < Size; ++i)
            if (!(_m[i] == rhs._m[i]))
                return false;
        return true;
    }

    template<class U, class = enable_if_t<is_convertible<
                          decltype(declval<T>() != declval<U>()), bool>::value>>
    constexpr const bool operator!=(const vector<U, Size> &rhs) const noexcept {
        for (int i = 0; i < Size; ++i)
            if (!(_m[i] != rhs._m[i]))
                return false;
        return true;
    }

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector<T, Size> &operator+=(const vector<U, Size> &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] += static_cast<T>(rhs._m[i]);
        return *this;
    }

    template<class U, class Common = common_type_t<T, U>>
    constexpr const vector<Common, Size>
    operator+(const vector<U, Size> &rhs) const noexcept {
        vector<Common, Size> tmp(*this);
        return tmp += rhs;
    }

    constexpr vector<T, Size> &operator+=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] += static_cast<T>(rhs);
        return *this;
    }

    constexpr vector<T, Size> const operator+(const T &rhs) const noexcept {
        vector<T, Size> tmp(*this);
        return tmp += rhs;
    }

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector<T, Size> &operator-=(const vector<U, Size> &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] -= static_cast<T>(rhs._m[i]);
        return *this;
    }

    template<class U, class Common = common_type_t<T, U>>
    constexpr const vector<Common, Size>
    operator-(const vector<U, Size> &rhs) const noexcept {
        vector<Common, Size> tmp(*this);
        return tmp -= rhs;
    }

    constexpr vector<T, Size> &operator-=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] -= rhs;
        return *this;
    }

    constexpr vector<T, Size> const operator-(const T &rhs) const noexcept {
        vector<T, Size> tmp(*this);
        return tmp -= rhs;
    }

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector<T, Size> &operator*=(const vector<U, Size> &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] *= static_cast<T>(rhs._m[i]);
        return *this;
    }

    template<class U, class Common = common_type_t<T, U>>
    constexpr const vector<Common, Size>
    operator*(const vector<U, Size> &rhs) const noexcept {
        vector<Common, Size> tmp(*this);
        return tmp *= rhs;
    }

    constexpr vector<T, Size> &operator*=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] *= rhs;
        return *this;
    }

    constexpr vector<T, Size> const operator*(const T &rhs) const noexcept {
        vector<T, Size> tmp(*this);
        return tmp *= rhs;
    }

    template<class U, class = enable_if_t<is_convertible<U, T>::value>>
    constexpr vector<T, Size> &operator/=(const vector<U, Size> &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] /= static_cast<T>(rhs._m[i]);
        return *this;
    }

    template<class U, class Common = common_type_t<T, U>>
    constexpr const vector<Common, Size>
    operator/(const vector<U, Size> &rhs) const noexcept {
        vector<Common, Size> tmp(*this);
        return tmp /= rhs;
    }

    constexpr vector<T, Size> &operator/=(const T &rhs) noexcept {
        for (int i = 0; i < Size; ++i)
            _m[i] /= rhs;
        return *this;
    }

    constexpr const vector<T, Size> operator/(const T &rhs) const noexcept {
        vector<T, Size> tmp(*this);
        return tmp /= rhs;
    }

    constexpr T &operator[](int index) noexcept {
        if (index < 0)
            index = Size + index;
        assert(index < Size && index >= 0);
        return _m[index];
    }

    constexpr const T &operator[](int index) const noexcept {
        if (index < 0)
            index = Size + index;
        assert(index < Size && index >= 0);
        return _m[index];
    }

    constexpr T *data() noexcept { return _m; }
    constexpr T *const data() const noexcept { return _m; }

private:
    template<class Arg, class... Args>
    constexpr void _va_helper(Arg arg, Args... args) noexcept {
        _m[Size - 1 - sizeof...(args)] = static_cast<T>(arg);
        _va_helper(args...);
    }

    constexpr void _va_helper() noexcept { return; }

    constexpr static size_t size = Size;
    T _m[Size];
};

template<class T, size_t Size, class U>
constexpr enable_if_t<is_convertible<U, T>::value, vector<T, Size>>
boardcast(const U &s) {
    vector<T, Size> v;
    for (int i = 0; i < Size; ++i)
        v.data()[i] = static_cast<T>(s);
    return v;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_convertible<U, T>::value, vector<T, Size>>
operator+(const U &lhs, const vector<T, Size> &rhs) {
    return rhs + lhs;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_convertible<U, T>::value, vector<T, Size>>
operator-(const U &lhs, const vector<T, Size> &rhs) {
    return boardcast<T, Size>(lhs) -= rhs;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_convertible<U, T>::value, vector<T, Size>>
operator*(const U &lhs, const vector<T, Size> &rhs) {
    return rhs * lhs;
}

template<class T, size_t Size, class U>
constexpr enable_if_t<is_convertible<U, T>::value, vector<T, Size>>
operator/(const U &lhs, const vector<T, Size> &rhs) {
    return boardcast<T, Size>(lhs) /= rhs;
}

template<class T, size_t Size, class U, class Common = common_type_t<T, U>>
constexpr const Common dot(const vector<T, Size> &lhs,
                           const vector<U, Size> &rhs) {
    return (lhs * rhs).sum();
}

template<class T, class U, class Common = common_type_t<T, U>>
constexpr const vector<Common, 3> cross(const vector<T, 3> &lhs,
                                        const vector<U, 3> &rhs) {
    return vector<T, 3>{lhs[1] * rhs[2] - lhs[2] * rhs[1],
                        lhs[2] * rhs[0] - lhs[0] * rhs[2],
                        lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

template<class T, class U, class Common = common_type_t<T, U>>
constexpr const Common cross(const vector<T, 2> &lhs, const vector<U, 2> &rhs) {
    return abs(lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

} // namespace La
} // namespace ala

#endif // HEAD