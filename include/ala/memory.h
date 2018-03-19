#ifndef _ALA_MEMORY_H
#define _ALA_MEMORY_H

// template <class _T>
// constexpr _T *addressof(_T &arg) noexcept {
// 	return reinterpret_cast<T *>(&const_cast<char &>(
// 	    reinterpret_cast<const volatile char &>(arg)));
// }

namespace ala {

template <class _T>
constexpr _T *addressof(_T &arg) noexcept {
	return __builtin_addressof(arg);
}

template <class _T>
const _T *addressof(const _T &&) = delete;

} // namespace ala

#include "ala/external/allocator.h"

#endif // HEAD