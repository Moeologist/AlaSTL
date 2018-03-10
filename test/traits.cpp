#include "ala/type_traits.hpp"
#include <type_traits>
#include <vector>
#include <list>
#include <iostream>
#include "traits_macro.h"
#include "traits_type.h"

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
	
	// TEST(is_swappable_with)
	// TEST(is_swappable)
	// TEST(is_nothrow_swappable_with)
	// TEST(is_nothrow_swappable)

	// TEST(has_virtual_destructor)
	// TEST(is_aggregate)
	// TEST(has_unique_object_representations)

	return 0;
}