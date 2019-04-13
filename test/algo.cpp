#include <iostream>
#include <ala/algorithm.h>

#include <vector>
#include <algorithm>
using namespace std;

struct OK {
    ~OK() {
        std::cout << "oh";
    }
    bool operator==(const OK &) {
        return true;
    };
};

int main() {
    int a[] = {1, 2, 3, 4, 5, 6};
    // ala::rotate(a, a + 1, a + 6);
    // ala::rotate(a, a + 3, a + 6);
    ala::rotate(a, a + 5, a + 6);
    vector<OK> v;
    v.push_back(OK());
    v.push_back(OK());
    v.push_back(OK());
    v.push_back(OK());
    v.push_back(OK());
    v.push_back(OK());
    std::cout << "\n";
    std::remove_if(v.begin(), v.end(), [](auto &&) { return true; });
    return 0;
}