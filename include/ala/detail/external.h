#ifndef _ALA_DETAIL_EXTERNAL_H
#define _ALA_DETAIL_EXTERNAL_H

#include <initializer_list>
#include <new>
#include <stdexcept>

#include <cassert>

#include <cstdint>
#include <cstddef>

#include <cstdint>
#include <climits>
#include <cfloat>

#include <cmath>

namespace ala {
using std::out_of_range;
using std::initializer_list;

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

}

#endif