#include <ala/map.h>
#include <ala/set.h>
#include <map>
#include <ala/random.h>
#include <ala/timer.h>
#include <iostream>

constexpr auto bp = 10000;

void test() {
    using namespace ala;
    ala::xoshiro128p x{10086};
    map<int, char> m, n;
    for (int sz = 0; sz < bp; ++sz) {
        for (int i = 0; i < sz; ++i)
            m[x()] = 'L';
        for (int i = 0; i < sz; ++i)
            m[i] = 'L';
        for (int i = 0; i < sz; ++i)
            n[i] = 'L';
        for (int i = 0; i < sz; ++i)
            n[x()] = 'L';
        // auto sm = m.size() + n.size();
        // m.merge(n);
        // m.merge(map<int, char>{});
        // assert(sm == m.size() + n.size());
        for (int i = 0; i < sz; ++i)
            m.erase(i);
        for (auto i = m.begin(); i != m.end();)
            i = m.erase(i);
        // std::cout << m.size() << std::endl;
    }
}

void test1() {
    using namespace std;
    map<int, char> m, n;
    ala::xoshiro128p x{10086};
    for (int sz = 0; sz < bp; ++sz) {
        for (int i = 0; i < sz; ++i)
            m[x()] = 'L';
        for (int i = 0; i < sz; ++i)
            m[i] = 'L';
        for (int i = 0; i < sz; ++i)
            n[i] = 'L';
        for (int i = 0; i < sz; ++i)
            n[x()] = 'L';
        // auto sm = m.size() + n.size();
        // m.merge(n);
        // m.merge(map<int, char>{});
        // assert(sm == m.size() + n.size());
        for (int i = 0; i < sz; ++i)
            m.erase(i);
        for (auto i = m.begin(); i != m.end();)
            i = m.erase(i);
        // std::cout << m.size() << std::endl;
    }
}
//@build_type=rel

// cflags=-fsanitize=address
template<typename _type_, typename = ala::void_t<>>
struct _has_construct: ala::false_type {};
template<typename _type_>
struct _has_construct<_type_, ala::void_t<decltype(&_type_::construct)>>
    : ala::true_type {};

struct t {
    void f(){};
    int p;
};

ALA_HAS_MEM(f)
ALA_HAS_MEM(p)
ALA_HAS_MEM(cc)

static_assert(_has_f<t>::value);
static_assert(_has_p<t>::value);

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