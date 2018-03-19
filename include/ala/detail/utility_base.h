#ifndef _ALA_DETAIL_UTILITY_BASE_H
#define _ALA_DETAIL_UTILITY_BASE_H

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

} // namespace ala

#endif