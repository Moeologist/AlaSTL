#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <ala/config.h>

namespace alam {

template<class T>
struct vector {
	T x, y, z;

	vector(T x_ = 0, T y_ = 0, T z_ = 0) : x(x_), y(y_), z(z_) {}

	ALA_FORCEINLINE vector<T> &normlize() {
		return *this = *this * (1 / sqrt(x * x + y * y + z * z));
	}

	ALA_FORCEINLINE vector<T> norm() {
		return *this * (1 / sqrt(x * x + y * y + z * z));
	}

	ALA_FORCEINLINE T operator[](const int index) const {
		switch (index) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return x;
	}

	ALA_FORCEINLINE friend vector<T> operator+(const vector<T> &a, const vector<T> &b) {
		return vector<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	ALA_FORCEINLINE friend vector<T> operator-(const vector<T> &a, const vector<T> &b) {
		return vector<T>(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	ALA_FORCEINLINE friend vector<T> operator*(const vector<T> &a, const vector<T> &b) {
		return vector<T>(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	ALA_FORCEINLINE friend vector<T> operator*(const vector<T> &a, const float b) {
		return vector<T>(a.x * b, a.y * b, a.z * b);
	}

	ALA_FORCEINLINE friend vector<T> operator*(const float b, const vector<T> &a) {
		return vector<T>(a.x * b, a.y * b, a.z * b);
	}

	ALA_FORCEINLINE friend float dot(const vector<T> &a, const vector<T> &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	ALA_FORCEINLINE friend vector<T> cross(const vector<T> &a, const vector<T> &b) {
		return vector<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
};

} // namespace alam

#endif // VECTOR_HPP