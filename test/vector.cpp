#include <ala/vector.h>
#include <iostream>
#include <vector>

struct X {
    ala::vector<X> data;
};

int main() {
    using namespace ala;
    X x;
    vector<int> v = {1, 2, 3, 4};
    auto k = v.erase(v.begin());
    for (const auto &i : v)
        std::cout << i << ",";
    v.push_back(1);
    // assert((v == vector<int>{2, 3, 4}));
    for (const auto &i : v)
        std::cout << i << ",";
    std::cout << __WCHAR_MAX__;
    return 0;
}