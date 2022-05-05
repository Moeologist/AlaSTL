#include "trick.h"

inline int f(int) {
    std::cout << "Hello\n";
    return 0;
}

int main() {
    auto l = lazy([&]() { return 0; });
    DELAY(
        std::cout << "Hello\n";
    );
    int a = 0;
    auto x = LAZY(f(0));
    a = x;
    auto p = make_property(
        [&a]() -> decltype(a) {
            std::cout << "getter";
            return a;
        },
        [&a](const int &x) -> decltype(a) {
            std::cout << "setter";
            if (x < 0)
                a = x;
            return a;
        });

    p = -1;
    p = 3;

    std::cout << p;
}