#ifndef CONFIG_EXTERNAL_HPP
#define CONFIG_EXTERNAL_HPP

#ifndef CONFIG_HPP
#error "Never use this head directly; include config.hpp instead."
#endif

#include <cstdlib>
#include <cassert>

#include <initializer_list>

#if ALA_ENABLE_RTTI
#include <typeinfo>
#endif

#if ALA_ENABLE_EXCEPTIONS
#include <exception>
#endif

#endif // CONFIG_EXTERNAL_HPP