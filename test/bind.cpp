#include "bind.h"
#include <cassert>
#include <EASTL/functional.h>
#define BOOST_CHECK assert

static int g_Aconstruct = 0;
static int g_Acopy = 0;
static int g_Amove = 0;
static int g_MoveAmove = 0;
static int g_MoveAcopy = 0;

void reset_counter()
{
    g_Aconstruct = g_Acopy = g_Amove = g_MoveAmove = g_MoveAcopy = 0;
}

struct A
{
    explicit A(int i) : m_i(i) { ++g_Aconstruct; /*Dump("A::construct " << m_i);*/ };
    A(const A& other) : m_i(other.m_i) { ++g_Acopy; /*Dump("A::copy " << m_i);*/ }
    A(A&& other) : m_i(other.m_i) { other.m_i = 0; ++g_Amove; /*Dump("A::move " << m_i);*/ }
    ~A() = default;

    mutable int m_i{ 0 };
    int f() const
    {
        //DumpX(m_i);
        return (m_i *= 10);
    }

    int mem_func(int a, int b)
    {
        return a + b + m_i;
    }

    int mem_func_const(int a, int b) const
    {
        return a + b + m_i;
    }

    int mem_func_volatile(int a, int b) volatile
    {
        return a + b + m_i;
    }
};


int func(A construct, A copy, A & lref, A const& clref, A && rref)
{
    construct.f();
    copy.f();
    lref.f();
    clref.f();
    rref.f();
    A temp(eastl::move(rref));
    return 0;
}

struct object_func
{
    inline void operator()(int& a, int b) const
    {
        a += b;
    }
};

struct MoveA
{
    MoveA() = default;
    MoveA(MoveA&&) { ++g_MoveAmove; /*Dump("MoveA::move");*/ }
    ~MoveA() = default;

    /// TODO: 支持vs2013 ctp
    MoveA(MoveA const&)
    {
        ++g_MoveAcopy;
    }
    //MoveA& operator=(MoveA const&) = default;
    //MoveA& operator=(MoveA&&) = default;
};

int move_func(MoveA const& a)
{
    return 0;
}

void void_move_func(MoveA const& a)
{
}

void test()
{
    {
        /// 自由函数转发测试 (形参: 值传递, 左值引用, 常量左值引用, 右值引用)
        reset_counter();
        A a(1), b(2), c(3), d(4);
        auto f = simple::bind(&func, simple::_1, simple::_2, simple::_3, /*simple::_5*/eastl::ref(c), simple::_4);
        f(A(9), a, b, /*c,*/ eastl::move(d));
        BOOST_CHECK(a.m_i == 1);
        BOOST_CHECK(b.m_i == 20);
        BOOST_CHECK(c.m_i == 30);
        BOOST_CHECK(d.m_i == 0);

        BOOST_CHECK(g_Aconstruct == 5);
        BOOST_CHECK(g_Acopy == 1);
        BOOST_CHECK(g_Amove == 2);
    }

    //////////////////////////////////////////////////////////////////////////
    /// @{ 非静态成员函数 (cv)
    {
        reset_counter();
        A a(1);
        auto f = simple::bind(&A::mem_func, simple::_1, 1, simple::_2);
        BOOST_CHECK(f(a, 2) == 4);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    {
        reset_counter();
        const A a(1);
        auto f = simple::bind(&A::mem_func_const, simple::_1, 1, simple::_2);
        BOOST_CHECK(f(a, 2) == 4);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    {
        reset_counter();
        volatile A a(1);
        auto f = simple::bind(&A::mem_func_volatile, simple::_1, 1, simple::_2);
        BOOST_CHECK(f(a, 2) == 4);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    {
        reset_counter();
        A a(2);
        auto f = simple::bind(&A::f, eastl::ref(a));
        f();
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 仿函数
    {
        object_func obj;
        auto f = simple::bind(obj, simple::_1, 3);
        int r = 1;
        f(r);
        BOOST_CHECK(r == 4);
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////
/*
    ////////////////////////////////////////////////////////////////////////////
    /// @{ 绑定非静态成员变量
    {
        reset_counter();
        A a(2);
        auto f = simple::bind(&A::m_i, simple::_1);
        int r = f(a);
        BOOST_CHECK(r == 2);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////
*/
    //////////////////////////////////////////////////////////////////////////
    /// @{ 移动语义绑定参数
    {
        reset_counter();
        MoveA a;
        auto f = simple::bind(&move_func, eastl::move(a));
        f();
        BOOST_CHECK(g_MoveAmove == 1);
    }
    {
        reset_counter();
        MoveA a;
        auto f = simple::bind(&void_move_func, eastl::move(a));
        f();
        BOOST_CHECK(g_MoveAmove == 1);
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////
}

int main()
{
    test();
    return 0;
}


/*
#include <iostream>
#include <string>
#include <functional>
#include <utility>

//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define TEST_FUNC_NAME __FUNCSIG__
#elif defined(__GNUC__)
#   define TEST_FUNC_NAME __PRETTY_FUNCTION__
#else
#   define TEST_FUNC_NAME __func__
#endif

#define TEST_CASE(...) \
    eastl::cout << eastl::endl \
              << "----------------------------------" << eastl::endl \
              << TEST_FUNC_NAME << " ->: " __VA_ARGS__ << eastl::endl \
              << "----------------------------------" << eastl::endl

//////////////////////////////////////////////////////////////////////////

namespace test_result_traits_np
{
    using namespace simple;

    class Foo {};

    class Func
    {
    public:
        int operator()(void) { return 0; }
    };

    template <typename>
    struct check;

    template <>
    struct check<int>
    {
        check(void)
        {
            eastl::cout << "int!" << eastl::endl;
        }
    };
}

void test_result_traits(void)
{
    TEST_CASE();
    using namespace test_result_traits_np;

    check<result_traits<int(*)(void)>::type>();
    check<result_traits<int(Foo::*)(void)>::type>();
    check<result_traits<Func>::type>();
}

//////////////////////////////////////////////////////////////////////////

namespace test_fr_np
{
    using namespace simple;

    void func(void)
    {
        eastl::cout << TEST_FUNC_NAME << eastl::endl;
    }

    class Func
    {
    public:
        int operator()(int a, double b)
        {
            eastl::cout << TEST_FUNC_NAME << "(" << a << ", " << b << ")" << eastl::endl;
            return 0;
        }
    };
}

void test_fr(void)
{
    TEST_CASE();
    using namespace test_fr_np;

    fr<void(void)> f1(func);
    f1();

    Func func;
    fr<Func, placeholder<2>, placeholder<1>> f2(func, _2, _1);
    f2(123, 321);
}

//////////////////////////////////////////////////////////////////////////

namespace test_bind_np
{
    using namespace simple;

    void func0_0(void)
    {
        eastl::cout << TEST_FUNC_NAME << eastl::endl;
    }

    int func0_1(void)
    {
        eastl::cout << TEST_FUNC_NAME << " ->: ";
        return 0;
    }

    int func1(long a)
    {
        eastl::cout << TEST_FUNC_NAME << "(" << a << ")" << " ->: ";
        return 1;
    }

    static int func2(int a, int b)
    {
        eastl::cout << TEST_FUNC_NAME << "(" << a << ", " << b << ")" << " ->: ";
        return 2;
    }

    static int func2_s(long a, int b)
    {
        eastl::cout << TEST_FUNC_NAME << "(" << a << ", " << b << ")" << " ->: ";
        return 2;
    }

    class A
    {
    public:
        virtual int func(long a, int b, long c, int d)
        {
            eastl::cout << TEST_FUNC_NAME << "(" << a << ", "
                << b << ", "
                << c << ", "
                << d << ")" << " ->: ";
            return 4;
        }
    };

    class B : public A
    {
    public:
        int func(long a, int b, long c, int d)
        {
            eastl::cout << TEST_FUNC_NAME << "(" << a << ", "
                << b << ", "
                << c << ", "
                << d << ")" << " ->: ";
            return 4;
        }

        int func_const(long a) const
        {
            eastl::cout << TEST_FUNC_NAME << "(" << a << ")" << " ->: ";
            return 1;
        }

        static int funcS(long a)
        {
            eastl::cout << TEST_FUNC_NAME << "(" << a << ")" << " ->: ";
            return 1;
        }
    };

    class Func0
    {
    public:
        int operator()(void)
        {
            eastl::cout << TEST_FUNC_NAME << " ->: ";
            return 0;
        }
    };

    class Func1
    {
    public:
        eastl::string& operator()(eastl::string& s)
        {
            eastl::cout << TEST_FUNC_NAME << "(" << s << ")" << " ->: ";
            return s;
        }
    };

    class Func2
    {
    public:
        int operator()(void) const volatile
        {
            eastl::cout << TEST_FUNC_NAME << " ->: ";
            return 0;
        }
    };

    class UncopyableFunc
    {
    public:
        UncopyableFunc(void) {}
        UncopyableFunc(UncopyableFunc&&) {}
        UncopyableFunc(const UncopyableFunc&) = delete;

        int operator()(void)
        {
            eastl::cout << TEST_FUNC_NAME << " ->: ";
            return 0;
        }
    };
}

void test_bind(void)
{
    TEST_CASE();
    using namespace test_bind_np;
    {
        bind(func0_0)();
        eastl::cout << bind(func0_1)() << eastl::endl;
        eastl::cout << bind(func1, _1)(123) << eastl::endl;
        eastl::cout << bind(func2, _1, _2)(1, 2) << eastl::endl;
        eastl::cout << bind(func2_s, _1, _1)(321) << eastl::endl;
    }
    eastl::cout << eastl::endl;
    {
        B a;
        eastl::cout << bind(&B::func, a, 1, _1, 3, 4)(2) << eastl::endl;
        eastl::cout << bind(&B::func_const, &a, 123)() << eastl::endl;
        A* pa = &a;
        eastl::cout << bind(&A::func, pa, _4, _3, _2, _1)(1, 2, 3, 4) << eastl::endl;
        eastl::cout << bind(B::funcS, 123)() << eastl::endl;
    }
    eastl::cout << eastl::endl;
    {
        Func0 f0;
        eastl::cout << bind(&f0)() << eastl::endl;
        Func1 f1;
        eastl::string s("Hello Bind!");
        eastl::cout << bind(f1, _1)(s) << eastl::endl;
        Func2 f2;
        eastl::cout << bind(f2)() << eastl::endl;
    }
    eastl::cout << eastl::endl;
    {
        UncopyableFunc ff;
        eastl::cout << bind(eastl::move(ff))() << eastl::endl;
        eastl::function<int(void)> fr = bind(eastl::move(ff));
        eastl::cout << fr() << eastl::endl;
    }
}

//////////////////////////////////////////////////////////////////////////

int main(void)
{
    test_result_traits();
    test_fr();
    test_bind();

    TEST_CASE(<< "Finished!");
    return 0;
}
*/
