#ifndef _ALA_DETAIL_EXTERNAL_STDDEF_H
#define _ALA_DETAIL_EXTERNAL_STDDEF_H

#include <cstdint>
#include <cstddef>

namespace ala {

typedef decltype(nullptr) nullptr_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

using std::size_t;
using std::ptrdiff_t;
using std::intptr_t;

} // namespace ala

#endif // HEAD