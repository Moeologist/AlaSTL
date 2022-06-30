#ifndef _ALA_DEQUE_H
#define _ALA_DEQUE_H

#include <ala/ring.h>

namespace ala {

template<class T, class Alloc = allocator<T>>
using deque = ring<T, Alloc>;

} // namespace ala

#endif