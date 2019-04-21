#include <iostream>
#include <ala/functional.h>
#include <functional>
#include <iostream>

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

template<class...>
class FK;

struct X {
    X() {
        std::cout << "def";
    };
    // ~X() {
    //     std::cout << "des";
    // }
    // ~X() = default;
    X(int) {
        std::cout << "int";
    }
    X(const X &) {
        std::cout << "cp";
    }
    // X(const X &) = default;
    X(X &&) {
        std::cout << "mv";
    }
    // X(X &&) = default;
    bool x() {
        return true;
    }
    X &operator=(X &&) = default;
    X &operator=(const X &) = default;
};

int f(int, int) {
    return 1;
}
int g(int, int) {
    return 2;
}
void test(ala::function<int(int, int)> const &arg) {
    using namespace ala;
    std::cout << "test function: ";
    if (arg.target<plus<int>>())
        std::cout << "it is plus\n";
    if (arg.target<minus<int>>())
        std::cout << "it is minus\n";

    int (*const *ptr)(int, int) = arg.target<int (*)(int, int)>();
    if (ptr && *ptr == f)
        std::cout << "it is the function f\n";
    if (ptr && *ptr == g)
        std::cout << "it is the function g\n";
}

void xmain() {
    using namespace ala;
    test(function<int(int, int)>(plus<int>()));
    test(function<int(int, int)>(minus<int>()));
    test(function<int(int, int)>(f));
    test(function<int(int, int)>(g));
}

int main() {
    xmain();
    using namespace ala;

    // FK<decltype(mp)> ohshit;

    // 存储自由函数
    function<void(int)> f_display = print_num;
    f_display(-9);

    function<void(int)> f_di = ala::move(f_display);
    f_display(-9);

    function<void(int)> x;
    x = ala::move(f_display);

    // 存储 lambda
    function<void()> f_display_42 = []() { print_num(42); };
    f_display_42();

    // 存储到 bind 调用的结果
    // function<void()> f_display_31337 = bind(print_num, 31337);
    // f_display_31337();

    // 存储到成员函数的调用

    const auto &padds = &Foo::print_add;
    function<void(const Foo &, int)> f_add_display = padds;
    const Foo foo(314159);
    f_add_display(foo, 1);
    f_add_display(314159, 1);

    // 存储到数据成员访问器的调用
    function<int(Foo const &)> f_num = &Foo::num_;
    std::cout << "num_: " << f_num(foo) << '\n';

    // 存储到成员函数及对象的调用
    // using placeholders::_1;
    // function<void(int)> f_add_display2 = bind(&Foo::print_add, foo, _1);
    // f_add_display2(2);

    // 存储到成员函数和对象指针的调用
    // function<void(int)> f_add_display3 = bind(&Foo::print_add, &foo, _1);
    // f_add_display3(3);

    // 存储到函数对象的调用
    function<void(int)> f_display_obj = PrintNum();
    f_display_obj(18);

    // bf f = &Ok::operator();

    // ala::function f(ala::plus<int>{});

    return 0;
}