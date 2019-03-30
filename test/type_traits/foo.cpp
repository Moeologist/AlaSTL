// @cflags=-stdlib=libc++
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-qualifiers"


#include <ala/type_traits.h>
#include <iostream>
#include <type_traits>

// Your function
#include "macro.h"
#include "type.h"

#pragma warning(push)
#pragma warning(disable : 4180)
int x(int p) {return p;}
int main() {
    TEST(is_void)
    TEST(is_null_pointer)
    TEST(is_integral)
    TEST(is_floating_point)
    TEST(is_array)
    TEST(is_pointer)
    TEST(is_lvalue_reference)
    TEST(is_rvalue_reference)
    TEST(is_member_object_pointer)
    TEST(is_member_function_pointer)
    TEST(is_enum)
    TEST(is_union)
    TEST(is_class)
    TEST(is_function)
    TEST(is_reference)
    TEST(is_arithmetic)
    TEST(is_fundamental)
    TEST(is_object)
    TEST(is_scalar)
    TEST(is_compound)
    TEST(is_member_pointer)

    TEST(is_const)
    TEST(is_volatile)
    TEST(is_trivial)
    TEST(is_trivially_copyable)
    TEST(is_standard_layout)
    TEST(is_pod)
    TEST(is_empty)
    TEST(is_polymorphic)
    TEST(is_abstract)
    TEST(is_final)
    TEST(is_signed)
    TEST(is_unsigned)

    TEST(is_constructible)
    TEST(is_trivially_constructible)
    TEST(is_nothrow_constructible)

    TEST_CONS(is_constructible)
    TEST_CONS(is_trivially_constructible)
    TEST_CONS(is_nothrow_constructible)

    TEST(is_default_constructible)
    TEST(is_trivially_default_constructible)
    TEST(is_nothrow_default_constructible)

    TEST_NOVOID(is_copy_constructible)
    TEST_NOVOID(is_trivially_copy_constructible)
    TEST_NOVOID(is_nothrow_copy_constructible)

    TEST_NOVOID(is_move_constructible)
    TEST_NOVOID(is_trivially_move_constructible)
    TEST_NOVOID(is_nothrow_move_constructible)

    TEST(is_destructible)
    TEST(is_trivially_destructible)
    TEST(is_nothrow_destructible)

    TEST_2(is_assignable)
    TEST_2(is_trivially_assignable)
    TEST_2(is_nothrow_assignable)

    TEST_NOVOID(is_copy_assignable)
    TEST_NOVOID(is_trivially_copy_assignable)
    TEST_NOVOID(is_nothrow_copy_assignable)

    TEST_NOVOID(is_move_assignable)
    TEST_NOVOID(is_trivially_move_assignable)
    TEST_NOVOID(is_nothrow_move_assignable)

    #if _ALA_CPP_STD >= 17
    TEST_2(is_swappable_with)
    TEST(is_swappable)
    TEST_2(is_nothrow_swappable_with)
    TEST(is_nothrow_swappable)
    #endif

    TEST(has_virtual_destructor)
    // TEST(is_aggregate)
    // TEST(has_unique_object_representations)

    TYPETEST(remove_const)
    TYPETEST(remove_volatile)
    TYPETEST(remove_cv)
    TYPETEST(add_const)
    TYPETEST(add_volatile)
    TYPETEST(add_cv)
    TYPETEST(remove_reference)
    TYPETEST(add_lvalue_reference)
    TYPETEST(add_rvalue_reference)
    TYPETEST_CV(make_signed, int)
    TYPETEST_CV(make_unsigned, int)
    TYPETEST(remove_extent)
    TYPETEST(remove_all_extents)
    TYPETEST(remove_pointer)
    TYPETEST(add_pointer)
    TYPETEST(decay)
    TYPETEST(common_type)

    // TYPETEST(enable_if)
    // TYPETEST(conditional)
    // TYPETEST(invoke_result)
    // TYPETEST(underlying_type)

    typedef int (CC::*pmf)(int);
    // std::cout<<typeid(decltype(&CC::x)).name();
    // std::cout<<typeid(pmf).name();
    static_assert(ala::is_constructible<C0, C0 &>::value == std::is_constructible<C0, C0 &>::value, "oh");
    static_assert(ala::is_constructible<C0&, C0 &>::value == std::is_constructible<C0&, C0 &>::value, "oh");
    static_assert(ala::is_constructible<C0&, C0 &&>::value == std::is_constructible<C0&, C0 &&>::value, "oh");
    static_assert(ala::is_constructible<C0, C0 &&>::value == std::is_constructible<C0, C0 &&>::value, "oh");
    static_assert(ala::is_constructible<C0&&, C0 &&>::value == std::is_constructible<C0&&, C0 &&>::value, "oh");
    static_assert(ala::is_constructible<C0&, Cd &&>::value == std::is_constructible<C0&, Cd &&>::value, "oh");
    static_assert(ala::is_constructible<C0&, Cd &&>::value == false, "oh");

    #if !defined(_ALA_MSVC) || _ALA_CPP_STD >= 17

    static_assert(ala::is_same<ala::invoke_result_t<pmf, CC, float>, int>::value, "oh");
    static_assert(ala::is_same<ala::invoke_result_t<decltype(&CC::v), CC>, double &&>::value, "oh");
    #endif

    struct A {};
    struct B { explicit B() = default; };

    A a1 = {};
    A a2 {};

    // B b1 = {}; // Error, would use explicit default constructor
    B b2 {};
    static_assert(ala::is_implicitly_default_constructible<A>::value, "oh");
    static_assert(!ala::is_implicitly_default_constructible<B>::value, "oh");



    return ala::invoke(x, 10086);;
}

#pragma warning(pop)
#pragma clang diagnostic pop
