#ifndef _ALA_DETAIL_MEMORY_BASE_H
#define _ALA_DETAIL_MEMORY_BASE_H

#include <new>

namespace ala {

#if _ALA_ENABLE_ALIGNED_NEW
using ::std::align_val_t;
#endif

using ::std::bad_array_new_length;

template<class T>
const T *addressof(const T &&) = delete;

template<class T>
constexpr T *addressof(T &arg) noexcept {
    return __builtin_addressof(arg);
}

} // namespace ala

#endif // HEAD