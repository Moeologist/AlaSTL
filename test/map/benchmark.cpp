#include <ala/map.h>
#include <ala/set.h>
#include <map>
#include <ala/detail/xorshift.h>
#include <ala/timer.h>
#include <iostream>

ala::detail::xoshiro128p x{10086};
ala::detail::xoshiro128p y{10086};
ala::detail::xoshiro<int, 1, 2, true, 0> xx;

constexpr auto sz = 100;

void test() {
    // ala::map<int, char> mx;
    // for (int i = 0; i < 5; ++i)
    //     mx[i] = 'L';
    // for (auto i = mx.begin(); i != mx.end();)
    //     i = mx.erase(i);

    ala::map<int, char> m, n;
    for (int k = 0; k < sz; ++k) {
    for (int i = 0; i < k; ++i)
        m[i] = 'L';
    // for (int i = 0; i < sz; ++i)
    //     m[i] = 'L';
    // for (int i = 0; i < sz; ++i)
    //     n[x()] = 'L';

    for (int i = 0; i < k; ++i)
        m.erase(i);}
    std::cout << m.size() << std::endl;
    for (auto i = m.begin(); i != m.end();)
        i = m.erase(i);
    std::cout << m.size() << std::endl;

    // size_t sz = m.size() + n.size();
    // m.merge(n);
    // m.merge(ala::map<int, char>{});
    // std::cout << (sz == m.size() + n.size());
}

void test1() {
    std::map<int, char> m, n;
    int r = 0;
    for (int i = 0; i < sz; ++i)
        m[y()] = 'L';
    for (int i = 0; i < sz; ++i)
        m[i] = 'L';
    for (int i = 0; i < sz; ++i)
        n[y()] = 'L';

    for (int i = 0; i < sz; ++i)
        m.erase(i);
    for (auto i = m.begin(); i != m.end();)
        i = m.erase(i);
    std::cout << m.size() << std::endl;

    // size_t sz = m.size() + n.size();
    // m.merge(n);
    // std::cout << (sz == m.size() + n.size());
}
//@build_type=dbg

// cflags=-fsanitize=address
int main() {
    std::cout << ala::timer(test);
    std::cout << ala::timer(test1);
    // ala::map<int, int> m;
    // ala::map<int, int> n;
    // for (int i = 0; i < 5; ++i)
    //     m[i] = n[i + 10] = 0;

    return 0;
}