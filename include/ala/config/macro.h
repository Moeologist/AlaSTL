#ifndef _ALA_CONFIG_MACRO_H
#define _ALA_CONFIG_MACRO_H

#ifdef _ALA_CPP17
	#define _ALA_VARIABLE_INLINE inline
#else
	#define _ALA_VARIABLE_INLINE	
#endif

#ifdef _ALA_DEBUG
	#define _ALA_FORCEINLINE
	#define _ALA_NOINLINE
#else
	#if defined(_ALA_MSVC) || defined(_ALA_CLANG_MSVC)
		#define _ALA_FORCEINLINE __forceinline
		#define _ALA_NOINLINE __declspec(noinline)
	#else
		#define _ALA_FORCEINLINE __attribute__((always_inline))
		#define _ALA_NOINLINE __attribute__((noinline))
	#endif
#endif

#endif // HEAD