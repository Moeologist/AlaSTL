#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <ala/config.hpp>

namespace amath {

namespace detail {

// template <int N, class T>
// ALA_FORCEINLINE void _assign_helper(T *_m, const T value) {
// 	_assign_helper<N - 1>(_m, value);
// 	_m[N - 1] = value;
// }

// template <class T>
// ALA_FORCEINLINE void _assign_helper<0>(T *, const T) {
// 	return;
// }

// template <int N, class T>
// ALA_FORCEINLINE void _assign_helper1(T *_m, const T *_n) {
// 	_assign_helper<N - 1>(m);
// 	_m[N - 1] = _n[N - 1];
// }

// template <class T>
// ALA_FORCEINLINE void _assign_helper1<0>(T *, const T *) {
// 	return;
// }

} // namespace detail

template <class T, size_t Size>
class vector {
	typedef T value_type;
	typedef size_t size_type;

  public:
	vector() {
		for (int i = 0; i < Size; ++i)
			_m[i] = 0;
	}

	ALA_FORCEINLINE vector(value_type val) {
		for (int i = 0; i < Size; ++i)
			_m[i] = val;
	}

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
		for (int i = 0; i < Size; ++i)
			sum += _m[i] * _m[i];
		sum = sqrt(sum);
		return sum == 0 ? *this = *this / sum : *this;
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

	ALA_FORCEINLINE vector<T, Size> operator+=(const vector<T, Size> &rhs) const {
		T m[size] = {0};
		for (int i = 0; i < size; ++i)
			_m[i] += rhs._m[i];
		return vector<T, Size>(m);
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
		for (int i = 0; i < size; ++i)
			sum += _m[i] * rhs._m[i];
		return sum;
	}

  private:
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
	T m[] = {lhs[1] * rhs[2] - lhs[2] * rhs[1],
	         lhs[2] * rhs[0] - lhs[0] * rhs[2],
	         lhs[0] * rhs[1] - lhs[1] * rhs[0]};
	return vector<T, 3>(m);
}

namespace detail {

template <class T, class FirstArg>
struct make_vector_helper {
	typedef T type;
};

template <class FirstArg>
struct make_vector_helper<void, FirstArg> {
	typedef FirstArg type;
};

template <class T, size_t Size, class Arg, class... Args>
void make_vector_assign(vector<T, Size> &vec, Arg arg, Args... args) {
	vec[Size - 1 - sizeof...(args)] = static_cast<T>(arg);
	make_vector_assign(vec, args...);
}

template <class T, size_t Size>
void make_vector_assign(vector<T, Size> &) {
	return;
}

} // namespace detail

template <class T = void, class Arg, class... Args>
vector<typename detail::make_vector_helper<T, Arg>::type, sizeof...(Args) + 1>
make_vector(Arg arg, Args... args) {
	vector<typename detail::make_vector_helper<T, Arg>::type, sizeof...(Args) + 1> vec;
	detail::make_vector_assign(vec, arg, args...);
	return vec;
}

} // namespace amath

#endif // VECTOR_HPP