#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ala/type_traits.h"

namespace ala {

template <typename T>
constexpr typename remove_reference<T>::type &&move(T &&x) noexcept {
	return static_cast<typename remove_reference<T>::type &&>(x);
}

template <typename Tp>
constexpr Tp &&forward(remove_reference_t<Tp> &t) noexcept {
	return static_cast<Tp &&>(t);
}

template <typename Tp>
constexpr Tp &&forward(remove_reference_t<Tp> &&t) noexcept {
	static_assert(!is_lvalue_reference_v<Tp>, "template argument substituting T is an lvalue reference type");
	return static_cast<Tp &&>(t);
}

template <class T, size_t N>
enable_if_t<is_swappable_v<T>>
swap(T (&lhs)[N], T (&rhs)[N]) noexcept(is_nothrow_swappable_v<T>) {
	if (&lhs != &rhs) {
		T *il = lhs;
		T *last = lhs + N;
		T *ir = rhs;
		for (; il != last; ++il, ++ir)
			swap(*il, *ir);
	}
}

template <class T>
enable_if_t<is_move_constructible_v<T> && is_move_assignable_v<T>>
swap(T &lhs, T &rhs) noexcept(is_nothrow_move_constructible_v<T> &&is_nothrow_move_assignable_v<T>) {
	T tmp = move(lhs);
	lhs = move(rhs);
	rhs = move(tmp);
}

template <typename T1, typename T2>
struct pair {

	typedef T1 first_type;
	typedef T2 second_type;

	constexpr pair() : first(T1()), second(T2()) {}

	constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

	template <typename U1, typename U2>
	constexpr pair(U1 &&a, U2 &&b) : first(forward<U1>(a)), second(forward<U2>(b)) {}

	template <typename U1, typename U2>
	constexpr pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}

	template <typename U1, typename U2>
	constexpr pair(pair<U1, U2> &&p) : first(forward<U1>(p.first)), second(forward<U2>(p.second)) {}

	pair(const pair &) = default;

	pair(pair &&) = default;

	pair &operator=(const pair &p) {
		first = p.first;
		second = p.second;
		return *this;
	}

	template <typename U1, typename U2>
	pair &operator=(const pair<U1, U2> &p) {
		first = p.first;
		second = p.second;
		return *this;
	}

	pair &operator=(pair &&p) {
		first = forward<T1>(p.first);
		second = forward<T2>(p.second);
		return *this;
	}

	template <typename U1, typename U2>
	pair &operator=(pair<U1, U2> &&p) {
		first = forward<U1>(p.first);
		second = forward<U2>(p.second);
		return *this;
	}

	void swap(pair &p) {
		swap(first, p.first);
		swap(second, p.second);
	}
	
	first_type first;
	second_type second;
};

template <typename _T1, typename _T2>
constexpr bool
operator==(const pair<T1, T2> &x, const pair<T1, T2> &y) { return x.first == y.first && x.second == y.second; }

template <typename T1, typename T2>
constexpr bool
operator<(const pair<T1, T2> &x, const pair<T1, T2> &y) { return x.first < y.first || (!(y.first < x.first) && x.second < y.second); }

template <typename T1, typename T2>
constexpr bool
operator!=(const pair<T1, T2> &x, const pair<T1, T2> &y) { return !(x == y); }

template <typename T1, typename T2>
constexpr bool
operator>(const pair<T1, T2> &x, const pair<T1, T2> &y) { return y < x; }

template <typename T1, typename T2>
constexpr bool
operator<=(const pair<T1, T2> &x, const pair<T1, T2> &y) { return !(y < x); }

template <typename T1, typename T2>
constexpr bool
operator>=(const pair<T1, T2> &x, const pair<T1, T2> &y) { return !(x < y); }

} // namespace ala

#endif // UTILITY_HPP