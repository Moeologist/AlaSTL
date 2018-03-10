#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <ala/config.hpp>

namespace amath {

template <class T, size_t Size>
class vector {
	typedef T value_type;
	typedef size_t size_type;

  public:
	template <class... Args>
	vector(Args... args) {
		size_t argc = sizeof...(args);
		va_constructor(argc, args...);
		for (int i = argc; i < Size; ++i)
			_m[i] = 0;
	}

	// vector() {
	// 	for (int i = 0; i < Size; ++i)
	// 		_m[i] = 0;
	// }

	ALA_FORCEINLINE vector(value_type *m) {
		for (int i = 0; i < Size; ++i)
			_m[i] = m[i];
	}

	ALA_FORCEINLINE vector(const vector<T, Size> &rhs) {
		for (int i = 0; i < size; ++i)
			_m[i] = rhs._m[i];
	}

	ALA_FORCEINLINE vector<T, Size> norm() const {
		T sum = 0;
		for (int i = 0; i < size; ++i)
			sum += _m[i] * _m[i];
		return *this / sqrt(sum);
	}

	ALA_FORCEINLINE vector<T, Size> &normlize() {
		T sum = 0;
		for (int i = 0; i < size; ++i)
			sum += _m[i] * _m[i];
		return *this = *this / sqrt(sum);
	}

	ALA_FORCEINLINE vector<T, Size> &operator=(const vector<T, Size> &rhs) {
		for (int i = 0; i < size; ++i)
			_m[i] = rhs._m[i];
		return *this;
	}

	ALA_FORCEINLINE bool operator==(const vector<T, Size> &rhs) {
		for (int i = 0; i < size; ++i)
			if (!(_m[i] == rhs._m[i]))
				return false;
		return true;
	}

	ALA_FORCEINLINE bool operator!=(const vector<T, Size> &rhs) {
		return !(*this == rhs);
	}

	ALA_FORCEINLINE vector<T, Size> operator+(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] + rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator+(const T &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] + rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator-(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] - rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator-(const T &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] - rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator*(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] * rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator*(const T &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] * rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator/(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] / rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator/(const T &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			m[i] = _m[i] / rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE T operator[](const int index) const {
		assert(index < size);
		if (index < 0)
			return _m[size + index];
		return _m[index];
	}

	ALA_FORCEINLINE T dot(const vector<T, Size> &rhs) const {
		T sum = 0;
		for (int i = 0; i < size; ++i)
			sum += _m[i] * rhs._m[i];
		return sum;
	}

  private:
	static const size_t size = Size;

	T _m[size];
	template <class Arg, class... Args>
	void va_constructor(size_t argc, Arg arg, Args... args) {
		_m[argc - sizeof...(args) - 1] = static_cast<T>(arg);
		va_constructor(argc, args...);
	}
	void va_constructor(size_t) {
		return;
	}
};

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator+(const Scalar &lhs, const vector<T, Size> &rhs) {
	return rhs + lhs;
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator-(const Scalar &lhs, const vector<T, Size> &rhs) {
	T m[Size] = {0};
	for (int i = 0; i < Size; ++i)
		m[i] = lhs._m[i] - rhs._m[i];
	return vector<T, Size>(m);
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator*(const Scalar &lhs, const vector<T, Size> &rhs) {
	return rhs * lhs;
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator/(const Scalar &lhs, const vector<T, Size> &rhs) {
	T m[Size] = {0};
	for (int i = 0; i < Size; ++i)
		m[i] = lhs._m[i] / rhs._m[i];
	return vector<T, Size>(m);
}

template <class T>
ALA_FORCEINLINE T dot(const vector<T, 3> &lhs, const vector<T, 3> &rhs) {
	return lhs.dot(rhs);
}

template <class T>
ALA_FORCEINLINE vector<T, 3> cross(const vector<T, 3> &lhs, const vector<T, 3> &rhs) {
	return vector<T, 3>(lhs[1] * rhs[2] - lhs[2] * rhs[1],
	                    lhs[2] * rhs[0] - lhs[0] * rhs[2],
	                    lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

} // namespace ala::math

#endif // VECTOR_HPP