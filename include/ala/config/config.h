#ifndef _ALA_CONFIG_CONFIG_H
#define _ALA_CONFIG_CONFIG_H

#if defined(_MSC_VER) && _MSC_VER >= 1900 // msvc or clang msvc target
	#if defined(__clang__) && defined(__clang_major__) && defined(__clang_minor__)
		#define _ALA_CLANG
		#define _ALA_CLANG_MSVC
		#if __clang_major__ == 3 && __clang_minor__ < 4 || __clang_major__ < 3
			#error "Unsupported compiler; ala need c++14 supported compiler at least."
		#endif
	#else
		#define _ALA_MSVC
	#endif

	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif

	#ifdef _M_AMD64
		#define _ALA_X86
	#else
		#define _ALA_X64
	#endif // Architectures

#elif defined(__GNUC__) // gcc or clang gnu target

	#if defined(__clang__) && defined(__clang_major__) && defined(__clang_minor__)
		#define _ALA_CLANG
		#define _ALA_CLANG_GNU
		#if __clang_major__ == 3 && __clang_minor__ < 4 || __clang_major__ < 3
			#error "Unsupported compiler; ala need c++14 supported compiler at least."
		#endif
	#else
		#define _ALA_GCC
		#if __GNUC__ < 5
			#error "Unsupported compiler; ala need c++14 supported compiler at least."
		#endif
	#endif

	#ifdef __x86_64__
		#define _ALA_X64
	#else
		#define _ALA_X86
	#endif // Architectures

#else
	#error "Unsupported compiler; ala need c++14 supported compiler at least."
#endif // Compilers

#ifdef _WIN32
	#define _ALA_WIN
	#ifdef _WIN64
		#define _ALA_WIN64
	#else
		#define _ALA_WIN32
	#endif
#elif defined(__linux__)
	#define _ALA_LINUX
#elif defined(__APPLE__)
	#define _ALA_MAC
#endif

#ifndef _ALA_DEBUG
	#if defined(_DEBUG) || defined(DEBUG)
		#define _ALA_DEBUG
	#endif
#endif

#if __cplusplus >= 201703
	#define _ALA_CPP17
#endif // language standard

#endif // HEAD