#ifndef _ALA_DETAIL_MEMORY_BASE_H
#define _ALA_DETAIL_MEMORY_BASE_H

#include <new>

namespace ala {

#if _ALA_ENABLE_ALIGNED_NEW
using ::std::align_val_t;
#endif

using ::std::bad_array_new_length;

} // namespace ala

#endif // HEAD