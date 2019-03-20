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

using pr = ala::pair<const int, char>;
// using pr = std::pair<const int, char>;

#define TEST_SIZE 1000

void test() {
    ala::rb_tree<pr, value_compare<pr>, ala::allocator<pr>> rb{
        value_compare<pr>(), ala::allocator<pr>()};
    // for (int i = 0; i < TEST_SIZE; ++i)
    //     rb.insert(ala::pair(i, 0));

    // for (int i = 0; i < TEST_SIZE; i += 7)
    //     rb.erase(ala::pair(i, 0));

    // std::cout << (*rb.begin()).first << ",";
    // std::cout << (*--rb.end()).first << ",";

    // for (int i = 0; i < TEST_SIZE; i += 1)
    //     rb.search(ala::pair<const int, char>(i, 0));

    // for (auto i = rb.begin(); i != rb.end(); ++i)
    //     std::cout << (*i)._data.first << ",";

    // for (auto i = --rb.end(); i != rb.begin(); --i)
    //     std::cout << (*i)._data.second << ",";
}

void test1() {
    std::map<const int, char> rb;
    for (int i = 0; i < TEST_SIZE; ++i)
        rb.insert(std::pair(i, 0));

    for (int i = 0; i < TEST_SIZE; i += 7)
        rb.erase(i);
    std::cout << (*rb.begin()).first << ",";
    std::cout << (*--rb.end()).first << ",";
}
//@build_type=dbg
int main() {
    // static_assert(std::is_copy_assignable<const int>::value, "oh");
    static_assert(ala::is_default_constructible<const int>::value &&
                      ala::is_default_constructible<char>::value,
                  "oh");

    // static_assert(ala::is_copy_assignable<const int>::value, "oh");
    // static_assert(std::is_copy_assignable<const int>::value, "oh");

    static_assert(ala::is_implicitly_default_constructible<const int>::value &&
                      ala::is_implicitly_default_constructible<char>::value,
                  "oh");

    static_assert(std::is_default_constructible<const int>::value &&
                      std::is_default_constructible<char>::value,
                  "oh");

    static_assert(
        std::__is_implicitly_default_constructible<const int>::value &&
            std::__is_implicitly_default_constructible<char>::value,
        "oh");
    std::cout << ala::timer(test);
    std::cout << ala::timer(test1);
    return 0;
}