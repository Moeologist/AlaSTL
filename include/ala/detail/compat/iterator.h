#ifndef _ALA_COMPAT_ITERATOR_H
#define _ALA_COMPAT_ITERATOR_H

#include <ala/type_traits.h>
#include <ala/detail/memory_base.h>
#include <ala/detail/functional_base.h>
#include <ala/concepts.h>
#include <ala/iterator.h>

namespace ala {
namespace ranges {
namespace _iter_move {

void iter_move();

struct _enable_adl_c {
    template<class I>
    auto _requires(I &&i) -> decltype(iter_move(ala::forward<I>(i)));
};

template<class I>
ALA_CONCEPT __enable_adl = __class_or_enum<remove_cvref_t<I>> &&
                           _requires<_enable_adl_c, I>;

struct _cpo_fn {
    template<class I>
    ALA_NODISCARD constexpr auto operator()(I &&i) const
        noexcept(noexcept(iter_move(ala::forward<I>(i))))
            -> enable_if_t<__class_or_enum<remove_cvref_t<I>> && __enable_adl<I>,
                           decltype(iter_move(ala::forward<I>(i)))> {
        return iter_move(ala::forward<I>(i));
    }

    template<class I>
    constexpr decltype(auto) _dispatch(I &&i, true_type) const {
        return ala::move(*ala::forward<I>(i));
    }

    template<class I>
    constexpr decltype(auto) _dispatch(I &&i, false_type) const {
        return *ala::forward<I>(i);
    }

    template<class I>
    ALA_NODISCARD constexpr auto
    operator()(I &&i) const noexcept(noexcept(*ala::forward<I>(i)))
        -> enable_if_t<!(__class_or_enum<remove_cvref_t<I>> &&
                         __enable_adl<I>),
                       decltype(this->_dispatch(
                           ala::forward<I>(i),
                           is_lvalue_reference<decltype(*ala::forward<I>(i))>{}))> {
        return this->_dispatch(
            ala::forward<I>(i),
            is_lvalue_reference<decltype(*ala::forward<I>(i))>{});
    }
};
} // namespace _iter_move
inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V _iter_move::_cpo_fn iter_move;
} // namespace _cpos

} // namespace ranges

template<class, class = void>
struct _rmcv_for_obj {};

template<class T>
struct _rmcv_for_obj<T, enable_if_t<is_object_v<T>>> {
    using value_type = remove_cv_t<T>;
};

struct __has_member_vt_c {
    template<class T>
    auto _requires() -> typename T::value_type;
};

template<class T>
ALA_CONCEPT __has_member_value_type = _requires<__has_member_vt_c, T>;

struct __has_member_et_c {
    template<class T>
    auto _requires() -> typename T::element_type;
};

template<class T>
ALA_CONCEPT __has_member_element_type = _requires<__has_member_et_c, T>;

template<class T, bool = __has_member_value_type<T>,
         bool = __has_member_element_type<T>>
struct _indirectly_readable_traits_mem {};

template<class T>
struct _indirectly_readable_traits_mem<T, true, false>
    : _rmcv_for_obj<typename T::value_type> {};

template<class T>
struct _indirectly_readable_traits_mem<T, false, true>
    : _rmcv_for_obj<typename T::element_type> {};

template<class T, bool = same_as<remove_cv_t<typename T::element_type>,
                                 remove_cv_t<typename T::value_type>>>
struct _indirectly_readable_traits_ev {};

template<class T>
struct _indirectly_readable_traits_ev<T, true>
    : _rmcv_for_obj<typename T::value_type> {};

template<class T>
struct _indirectly_readable_traits_mem<T, true, true>
    : _indirectly_readable_traits_ev<T> {};

template<class I, bool = is_array_v<I>>
struct _indirectly_readable_traits: _indirectly_readable_traits_mem<I> {};

template<class I>
struct _indirectly_readable_traits<I, true> {
    using value_type = remove_cv_t<remove_extent_t<I>>;
};

template<class I>
struct indirectly_readable_traits: _indirectly_readable_traits<I> {};

template<class I>
struct indirectly_readable_traits<const I>: indirectly_readable_traits<I> {};

template<class T>
struct indirectly_readable_traits<T *>: _rmcv_for_obj<T> {};

template<class Iter>
using iter_value_t = typename conditional_t<
    _has__iter_traits_no_spec<iterator_traits<remove_cvref_t<Iter>>>::value,
    indirectly_readable_traits<remove_cvref_t<Iter>>,
    iterator_traits<remove_cvref_t<Iter>>>::value_type;

template<class T>
using _lvalue_ref_t = T &;

struct __referenceable_c {
    template<class T>
    auto _requires() -> _lvalue_ref_t<T>;
};

template<class T>
ALA_CONCEPT __referenceable = _requires<__referenceable_c, T>;

struct __dereferenceable_c {
    template<class T>
    auto _requires(T &t) -> enable_if_t<__referenceable<decltype(*t)>>;
};

template<class T>
ALA_CONCEPT __dereferenceable = _requires<__dereferenceable_c, T>;

template<class T>
using iter_reference_t =
    enable_if_t<__dereferenceable<T>, decltype(*declval<T &>())>;

struct __has_member_dt_c {
    template<class T>
    auto _requires() -> typename T::difference_type;
};

template<class T>
ALA_CONCEPT __has_member_difference_type = _requires<__has_member_dt_c, T>;

struct __has_integral_minus_c {
    template<class T>
    auto _requires(const T &x, const T &y)
        -> enable_if_t<integral<decltype(x - y)>>;
};

template<class T>
ALA_CONCEPT __has_integral_minus = _requires<__has_integral_minus_c, T>;

template<class T, bool = __has_member_difference_type<T>,
         bool = __has_integral_minus<T>>
struct _incrementable_traits_mem {
    using difference_type = typename T::difference_type;
};

template<class T>
struct _incrementable_traits_mem<T, false, true> {
    using difference_type = make_signed_t<decltype(declval<T>() - declval<T>())>;
};

template<class T>
struct _incrementable_traits_mem<T, false, false> {};

template<class T>
struct incrementable_traits: _incrementable_traits_mem<T> {};

template<class T, bool = is_object_v<T>>
struct _incrementable_traits_ptr {};

template<class T>
struct _incrementable_traits_ptr<T, true> {
    using difference_type = ptrdiff_t;
};

template<class T>
struct incrementable_traits<T *>: _incrementable_traits_ptr<T> {};

template<class I>
struct incrementable_traits<const I>: incrementable_traits<I> {};

template<class T>
using iter_difference_t = typename conditional_t<
    _has__iter_traits_no_spec<iterator_traits<remove_cvref_t<T>>>::value,
    incrementable_traits<remove_cvref_t<T>>,
    iterator_traits<remove_cvref_t<T>>>::difference_type;

struct _iter_rvalue_reference_t_c {
    template<class T>
    auto _requires(T &t)
        -> enable_if_t<__referenceable<decltype(ranges::iter_move(t))>>;
};

template<class T>
using iter_rvalue_reference_t =
    enable_if_t<__dereferenceable<T> && _requires<_iter_rvalue_reference_t_c, T>,
                decltype(ranges::iter_move(declval<T &>()))>;

struct __indirectly_readable_impl_c {
    template<class T>
    auto _requires(const T t)
        -> void_t<iter_value_t<T>, iter_reference_t<T>, iter_rvalue_reference_t<T>,
                  enable_if_t<same_as<decltype(*t), iter_reference_t<T>> &&
                              same_as<decltype(ranges::iter_move(t)),
                                      iter_rvalue_reference_t<T>>>>;
};

template<class T>
ALA_CONCEPT __indirectly_readable_impl =
    _requires<__indirectly_readable_impl_c, T> &&
    common_reference_with<iter_reference_t<T> &&, iter_value_t<T> &> &&
    common_reference_with<iter_reference_t<T> &&, iter_rvalue_reference_t<T> &&> &&
    common_reference_with<iter_rvalue_reference_t<T> &&, const iter_value_t<T> &>;

template<class T>
ALA_CONCEPT indirectly_readable = __indirectly_readable_impl<remove_cvref_t<T>>;

template<class T>
using iter_common_reference_t =
    enable_if_t<indirectly_readable<T>,
                common_reference_t<iter_reference_t<T>, iter_value_t<T> &>>;

struct _indirectly_writable_c {
    template<class Out, class T>
    auto _requires(Out &&o, T &&t)
        -> decltype(*o = ala::forward<T>(t),
                    *ala::forward<Out>(o) = ala::forward<T>(t),
                    const_cast<const iter_reference_t<Out> &&>(*o) =
                        ala::forward<T>(t),
                    const_cast<const iter_reference_t<Out> &&>(
                        *ala::forward<Out>(o)) = ala::forward<T>(t));
};

template<class Out, class T>
ALA_CONCEPT indirectly_writable = _requires<_indirectly_writable_c, Out, T>;

template<class T>
ALA_CONCEPT __signed_integer_like = signed_integral<T>;

struct _weakly_incrementable_c {
    template<class I>
    auto _requires(I i)
        -> void_t<iter_difference_t<I>,
                  enable_if_t<__signed_integer_like<iter_difference_t<I>>>,
                  enable_if_t<same_as<decltype(++i), I &>>,
                  decltype(i++) // not required to be equality-preserving
                  >;
};

template<class I>
ALA_CONCEPT weakly_incrementable =
    // TODO: remove this once the clang bug is fixed (bugs.llvm.org/PR48173).
    (!same_as<I, bool>)&& // Currently, clang does not handle bool correctly.
    movable<I> &&
    _requires<_weakly_incrementable_c, I>;

struct _incrementable_c {
    template<class I>
    auto _requires(I i) -> enable_if_t<same_as<decltype(i++), I>>;
};

template<class I>
ALA_CONCEPT incrementable = regular<I> && weakly_incrementable<I> &&
                            _requires<_incrementable_c, I>;

struct _input_or_output_iterator_c {
    template<class I>
    auto _requires(I i) -> enable_if_t<__referenceable<decltype(*i)>>;
};

template<class I>
ALA_CONCEPT input_or_output_iterator = _requires<_input_or_output_iterator_c, I> &&
                                       weakly_incrementable<I>;

template<class S, class I>
ALA_CONCEPT sentinel_for = semiregular<S> && input_or_output_iterator<I> &&
                           __weakly_equality_comparable_with<S, I>;

template<class, class>
ALA_INLINE_CONSTEXPR_V bool disable_sized_sentinel_for = false;

struct _sized_sentinel_for_c {
    template<class S, class I>
    auto _requires(const I &i, const S &s)
        -> enable_if_t<same_as<decltype(s - i), iter_difference_t<I>> &&
                       same_as<decltype(i - s), iter_difference_t<I>>>;
};

template<class S, class I>
ALA_CONCEPT sized_sentinel_for =
    sentinel_for<S, I> &&
    !disable_sized_sentinel_for<remove_cv_t<S>, remove_cv_t<I>> &&
    _requires<_sized_sentinel_for_c, S, I>;

struct _input_iterator_c {
    template<class I>
    auto _requires() -> _iter_concept_t<I>;
};

template<class I>
ALA_CONCEPT input_iterator = input_or_output_iterator<I> &&
                             indirectly_readable<I> &&
                             _requires<_input_iterator_c, I> &&
                             derived_from<_iter_concept_t<I>, input_iterator_tag>;

struct _output_iterator_c {
    template<class I, class T>
    auto _requires(I i, T &&t) -> decltype(*i++ = ala::forward<T>(t));
};

template<class I, class T>
ALA_CONCEPT output_iterator = input_or_output_iterator<I> &&
                              indirectly_writable<I, T> &&
                              _requires<_output_iterator_c, I, T>;

template<class I>
ALA_CONCEPT forward_iterator =
    input_iterator<I> && derived_from<_iter_concept_t<I>, forward_iterator_tag> &&
    incrementable<I> && sentinel_for<I, I>;

struct _bidirectional_iterator_c {
    template<class I>
    auto _requires(I i)
        -> enable_if_t<same_as<decltype(--i), I &> && same_as<decltype(i--), I>>;
};

template<class I>
ALA_CONCEPT bidirectional_iterator =
    forward_iterator<I> &&
    derived_from<_iter_concept_t<I>, bidirectional_iterator_tag> &&
    _requires<_bidirectional_iterator_c, I>;

struct _random_access_iterator_c {
    template<class I>
    auto _requires(I i, const I j, const iter_difference_t<I> n)
        -> enable_if_t<same_as<decltype(i += n), I &> &&
                       same_as<decltype(j + n), I> && same_as<decltype(n + j), I> &&
                       same_as<decltype(i -= n), I &> && same_as<decltype(j - n), I> &&
                       same_as<decltype(j[n]), iter_reference_t<I>>>;
};

template<class I>
ALA_CONCEPT random_access_iterator =
    bidirectional_iterator<I> &&
    derived_from<_iter_concept_t<I>, random_access_iterator_tag> &&
    totally_ordered<I> && sized_sentinel_for<I, I> &&
    _requires<_random_access_iterator_c, I>;

struct _contiguous_iterator_c {
    template<class I>
    auto _requires(const I &i) -> enable_if_t<
        same_as<decltype(ala::to_address(i)), add_pointer_t<iter_reference_t<I>>>>;
};

template<class I>
ALA_CONCEPT contiguous_iterator =
    random_access_iterator<I> &&
    derived_from<_iter_concept_t<I>, contiguous_iterator_tag> &&
    is_lvalue_reference_v<iter_reference_t<I>> &&
    same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>> &&
    _requires<_contiguous_iterator_c, I>;

// clang-format off
template<class F, class It>
ALA_CONCEPT indirectly_unary_invocable = indirectly_readable<It> &&
                                     copy_constructible<F> &&
                                     invocable<F &, iter_value_t<It> &> &&
                                     invocable<F &, iter_reference_t<It>> &&
                                     invocable<F &, iter_common_reference_t<It>> &&
                                     common_reference_with<invoke_result_t<F &, iter_value_t<It> &>,
                                                           invoke_result_t<F &, iter_reference_t<It>>>;

template<class F, class It>
ALA_CONCEPT indirectly_regular_unary_invocable = indirectly_readable<It> &&
                                             copy_constructible<F> &&
                                             regular_invocable<F &, iter_value_t<It> &> &&
                                             regular_invocable<F &, iter_reference_t<It>> &&
                                             regular_invocable<F &, iter_common_reference_t<It>> &&
                                             common_reference_with<invoke_result_t<F &, iter_value_t<It> &>,
                                                                   invoke_result_t<F &, iter_reference_t<It>>>;

template<class F, class It>
ALA_CONCEPT indirect_unary_predicate = indirectly_readable<It> &&
                                   copy_constructible<F> &&
                                   predicate<F &, iter_value_t<It> &> &&
                                   predicate<F &, iter_reference_t<It>> &&
                                   predicate<F &, iter_common_reference_t<It>>;

template<class F, class It1, class It2>
ALA_CONCEPT indirect_binary_predicate = indirectly_readable<It1> &&
                                    indirectly_readable<It2> &&
                                    copy_constructible<F> &&
                                    predicate<F &, iter_value_t<It1> &, iter_value_t<It2> &> &&
                                    predicate<F &, iter_value_t<It1> &, iter_reference_t<It2>> &&
                                    predicate<F &, iter_reference_t<It1>, iter_value_t<It2> &> &&
                                    predicate<F &, iter_reference_t<It1>, iter_reference_t<It2>> &&
                                    predicate<F &, iter_common_reference_t<It1>, iter_common_reference_t<It2>>;

template<class F, class It1, class It2 = It1>
ALA_CONCEPT indirect_equivalence_relation = indirectly_readable<It1> &&
                                        indirectly_readable<It2> &&
                                        copy_constructible<F> &&
                                        equivalence_relation<F &, iter_value_t<It1> &, iter_value_t<It2> &> &&
                                        equivalence_relation<F &, iter_value_t<It1> &, iter_reference_t<It2>> &&
                                        equivalence_relation<F &, iter_reference_t<It1>, iter_value_t<It2> &> &&
                                        equivalence_relation<F &, iter_reference_t<It1>, iter_reference_t<It2>> &&
                                        equivalence_relation<F &, iter_common_reference_t<It1>, iter_common_reference_t<It2>>;

template<class F, class It1, class It2 = It1>
ALA_CONCEPT indirect_strict_weak_order = indirectly_readable<It1> &&
                                     indirectly_readable<It2> &&
                                     copy_constructible<F> &&
                                     strict_weak_order<F &,iter_value_t<It1>&, iter_value_t<It2> &> &&
                                     strict_weak_order<F &, iter_value_t<It1> &, iter_reference_t<It2>> &&
                                     strict_weak_order<F &, iter_reference_t<It1>, iter_value_t<It2> &> &&
                                     strict_weak_order<F &, iter_reference_t<It1>, iter_reference_t<It2>> &&
                                     strict_weak_order<F &, iter_common_reference_t<It1>, iter_common_reference_t<It2>>;

template<class F, class... _Its>
using indirect_result_t =  enable_if_t<
    _and_<bool_constant<indirectly_readable<_Its>>...>::value && invocable<F, iter_reference_t<_Its>...>,
    invoke_result_t<F, iter_reference_t<_Its>...>>;

template<class In, class Out>
ALA_CONCEPT indirectly_movable = indirectly_readable<In> &&
                             indirectly_writable<Out, iter_rvalue_reference_t<In>>;

template<class In, class Out>
ALA_CONCEPT indirectly_movable_storable = indirectly_movable<In, Out> &&
                                      indirectly_writable<Out, iter_value_t<In>> &&
                                      movable<iter_value_t<In>> &&
                                      constructible_from<iter_value_t<In>, iter_rvalue_reference_t<In>> &&
                                      assignable_from <iter_value_t<In>&, iter_rvalue_reference_t<In>> ;

template<class In, class Out>
ALA_CONCEPT indirectly_copyable = indirectly_readable<In> &&
                              indirectly_writable<Out, iter_reference_t<In>>;

template<class In, class Out>
ALA_CONCEPT indirectly_copyable_storable = indirectly_copyable<In, Out> &&
                                       indirectly_writable<Out, iter_value_t<In>&> &&
                                       indirectly_writable<Out, const iter_value_t<In>&> &&
                                       indirectly_writable<Out, iter_value_t<In>&&> &&
                                       indirectly_writable<Out, const iter_value_t<In>&&> &&
                                       copyable<iter_value_t<In>> &&
                                       constructible_from<iter_value_t<In>, iter_reference_t<In>> &&
                                       assignable_from<iter_value_t<In>&, iter_reference_t<In>>;

// clang-format on

namespace ranges {
namespace _iter_swap {

template<class I1, class I2>
void iter_swap(I1, I2) = delete;

struct __enable_adl_c {
    template<class I1, class I2>
    auto _requires(I1 &&x, I2 &&y)
        -> decltype(iter_swap(ala::forward<I1>(x), ala::forward<I2>(y)));
};

template<class I1, class I2>
ALA_CONCEPT __enable_adl =
    (__class_or_enum<remove_cvref_t<I1>> ||
     __class_or_enum<remove_cvref_t<I2>>)&&_requires<__enable_adl_c, I1, I2>;

template<class I1, class I2>
ALA_CONCEPT __readable_and_swappable =
    indirectly_readable<I1> && indirectly_readable<I2> &&
    swappable_with<iter_reference_t<I1>, iter_reference_t<I2>>;

struct _cpo_fn {
    template<class I1, class I2>
    constexpr enable_if_t<__enable_adl<I1, I2>> operator()(I1 &&x, I2 &&y) const
        noexcept(noexcept(iter_swap(ala::forward<I1>(x), ala::forward<I2>(y)))) {
        (void)iter_swap(ala::forward<I1>(x), ala::forward<I2>(y));
    }

    template<class I1, class I2>
    constexpr enable_if_t<(!__enable_adl<I1, I2>)&&__readable_and_swappable<I1, I2>>
    operator()(I1 &&x, I2 &&y) const
        noexcept(noexcept(ranges::swap(*ala::forward<I1>(x),
                                       *ala::forward<I2>(y)))) {
        ranges::swap(*ala::forward<I1>(x), *ala::forward<I2>(y));
    }

    template<class I1, class I2>
    constexpr enable_if_t<
        (!__enable_adl<I1, I2> &&
         !__readable_and_swappable<I1, I2>)&&indirectly_movable_storable<I1, I2> &&
        indirectly_movable_storable<I2, I1>>
    operator()(I1 &&x, I2 &&y) const
        noexcept(noexcept(iter_value_t<I2>(ranges::iter_move(y)))
                     &&noexcept(*y = ranges::iter_move(x)) &&noexcept(
                         *ala::forward<I1>(x) = declval<iter_value_t<I2>>())) {
        iter_value_t<I2> __old(ranges::iter_move(y));
        *y = ranges::iter_move(x);
        *ala::forward<I1>(x) = ala::move(__old);
    }
};
} // namespace _iter_swap
inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V _iter_swap::_cpo_fn iter_swap;
} // namespace _cpos
} // namespace ranges

struct _indirectly_swappable_c {
    template<class I1, class I2>
    auto _requires(const I1 i1, const I2 i2)
        -> decltype(ranges::iter_swap(i1, i1), ranges::iter_swap(i2, i2),
                    ranges::iter_swap(i1, i2), ranges::iter_swap(i2, i1));
};

template<class I1, class I2 = I1>
ALA_CONCEPT indirectly_swappable = indirectly_readable<I1> &&
                                   indirectly_readable<I2> &&
                                   _requires<_indirectly_swappable_c, I1, I2>;

template<class I, class Proj,
         bool = indirectly_readable<I> &&indirectly_regular_unary_invocable<I, Proj>>
struct _projected {};

template<class I, class Proj>
struct _projected<I, Proj, true> {
    using value_type = remove_cvref_t<indirect_result_t<Proj &, I>>;
    indirect_result_t<Proj &, I> operator*() const; // not defined
};

template<class I, class Proj>
struct projected {};

template<class I, class Proj, bool = weakly_incrementable<I>>
struct _incrementable_traits {};

template<class I, class Proj>
struct _incrementable_traits<I, Proj, true> {
    using difference_type = iter_difference_t<I>;
};

template<class I, class Proj>
struct incrementable_traits<projected<I, Proj>>: _incrementable_traits<I, Proj> {
};

// clang-format off

template <class It1, class It2, class R, class P1 = identity, class P2 = identity>
ALA_CONCEPT indirectly_comparable = indirect_binary_predicate<R, projected<It1, P1>, projected<It2, P2>>;

template <class _It>
ALA_CONCEPT permutable = forward_iterator<_It> &&
                     indirectly_movable_storable<_It, _It> &&
                     indirectly_swappable<_It, _It>;

namespace ranges { struct less; }
template <class It1, class It2, class Out, class R = ranges::less, class P1 = identity, class P2 = identity>
ALA_CONCEPT mergeable = input_iterator<It1> &&
                    input_iterator<It2> &&
                    weakly_incrementable<Out> &&
                    indirectly_copyable<It1, Out> &&
                    indirectly_copyable<It2, Out> &&
                    indirect_strict_weak_order<R, projected<It1, P1>, projected<It2, P2>>;

} // namespace ala

#endif