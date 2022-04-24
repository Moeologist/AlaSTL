#ifndef _ALA_TIMER_H
#define _ALA_TIMER_H

#include <ala/type_traits.h>

#ifdef _ALA_WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <Windows.h>
#elif defined _ALA_LINUX
    #include <time.h>
#elif defined _ALA_APPLE
    #include <mach/mach_time.h>
#endif

namespace ala {

namespace util {

#ifdef _ALA_WIN32

template<class Fn, class... Args>
long long timer(Fn &&fn, Args &&... args) {
    std::string units[] = {"ns", "us", "ms", "s"};
    LARGE_INTEGER before, after, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&before);
    ala::invoke(ala::forward<Fn>(fn), ala::forward<Args>(args)...);
    QueryPerformanceCounter(&after);
    long long ns_time = (after.QuadPart - before.QuadPart) * 1000 /
                        (frequency.QuadPart / 1000000);
    return ns_time;
}

#elif defined _ALA_LINUX

template<class Fn, class... Args>
long long timer(Fn &&fn, Args &&... args) {
    std::string units[] = {"ns", "us", "ms", "s"};
    struct timespec before, after;
    clock_gettime(CLOCK_MONOTONIC, &before);
    ala::invoke(ala::forward<Fn>(fn), ala::forward<Args>(args)...);
    clock_gettime(CLOCK_MONOTONIC, &after);
    long long ns_time = ((after.tv_nsec - before.tv_nsec) +
                         (after.tv_sec - before.tv_sec) * 1000000000);
    return ns_time;
}

#elif defined _ALA_APPLE

template<class Fn, class... Args>
long long timer(Fn &&fn, Args &&... args) {
    std::string units[] = {"ns", "us", "ms", "s"};
    uint_fast64_t before, after;
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    before = mach_absolute_time();
    ala::invoke(ala::forward<Fn>(fn), ala::forward<Args>(args)...);
    after = mach_absolute_time();
    long long ns_time = (after - before) * info.denom / info.numer;
    return ns_time;
}

#endif

} // namespace util

} // namespace ala

#endif // HEAD