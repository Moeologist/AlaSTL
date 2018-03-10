#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ala/type_traits.hpp"

namespace ala {

template <typename _T>
constexpr typename remove_reference<_T>::type &&move(_T &&x) noexcept {
	return static_cast<typename remove_reference<_T>::type &&>(x);
}

template <typename _It>
struct iterator_traits { using value_type = typename _It::value_type; };

template <typename _It>
struct iterator_traits<const _It> { using value_type = typename _It::value_type; };

template <typename _It>
struct iterator_traits<_It *> { using value_type = _It; };

template <typename _It>
struct iterator_traits<const _It *> { using value_type = _It; };

template <typename _F>
struct function_traits {};

template <typename _Ret, typename _Arg>
struct function_traits<_Ret (*)(_Arg)> {
	typedef _Ret result_type;
	typedef _Arg params_type;
};

template <typename _Ret, typename _Arg>
struct function_traits<_Ret (*)(_Arg, _Arg)> {
	typedef _Ret result_type;
	typedef _Arg params_type;
};

template <class _Ty, size_t _Size, class>
inline void swap(_Ty (&_Left)[_Size], _Ty (&_Right)[_Size])
    noexcept(is_nothrow_swappable_v<_Ty>) {
	if (&_Left != &_Right) { 
		_Ty *_First1 = _Left;
		_Ty *_Last1 = _First1 + _Size;
		_Ty *_First2 = _Right;
		for (; _First1 != _Last1; ++_First1, ++_First2)
			swap(*_First1, *_First2);
	}
}

template <class _Ty, class>
inline void swap(_Ty &_Left, _Ty &_Right)
    noexcept(is_nothrow_move_constructible_v<_Ty && is_nothrow_move_assignable_v<_Ty>) {
	_Ty _Tmp = move(_Left);
	_Left = move(_Right);
	_Right = move(_Tmp);
}

} // namespace ala

#endif // UTILITY_HPP