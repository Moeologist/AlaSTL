#ifndef TIMER_HPP
#define TIMER_HPP

#ifdef _WIN32

#include <windows.h>
template <class Callable, class... Args>
long long timer(Callable callable, Args... args) {
	LARGE_INTEGER before, after, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&before);
	callable(args...);
	QueryPerformanceCounter(&after);
	return (after.QuadPart - before.QuadPart) * 1000000 / frequency.QuadPart;
}

#elif defined __linux

#include <time.h>
template <class Callable, class... Args>
long long timer(Callable callable, Args... args) {
	struct timespec before, after;
	clock_gettime(CLOCK_MONOTONIC, &before);
	callable(args...);
	clock_gettime(CLOCK_MONOTONIC, &after);
	return (after.tv_nsec - before.tv_nsec)/1000;
}

#endif

#endif