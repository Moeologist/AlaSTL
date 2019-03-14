#ifndef _ALA_TIMER_H
#define _ALA_TIMER_H

#include "ala/config.h"
#include <string>

#ifdef _ALA_WIN
#include <windows.h>
#elif defined _ALA_LINUX
#include <time.h>
#elif defined _ALA_MAC
#include <mach/mach_time.h>
#endif

namespace ala {

#ifdef _ALA_WIN

template<class Callable, class... Args>
std::string timer(Callable callable, Args... args) {
    std::string units[] = {"ns", "us", "ms", "s"};
    LARGE_INTEGER before, after, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&before);
    callable(args...);
    QueryPerformanceCounter(&after);
    long long ns_time = (after.QuadPart - before.QuadPart) * 1000 /
                        (frequency.QuadPart / 1000000);
    int unit_index = 0;
    while (ns_time > 10000 && unit_index < 4) {
        ++unit_index;
        ns_time /= 1000;
    }
    return std::to_string(ns_time) + "" + units[unit_index];
}

#elif defined _ALA_LINUX

template<class Callable, class... Args>
std::string timer(Callable callable, Args... args) {
    std::string units[] = {"ns", "us", "ms", "s"};
    struct timespec before, after;
    clock_gettime(CLOCK_MONOTONIC, &before);
    callable(args...);
    clock_gettime(CLOCK_MONOTONIC, &after);
    long long ns_time = ((after.tv_nsec - before.tv_nsec) +
                         (after.tv_sec - before.tv_sec) * 1000000000);
    int unit_index = 0;
    while (ns_time > 10000 && unit_index < 4) {
        ++unit_index;
        ns_time /= 1000;
    }
    return std::to_string(ns_time) + "" + units[unit_index];
}

#elif defined _ALA_MAC

template<class Callable, class... Args>
std::string timer(Callable callable, Args... args) {
    std::string units[] = {"ns", "us", "ms", "s"};
    uint64_t before, after;
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    before = mach_absolute_time();
    callable(args...);
    after = mach_absolute_time();
    long long ns_time = (after - before) * info.denom / info.numer;
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