#include <ala/map.h>
#include <ala/set.h>
#include <map>
#include <ala/detail/xorshift.h>
#include <ala/timer.h>
#include <iostream>


ala::detail::xoshiro128p x{10086};
ala::detail::xoshiro128p y{10086};
ala::detail::xoshiro<int, 1, 2, true, 0> xx;

constexpr auto sz = 10000;

int test() {
    // for (int i = 0; i < 62; ++i)
    //     x.jump();
    xx.jump();
    x.jump();
    x();
    y();
    y.jump();
    x.jump();
    x.jump();
    ala::map<int, int> m;
    int r = 0;
    for (int i = 0; i < sz; ++i)
        m[x()] = i;
    for (int i = 0; i < sz; ++i)
        m[i] = i;
    std::cout << m.size() << std::endl;
    // for (int i = 0; i < sz; ++i)
    //     m.erase(i);
    // std::cout << m.size() << std::endl;
    return r;
}

int test1() {
    std::map<int, int> m;
    int r = 0;
    for (int i = 0; i < sz; ++i)
        m[y()] = i;
    for (int i = 0; i < sz; ++i)
        m[i] = i;
    std::cout << m.size() << std::endl;
    // for (int i = 0; i < sz; ++i)
    //     m.erase(i);
    // std::cout << m.size() << std::endl;
    return r;
}
//@build_type=dbg
int main() {
    std::cout << ala::timer(test);
    std::cout << ala::timer(test1);
    // ala::map<int, int> m;
    // ala::map<int, int> n;
    // for (int i = 0; i < 5; ++i)
    //     m[i] = n[i + 10] = 0;

    return 0;
}