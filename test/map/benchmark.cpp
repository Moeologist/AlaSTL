#include <ala/map.h>
#include <ala/set.h>
#include <map>
#include <ala/random.h>
#include <ala/timer.h>
#include <iostream>

ala::xoshiro128p x{10086};
ala::xoshiro128p y{10086};
ala::xoshiro<int, 1, 2, true, 0> xx;

constexpr auto sz = 1000;

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
            m.erase(i);
    }
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

int t() {
    ala::xoshiro256p x;
    auto s = .0;
    for (int i = 0; i < 100000000; ++i)
        s = ala::generate_real(x);
    return s;
}

int t1() {
    ala::xoshiro256p x;
    auto s = 0;
    for (int i = 0; i < 100000000; ++i)
        s = x();
    return s;
}

int t2() {
    ala::xoshiro256ss x;
    auto s = 0;
    for (int i = 0; i < 100000000; ++i)
        s += x();
    return s;
}

int t3() {
    ala::minstd_rand x;
    auto s = 0;
    for (int i = 0; i < 100000000; ++i)
        s += x();
    return s;
}
int main() {
    std::cout << ala::timer(t);
    std::cout << ala::timer(t1);
    std::cout << ala::timer(t2);
    std::cout << ala::timer(t3);
    // std::cout << ala::timer(test);
    // std::cout << ala::timer(test1);
    ala::set<int> st;
    return 0;
}