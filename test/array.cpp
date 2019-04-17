#include <ala/array.h>
#include <array>
#include <ala/tuple.h>
#include <iostream>

template<class...>
struct FK;

struct Foo {
    Foo(int num): num_(num) {}
    void print_add(int i) const {
        std::cout << num_ + i << '\n';
    }
    int num_;
};

void print_num(int i) {
    std::cout << i << '\n';
}

struct PrintNum {
    void operator()(int i) const {
        std::cout << i << '\n';
    }
};

int test() {
    // 调用自由函数
    ala::invoke(print_num, -9);

    // 调用 lambda
    ala::invoke([]() { print_num(42); });

    // 调用成员函数
    const Foo foo(314159);
    ala::invoke(&Foo::print_add, &foo, 1);

    // 调用（访问）数据成员
    std::cout << "num_: " << ala::invoke(&Foo::num_, foo) << '\n';

    // 调用函数对象
    ala::invoke(PrintNum(), 18);
}

int main() {
    test();
    using namespace ala;
    using ar = array<int, 2>;
    constexpr ar x = {};
    ar y = {1, 2};
    apply([](auto &&x, auto &&y) { ++x + y; }, y);
    x.cbegin();
    x.crbegin();
    return 0;
}