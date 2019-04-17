#include <ala/vector.h>
#include <iostream>
#include <vector>

struct t {
    void f(){};
};

int main() {
    using namespace ala;
    vector<int> v = {1, 2, 3, 4};
    auto k = v.erase(v.begin());
    for (const auto &i : v)
        std::cout << i << ",";
    v.push_back(1);
    assert((v == vector{2, 3, 4}));
    for (const auto &i : v)
        std::cout << i << ",";
    return 0;
}