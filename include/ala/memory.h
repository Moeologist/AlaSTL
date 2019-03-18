#ifndef _ALA_MEMORY_H
#define _ALA_MEMORY_H

// template <class T>
// constexpr T *addressof(T &arg) noexcept {
// 	return reinterpret_cast<T *>(&const_cast<char &>(
// 	    reinterpret_cast<const volatile char &>(arg)));
// }

namespace ala {

template<class T>
constexpr T *addressof(T &arg) noexcept {
    return __builtin_addressof(arg);
}

template<class T>
const T *addressof(const T &&) = delete;

} // namespace ala

#include <ala/detail/external/allocator.h>

#endif // HEAD