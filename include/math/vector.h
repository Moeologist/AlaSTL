#ifndef ALAM_VECTOR_HPP
#define ALAM_VECTOR_HPP

#include "ala/config.h"
#include <initializer_list>
#include <cassert>
#include <cmath>

namespace ala::math {

template <class T, size_t Size>
class vector {
	typedef T value_type;
	typedef size_t size_type;

  public:
	vector() {
		for (int i = 0; i < Size; ++i)
			_m[i] = 0;
	}

	_ALA_FORCEINLINE vector(T *m) {
		for (int i = 0; i < Size; ++i)
			_m[i] = m[i];
	}

	_ALA_FORCEINLINE vector(std::initializer_list<T> il) {
		typename std::initializer_list<T>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = *it;
		for (; i < Size; ++i)
			_m[i] = 0;
	}

	// _ALA_FORCEINLINE vector(std::initializer_list<int> il) {
	// 	typename std::initializer_list<int>::iterator it = il.begin();
	// 	int i = 0;
	// 	for (; i < il.size(); ++i, ++it)
	// 		_m[i] = *it;
	// 	for (; i < Size; ++i)
	// 		_m[i] = 0;
	// }

	// _ALA_FORCEINLINE vector(std::initializer_list<double> il) {
	// 	typename std::initializer_list<double>::iterator it = il.begin();
	// 	int i = 0;
	// 	for (; i < il.size(); ++i, ++it)
	// 		_m[i] = *it;
	// 	for (; i < Size; ++i)
	// 		_m[i] = 0;
	// }

	_ALA_FORCEINLINE vector(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] = rhs._m[i];
	}

	_ALA_FORCEINLINE T magnitude() const {
		T sum = 0;
		for (int i = 0; i < Size; ++i)
			sum += _m[i] * _m[i];
		return sqrt(sum);
	}

	_ALA_FORCEINLINE vector<T, Size> &normlize() {
		T mag = magnitude();
		return mag == 0 ? *this : *this = *this / mag;
	}

	_ALA_FORCEINLINE vector<T, Size> norm() const {
		vector<T, Size> temp(*this);
		return temp.normlize();
	}

	_ALA_FORCEINLINE vector<T, Size> &operator=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] = rhs._m[i];
		return *this;
	}

	_ALA_FORCEINLINE bool const operator==(const vector<T, Size> &rhs) {
		return !(*this != rhs);
	}

	_ALA_FORCEINLINE bool const operator!=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			if (_m[i] == rhs._m[i])
				return false;
		return true;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator+=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] += rhs._m[i];
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator+=(const T &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] += rhs;
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator+(const vector<T, Size> &rhs) const {
		vector<T, Size> temp(*this);
		return temp += rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator+(const T &rhs) const {
		vector<T, Size> temp(*this);
		return temp += rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator-=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] -= rhs._m[i];
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator-=(const T &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] -= rhs;
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator-(const vector<T, Size> &rhs) const {
		vector<T, Size> temp(*this);
		return temp -= rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator-(const T &rhs) const {
		vector<T, Size> temp(*this);
		return temp -= rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator*=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] *= rhs._m[i];
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator*=(const T &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] *= rhs;
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator*(const vector<T, Size> &rhs) const {
		vector<T, Size> temp(*this);
		return temp *= rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator*(const T &rhs) const {
		vector<T, Size> temp(*this);
		return temp *= rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator/=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] /= rhs._m[i];
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> &operator/=(const T &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] /= rhs;
		return *this;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator/(const vector<T, Size> &rhs) const {
		vector<T, Size> temp(*this);
		return temp /= rhs;
	}

	_ALA_FORCEINLINE vector<T, Size> const operator/(const T &rhs) const {
		vector<T, Size> temp(*this);
		return temp /= rhs;
	}

	_ALA_FORCEINLINE T &operator[](const int index) {
		assert(index < Size);
		if (index < 0)
			return _m[Size + index];
		return _m[index];
	}

	_ALA_FORCEINLINE T const &operator[](const int index) const {
		assert(index < Size);
		if (index < 0)
			return _m[Size + index];
		return _m[index];
	}

	_ALA_FORCEINLINE T const dot(const vector<T, Size> &rhs) const {
		T sum = 0;
		for (int i = 0; i < Size; ++i)
			sum += _m[i] * rhs._m[i];
		return sum;
	}

  private:
	static const size_t size = Size;

	T _m[Size];
};

template <class T, size_t Size, class Scalar>
_ALA_FORCEINLINE vector<T, Size> operator+(const Scalar &lhs, const vector<T, Size> &rhs) {
	return rhs + lhs;
}

template <class T, size_t Size, class Scalar>
_ALA_FORCEINLINE vector<T, Size> operator-(const Scalar &lhs, const vector<T, Size> &rhs) {
	T m[Size];
	for (int i = 0; i < Size; ++i)
		m[i] = lhs._m[i] - rhs._m[i];
	return vector<T, Size>(m);
}

template <class T, size_t Size, class Scalar>
_ALA_FORCEINLINE vector<T, Size> operator*(const Scalar &lhs, const vector<T, Size> &rhs) {
	return rhs * lhs;
}

template <class T, size_t Size, class Scalar>
_ALA_FORCEINLINE vector<T, Size> operator/(const Scalar &lhs, const vector<T, Size> &rhs) {
	T m[Size];
	for (int i = 0; i < Size; ++i)
		m[i] = lhs._m[i] / rhs._m[i];
	return vector<T, Size>(m);
}

template <class T, size_t Size>
_ALA_FORCEINLINE T dot(const vector<T, Size> &lhs, const vector<T, Size> &rhs) {
	return lhs.dot(rhs);
}

template <class T>
_ALA_FORCEINLINE vector<T, 3> const cross(const vector<T, 3> &lhs, const vector<T, 3> &rhs) {
	T m[] = {lhs[1] * rhs[2] - lhs[2] * rhs[1],
	         lhs[2] * rhs[0] - lhs[0] * rhs[2],
	         lhs[0] * rhs[1] - lhs[1] * rhs[0]};
	return vector<T, 3>(m);
}

template <class T>
_ALA_FORCEINLINE T const cross(const vector<T, 2> &lhs, const vector<T, 2> &rhs) {
	return abs(lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

} // namespace ala::math

#endif // ALAM_VECTOR_HPP