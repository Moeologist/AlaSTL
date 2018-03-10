#ifndef ALAM_VECTOR_HPP
#define ALAM_VECTOR_HPP

#include <ala/config.hpp>

namespace alam {

template <class T, size_t Size>
class vector {
	typedef T value_type;
	typedef size_t size_type;

  public:
	vector() {
		for (int i = 0; i < Size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(value_type *m) {
		for (int i = 0; i < Size; ++i)
			_m[i] = m[i];
	}

	ALA_FORCEINLINE vector(std::initializer_list<T> il) {
		typename std::initializer_list<T>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = *it;
		for (; i < Size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(std::initializer_list<int> il) {
		typename std::initializer_list<int>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = (T)*it;
		for (; i < Size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(std::initializer_list<double> il) {
		typename std::initializer_list<double>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = (T)*it;
		for (; i < Size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] = rhs._m[i];
	}

	ALA_FORCEINLINE vector<T, Size> norm() const {
		vector<T, Size> vec(*this);
		vec.normlize();
		return vec;
	}

	ALA_FORCEINLINE vector<T, Size> &normlize() {
		T sum = 0;
		for (int i = 0; i < Size; ++i)
			sum += _m[i] * _m[i];
		return *this = *this / sqrt(sum);
	}

	ALA_FORCEINLINE vector<T, Size> &operator=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			_m[i] = rhs._m[i];
		return *this;
	}

	ALA_FORCEINLINE bool operator==(const vector<T, Size> &rhs) {
		return !(*this != rhs);
	}

	ALA_FORCEINLINE bool operator!=(const vector<T, Size> &rhs) {
		for (int i = 0; i < Size; ++i)
			if (_m[i] == rhs._m[i])
				return false;
		return true;
	}

	// ALA_FORCEINLINE vector<T, Size> operator+=(const vector<T, Size> &rhs) const {
	// 	for (int i = 0; i < Size; ++i)
	// 		_m[i] += rhs._m[i];
	// 	return *this;
	// }

	ALA_FORCEINLINE vector<T, Size> operator+(const vector<T, Size> &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] + rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator+(const T &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] + rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator-(const vector<T, Size> &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] - rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator-(const T &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] - rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator*(const vector<T, Size> &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] * rhs._m[i];
		return vector<T, Size>(m);
		// return vector{ _m[0] * rhs._m[0], _m[1] * rhs._m[1], _m[2] * rhs._m[2] };
	}

	ALA_FORCEINLINE vector<T, Size> operator*(const T &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] * rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator/(const vector<T, Size> &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] / rhs._m[i];
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator/(const T &rhs) const {
		T m[Size] = {0};
		for (int i = 0; i < Size; ++i)
			m[i] = _m[i] / rhs;
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE T &operator[](const int index) {
		if (index == 0)
			return _m[0];
		if (index == 1)
			return _m[1];
		if (index == 2)
			return _m[2];
		// assert(index < Size);
		//if (index < 0)
		//	return _m[Size + index];
		//return _m[index];
	}

	ALA_FORCEINLINE const T &operator[](const int index) const {
		if (index == 0)
			return _m[0];
		if (index == 1)
			return _m[1];
		if (index == 2)
			return _m[2];
		// assert(index < Size);
		//if (index < 0)
		//	return _m[Size + index];
		//return _m[index];
	}

	ALA_FORCEINLINE T dot(const vector<T, Size> &rhs) const {
		 T sum = 0;
		 for (int i = 0; i < Size; ++i)
		 	sum += _m[i] * rhs._m[i];
		 return sum;
		// return _m[0] * rhs._m[0] + _m[1] * rhs._m[1] + _m[2] * rhs._m[2];
	}

  private:
	static const size_t size = Size;

	T _m[Size];
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

template <class T, size_t Size>
ALA_FORCEINLINE T dot(const vector<T, Size> &lhs, const vector<T, Size> &rhs) {
	return lhs.dot(rhs);
}

template <class T>
ALA_FORCEINLINE vector<T, 3> cross(const vector<T, 3> &lhs, const vector<T, 3> &rhs) {
	T m[] = {lhs[1] * rhs[2] - lhs[2] * rhs[1],
	         lhs[2] * rhs[0] - lhs[0] * rhs[2],
	         lhs[0] * rhs[1] - lhs[1] * rhs[0]};
	return vector<T, 3>(m);
}

template <class T>
ALA_FORCEINLINE vector<T, 4> cross(const vector<T, 4> &lhs, const vector<T, 4> &rhs) {
	T m[] = {lhs[1] * rhs[2] - lhs[2] * rhs[1],
	         lhs[2] * rhs[0] - lhs[0] * rhs[2],
	         lhs[0] * rhs[1] - lhs[1] * rhs[0], 0};
	return vector<T, 4>(m);
}

template <class T>
ALA_FORCEINLINE vector<T, 8> cross(const vector<T, 8> &lhs, const vector<T, 8> &rhs) {
	T m[] = {lhs[1] * rhs[2] - lhs[2] * rhs[1],
	         lhs[2] * rhs[0] - lhs[0] * rhs[2],
	         lhs[0] * rhs[1] - lhs[1] * rhs[0], 0, 0, 0, 0, 0};
	return vector<T, 8>(m);
}

} // namespace alam

#endif // ALAM_VECTOR_HPP