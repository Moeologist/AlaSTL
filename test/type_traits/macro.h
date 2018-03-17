#define TEST_CV(OP, TP) \
static_assert(ala:: OP ## _v< TP >                   == std:: OP < TP >::value,"");\
static_assert(ala:: OP ## _v< TP const>              == std:: OP < TP const>::value,"");\
static_assert(ala:: OP ## _v< TP volatile>           == std:: OP < TP volatile>::value,"");\
static_assert(ala:: OP ## _v< TP const volatile>     == std:: OP < TP const volatile>::value,"");

#define TEST_STAR(OP, TP) \
static_assert(ala:: OP ## _v< TP *>                  == std:: OP < TP *>::value,"");\
static_assert(ala:: OP ## _v< TP const*>             == std:: OP < TP const*>::value,"");\
static_assert(ala:: OP ## _v< TP volatile*>          == std:: OP < TP volatile*>::value,"");\
static_assert(ala:: OP ## _v< TP const volatile*>    == std:: OP < TP const volatile*>::value,"");

#define TEST_REF(OP, TP) \
static_assert(ala:: OP ## _v< TP &>                  == std:: OP < TP &>::value,"");\
static_assert(ala:: OP ## _v< TP const&>             == std:: OP < TP const&>::value,"");\
static_assert(ala:: OP ## _v< TP volatile&>          == std:: OP < TP volatile&>::value,"");\
static_assert(ala:: OP ## _v< TP const volatile&>    == std:: OP < TP const volatile&>::value,"");

#define TEST_RREF(OP, TP) \
static_assert(ala:: OP ## _v< TP &&>                 == std:: OP < TP &&>::value,"");\
static_assert(ala:: OP ## _v< TP const&&>            == std:: OP < TP const&&>::value,"");\
static_assert(ala:: OP ## _v< TP volatile&&>         == std:: OP < TP volatile&&>::value,"");\
static_assert(ala:: OP ## _v< TP const volatile&&>   == std:: OP < TP const volatile&&>::value,"");

#define TEST_EXT(OP, TP) \
static_assert(ala:: OP ## _v< TP [8]>                == std:: OP < TP [8]>::value,"");\
static_assert(ala:: OP ## _v< TP const [8]>          == std:: OP < TP const [8]>::value,"");\
static_assert(ala:: OP ## _v< TP volatile [8]>       == std:: OP < TP volatile [8]>::value,"");\
static_assert(ala:: OP ## _v< TP const volatile [8]> == std:: OP < TP const volatile [8]>::value,"");

#define TEST_ALL(OP, TP) \
TEST_CV(OP, TP)\
TEST_STAR(OP, TP)\
TEST_REF(OP, TP)\
TEST_RREF(OP, TP)\
TEST_EXT(OP, TP)

#define TEST_NOEXT(OP, TP) \
TEST_CV(OP, TP)\
TEST_STAR(OP, TP)\
TEST_REF(OP, TP)\
TEST_RREF(OP, TP)

#define TEST_NOVOID(OP) \
TEST_ALL(OP, char)\
TEST_ALL(OP, int)\
TEST_ALL(OP, long )\
TEST_ALL(OP, long long)\
TEST_ALL(OP, float)\
TEST_ALL(OP, double)\
TEST_ALL(OP, long double)\
TEST_ALL(OP, wchar_t)\
TEST_ALL(OP, char16_t)\
TEST_ALL(OP, char32_t)\
TEST_ALL(OP, ala::int8_t)\
TEST_ALL(OP, ala::uint8_t)\
TEST_ALL(OP, ala::int16_t)\
TEST_ALL(OP, ala::uint16_t)\
TEST_ALL(OP, ala::int32_t)\
TEST_ALL(OP, ala::uint32_t)\
TEST_ALL(OP, ala::int64_t)\
TEST_ALL(OP, ala::uint64_t)\
TEST_ALL(OP, ala::size_t)\
TEST_ALL(OP, ala::ptrdiff_t)\
TEST_ALL(OP, ala::intptr_t)\
TEST_NOEXT(OP, C0)\
TEST_NOEXT(OP, C1)\
TEST_NOEXT(OP, C2)\
TEST_NOEXT(OP, S0)\
TEST_NOEXT(OP, S1)\
TEST_NOEXT(OP, S2)\
TEST_NOEXT(OP, E)\
TEST_NOEXT(OP, U)\
TEST_NOEXT(OP, X0)\
TEST_NOEXT(OP, X1)\
TEST_NOEXT(OP, X2)\
TEST_NOEXT(OP, X3)\
TEST_NOEXT(OP, X4)\
TEST_NOEXT(OP, X5)\
TEST_NOEXT(OP, X6)

#define TEST(OP) \
TEST_CV(OP, void)\
TEST_STAR(OP, void)\
TEST_NOVOID(OP)

#define TEST_CONS_SUB(OP, TP) \
static_assert(ala:: OP ## _v< TP > == std:: OP < TP >::value,"");\
static_assert(ala:: OP ## _v< TP, int, float> == std:: OP < TP, int, float>::value,"");\
static_assert(ala:: OP ## _v< TP, TP> == std:: OP < TP, TP>::value,"");\
static_assert(ala:: OP ## _v< TP, TP&> == std:: OP < TP, TP&>::value,"");\
static_assert(ala:: OP ## _v< TP, TP&> == std:: OP < TP, TP&>::value,"");\
static_assert(ala:: OP ## _v< TP, TP&&> == std:: OP < TP, TP&&>::value,"");\
static_assert(ala:: OP ## _v< TP, TP*> == std:: OP < TP, TP*>::value,"");\
static_assert(ala:: OP ## _v< TP, int, int> == std:: OP < TP, int, int>::value,"");\
static_assert(ala:: OP ## _v< TP, int, int> == std:: OP < TP, int, int>::value,"");\
static_assert(ala:: OP ## _v< TP, int> == std:: OP < TP, int>::value,"");\
static_assert(ala:: OP ## _v< TP, double> == std:: OP < TP, double>::value,"");\
static_assert(ala:: OP ## _v< TP, void> == std:: OP < TP, void>::value,"");\
static_assert(ala:: OP ## _v< TP, TP, TP> == std:: OP < TP, TP, TP>::value,"");

#define TEST_CONS(OP) \
TEST_CONS_SUB(OP, C0)\
TEST_CONS_SUB(OP, C1)\
TEST_CONS_SUB(OP, C2)\
TEST_CONS_SUB(OP, S0)\
TEST_CONS_SUB(OP, S1)\
TEST_CONS_SUB(OP, S2)\
TEST_CONS_SUB(OP, E)\
TEST_CONS_SUB(OP, U)\
TEST_CONS_SUB(OP, X0)\
TEST_CONS_SUB(OP, X1)\
TEST_CONS_SUB(OP, X2)\
TEST_CONS_SUB(OP, X3)\
TEST_CONS_SUB(OP, X4)\
TEST_CONS_SUB(OP, X5)\
TEST_CONS_SUB(OP, X6)

#define TEST_2_SUB(OP, TP) \
static_assert(ala:: OP ## _v< TP, int> == std:: OP < TP, int>::value,"");\
static_assert(ala:: OP ## _v< TP, TP> == std:: OP < TP, TP>::value,"");\
static_assert(ala:: OP ## _v< TP, TP&> == std:: OP < TP, TP&>::value,"");\
static_assert(ala:: OP ## _v< TP, TP&> == std:: OP < TP, TP&>::value,"");\
static_assert(ala:: OP ## _v< TP, TP&&> == std:: OP < TP, TP&&>::value,"");\
static_assert(ala:: OP ## _v< TP, TP*> == std:: OP < TP, TP*>::value,"");\
static_assert(ala:: OP ## _v< TP, int> == std:: OP < TP, int>::value,"");\
static_assert(ala:: OP ## _v< TP, double> == std:: OP < TP, double>::value,"");\
static_assert(ala:: OP ## _v< TP, void> == std:: OP < TP, void>::value,"");

#define TEST_2(OP) \
TEST_2_SUB(OP, C0)\
TEST_2_SUB(OP, C1)\
TEST_2_SUB(OP, C2)\
TEST_2_SUB(OP, S0)\
TEST_2_SUB(OP, S1)\
TEST_2_SUB(OP, S2)\
TEST_2_SUB(OP, E)\
TEST_2_SUB(OP, U)\
TEST_2_SUB(OP, X0)\
TEST_2_SUB(OP, X1)\
TEST_2_SUB(OP, X2)\
TEST_2_SUB(OP, X3)\
TEST_2_SUB(OP, X4)\
TEST_2_SUB(OP, X5)\
TEST_2_SUB(OP, X6)


#define TYPETEST_CV(OP, TP) \
static_assert(std::is_same<ala:: OP ## _t< TP >                   , std:: OP < TP >::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const>              , std:: OP < TP const>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP volatile>           , std:: OP < TP volatile>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const volatile>     , std:: OP < TP const volatile>::type>::value,"");

#define TYPETEST_STAR(OP, TP) \
static_assert(std::is_same<ala:: OP ## _t< TP *>                  , std:: OP < TP *>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const*>             , std:: OP < TP const*>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP volatile*>          , std:: OP < TP volatile*>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const volatile*>    , std:: OP < TP const volatile*>::type>::value,"");

#define TYPETEST_REF(OP, TP) \
static_assert(std::is_same<ala:: OP ## _t< TP &>                  , std:: OP < TP &>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const&>             , std:: OP < TP const&>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP volatile&>          , std:: OP < TP volatile&>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const volatile&>    , std:: OP < TP const volatile&>::type>::value,"");

#define TYPETEST_RREF(OP, TP) \
static_assert(std::is_same<ala:: OP ## _t< TP &&>                 , std:: OP < TP &&>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const&&>            , std:: OP < TP const&&>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP volatile&&>         , std:: OP < TP volatile&&>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const volatile&&>   , std:: OP < TP const volatile&&>::type>::value,"");

#define TYPETEST_EXT(OP, TP) \
static_assert(std::is_same<ala:: OP ## _t< TP [8]>                , std:: OP < TP [8]>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const [8]>          , std:: OP < TP const [8]>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP volatile [8]>       , std:: OP < TP volatile [8]>::type>::value,"");\
static_assert(std::is_same<ala:: OP ## _t< TP const volatile [8]> , std:: OP < TP const volatile [8]>::type>::value,"");

#define TYPETEST_ALL(OP, TP) \
TYPETEST_CV(OP, TP)\
TYPETEST_STAR(OP, TP)\
TYPETEST_REF(OP, TP)\
TYPETEST_RREF(OP, TP)\
TYPETEST_EXT(OP, TP)

#define TYPETEST_NOEXT(OP, TP) \
TYPETEST_CV(OP, TP)\
TYPETEST_STAR(OP, TP)\
TYPETEST_REF(OP, TP)\
TYPETEST_RREF(OP, TP)

#define TYPETEST_NOVOID(OP) \
TYPETEST_ALL(OP, char)\
TYPETEST_ALL(OP, int)\
TYPETEST_ALL(OP, long )\
TYPETEST_ALL(OP, long long)\
TYPETEST_ALL(OP, float)\
TYPETEST_ALL(OP, double)\
TYPETEST_ALL(OP, long double)\
TYPETEST_ALL(OP, wchar_t)\
TYPETEST_ALL(OP, char16_t)\
TYPETEST_ALL(OP, char32_t)\
TYPETEST_ALL(OP, ala::int8_t)\
TYPETEST_ALL(OP, ala::uint8_t)\
TYPETEST_ALL(OP, ala::int16_t)\
TYPETEST_ALL(OP, ala::uint16_t)\
TYPETEST_ALL(OP, ala::int32_t)\
TYPETEST_ALL(OP, ala::uint32_t)\
TYPETEST_ALL(OP, ala::int64_t)\
TYPETEST_ALL(OP, ala::uint64_t)\
TYPETEST_ALL(OP, ala::size_t)\
TYPETEST_ALL(OP, ala::ptrdiff_t)\
TYPETEST_ALL(OP, ala::intptr_t)\
TYPETEST_NOEXT(OP, C0)\
TYPETEST_NOEXT(OP, C1)\
TYPETEST_NOEXT(OP, C2)\
TYPETEST_NOEXT(OP, S0)\
TYPETEST_NOEXT(OP, S1)\
TYPETEST_NOEXT(OP, S2)\
TYPETEST_NOEXT(OP, E)\
TYPETEST_NOEXT(OP, U)\
TYPETEST_NOEXT(OP, X0)\
TYPETEST_NOEXT(OP, X1)\
TYPETEST_NOEXT(OP, X2)\
TYPETEST_NOEXT(OP, X3)\
TYPETEST_NOEXT(OP, X4)\
TYPETEST_NOEXT(OP, X5)\
TYPETEST_NOEXT(OP, X6)

#define TYPETEST(OP) \
TYPETEST_CV(OP, void)\
TYPETEST_STAR(OP, void)\
TYPETEST_NOVOID(OP)
