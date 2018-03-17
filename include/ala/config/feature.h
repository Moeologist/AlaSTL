#ifndef _ALA_CONFIG_FEATURE_H
#define _ALA_CONFIG_FEATURE_H

#ifndef _ALA_CONFIG_H
	#error "Never use this head directly; include config.hpp instead."
#endif

#ifndef ALA_ENABLE_RTTI
	#define ALA_ENABLE_RTTI 0
#endif

#ifndef ALA_ENABLE_EXCEPTIONS
	#define ALA_ENABLE_EXCEPTIONS 0
#endif

#ifndef ALA_ENABLE_TYPE_TRAITS_BIF
	#ifdef ALA_GCC
		#define ALA_ENABLE_TYPE_TRAITS_BIF 0
	#else
		#define ALA_ENABLE_TYPE_TRAITS_BIF 1
	#endif
#else
	#if defined(ALA_GCC) && ALA_ENABLE_TYPE_TRAITS_BIF
		#error "You can't define ALA_ENABLE_TYPE_TRAITS_BIF = 1 when using gcc."
	#endif
#endif

#endif // HEAD