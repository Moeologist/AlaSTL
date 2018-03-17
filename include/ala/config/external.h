#ifndef _ALA_CONFIG_EXTERNAL_H
#define _ALA_CONFIG_EXTERNAL_H

#ifndef _ALA_CONFIG_H
#error "Never use this head directly; include config.hpp instead."
#endif

#include <cstdlib>
#include <cassert>

// #include <initializer_list>

#if ALA_ENABLE_RTTI
	// #include <typeinfo>
#endif

#if ALA_ENABLE_EXCEPTIONS
	// #include <exception>
#endif

#endif // HEAD