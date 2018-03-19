#ifndef _ALA_CONFIG_FEATURE_H
#define _ALA_CONFIG_FEATURE_H

#ifndef ALA_ENABLE_RTTI
	#define ALA_ENABLE_RTTI 0
#endif

#ifndef ALA_ENABLE_EXCEPTIONS
	#define ALA_ENABLE_EXCEPTIONS 0
#endif

#ifndef ALA_ENABLE_TYPE_TRAITS_BIF
	#ifdef _ALA_MSVC
		#define ALA_ENABLE_TYPE_TRAITS_BIF 1
	#else
		#define ALA_ENABLE_TYPE_TRAITS_BIF 0
	#endif
#else
	#if defined(ALA_GCC) && ALA_ENABLE_TYPE_TRAITS_BIF
		#error "You can't define ALA_ENABLE_TYPE_TRAITS_BIF = 1 when using gcc."
	#endif
#endif

#endif // HEAD