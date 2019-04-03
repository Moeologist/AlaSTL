#include <ala/map.h>
#include <ala/multimap.h>
#include <ala/set.h>
#include <ala/multiset.h>
#include <map>
#include <ala/random.h>
#include <ala/timer.h>
#include <iostream>

ala::xoshiro128p x{10086};
ala::xoshiro128p y{10086};
ala::xoshiro<int, 1, 2, true, 0> xx;

constexpr auto bp = 10;

void test() {
    ala::map<int, char> m, n;
    for (int sz = 0; sz < bp; ++sz) {
        for (int i = 0; i < sz; ++i)
            m[x()] = 'L';
        for (int i = 0; i < sz; ++i)
            m[i] = 'L';
        for (int i = 0; i < sz; ++i)
            n[x()] = 'L';
        std::cout << "sz:" << m.size() + n.size() << "\n";
        // m.merge(n);
        // m.merge(ala::map<int, char>{});
        std::cout << "sz:" << m.size() + n.size() << "\n";
        for (int i = 0; i < sz; ++i)
            m.erase(i);
        for (auto i = m.begin(); i != m.end();)
            i = m.erase(i);
        std::cout << m.size() << std::endl;
    }
}

void test1() {
    std::map<int, char> m, n;
    for (int sz = 0; sz < bp; ++sz) {
        for (int i = 0; i < sz; ++i)
            m[y()] = 'L';
        for (int i = 0; i < sz; ++i)
            m[i] = 'L';
        for (int i = 0; i < sz; ++i)
            n[y()] = 'L';
        std::cout << "sz:" << m.size() + n.size() << "\n";
        // m.merge(n);
        // m.merge(ala::map<int, char>{});
        std::cout << "sz:" << m.size() + n.size() << "\n";
        for (int i = 0; i < sz; ++i)
            m.erase(i);
        for (auto i = m.begin(); i != m.end();)
            i = m.erase(i);
        std::cout << m.size() << std::endl;
    }
}
//@build_type=dbg

// cflags=-fsanitize=address

int main() {
    std::cout << ala::timer(test);
    std::cout << ala::timer(test1);
    ala::set<int> st;
    ala::multimap<int, char> mmp;
    mmp.insert(ala::pair(123, 'c'));
    mmp.insert(ala::pair(123, 'c'));
    mmp.insert(ala::pair(123, 'c'));
    mmp.insert(ala::pair(123, 'c'));
    mmp.insert(ala::pair(123, 'c'));
    mmp.insert(ala::pair(001, 'c'));
    assert(mmp.count(123) == 5);
    assert((mmp.equal_range(123) == ala::pair(++mmp.begin(), mmp.end())));
    auto s = mmp.erase(123);
    return 0;
}