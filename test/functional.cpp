#include <iostream>
#include <ala/functional.h>
#include <functional>
#include <iostream>

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

struct Foo {
    Foo(int num): num_(num) {}
    void print_add(int i) && {
        std::cout << num_ + i << '\n';
    }
    int num_;
};

void print_num(int i) {
    std::cout << i << '\n';
}

struct PrintNum {
    void operator()(int i) & {
        std::cout << i << '\n';
    }
};

template<class...>
class FK;

struct placeholder {};

static_assert(ala::is_nothrow_move_constructible_v<placeholder>);

int osp(int x, int y, int z) {
    return x + y + z;
}

int osp(int x, int y) {
    return x + y;
}

int main() {
    using namespace ala;
    int (*po)(int, int, int) = osp;

    auto bd = bind(po, 1,
                   bind(po, placeholders::_1, placeholders::_2, placeholders::_3),
                   placeholders::_2);

    auto bdr = bd(1, 2, 3);
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
    auto t = 0b1000;
    // FK<decltype(t)> ohshit;

    // 存储到 bind 调用的结果
    // function<void()> f_display_31337 = bind(print_num, 31337);
    // f_display_31337();

    // 存储到成员函数的调用
    function att = PrintNum();
    att(1);

    typedef int ftp(int,int);

    const auto &padds = &Foo::print_add;


    function<void( Foo &&, int)> f_add_display = padds;
    function f_add_display1 = padds;
    static_assert(is_same<decltype(f_add_display), decltype(f_add_display1)>::value);
    static_assert(is_member_function_pointer_v<decltype(&Foo::print_add)>);
    Foo foo(314159);
    f_add_display(ala::move(foo), 1);
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