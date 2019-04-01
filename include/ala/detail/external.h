#ifndef _ALA_DETAIL_EXTERNAL_H
#define _ALA_DETAIL_EXTERNAL_H

#include <new>

#include <stdexcept>
#include <initializer_list>
#include <limits>

#include <cassert>

#include <cstdint>
#include <cstddef>

#include <cmath>

namespace ala {
using std::exception;
using std::out_of_range;

using std::initializer_list;

using std::numeric_limits;

typedef decltype(nullptr) nullptr_t;
using std::size_t;
using std::ptrdiff_t;
using std::max_align_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int_fast8_t;
using std::int_fast16_t;
using std::int_fast32_t;
using std::int_fast64_t;
using std::int_least8_t;
using std::int_least16_t;
using std::int_least32_t;
using std::int_least64_t;
using std::intmax_t;
using std::intptr_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint_fast8_t;
using std::uint_fast16_t;
using std::uint_fast32_t;
using std::uint_fast64_t;
using std::uint_least8_t;
using std::uint_least16_t;
using std::uint_least32_t;
using std::uint_least64_t;
using std::uintmax_t;
using std::uintptr_t;

using std::abs;
using std::fabs;
using std::fmod;
using std::remainder;
using std::remquo;
using std::fma;
using std::fmax;
using std::fmin;
using std::fdim;
using std::nan;
using std::exp;
using std::exp2;
using std::expm1;
using std::log;
using std::log10;
using std::log2;
using std::log1p;
using std::pow;
using std::sqrt;
using std::cbrt;
using std::hypot;
using std::sin;
using std::cos;
using std::tan;
using std::asin;
using std::acos;
using std::atan;
using std::atan2;
using std::sinh;
using std::cosh;
using std::tanh;
using std::asinh;
using std::acosh;
using std::atanh;

} // namespace ala

#endif