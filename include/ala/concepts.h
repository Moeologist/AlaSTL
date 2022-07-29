#ifndef _ALA_CONCEPTS_H
#define _ALA_CONCEPTS_H

#include <ala/type_traits.h>

#ifndef _ALA_ENABLE_CONCEPTS

    #include <ala/compat/concepts.h>

#else
namespace ala {

template<class T, class U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template<class Derived, class Base>
concept derived_from =
    is_base_of_v<Base, Derived> &&
    is_convertible_v<const volatile Derived *, const volatile Base *>;

template<class From, class To>
concept convertible_to = is_convertible_v<From, To> &&
                         requires { static_cast<To>(declval<From>()); };

template<class T>
concept __boolean_testable_impl = convertible_to<T, bool>;

template<class T>
concept __boolean_testable = __boolean_testable_impl<T> &&
                             requires(T &&t) {
                                 {
                                     !static_cast<T &&>(t)
                                     } -> __boolean_testable_impl;
                             };

template<class T, class U>
concept common_reference_with =
    same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
    convertible_to<T, common_reference_t<T, U>> &&
    convertible_to<U, common_reference_t<T, U>>;

template<class T, class U>
concept common_with =
    same_as<common_type_t<T, U>, common_type_t<U, T>> &&
    requires {
        static_cast<common_type_t<T, U>>(declval<T>());
        static_cast<common_type_t<T, U>>(declval<U>());
    } &&
    common_reference_with<add_lvalue_reference_t<const T>,
                          add_lvalue_reference_t<const U>> &&
    common_reference_with<add_lvalue_reference_t<common_type_t<T, U>>,
                          common_reference_t<add_lvalue_reference_t<const T>,
                                             add_lvalue_reference_t<const U>>>;

template<class T>
concept integral = is_integral_v<T>;

template<class T>
concept signed_integral = integral<T> && is_signed_v<T>;

template<class T>
concept unsigned_integral = integral<T> && !
signed_integral<T>;

template<class T>
concept __integeral_arithmetic = integral<T> && !
same_as<T, bool>;

template<class T>
concept __signed_integeral_arithmetic = signed_integral<T> && !
same_as<T, bool>;

template<class T>
concept __unsigned_integeral_arithmetic = unsigned_integral<T> && !
same_as<T, bool>;

template<class T>
concept floating_point = is_floating_point_v<T>;

template<class LHS, class RHS>
concept assignable_from = is_lvalue_reference_v<LHS> &&
                          common_reference_with<const remove_reference_t<LHS> &,
                                                const remove_reference_t<RHS> &> &&
                          requires(LHS lhs, RHS &&rhs) {
                              { lhs = ala::forward<RHS>(rhs) } -> same_as<LHS>;
                          };

template<class T>
concept destructible = is_nothrow_destructible_v<T>;

template<class T, class... Args>
concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;

template<class T>
concept default_initializable = constructible_from<T> && requires { T{}; } &&
                                requires {
                                    ::new (static_cast<void *>(nullptr)) T;
                                };

template<class T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

template<class T>
concept copy_constructible = move_constructible<T> &&
                             constructible_from<T, T &> &&
                             convertible_to<T &, T> &&
                             constructible_from<T, const T &> &&
                             convertible_to<const T &, T> &&
                             constructible_from<T, const T> &&
                             convertible_to<const T, T>;

namespace ranges {

template<class T>
concept __class_or_enum = is_class_v<T> || is_union_v<T> || is_enum_v<T>;

namespace _swap {
template<class T>
void swap(T &, T &) = delete;

template<class T, class U>
concept __enable_adl = (__class_or_enum<remove_cvref_t<T>> ||
                        __class_or_enum<remove_cvref_t<U>>) &&
                       requires(T &&t, U &&u) {
                           swap(static_cast<T &&>(t), static_cast<U &&>(u));
                       };

struct _cpo_fn {
    template<class T, class U>
        requires __enable_adl<T, U>
    constexpr void operator()(T &&t, U &&u) const
        noexcept(noexcept(swap(static_cast<T &&>(t), static_cast<U &&>(u)))) {
        swap(static_cast<T &&>(t), static_cast<U &&>(u));
    }

    template<class T>
        requires(!__enable_adl<T &, T &> && move_constructible<T> &&
                 assignable_from<T &, T>)
    constexpr void operator()(T &x, T &y) const noexcept(
        is_nothrow_move_constructible_v<T> &&is_nothrow_move_assignable_v<T>) {
        T tmp(static_cast<T &&>(x));
        x = static_cast<T &&>(y);
        y = static_cast<T &&>(tmp);
    }

    template<class T, class U, size_t N>
    constexpr void operator()(T (&t)[N], U (&u)[N]) const
        noexcept(noexcept((*this)(t[0], u[0])))
        requires requires { (*this)(t[0], u[0]); }
    {
        for (size_t i = 0; i < N; ++i) {
            (*this)(t[i], u[i]);
        }
    }
};

} // namespace _swap

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V _swap::_cpo_fn swap;
}
} // namespace ranges

template<class T>
concept swappable = requires(T &a, T &b) { ranges::swap(a, b); };

template<class T, class U>
concept swappable_with =
    common_reference_with<T, U> &&
    requires(T &&t, U &&u) {
        ala::ranges::swap(ala::forward<T>(t), ala::forward<T>(t));
        ala::ranges::swap(ala::forward<U>(u), ala::forward<U>(u));
        ala::ranges::swap(ala::forward<T>(t), ala::forward<U>(u));
        ala::ranges::swap(ala::forward<U>(u), ala::forward<T>(t));
    };

template<class T, class U>
concept __weakly_equality_comparable_with =
    requires(const remove_reference_t<T> &t, const remove_reference_t<U> &u) {
        { t == u } -> __boolean_testable;
        { t != u } -> __boolean_testable;
        { u == t } -> __boolean_testable;
        { u != t } -> __boolean_testable;
    };

template<class T>
concept equality_comparable = __weakly_equality_comparable_with<T, T>;

template<class T, class U>
concept equality_comparable_with =
    equality_comparable<T> && equality_comparable<U> &&
    common_reference_with<const remove_reference_t<T> &,
                          const remove_reference_t<U> &> &&
    equality_comparable<common_reference_t<const remove_reference_t<T> &,
                                           const remove_reference_t<U> &>> &&
    __weakly_equality_comparable_with<T, U>;

template<class T>
concept totally_ordered = equality_comparable<T> &&
                          requires(const remove_reference_t<T> &a,
                                   const remove_reference_t<T> &b) {
                              { a < b } -> __boolean_testable;
                              { a > b } -> __boolean_testable;
                              { a <= b } -> __boolean_testable;
                              { a >= b } -> __boolean_testable;
                          };

template<class T, class U>
concept totally_ordered_with =
    totally_ordered<T> && totally_ordered<U> &&
    common_reference_with<const remove_reference_t<T> &,
                          const remove_reference_t<U> &> &&
    totally_ordered<common_reference_t<const remove_reference_t<T> &,
                                       const remove_reference_t<U> &>> &&
    equality_comparable_with<T, U> &&
    requires(const remove_reference_t<T> &t, const remove_reference_t<U> &u) {
        { t < u } -> __boolean_testable;
        { t > u } -> __boolean_testable;
        { t <= u } -> __boolean_testable;
        { t >= u } -> __boolean_testable;
        { u < t } -> __boolean_testable;
        { u > t } -> __boolean_testable;
        { u <= t } -> __boolean_testable;
        { u >= t } -> __boolean_testable;
    };

template<class T>
concept movable = is_object_v<T> && move_constructible<T> &&
                  assignable_from<T &, T> && swappable<T>;

template<class T>
concept copyable = copy_constructible<T> && movable<T> &&
                   assignable_from<T &, T &> && assignable_from<T &, const T &> &&
                   assignable_from<T &, const T>;

template<class T>
concept semiregular = copyable<T> && default_initializable<T>;

template<class T>
concept regular = semiregular<T> && equality_comparable<T>;

template<class F, class... Args>
concept invocable = requires(F &&f, Args &&...args) {
                        ala::invoke(static_cast<F &&>(f),
                                    static_cast<Args &&>(args)...);
                    };

template<class F, class... Args>
concept regular_invocable = invocable<F, Args...>;

template<class F, class... Args>
concept predicate = regular_invocable<F, Args...> &&
                    __boolean_testable<invoke_result_t<F, Args...>>;

template<class R, class T, class U>
concept relation = predicate<R, T, T> && predicate<R, U, U> &&
                   predicate<R, T, U> && predicate<R, U, T>;

template<class R, class T, class U>
concept equivalence_relation = relation<R, T, U>;

template<class R, class T, class U>
concept strict_weak_order = relation<R, T, U>;
} // namespace ala

#endif

#endif