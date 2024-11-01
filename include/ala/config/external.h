#ifndef _ALA_CONFIG_EXTERNAL_H
#define _ALA_CONFIG_EXTERNAL_H

#include <stdexcept>
#include <initializer_list>

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cmath>

namespace ala {
using ::std::exception;
using ::std::out_of_range;
using ::std::overflow_error;

using ::std::initializer_list;

using ::std::memmove;
using ::std::memcpy;
using ::std::memset;

using ::std::nullptr_t;

using ::std::size_t;
using ::std::ptrdiff_t;
using ::std::max_align_t;

using ::std::int8_t;
using ::std::int16_t;
using ::std::int32_t;
using ::std::int64_t;
using ::std::int_fast8_t;
using ::std::int_fast16_t;
using ::std::int_fast32_t;
using ::std::int_fast64_t;
using ::std::int_least8_t;
using ::std::int_least16_t;
using ::std::int_least32_t;
using ::std::int_least64_t;
using ::std::intmax_t;
using ::std::intptr_t;
using ::std::uint8_t;
using ::std::uint16_t;
using ::std::uint32_t;
using ::std::uint64_t;
using ::std::uint_fast8_t;
using ::std::uint_fast16_t;
using ::std::uint_fast32_t;
using ::std::uint_fast64_t;
using ::std::uint_least8_t;
using ::std::uint_least16_t;
using ::std::uint_least32_t;
using ::std::uint_least64_t;
using ::std::uintmax_t;
using ::std::uintptr_t;

} // namespace ala

#endif