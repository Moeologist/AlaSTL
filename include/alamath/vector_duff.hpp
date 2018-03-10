#ifndef ALAM_VECTOR_HPP
#define ALAM_VECTOR_HPP

#include <ala/config.hpp>
#include <alam/duff.hpp>

namespace alam {

template <class T, size_t Size>
class vector {
	typedef T value_type;
	typedef size_t size_type;

  public:
	vector() {
		T *_ptr = _m;
		DUFF_LOOP(*_ptr++ = 0);
	}

	ALA_FORCEINLINE vector(value_type *m) {
		T *_ptr = _m;
		DUFF_LOOP(*_ptr++ = *m++);
	}

	ALA_FORCEINLINE vector(std::initializer_list<T> il,void* =nullptr) {
		typename std::initializer_list<T>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = *it;
		for (; i < size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(std::initializer_list<int> il) {
		typename std::initializer_list<int>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = (T)*it;
		for (; i < size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(std::initializer_list<double> il) {
		typename std::initializer_list<double>::iterator it = il.begin();
		int i = 0;
		for (; i < il.size(); ++i, ++it)
			_m[i] = (T)*it;
		for (; i < size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(const vector<T, Size> &rhs) {
		T *_ptr = _m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(*_ptr++ = *rhsptr++);
	}

	ALA_FORCEINLINE vector<T, Size> norm() const {
		vector<T, Size> vec(*this);
		vec.normlize();
		return vec;
	}

	ALA_FORCEINLINE vector<T, Size> &normlize() {
		T sum = 0;
		T *_ptr = _m;
		DUFF_LOOP(sum += *_ptr * *_ptr++);
		return *this = *this / sqrt(sum);
	}

	ALA_FORCEINLINE vector<T, Size> &operator=(const vector<T, Size> &rhs) {
		T *_ptr = _m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(*_ptr++ = *rhsptr++);
		return *this;
	}

	ALA_FORCEINLINE bool operator==(const vector<T, Size> &rhs) {
		return !(*this != rhs);
	}

	ALA_FORCEINLINE bool operator!=(const vector<T, Size> &rhs) {
		T *_ptr = _m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP({if (*_ptr++ == *rhsptr++) return false; });
		return true;
	}

	ALA_FORCEINLINE vector<T, Size> operator+(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(*ptr++ = *_ptr++ + *rhsptr++);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator+(const T &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m;
		DUFF_LOOP(*ptr++ = *_ptr++ + rhs);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator-(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(*ptr++ = *_ptr++ - *rhsptr++);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator-(const T &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m;
		DUFF_LOOP(*ptr++ = *_ptr++ - rhs);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator*(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(*ptr++ = *_ptr++ * *rhsptr++);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator*(const T &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m;
		DUFF_LOOP(*ptr++ = *_ptr++ * rhs);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator/(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(*ptr++ = *_ptr++ / *rhsptr++);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE vector<T, Size> operator/(const T &rhs) const {
		T m[size] = {0};
		T *ptr = m, *_ptr = (T*)_m;
		DUFF_LOOP(*ptr++ = *_ptr++ / rhs);
		return vector<T, Size>(m);
	}

	ALA_FORCEINLINE T &operator[](const int index) {
		// assert(index < size);
		if (index < 0)
			return _m[size + index];
		return _m[index];
	}

	ALA_FORCEINLINE const T &operator[](const int index) const {
		// assert(index < size);
		if (index < 0)
			return _m[size + index];
		return _m[index];
	}

	ALA_FORCEINLINE T dot(const vector<T, Size> &rhs) const {
		T sum = 0;
		T *_ptr = (T*)_m, *rhsptr = (T*)rhs._m;
		DUFF_LOOP(sum += *_ptr++ * *rhsptr++);
		return sum;
	}


	static const size_t size = Size;

	T _m[size];
};

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator+(const Scalar &lhs, const vector<T, Size> &rhs) {
	return rhs + lhs;
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator-(const Scalar &lhs, const vector<T, Size> &rhs) {
	T m[Size] = {0};
	T *ptr = m, *rhsptr = (T*)rhs._m;
	DUFF_LOOP(*ptr++ = lhs - *rhsptr++);
	return vector<T, Size>(m);
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator*(const Scalar &lhs, const vector<T, Size> &rhs) {
	return rhs * lhs;
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator/(const Scalar &lhs, const vector<T, Size> &rhs) {
	T m[Size] = {0};
	T *ptr = m, *rhsptr = (T*)rhs._m;
	DUFF_LOOP(*ptr++ = lhs / *rhsptr++);
	return vector<T, Size>(m);
}

template <class T,size_t Size>
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
	         lhs[0] * rhs[1] - lhs[1] * rhs[0],0};
	return vector<T, 4>(m);
}

template <class T>
ALA_FORCEINLINE vector<T, 8> cross(const vector<T, 8> &lhs, const vector<T, 8> &rhs) {
	T m[] = {lhs[1] * rhs[2] - lhs[2] * rhs[1],
	         lhs[2] * rhs[0] - lhs[0] * rhs[2],
	         lhs[0] * rhs[1] - lhs[1] * rhs[0],0,0,0,0,0};
	return vector<T, 8>(m);
}

} // namespace alam

#endif // ALAM_VECTOR_HPP