#ifndef MEMORY_HPP
#define MEMORY_HPP

// template <class _T>
// constexpr _T *addressof(_T &arg) noexcept {
// 	return reinterpret_cast<T *>(&const_cast<char &>(
// 	    reinterpret_cast<const volatile char &>(arg)));
// }

template <class _T>
constexpr _T *addressof(_T &arg) noexcept {
	return __builtin_addressof(arg);
}

template <class _T>
const _T *addressof(const _T &&) = delete;

std::addressof

#endif