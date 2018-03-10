#define GENERATOR(N) \
template <class T>\
class vector<T, N> {\
	typedef T value_type;\
	typedef size_t size_type;\
\
  public:\
	vector() {\
		REPEAT_ ## N (= 0;)\
	}\
\
	ALA_FORCEINLINE vector(T *m) {\
		T *_ptr = static_cast<T*>(_m), *ptr = m;\
		REPEAT_ ## N (*_ptr++ = *ptr++;)\
	}\
\
	ALA_FORCEINLINE vector(std::initializer_list<T> il) {\
		typename std::initializer_list<T>::iterator it = il.begin();\
		int i = 0;\
		for (; i < il.size(); ++i, ++it)\
			_m[i] = *it;\
		for (; i < N; ++i)\
			_m[i] = 0;\
	}\
\
	ALA_FORCEINLINE vector(std::initializer_list<int> il) {\
		typename std::initializer_list<int>::iterator it = il.begin();\
		int i = 0;\
		for (; i < il.size(); ++i, ++it)\
			_m[i] = *it;\
		for (; i < N; ++i)\
			_m[i] = 0;\
	}\
\
	ALA_FORCEINLINE vector(std::initializer_list<double> il) {\
		typename std::initializer_list<double>::iterator it = il.begin();\
		int i = 0;\
		for (; i < il.size(); ++i, ++it)\
			_m[i] = *it;\
		for (; i < N; ++i)\
			_m[i] = 0;\
	}\
\
	ALA_FORCEINLINE vector(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (*_ptr++ = *ptr++;)\
	}\
\
	ALA_FORCEINLINE vector<T, N> norm() const {\
		vector<T, N> temp(*this);\
		return temp.normlize();\
	}\
\
	ALA_FORCEINLINE vector<T, N> &normlize() {\
		T sum = 0;\
		T *_ptr = static_cast<T*>(_m);\
		REPEAT_ ## N (sum += *_ptr * *_ptr++;)\
		return *this = *this / sqrt(sum);\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator=(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (*_ptr++ = *ptr++;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE bool operator==(const vector<T, N> &rhs) {\
		return !(*this != rhs);\
	}\
\
	ALA_FORCEINLINE bool operator!=(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (if (*_ptr++ == *ptr++) return false;)\
		return true;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator+=(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (*_ptr++ += *ptr++;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator+=(const T &rhs) {\
		T *_ptr = static_cast<T*>(_m);\
		REPEAT_ ## N (*_ptr++ += rhs;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator+(const vector<T, N> &rhs) const {\
		vector<T, N> temp(*this);\
		return temp += rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator+(const T &rhs) const {\
		vector<T, N> temp(*this);\
		return temp += rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator-=(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (*_ptr++ -= *ptr++;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator-=(const T &rhs) {\
		T *_ptr = static_cast<T*>(_m);\
		REPEAT_ ## N (*_ptr++ -= rhs;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator-(const vector<T, N> &rhs) const {\
		vector<T, N> temp(*this);\
		return temp -= rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator-(const T &rhs) const {\
		vector<T, N> temp(*this);\
		return temp -= rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator*=(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (*_ptr++ *= *ptr++;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator*=(const T &rhs) {\
		T *_ptr = static_cast<T*>(_m);\
		REPEAT_ ## N (*_ptr++ *= rhs;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator*(const vector<T, N> &rhs) const {\
		vector<T, N> temp(*this);\
		return temp *= rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator*(const T &rhs) const {\
		vector<T, N> temp(*this);\
		return temp *= rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator/=(const vector<T, N> &rhs) {\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (*_ptr++ /= *ptr++;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> &operator/=(const T &rhs) {\
		T *_ptr = static_cast<T*>(_m);\
		REPEAT_ ## N (*_ptr++ /= rhs;)\
		return *this;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator/(const vector<T, N> &rhs) const {\
		vector<T, N> temp(*this);\
		return temp /= rhs;\
	}\
\
	ALA_FORCEINLINE vector<T, N> operator/(const T &rhs) const {\
		vector<T, N> temp(*this);\
		return temp /= rhs;\
	}\
\
	ALA_FORCEINLINE T &operator[](const int index) {\
		assert(index < N);\
		if (index < 0)\
			return _m[N + index];\
		return _m[index];\
	}\
\
	ALA_FORCEINLINE const T &operator[](const int index) const {\
		assert(index < N);\
		if (index < 0)\
			return _m[N + index];\
		return _m[index];\
	}\
\
	ALA_FORCEINLINE T dot(const vector<T, N> &rhs) const {\
		T sum = 0;\
		T *_ptr = static_cast<T*>(_m), *ptr = static_cast<T*>(rhs._m);\
		REPEAT_ ## N (sum += *_ptr++ * *ptr++;)\
		return sum;\
	}\
\
  private:\
	static const size_t size = N;\
\
	T _m[N];\
};\
\
template <class T, class Scalar>\
ALA_FORCEINLINE vector<T, N> operator+(const Scalar &lhs, const vector<T, N> &rhs) {\
	return rhs + lhs;\
}\
\
template <class T, class Scalar>\
ALA_FORCEINLINE vector<T, N> operator-(const Scalar &lhs, const vector<T, N> &rhs) {\
	T m[N];\
	T ptr = static_cast<T*>(m), *lptr = static_cast<T*>(lhs._m), *rptr = static_cast<T*>(rhs._m);\
	REPEAT_ ## N (*ptr++ = *lptr++ - *rptr++;)\
	return vector<T, N>(m);\
}\
\
template <class T, class Scalar>\
ALA_FORCEINLINE vector<T, N> operator*(const Scalar &lhs, const vector<T, N> &rhs) {\
	return rhs * lhs;\
}\
\
template <class T, class Scalar>\
ALA_FORCEINLINE vector<T, N> operator/(const Scalar &lhs, const vector<T, N> &rhs) {\
	T m[N];\
	T ptr = static_cast<T*>(m), *lptr = static_cast<T*>(lhs._m), *rptr = static_cast<T*>(rhs._m);\
	REPEAT_ ## N (*ptr++ = *lptr++ / *rptr++;)\
	return vector<T, N>(m);\
}\
\
template <class T>\
ALA_FORCEINLINE T dot(const vector<T, N> &lhs, const vector<T, N> &rhs) {\
	return lhs.dot(rhs);\
}