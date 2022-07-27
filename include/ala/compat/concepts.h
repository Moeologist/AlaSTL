#ifndef _ALA_COMPAT_CONCEPTS_H
#define _ALA_COMPAT_CONCEPTS_H

#include <ala/type_traits.h>

namespace ala {

template<typename, typename...>
auto _requires_test(...) -> false_type;

template<typename T, typename... Args,
         typename = decltype(&T::template _requires<Args...>)>
auto _requires_test(int) -> true_type;

template<typename T, typename... Args>
ALA_CONCEPT _requires = decltype(_requires_test<T, Args...>(0))::value;

template<typename T, typename U>
ALA_CONCEPT same_as = is_same_v<T, U> &&is_same_v<U, T>;

template<typename Derived, typename Base>
ALA_CONCEPT derived_from = is_base_of_v<Base, Derived>
    &&is_convertible_v<const volatile Derived *, const volatile Base *>;

struct _convertible_to {
    template<typename From, typename To>
    auto _requires() -> decltype(static_cast<To>(declval<From>()));
};

template<typename From, typename To>
ALA_CONCEPT convertible_to =
    is_convertible_v<From, To> &&_requires<_convertible_to, From, To>;

template<class T>
ALA_CONCEPT __boolean_testable_impl = convertible_to<T, bool>;

struct ___boolean_testable {
    template<typename T>
    auto requires_(T &&t)
        -> enable_if_t<__boolean_testable_impl<decltype(!static_cast<T &&>(t))>>;
};

template<class T>
ALA_CONCEPT __boolean_testable =
    __boolean_testable_impl<T> &&_requires<___boolean_testable, T>;

template<typename T, typename U>
ALA_CONCEPT common_reference_with =
    same_as<common_reference_t<T, U>, common_reference_t<U, T>>
        &&convertible_to<T, common_reference_t<T, U>>
            &&convertible_to<U, common_reference_t<T, U>>;

struct _common_with {
    template<typename T, typename U>
    auto requires_() -> decltype(static_cast<common_type_t<T, U>>(declval<T>()),
                                 static_cast<common_type_t<T, U>>(declval<U>()));
};

template<typename T, typename U>
ALA_CONCEPT common_with = same_as<common_type_t<T, U>, common_type_t<U, T>>
    &&_requires<_common_with, T, U> &&common_reference_with<
        add_lvalue_reference_t<const T>, add_lvalue_reference_t<const U>> &&
        common_reference_with<add_lvalue_reference_t<common_type_t<T, U>>,
                              common_reference_t<add_lvalue_reference_t<const T>,
                                                 add_lvalue_reference_t<const U>>>;

template<typename T>
ALA_CONCEPT integral = is_integral_v<T>;

template<typename T>
ALA_CONCEPT signed_integral = integral<T> &&is_signed_v<T>;

template<typename T>
ALA_CONCEPT unsigned_integral = integral<T> && !signed_integral<T>;

template<typename T>
ALA_CONCEPT floating_point = is_floating_point_v<T>;

struct _assignable_from {
    template<class LHS, class RHS>
    auto _requires(LHS lhs, RHS &&rhs)
        -> enable_if_t<same_as<decltype(lhs = ala::forward<RHS>(rhs)), LHS>>;
};

template<class LHS, class RHS>
ALA_CONCEPT assignable_from = is_lvalue_reference_v<LHS> &&common_reference_with<
    const remove_reference_t<LHS> &, const remove_reference_t<RHS> &>
    &&_requires<_assignable_from, LHS, RHS>;

template<class T>
ALA_CONCEPT destructible = is_nothrow_destructible_v<T>;

template<class T, class... Args>
ALA_CONCEPT constructible_from = destructible<T> &&is_constructible_v<T, Args...>;

struct _default_initializable1 {
    template<class T>
    auto _requires() -> decltype(T{});
};

struct _default_initializable2 {
    template<class T>
    auto _requires() -> decltype(::new (static_cast<void *>(nullptr)) T);
};

template<class T>
ALA_CONCEPT default_initializable = constructible_from<T> &&
    _requires<_default_initializable1, T> &&_requires<_default_initializable2, T>;

template<class T>
ALA_CONCEPT move_constructible = constructible_from<T, T> &&convertible_to<T, T>;

template<class T>
ALA_CONCEPT copy_constructible =
    move_constructible<T> &&constructible_from<T, T &> &&convertible_to<T &, T>
        &&constructible_from<T, const T &> &&convertible_to<const T &, T>
            &&constructible_from<T, const T> &&convertible_to<const T, T>;

namespace ranges {

template<class T>
ALA_CONCEPT __class_or_enum = is_class_v<T> || is_union_v<T> || is_enum_v<T>;

namespace _swap {
template<class T>
void swap(T &, T &) = delete;

struct _enable_adl {
    template<class T, class U>
    auto _require(T &&t, U &&u)
        -> decltype(swap(static_cast<T &&>(t), static_cast<U &&>(u)));
};

template<class T, class U>
ALA_CONCEPT __enable_adl =
    (__class_or_enum<remove_cvref_t<T>> ||
     __class_or_enum<remove_cvref_t<U>>)&&_requires<_enable_adl, T, U>;

struct _cpo_fn {
    template<class T, class U>
    constexpr auto operator()(T &&t, U &&u) const
        noexcept(noexcept(swap(static_cast<T &&>(t), static_cast<U &&>(u))))
            -> enable_if_t<__enable_adl<T, U>> {
        swap(static_cast<T &&>(t), static_cast<U &&>(u));
    }

    template<class T>
    constexpr auto operator()(T &x, T &y) const noexcept(
        is_nothrow_move_constructible_v<T> &&is_nothrow_move_assignable_v<T>)
        -> enable_if_t<!__enable_adl<T &, T &> && move_constructible<T> &&
                       assignable_from<T &, T>> {
        T tmp(static_cast<T &&>(x));
        x = static_cast<T &&>(y);
        y = static_cast<T &&>(tmp);
    }

    template<class T, class U, size_t N>
    constexpr auto operator()(T (&t)[N], U (&u)[N]) const
        noexcept(noexcept((*this)(t[0], u[0])))
            -> void_t<decltype((*this)(t[0], u[0]))> {
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

struct _swappable {
    template<class T>
    auto _requires(T &a, T &b) -> decltype(ranges::swap(a, b));
};

template<class T>
ALA_CONCEPT swappable = _requires<_swappable, T>;

struct _swappable_with {
    template<class T, class U>
    auto _requires(T &t, U &u)
        -> decltype(ala::ranges::swap(ala::forward<T>(t), ala::forward<T>(t)),
                    ala::ranges::swap(ala::forward<U>(u), ala::forward<U>(u)),
                    ala::ranges::swap(ala::forward<T>(t), ala::forward<U>(u)),
                    ala::ranges::swap(ala::forward<U>(u), ala::forward<T>(t)));
};

template<class T, class U>
ALA_CONCEPT swappable_with =
    common_reference_with<T, U> &&_requires<_swappable_with, T, U>;

struct ___weakly_equality_comparable_with {
    template<class T, class U>
    auto _requires(const remove_reference_t<T> &t, const remove_reference_t<U> &u)
        -> enable_if_t<__boolean_testable<decltype(t == u)> &&
                       __boolean_testable<decltype(t != u)> &&
                       __boolean_testable<decltype(u == t)> &&
                       __boolean_testable<decltype(u != t)>>;
};

template<class T, class U>
ALA_CONCEPT __weakly_equality_comparable_with =
    _requires<___weakly_equality_comparable_with, T, U>;

template<class T>
ALA_CONCEPT equality_comparable = __weakly_equality_comparable_with<T, T>;

template<class T, class U>
ALA_CONCEPT equality_comparable_with =
    equality_comparable<T> &&equality_comparable<U> &&common_reference_with<
        const remove_reference_t<T> &, const remove_reference_t<U> &>
        &&equality_comparable<common_reference_t<const remove_reference_t<T> &,
                                                 const remove_reference_t<U> &>>
            &&__weakly_equality_comparable_with<T, U>;

struct _totally_ordered {
    template<class T>
    auto _requires(const remove_reference_t<T> &a, const remove_reference_t<T> &b)
        -> enable_if_t<__boolean_testable<decltype(a < b)> &&
                       __boolean_testable<decltype(a > b)> &&
                       __boolean_testable<decltype(a <= b)> &&
                       __boolean_testable<decltype(a >= b)>>;
};

template<class T>
ALA_CONCEPT totally_ordered =
    equality_comparable<T> &&_requires<_totally_ordered, T>;

struct _totally_ordered_with {
    template<class T, class U>
    auto _requires(const remove_reference_t<T> &t, const remove_reference_t<U> &u)
        -> enable_if_t<__boolean_testable<decltype(t < u)> &&
                       __boolean_testable<decltype(t > u)> &&
                       __boolean_testable<decltype(t <= u)> &&
                       __boolean_testable<decltype(t >= u)> &&
                       __boolean_testable<decltype(u < t)> &&
                       __boolean_testable<decltype(u > t)> &&
                       __boolean_testable<decltype(u <= t)> &&
                       __boolean_testable<decltype(u >= t)>>;
};

template<class T, class U>
ALA_CONCEPT totally_ordered_with = totally_ordered<T> &&totally_ordered<U> &&
    common_reference_with<const remove_reference_t<T> &, const remove_reference_t<U> &>
        &&totally_ordered<common_reference_t<const remove_reference_t<T> &,
                                             const remove_reference_t<U> &>> &&
            equality_comparable_with<T, U> &&_requires<_totally_ordered_with, T, U>;

template<class T>
ALA_CONCEPT movable = is_object_v<T> &&move_constructible<T>
    &&assignable_from<T &, T> &&swappable<T>;

template<class T>
ALA_CONCEPT copyable = copy_constructible<T> &&movable<T> &&assignable_from<T &, T &>
    &&assignable_from<T &, const T &> &&assignable_from<T &, const T>;

template<class T>
ALA_CONCEPT semiregular = copyable<T> &&default_initializable<T>;

template<class T>
ALA_CONCEPT regular = semiregular<T> &&equality_comparable<T>;

struct _invocable {
    template<class F, class... Args>
    auto _requires(F &&f, Args &&...args)
        -> decltype(ala::invoke(static_cast<F &&>(f),
                                static_cast<Args &&>(args)...));
};

template<class F, class... Args>
ALA_CONCEPT invocable = _requires<_invocable, F, Args...>;

template<class F, class... Args>
ALA_CONCEPT regular_invocable = invocable<F, Args...>;

template<class F, class... Args>
ALA_CONCEPT predicate = regular_invocable<F, Args...>
    &&__boolean_testable<invoke_result_t<F, Args...>>;

template<class R, class T, class U>
ALA_CONCEPT relation = predicate<R, T, T> &&predicate<R, U, U>
    &&predicate<R, T, U> &&predicate<R, U, T>;

template<class R, class T, class U>
ALA_CONCEPT equivalence_relation = relation<R, T, U>;

template<class R, class T, class U>
ALA_CONCEPT strict_weak_order = relation<R, T, U>;
} // namespace ala

#endif
