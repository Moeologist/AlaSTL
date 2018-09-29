#ifndef _ALA_CONFIG_FEATURE_H
#define _ALA_CONFIG_FEATURE_H

#ifndef _ALA_ENABLE_RTTI
	#define ALA_ENABLE_RTTI 0
#endif

#ifndef _ALA_ENABLE_EXCEPTIONS
	#define ALA_ENABLE_EXCEPTIONS 0
#endif

#ifndef _ALA_ENABLE_TYPE_TRAITS_BIF
	#ifdef _ALA_MSVC
		#define _ALA_ENABLE_TYPE_TRAITS_BIF 1
	#else
		#define _ALA_ENABLE_TYPE_TRAITS_BIF 0
	#endif
#else
	#if defined(_ALA_GCC) && _ALA_ENABLE_TYPE_TRAITS_BIF
		#error "You can't define _ALA_ENABLE_TYPE_TRAITS_BIF = 1 when using gcc."
	#endif
#endif

#endif // HEAD