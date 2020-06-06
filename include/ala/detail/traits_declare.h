#ifndef _ALA_DETAIL_TRAITS_DECLARE_H
#define _ALA_DETAIL_TRAITS_DECLARE_H

#include <ala/config.h>

namespace ala {
// clang-format off

// helper class:
template<typename T, T Value>
struct integral_constant;
template<bool B>
using bool_constant = integral_constant<bool, B>;
using true_type     = bool_constant<true>;
using false_type    = bool_constant<false>;

// primary type categories:
template<typename T> struct is_void;
template<typename T> struct is_null_pointer;
template<typename T> struct is_integral;
template<typename T> struct is_floating_point;
template<typename T> struct is_array;
template<typename T> struct is_pointer;
template<typename T> struct is_lvalue_reference;
template<typename T> struct is_rvalue_reference;
template<typename T> struct is_member_object_pointer;
template<typename T> struct is_member_function_pointer;
template<typename T> struct is_enum;
template<typename T> struct is_union;
template<typename T> struct is_class;
template<typename T> struct is_function;

// composite type categories:
template<typename T> struct is_reference;
template<typename T> struct is_arithmetic;
template<typename T> struct is_fundamental;
template<typename T> struct is_object;
template<typename T> struct is_scalar;
template<typename T> struct is_compound;
template<typename T> struct is_member_pointer;

// type properties:
template<typename T> struct is_const;
template<typename T> struct is_volatile;
template<typename T> struct is_trivial;
template<typename T> struct is_trivially_copyable;
template<typename T> struct is_standard_layout;
template<typename T> struct is_pod;
template<typename T> struct is_literal_type;
template<typename T> struct is_empty;
template<typename T> struct is_polymorphic;
template<typename T> struct is_abstract;
template<typename T> struct is_final;
template<typename T> struct is_signed;
template<typename T> struct is_unsigned;

template<typename T, typename... Args> struct is_constructible;
template<typename T, typename... Args> struct is_trivially_constructible;
template<typename T, typename... Args> struct is_nothrow_constructible;
template<typename T, typename U> struct is_assignable;
template<typename T, typename U> struct is_swappable_with;
template<typename T, typename U> struct is_trivially_assignable;
template<typename T, typename U> struct is_nothrow_assignable;
template<typename T, typename U> struct is_nothrow_swappable_with;
template<typename T> struct is_default_constructible;
template<typename T> struct is_copy_constructible;
template<typename T> struct is_move_constructible;
template<typename T> struct is_copy_assignable;
template<typename T> struct is_move_assignable;
template<typename T> struct is_swappable;
template<typename T> struct is_destructible;
template<typename T> struct is_trivially_default_constructible;
template<typename T> struct is_trivially_copy_constructible;
template<typename T> struct is_trivially_move_constructible;
template<typename T> struct is_trivially_copy_assignable;
template<typename T> struct is_trivially_move_assignable;
template<typename T> struct is_trivially_destructible;
template<typename T> struct is_nothrow_default_constructible;
template<typename T> struct is_nothrow_copy_constructible;
template<typename T> struct is_nothrow_move_constructible;
template<typename T> struct is_nothrow_copy_assignable;
template<typename T> struct is_nothrow_move_assignable;
template<typename T> struct is_nothrow_swappable;
template<typename T> struct is_nothrow_destructible;
template<typename T> struct has_virtual_destructor;

// type property queries:
template<typename T>             struct alignment_of;
template<typename T>             struct rank;
template<typename T, unsigned I> struct extent;

// type relations:
template<typename T, typename U>                   struct is_same;
template<typename Base, typename Derived>          struct is_base_of;
template<typename From, typename To>               struct is_convertible;
template<typename Fn, typename... Args>            struct is_invocable;
template<typename Ret, typename Fn, typename... Args> struct is_invocable_r;
template<typename Fn, typename... Args>            struct is_nothrow_invocable;
template<typename Ret, typename Fn, typename... Args> struct is_nothrow_invocable_r;

// const-volatile modifications:
template<typename T> struct remove_const;
template<typename T> struct remove_volatile;
template<typename T> struct remove_cv;
template<typename T> struct add_const;
template<typename T> struct add_volatile;
template<typename T> struct add_cv;
template<typename T> using remove_const_t    = typename remove_const<T>::type;
template<typename T> using remove_volatile_t = typename remove_volatile<T>::type;
template<typename T> using remove_cv_t       = typename remove_cv<T>::type;
template<typename T> using add_const_t       = typename add_const<T>::type;
template<typename T> using add_volatile_t    = typename add_volatile<T>::type;
template<typename T> using add_cv_t          = typename add_cv<T>::type;

// reference modifications:
template<typename T> struct remove_reference;
template<typename T> struct add_lvalue_reference;
template<typename T> struct add_rvalue_reference;
template<typename T> using remove_reference_t     = typename remove_reference<T>::type;
template<typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
template<typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

// sign modifications:
template<typename T> struct make_signed;
template<typename T> struct make_unsigned;
template<typename T> using make_signed_t   = typename make_signed<T>::type;
template<typename T> using make_unsigned_t = typename make_unsigned<T>::type;

// array modifications:
template<typename T> struct remove_extent;
template<typename T> struct remove_all_extents;
template<typename T> using remove_extent_t      = typename remove_extent<T>::type;
template<typename T> using remove_all_extents_t = typename remove_all_extents<T>::type;

// pointer modifications:
template<typename T> struct remove_pointer;
template<typename T> struct add_pointer;
template<typename T> using remove_pointer_t = typename remove_pointer<T>::type;
template<typename T> using add_pointer_t    = typename add_pointer<T>::type;

// other transformations:
template<size_t Size, size_t Align>    struct aligned_storage;
template<size_t Size, typename... Ts>  struct aligned_union;
template<typename T>                   struct decay;
template<typename T>                   struct remove_cvref;
template<bool, typename T = void>      struct enable_if;
template<bool, typename T, typename F> struct conditional;
template<typename... T>                struct common_type;
template<typename T>                   struct underlying_type;
template<typename>                     struct result_of;
template<typename F, typename... Args> struct invoke_result;

template<size_t Size> struct _maxalign;
template<size_t Size, size_t Align = _maxalign<Size>::value> using aligned_storage_t = typename aligned_storage<Size, Align>::type;
template<size_t Size, typename... Ts>                        using aligned_union_t   = typename aligned_union<Size, Ts...>::type;
template<typename T>                                         using decay_t           = typename decay<T>::type;
template<typename T>                                         using remove_cvref_t    = typename remove_cvref<T>::type;
template<bool b, typename T = void>                          using enable_if_t       = typename enable_if<b, T>::type;
template<bool b, typename T, typename F>                     using conditional_t     = typename conditional<b, T, F>::type;
template<typename... T>                                      using common_type_t     = typename common_type<T...>::type;
template<typename T>                                         using underlying_type_t = typename underlying_type<T>::type;
template<typename T>                                         using result_of_t       = typename result_of<T>::type;
template<typename F, typename... Args>                       using invoke_result_t   = typename invoke_result<F, Args...>::type;

#if _ALA_LANG >= 201703L
template<typename...> using void_t = void;
#else
// http://open-std.org/JTC1/SC22/WG21/docs/cwg_defects.html#1558
template<typename... Ts> struct _void_t_impl { typedef void type; };
template<typename... Ts> using void_t = typename _void_t_impl<Ts...>::type;
#endif

// logical operator traits:
template<typename... B> struct conjunction;
template<typename... B> struct disjunction;
template<typename B>    struct negation;

// endian
enum class endian;

// primary type categories
#if _ALA_ENABLE_INLINE_VAR
template<typename T> inline constexpr bool is_void_v                    = is_void<T>::value;
template<typename T> inline constexpr bool is_null_pointer_v            = is_null_pointer<T>::value;
template<typename T> inline constexpr bool is_integral_v                = is_integral<T>::value;
template<typename T> inline constexpr bool is_floating_point_v          = is_floating_point<T>::value;
template<typename T> inline constexpr bool is_array_v                   = is_array<T>::value;
template<typename T> inline constexpr bool is_pointer_v                 = is_pointer<T>::value;
template<typename T> inline constexpr bool is_lvalue_reference_v        = is_lvalue_reference<T>::value;
template<typename T> inline constexpr bool is_rvalue_reference_v        = is_rvalue_reference<T>::value;
template<typename T> inline constexpr bool is_member_object_pointer_v   = is_member_object_pointer<T>::value;
template<typename T> inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
template<typename T> inline constexpr bool is_enum_v                    = is_enum<T>::value;
template<typename T> inline constexpr bool is_union_v                   = is_union<T>::value;
template<typename T> inline constexpr bool is_class_v                   = is_class<T>::value;
template<typename T> inline constexpr bool is_function_v                = is_function<T>::value;

// composite type categories
template<typename T> inline constexpr bool is_reference_v      = is_reference<T>::value;
template<typename T> inline constexpr bool is_arithmetic_v     = is_arithmetic<T>::value;
template<typename T> inline constexpr bool is_fundamental_v    = is_fundamental<T>::value;
template<typename T> inline constexpr bool is_object_v         = is_object<T>::value;
template<typename T> inline constexpr bool is_scalar_v         = is_scalar<T>::value;
template<typename T> inline constexpr bool is_compound_v       = is_compound<T>::value;
template<typename T> inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

// type properties
template<typename T, typename... Args> inline constexpr bool is_constructible_v           = is_constructible<T, Args...>::value;
template<typename T, typename... Args> inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;
template<typename T, typename... Args> inline constexpr bool is_nothrow_constructible_v   = is_nothrow_constructible<T, Args...>::value;
template<typename T, typename U> inline constexpr bool is_assignable_v             = is_assignable<T, U>::value;
template<typename T, typename U> inline constexpr bool is_swappable_with_v         = is_swappable_with<T, U>::value;
template<typename T, typename U> inline constexpr bool is_trivially_assignable_v   = is_trivially_assignable<T, U>::value;
template<typename T, typename U> inline constexpr bool is_nothrow_assignable_v     = is_nothrow_assignable<T, U>::value;
template<typename T, typename U> inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
template<typename T> inline constexpr bool is_const_v                           = is_const<T>::value;
template<typename T> inline constexpr bool is_volatile_v                        = is_volatile<T>::value;
template<typename T> inline constexpr bool is_trivial_v                         = is_trivial<T>::value;
template<typename T> inline constexpr bool is_trivially_copyable_v              = is_trivially_copyable<T>::value;
template<typename T> inline constexpr bool is_standard_layout_v                 = is_standard_layout<T>::value;
template<typename T> inline constexpr bool is_pod_v                             = is_pod<T>::value;
template<typename T> inline constexpr bool is_empty_v                           = is_empty<T>::value;
template<typename T> inline constexpr bool is_polymorphic_v                     = is_polymorphic<T>::value;
template<typename T> inline constexpr bool is_abstract_v                        = is_abstract<T>::value;
template<typename T> inline constexpr bool is_final_v                           = is_final<T>::value;
template<typename T> inline constexpr bool is_signed_v                          = is_signed<T>::value;
template<typename T> inline constexpr bool is_unsigned_v                        = is_unsigned<T>::value;
template<typename T> inline constexpr bool is_default_constructible_v           = is_default_constructible<T>::value;
template<typename T> inline constexpr bool is_copy_constructible_v              = is_copy_constructible<T>::value;
template<typename T> inline constexpr bool is_move_constructible_v              = is_move_constructible<T>::value;
template<typename T> inline constexpr bool is_copy_assignable_v                 = is_copy_assignable<T>::value;
template<typename T> inline constexpr bool is_move_assignable_v                 = is_move_assignable<T>::value;
template<typename T> inline constexpr bool is_swappable_v                       = is_swappable<T>::value;
template<typename T> inline constexpr bool is_destructible_v                    = is_destructible<T>::value;
template<typename T> inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;
template<typename T> inline constexpr bool is_trivially_copy_constructible_v    = is_trivially_copy_constructible<T>::value;
template<typename T> inline constexpr bool is_trivially_move_constructible_v    = is_trivially_move_constructible<T>::value;
template<typename T> inline constexpr bool is_trivially_copy_assignable_v       = is_trivially_copy_assignable<T>::value;
template<typename T> inline constexpr bool is_trivially_move_assignable_v       = is_trivially_move_assignable<T>::value;
template<typename T> inline constexpr bool is_trivially_destructible_v          = is_trivially_destructible<T>::value;
template<typename T> inline constexpr bool is_nothrow_default_constructible_v   = is_nothrow_default_constructible<T>::value;
template<typename T> inline constexpr bool is_nothrow_copy_constructible_v      = is_nothrow_copy_constructible<T>::value;
template<typename T> inline constexpr bool is_nothrow_move_constructible_v      = is_nothrow_move_constructible<T>::value;
template<typename T> inline constexpr bool is_nothrow_copy_assignable_v         = is_nothrow_copy_assignable<T>::value;
template<typename T> inline constexpr bool is_nothrow_move_assignable_v         = is_nothrow_move_assignable<T>::value;
template<typename T> inline constexpr bool is_nothrow_swappable_v               = is_nothrow_swappable<T>::value;
template<typename T> inline constexpr bool is_nothrow_destructible_v            = is_nothrow_destructible<T>::value;
template<typename T> inline constexpr bool has_virtual_destructor_v             = has_virtual_destructor<T>::value;
template<typename T> inline constexpr bool is_literal_type_v                    = is_literal_type<T>::value;
#endif

#if (defined(_ALA_CLANG) && !ALA_IS_IDENTIFIER(__has_unique_object_representations)) || \
    (defined(_ALA_GCC) && __GNUC__ >= 7) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1911)
template<typename T> struct has_unique_object_representations;
#if _ALA_ENABLE_INLINE_VAR
template<typename T> inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;
#endif
#endif

#if (defined(_ALA_CLANG) && !ALA_IS_IDENTIFIER(__is_aggregate)) || \
    (defined(_ALA_GCC) && _ALA_GCC_VER >= 7001) || \
    (defined(_ALA_MSVC) && MSC_VER >= 1915)
template<typename T> struct is_aggregate;
#if _ALA_ENABLE_INLINE_VAR
template<typename T> inline constexpr bool is_aggregate_v = is_aggregate<T>::value;
#endif
#endif

// type property queries
#if _ALA_ENABLE_INLINE_VAR
template<typename T>                 inline constexpr size_t alignment_of_v = alignment_of<T>::value;
template<typename T>                 inline constexpr size_t rank_v         = rank<T>::value;
template<typename T, unsigned I = 0> inline constexpr size_t extent_v       = extent<T, I>::value;

// type relations
template<typename T, typename U>                    inline constexpr bool is_same_v                = is_same<T, U>::value;
template<typename Base, typename Derived>           inline constexpr bool is_base_of_v             = is_base_of<Base, Derived>::value;
template<typename From, typename To>                inline constexpr bool is_convertible_v         = is_convertible<From, To>::value;
template<typename Fn, typename... Args>             inline constexpr bool is_invocable_v           = is_invocable<Fn, Args...>::value;
template<typename R, typename Fn, typename... Args> inline constexpr bool is_invocable_r_v         = is_invocable_r<R, Fn, Args...>::value;
template<typename Fn, typename... Args>             inline constexpr bool is_nothrow_invocable_v   = is_nothrow_invocable<Fn, Args...>::value;
template<typename R, typename Fn, typename... Args> inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, Fn, Args...>::value;

// logical operator traits:
template<typename... Bs> inline constexpr bool conjunction_v = conjunction<Bs...>::value;
template<typename... Bs> inline constexpr bool disjunction_v = disjunction<Bs...>::value;
template<typename B>     inline constexpr bool negation_v    = negation<B>::value;
#endif

// C++20

template<typename From, typename To>
struct is_nothrow_convertible;

template<typename T> struct type_identity;
template<typename T> using type_identity_t = typename type_identity<T>::type;

template<typename T> struct unwrap_reference;
template<typename T> using unwrap_reference_t = typename unwrap_reference<T>::type;

template<typename T> struct unwrap_ref_decay;
template<typename T> using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;

template<typename T>
struct is_bounded_array;

template<typename T>
struct is_unbounded_array;

#if _ALA_ENABLE_INLINE_VAR
template<typename From, typename To> inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;
template<typename T>                 inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
template<typename T>                 inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;
#endif

template<typename...>
struct common_reference;

template<typename... T>
using common_reference_t = typename common_reference<T...>::type;

template<typename, typename, template<typename> class, template<typename> class>
struct basic_common_reference;

// Extra
template<class T>                                 struct is_reference_wrapper;
template<class T>                                 struct is_implicitly_default_constructible;
template<typename T, template<typename...> class> struct is_specification;

#if _ALA_ENABLE_INLINE_VAR
template<typename T> inline constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;
template<typename T> inline constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<T>::value;
template<typename T, template<typename...> class Templt>
inline constexpr bool is_specification_v = is_specification<T, Templt>::value;
#endif
// clang-format on

} // namespace ala

#endif // HEAD