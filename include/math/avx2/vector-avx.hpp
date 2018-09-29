
// template <size_t Size>
// class vector<float, Size> {
// 	typedef float value_type;
// 	typedef size_t size_type;

//   public:
// 	template <class... Args>
// 	vector(Args... args) {
// 		size_t argc = sizeof...(args);
// 		va_constructor(argc, args...);
// 		for (int i = argc; i < _msize; ++i)
// 			_m[i] = 0;
// 	}

// 	vector(value_type *m) {
// 		for (int i = 0; i < size; ++i)
// 			_m[i] = m[i];
// 		for (int i = size; i < _msize; ++i)
// 			_m[i] = m[i];
// 	}

// 	vector(__m256 *v) {
// 		for (int i = 0; i < _vsize; ++i)
// 			_v[i] = v[i];
// 	}

// 	vector(const vector<float, Size> &rhs) {
// 		for (int i = 0; i < _vsize; ++i)
// 			_v[i] = rhs._v[i];
// 	}

// 	float sum() {
// 		float s = 0;
// 		for (int i = 0; i < size; ++i)
// 			s += _m[i];
// 		return s;
// 	}

// 	vector<float, Size> norm() const {
// 		float s = 0;
// 		s += ((*this) * (*this)).sum();
// 		return *this / sqrt(s);
// 	}

// 	vector<float, Size> &normlize() {
// 		return *this = this->norm();
// 	}

// 	ALA_FORCEINLINE vector<float, Size> &operator=(const vector<float, Size> &rhs) {
// 		for (int i = 0; i < _vsize; ++i)
// 			_v[i] = rhs._v[i];
// 		return *this;
// 	}

// 	ALA_FORCEINLINE bool operator==(const vector<float, Size> &rhs) {
// 		for (int i = 0; i < size; ++i)
// 			if (!(_m[i] == rhs._m[i]))
// 				return false;
// 		return true;
// 	}

// 	ALA_FORCEINLINE bool operator!=(const vector<float, Size> &rhs) {
// 		return !(*this == rhs);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator+(const vector<float, Size> &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_add_ps(_v[i], rhs._v[i]);
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator+(const float &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_add_ps(_v[i], _mm256_broadcast_ss(&rhs));
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator-(const vector<float, Size> &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_sub_ps(_v[i], rhs._v[i]);
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator-(const float &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_sub_ps(_v[i], _mm256_broadcast_ss(&rhs));
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator*(const vector<float, Size> &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_mul_ps(_v[i], rhs._v[i]);
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator*(const float &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_mul_ps(_v[i], _mm256_broadcast_ss(&rhs));
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator/(const vector<float, Size> &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_div_ps(_v[i], rhs._v[i]);
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE vector<float, Size> operator/(const float &rhs) const {
// 		__m256 v[_vsize];
// 		for (int i = 0; i < _vsize; ++i)
// 			v[i] = _mm256_div_ps(_v[i], _mm256_broadcast_ss(&rhs));
// 		return vector<float, Size>(v);
// 	}

// 	ALA_FORCEINLINE float operator[](const int index) const {
// 		assert(index < size);
// 		if (index < 0)
// 			return _m[size + index];
// 		return _m[index];
// 	}

// 	ALA_FORCEINLINE float dot(const vector<float, Size> &rhs) const {
// 		float sum = 0;
// 		sum += ((*this) * rhs).sum();
// 		return sum;
// 	}
// 	static const size_t size = Size;
// 	static const size_t _vsize = (Size + 7) / 8;
// 	static const size_t _msize = (Size + 7) / 8 * 8;
// 	union {
// 		float _m[_msize];
// 		__m256 _v[_vsize];
// 		struct {
// 			float x, y, z, w;
// 			float _placeholder[_msize - 4];
// 		};
// 	};

//   private:
// 	template <class Arg, class... Args>
// 	void va_constructor(size_t n, Arg arg, Args... args) {
// 		_m[n - sizeof...(args) - 1] = static_cast<float>(arg);
// 		va_constructor(n, args...);
// 	}
// 	void va_constructor(int n) {
// 		return;
// 	}
// };

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator-(const Scalar &lhs, const vector<T, Size> &rhs) {
	const size_t _vsize = vector<T, Size>::_vsize;
	__m256 v[_vsize];
	T l = lhs;
	for (int i = 0; i < _vsize; ++i)
		v[i] = _mm256_sub_ps(_mm256_broadcast_ss(&l), rhs._v[i]);
	return vector<float, Size>(v);
}

template <class T, size_t Size, class Scalar>
ALA_FORCEINLINE vector<T, Size> operator/(const Scalar &lhs, const vector<T, Size> &rhs) {
	const size_t _vsize = vector<T, Size>::_vsize;
	__m256 v[_vsize];
	T l = lhs;
	for (int i = 0; i < _vsize; ++i)
		v[i] = _mm256_div_ps(_mm256_broadcast_ss(&l), rhs._v[i]);
	return vector<float, Size>(v);
}