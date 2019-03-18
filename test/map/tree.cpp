#include <ala/detail/rb_tree.h>
#include <ala/detail/pair.h>
#include <ala/timer.h>

#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

enum COLOR { BLACK = 0, RED = 1, NIL = 2 };

struct C {
    int x[100];
    C() = default;
    ~C() { std::cout << "fuck"; };
    C(const char &) { std::cout << "fuck"; }
};

template<class value_type>
struct value_compare {
public:
    bool operator()(const value_type &x, const value_type &y) const {
        return x.first < y.first;
    }
};

ala::rb_tree<ala::pair<const int, char>,
             value_compare<ala::pair<const int, char>>>
    rb;

void test() {
    for (int i = 0; i < 1000; ++i)
        rb.insert(ala::pair<const int, char>(i, 0));

    for (int i = 0; i < 1000; i += 7)
        rb.erase(ala::pair<const int, char>(i, 0));

    // for (int i = 0; i < 1000; i += 1)
    //     rb.search(ala::pair<const int, char>(i, 0));

    for (auto i = rb.begin(); i != rb.end(); ++i)
        std::cout << (*i)._data.first << ",";

    for (auto i = --rb.end(); i != rb.begin(); --i)
        std::cout << (*i)._data.second << ",";
}

int main() {
    std::cout << ala::timer(test);
    // auto x = rb._root;
    // r(x);
    return 0;
}