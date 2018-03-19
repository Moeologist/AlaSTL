#ifndef _ALA_TIMER_H
#define _ALA_TIMER_H

#include <string>

#ifdef _WIN32
#include <windows.h>
#elif defined __linux
#include <time.h>
#endif

namespace ala {

#ifdef _WIN32

template <class Callable, class... Args>
std::string timer(Callable callable, Args... args) {
	std::string units[] = {"ns", "us", "ms", "s"};
	LARGE_INTEGER before, after, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&before);
	callable(args...);
	QueryPerformanceCounter(&after);
	long long ns_time = (after.QuadPart - before.QuadPart) * 1000 / (frequency.QuadPart / 1000000);
	int unit_index = 0;
	while (ns_time > 10000 && unit_index < 4) {
		++unit_index;
		ns_time /= 1000;
	}
	return std::to_string(ns_time) + "" + units[unit_index];
}

#elif defined __linux

template <class Callable, class... Args>
std::string timer(Callable callable, Args... args) {
	std::string units[] = {"ns", "us", "ms", "s"};
	struct timespec before, after;
	clock_gettime(CLOCK_MONOTONIC, &before);
	callable(args...);
	clock_gettime(CLOCK_MONOTONIC, &after);
	long long ns_time = after.tv_nsec - before.tv_nsec;
	int unit_index = 0;	
	while (ns_time > 10000 && unit_index < 4) {
		++unit_index;
		ns_time /= 1000;
	}
	return std::to_string(ns_time) + "" + units[unit_index];
}

#endif

} // namespace ala

#endif // HEAD