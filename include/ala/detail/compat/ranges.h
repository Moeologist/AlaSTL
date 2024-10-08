#ifndef _ALA_COMPAT_RANGES_H
#define _ALA_COMPAT_RANGES_H

#include <ala/iterator.h>

namespace ala {

namespace ranges {

template<class R>
ALA_INLINE_CONSTEXPR_V bool enable_borrowed_range = false;

template<class T>
ALA_CONCEPT __can_borrow = is_lvalue_reference_v<T> ||
                           enable_borrowed_range<remove_cvref_t<T>>;

// Work around Clang bug https://llvm.org/PR52970
// TODO: remove this workaround once libc++ no longer has to support Clang 13 (it was fixed in Clang 14).
template<class T>
ALA_CONCEPT __workaround_52970 = is_class_v<remove_cvref_t<T>> ||
                                 is_union_v<remove_cvref_t<T>>;

namespace _begin {

struct __member_begin_c {
    template<class T>
    auto _requires(T &&t) -> enable_if_t<input_or_output_iterator<
        decltype(static_cast<decay_t<decltype((t.begin()))>>(t.begin()))>>;
};

template<class T>
ALA_CONCEPT __member_begin = __can_borrow<T> && __workaround_52970<T> &&
                             _requires<__member_begin_c, T>;

template<class T>
void begin(T &) = delete;

template<class T>
void begin(const T &) = delete;

struct __adl_begin_c {
    template<class T>
    auto _requires(T &&t) -> enable_if_t<input_or_output_iterator<
        decltype(static_cast<decay_t<decltype((begin(t)))>>(begin(t)))>>;
};

template<class T>
ALA_CONCEPT __adl_begin = !__member_begin<T> && __can_borrow<T> &&
                          __class_or_enum<remove_cvref_t<T>> &&
                          _requires<__adl_begin_c, T>;

struct _cpo_fn {
    template<class T>
    ALA_NODISCARD constexpr auto operator()(T (&t)[]) const noexcept
        -> enable_if_t<(sizeof(T) != 0), decltype(t + 0)> // Disallow incomplete element types.
    {
        return t + 0;
    }

    template<class T, size_t N>
    ALA_NODISCARD constexpr auto operator()(T (&t)[N]) const noexcept
        -> enable_if_t<(sizeof(T) != 0), decltype(t + 0)> // Disallow incomplete element types.
    {
        return t + 0;
    }

    template<class T>
    ALA_NODISCARD constexpr auto _dispatch(T &&t, priority_tag<1>) const
        noexcept(noexcept(static_cast<decay_t<decltype((t.begin()))>>(t.begin())))
            -> enable_if_t<__member_begin<T>, decay_t<decltype((t.begin()))>> {
        return static_cast<decay_t<decltype((t.begin()))>>(t.begin());
    }

    template<class T>
    ALA_NODISCARD constexpr auto _dispatch(T &&t, priority_tag<0>) const
        noexcept(noexcept(static_cast<decay_t<decltype((begin(t)))>>(begin(t))))
            -> enable_if_t<__adl_begin<T>, decay_t<decltype((begin(t)))>> {
        return static_cast<decay_t<decltype((begin(t)))>>(begin(t));
    }

    template<class T>
    void _dispatch(T &&, priority_tag<0>) const = delete;

    template<typename T>
    constexpr auto operator()(T &&t) const noexcept(
        noexcept(_cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{})))
        -> decltype(_cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{})) {
        return _cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{});
    }
};
} // namespace _begin

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto begin = _begin::_cpo_fn{};
} // namespace _cpos

template<class T>
using iterator_t = decltype(ranges::begin(declval<T &>()));

namespace _end {

struct __member_end_c {
    template<class T>
    auto _requires(T &&t) -> enable_if_t<sentinel_for<
        decltype(static_cast<decay_t<decltype((t.end()))>>(t.end())), iterator_t<T>>>;
};

template<class T>
ALA_CONCEPT __member_end = __can_borrow<T> && __workaround_52970<T> &&
                           _requires<__member_end_c, T>;

template<class T>
void end(T &) = delete;

template<class T>
void end(const T &) = delete;

struct __adl_end_c {
    template<class T>
    auto _requires(T &&t) -> enable_if_t<sentinel_for<
        decltype(static_cast<decay_t<decltype((end(t)))>>(end(t))), iterator_t<T>>>;
};

template<class T>
ALA_CONCEPT __adl_end = !__member_end<T> && __can_borrow<T> &&
                        __class_or_enum<remove_cvref_t<T>> &&
                        _requires<__adl_end_c, T>;

class _cpo_fn {
public:
    template<class T, size_t N>
    ALA_NODISCARD constexpr auto operator()(T (&t)[N]) const noexcept
        -> enable_if_t<(sizeof(T) != 0), decltype(t + N)> // Disallow incomplete element types.
    {
        return t + N;
    }

    template<class T>
    ALA_NODISCARD constexpr auto _dispatch(T &&t, priority_tag<2>) const
        noexcept(noexcept(static_cast<decay_t<decltype((t.end()))>>(t.end())))
            -> enable_if_t<__member_end<T>, decay_t<decltype((t.end()))>> {
        return static_cast<decay_t<decltype((t.end()))>>(t.end());
    }

    template<class T>
    ALA_NODISCARD constexpr auto _dispatch(T &&t, priority_tag<1>) const
        noexcept(noexcept(static_cast<decay_t<decltype((end(t)))>>(end(t))))
            -> enable_if_t<__adl_end<T>, decay_t<decltype((begin(t)))>> {
        return static_cast<decay_t<decltype((end(t)))>>(end(t));
    }

    template<class T>
    void _dispatch(T &&, priority_tag<0>) const = delete;

    template<typename T>
    constexpr auto operator()(T &&t) const noexcept(
        noexcept(_cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{})))
        -> decltype(_cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{})) {
        return _cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{});
    }
};
} // namespace _end

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto end = _end::_cpo_fn{};
} // namespace _cpos

namespace _cbegin {
struct _cpo_fn {
    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const noexcept(
        noexcept(ranges::begin(static_cast<const remove_reference_t<T> &>(t))))
        -> enable_if_t<is_lvalue_reference_v<T &&>,
                       decltype(ranges::begin(
                           static_cast<const remove_reference_t<T> &>(t)))> {
        return ranges::begin(static_cast<const remove_reference_t<T> &>(t));
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(ranges::begin(static_cast<const T &&>(t))))
            -> enable_if_t<is_rvalue_reference_v<T &&>,
                           decltype(ranges::begin(static_cast<const T &&>(t)))> {
        return ranges::begin(static_cast<const T &&>(t));
    }
};
} // namespace _cbegin

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto cbegin = _cbegin::_cpo_fn{};
} // namespace _cpos

namespace _cend {
struct _cpo_fn {
    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const noexcept(
        noexcept(ranges::end(static_cast<const remove_reference_t<T> &>(t))))
        -> enable_if_t<
            is_lvalue_reference_v<T &&>,
            decltype(ranges::end(static_cast<const remove_reference_t<T> &>(t)))> {
        return ranges::end(static_cast<const remove_reference_t<T> &>(t));
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(ranges::end(static_cast<const T &&>(t))))
            -> enable_if_t<is_rvalue_reference_v<T &&>,
                           decltype(ranges::end(static_cast<const T &&>(t)))> {
        return ranges::end(static_cast<const T &&>(t));
    }
};
} // namespace _cend

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto cend = _cend::_cpo_fn{};
} // namespace _cpos

template<class>
ALA_INLINE_CONSTEXPR_V bool disable_sized_range = false;

namespace _size {
template<class T>
void size(T &) = delete;

template<class T>
void size(const T &) = delete;

template<class T>
ALA_CONCEPT __size_enabled = !disable_sized_range<remove_cvref_t<T>>;

struct __member_size_c {
    template<class T>
    auto _requires(T &&t) -> enable_if_t<__integeral_arithmetic<
        decltype(static_cast<decay_t<decltype((t.size()))>>(t.size()))>>;
};

template<class T>
ALA_CONCEPT __member_size = __size_enabled<T> && __workaround_52970<T> &&
                            _requires<__member_size_c, T>;

struct __adl_size_c {
    template<class T>
    auto _requires(T &&t) -> enable_if_t<__integeral_arithmetic<
        decltype(static_cast<decay_t<decltype((size(t)))>>(size(t)))>>;
};

template<class T>
ALA_CONCEPT __adl_size = __size_enabled<T> && !__member_size<T> &&
                         __class_or_enum<remove_cvref_t<T>> &&
                         _requires<__adl_size_c, T>;

struct __difference_c {
    template<class T>
    auto _requires(T &&t)
        -> enable_if_t<forward_iterator<decltype(ranges::begin(t))> &&
                       sized_sentinel_for<decltype(ranges::end(t)),
                                          decltype(ranges::begin(declval<T>()))>>;
};

template<class T>
ALA_CONCEPT __difference = !__member_size<T> && !__adl_size<T> &&
                           __class_or_enum<remove_cvref_t<T>> &&
                           _requires<__difference_c, T>;

struct _cpo_fn {
    template<class T, size_t Sz>
    ALA_NODISCARD constexpr size_t operator()(T (&&)[Sz]) const noexcept {
        return Sz;
    }

    template<class T, size_t Sz>
    ALA_NODISCARD constexpr size_t operator()(T (&)[Sz]) const noexcept {
        return Sz;
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const noexcept(
        noexcept(static_cast<decay_t<decltype((t.size()))>>(t.size())))
        -> enable_if_t<__member_size<T> &&
                           __integeral_arithmetic<decay_t<decltype((t.size()))>>,
                       decay_t<decltype((t.size()))>> {
        return static_cast<decay_t<decltype((t.size()))>>(t.size());
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((size(t)))>>(size(t))))
            -> enable_if_t<__adl_size<T> &&
                               __integeral_arithmetic<decay_t<decltype((size(t)))>>,
                           decay_t<decltype((size(t)))>> {
        return static_cast<decay_t<decltype((size(t)))>>(size(t));
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(ranges::end(t) - ranges::begin(t))) -> enable_if_t<
            __difference<T> &&
                __integeral_arithmetic<make_unsigned_t<
                    remove_cvref_t<decltype(ranges::end(t) - ranges::begin(t))>>>,
            make_unsigned_t<remove_cvref_t<decltype(ranges::end(t) - ranges::begin(t))>>> {
        return static_cast<make_unsigned_t<
            remove_cvref_t<decltype(ranges::end(t) - ranges::begin(t))>>>(
            ranges::end(t) - ranges::begin(t));
    }
};
} // namespace _size

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto size = _size::_cpo_fn{};
} // namespace _cpos

namespace _ssize {
struct _cpo_fn {
    struct _c {
        template<class T>
        auto _requires(T &&t) -> decltype(ranges::size(t));
    };

    template<class T>
    constexpr decltype(auto) _dispatch(T &&t, true_type) const {
        return static_cast<ptrdiff_t>(ranges::size(t));
    }

    template<class T>
    constexpr decltype(auto) _dispatch(T &&t, false_type) const {
        return static_cast<make_signed_t<decltype(ranges::size(t))>>(
            ranges::size(t));
    }

    template<class T, class = enable_if_t<_requires<_c, T>>>
    constexpr auto _call(T &&t) const {
        return this->_dispatch(
            ala::forward<T>(t),
            bool_constant<(sizeof(ptrdiff_t) >
                           sizeof(make_signed_t<decltype(ranges::size(t))>))>{});
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(ranges::size(t)))
            -> enable_if_t<integral<decltype(_cpo_fn::_call(ala::forward<T>(t)))>,
                           decltype(_cpo_fn::_call(ala::forward<T>(t)))> {
        return _cpo_fn::_call(ala::forward<T>(t));
    }
};
} // namespace _ssize

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto ssize = _ssize::_cpo_fn{};
} // namespace _cpos

namespace _empty {
struct _cpo_fn {
    template<typename T>
    static constexpr auto _dispatch(T &&t, priority_tag<2>) noexcept(
        noexcept((bool(ala::forward<T>(t).empty()))))
        -> decltype((bool(ala::forward<T>(t).empty()))) {
        return bool((ala::forward<T>(t).empty()));
    }

    template<typename T>
    static constexpr auto _dispatch(T &&t, priority_tag<1>) noexcept(
        noexcept(ranges::size(ala::forward<T>(t)) == 0))
        -> decltype(ranges::size(ala::forward<T>(t)) == 0) {
        return ranges::size(ala::forward<T>(t)) == 0;
    }

    template<typename T, typename I = decltype(ranges::begin(ala::declval<T>()))>
    static constexpr auto
    _dispatch(T &&t, priority_tag<0>) noexcept(noexcept(ranges::begin(t) ==
                                                        ranges::end(t)))
        -> ala::enable_if_t<forward_iterator<I>,
                            decltype(ranges::begin(t) == ranges::end(t))> {
        return ranges::begin(t) == ranges::end(t);
    }

    template<typename T>
    constexpr auto operator()(T &&t) const noexcept(
        noexcept(_cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{})))
        -> decltype(_cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{})) {
        return _cpo_fn::_dispatch(ala::forward<T>(t), priority_tag<2>{});
    }
};
} // namespace _empty

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto empty = _empty::_cpo_fn{};
} // namespace _cpos

struct _range_c {
    template<class T>
    auto _requires(T &t)
        -> decltype(ranges::begin(t),
                    ranges::end(t)); // equality-preserving for forward iterators
};

template<class T>
ALA_CONCEPT range = _requires<_range_c, T>;

struct _sized_range_c {
    template<class T>
    auto _requires(T &t) -> decltype(ranges::size(t));
};

template<class T>
ALA_CONCEPT sized_range = ranges::range<T> && _requires<_sized_range_c, T>;

template<class R>
using sentinel_t =
    enable_if_t<ranges::range<R>, decltype(ranges::end(declval<R &>()))>;

template<class R>
using range_size_t =
    enable_if_t<ranges::sized_range<R>, decltype(ranges::size(declval<R &>()))>;

template<class R>
using range_difference_t =
    enable_if_t<ranges::range<R>, iter_difference_t<ranges::iterator_t<R>>>;

template<class R>
using range_value_t =
    enable_if_t<ranges::range<R>, iter_value_t<ranges::iterator_t<R>>>;

template<class R>
using range_reference_t =
    enable_if_t<ranges::range<R>, iter_reference_t<ranges::iterator_t<R>>>;

template<class R>
using range_rvalue_reference_t =
    enable_if_t<ranges::range<R>, iter_rvalue_reference_t<ranges::iterator_t<R>>>;

template<class R>
ALA_CONCEPT borrowed_range = ranges::range<R> && is_lvalue_reference_v<R> ||
                             enable_borrowed_range<remove_cvref_t<R>>;

template<class T>
ALA_CONCEPT input_range = ranges::range<T> &&
                          input_iterator<ranges::iterator_t<T>>;

template<class R, class T>
ALA_CONCEPT output_range = ranges::range<R> &&
                           output_iterator<ranges::iterator_t<R>, T>;

template<class T>
ALA_CONCEPT forward_range = ranges::input_range<T> &&
                            forward_iterator<ranges::iterator_t<T>>;

template<class T>
ALA_CONCEPT bidirectional_range = ranges::forward_range<T> &&
                                  bidirectional_iterator<ranges::iterator_t<T>>;

template<class T>
ALA_CONCEPT random_access_range = ranges::bidirectional_range<T> &&
                                  random_access_iterator<ranges::iterator_t<T>>;

namespace _data {
template<class T>
ALA_CONCEPT __ptr_to_object = is_pointer_v<T> && is_object_v<remove_pointer_t<T>>;

struct __member_data_c {
    template<class T>
    auto _requires(T &t) -> enable_if_t<__ptr_to_object<
        decltype(static_cast<decay_t<decltype((t.data()))>>(t.data()))>>;
};

template<class T>
ALA_CONCEPT __member_data = __can_borrow<T> && __workaround_52970<T> &&
                            _requires<__member_data_c, T>;

struct __ranges_begin_invocable_c {
    template<class T>
    auto _requires(T &t)
        -> enable_if_t<contiguous_iterator<decltype(ranges::begin(t))>>;
};

template<class T>
ALA_CONCEPT __ranges_begin_invocable = !__member_data<T> && __can_borrow<T> &&
                                       _requires<__ranges_begin_invocable_c, T>;

struct _cpo_fn {
    template<class T>
    constexpr auto operator()(T &&t) const noexcept(noexcept(t.data()))
        -> enable_if_t<__member_data<T>, decltype(t.data())> {
        return t.data();
    }

    template<class T>
    constexpr auto operator()(T &&t) const
        noexcept(noexcept(ala::to_address(ranges::begin(t))))
            -> enable_if_t<__ranges_begin_invocable<T>,
                           decltype(ala::to_address(ranges::begin(t)))> {
        return ala::to_address(ranges::begin(t));
    }
};
} // namespace _data

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto data = _data::_cpo_fn{};
} // namespace _cpos

namespace _cdata {
struct _cpo_fn {
    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const noexcept(
        noexcept(ranges::data(static_cast<const remove_reference_t<T> &>(t))))
        -> enable_if_t<is_lvalue_reference_v<T &&>,
                       decltype(ranges::data(
                           static_cast<const remove_reference_t<T> &>(t)))> {
        return ranges::data(static_cast<const remove_reference_t<T> &>(t));
    }

    template<class T>
    ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(ranges::data(static_cast<const T &&>(t))))
            -> enable_if_t<is_lvalue_reference_v<T &&>,
                           decltype(ranges::data(static_cast<const T &&>(t)))> {
        return ranges::data(static_cast<const T &&>(t));
    }
};
} // namespace _cdata

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto cdata = _cdata::_cpo_fn{};
} // namespace _cpos

struct _contiguous_range_c {
    template<class T>
    auto _requires(T &t)
        -> enable_if_t<same_as<decltype(ranges::data(t)),
                               add_pointer_t<ranges::range_reference_t<T>>>>;
};

template<class T>
ALA_CONCEPT contiguous_range = ranges::random_access_range<T> &&
                               contiguous_iterator<ranges::iterator_t<T>> &&
                               _requires<_contiguous_range_c, T>;

template<class T>
ALA_CONCEPT common_range = ranges::range<T> &&
                           same_as<ranges::iterator_t<T>, ranges::sentinel_t<T>>;

namespace _advance {

struct _cpo_fn {
private:
    template<typename T>
    static constexpr T abs(T t) {
        if (t < T{0}) {
            return -t;
        }
        return t;
    }

    template<typename R>
    static constexpr auto impl(R &r, iter_difference_t<R> n)
        -> ala::enable_if_t<random_access_iterator<R>> {
        r += n;
    }

    template<typename I>
    static constexpr auto impl(I &i, iter_difference_t<I> n)
        -> ala::enable_if_t<bidirectional_iterator<I> && !random_access_iterator<I>> {
        constexpr auto zero = iter_difference_t<I>{0};

        if (n > zero) {
            while (n-- > zero) {
                ++i;
            }
        } else {
            while (n++ < zero) {
                --i;
            }
        }
    }

    template<typename I>
    static constexpr auto impl(I &i, iter_difference_t<I> n)
        -> ala::enable_if_t<!bidirectional_iterator<I>> {
        while (n-- > iter_difference_t<I>{0}) {
            ++i;
        }
    }

    template<typename I, typename S>
    static constexpr auto impl(I &i, S bound, priority_tag<2>)
        -> ala::enable_if_t<assignable_from<I &, S>> {
        i = ala::move(bound);
    }

    template<typename I, typename S>
    static constexpr auto impl(I &i, S bound, priority_tag<1>)
        -> ala::enable_if_t<sized_sentinel_for<S, I>> {
        _cpo_fn::impl(i, bound - i);
    }

    template<typename I, typename S>
    static constexpr void impl(I &i, S bound, priority_tag<0>) {
        while (i != bound) {
            ++i;
        }
    }

    template<typename I, typename S>
    static constexpr auto impl(I &i, iter_difference_t<I> n, S bound)
        -> ala::enable_if_t<sized_sentinel_for<S, I>, iter_difference_t<I>> {
        if (_cpo_fn::abs(n) >= _cpo_fn::abs(bound - i)) {
            auto dist = bound - i;
            _cpo_fn::impl(i, bound, priority_tag<2>{});
            return dist;
        } else {
            _cpo_fn::impl(i, n);
            return n;
        }
    }

    template<typename I, typename S>
    static constexpr auto impl(I &i, iter_difference_t<I> n, S bound)
        -> ala::enable_if_t<bidirectional_iterator<I> && !sized_sentinel_for<S, I>,
                            iter_difference_t<I>> {
        constexpr iter_difference_t<I> zero{0};
        iter_difference_t<I> counter{0};

        if (n < zero) {
            do {
                --i;
                --counter; // Yes, really
            } while (++n < zero && i != bound);
        } else {
            while (n-- > zero && i != bound) {
                ++i;
                ++counter;
            }
        }

        return counter;
    }

    template<typename I, typename S>
    static constexpr auto impl(I &i, iter_difference_t<I> n, S bound)
        -> ala::enable_if_t<!bidirectional_iterator<I> && !sized_sentinel_for<S, I>,
                            iter_difference_t<I>> {
        constexpr iter_difference_t<I> zero{0};
        iter_difference_t<I> counter{0};

        while (n-- > zero && i != bound) {
            ++i;
            ++counter;
        }

        return counter;
    }

public:
    template<typename I>
    constexpr auto operator()(I &i, iter_difference_t<I> n) const
        -> ala::enable_if_t<input_or_output_iterator<I>> {
        _cpo_fn::impl(i, n);
    }

    template<typename I, typename S>
    constexpr auto operator()(I &i, S bound) const
        -> ala::enable_if_t<input_or_output_iterator<I> && sentinel_for<S, I>> {
        _cpo_fn::impl(i, bound, priority_tag<2>{});
    }

    template<typename I, typename S>
    constexpr auto operator()(I &i, iter_difference_t<I> n, S bound) const
        -> ala::enable_if_t<input_or_output_iterator<I> && sentinel_for<S, I>,
                            iter_difference_t<I>> {
        return n - _cpo_fn::impl(i, n, bound);
    }
};

} // namespace _advance

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto advance = _advance::_cpo_fn{};
} // namespace _cpos

namespace _distance {

struct _cpo_fn {
private:
    template<typename I, typename S>
    static constexpr auto impl(I i, S s)
        -> ala::enable_if_t<sized_sentinel_for<S, I>, iter_difference_t<I>> {
        return s - i;
    }

    template<typename I, typename S>
    static constexpr auto impl(I i, S s)
        -> ala::enable_if_t<!sized_sentinel_for<S, I>, iter_difference_t<I>> {
        iter_difference_t<I> counter{0};
        while (i != s) {
            ++i;
            ++counter;
        }
        return counter;
    }

    template<typename R>
    static constexpr auto impl(R &&r)
        -> ala::enable_if_t<sized_range<R>, iter_difference_t<iterator_t<R>>> {
        return static_cast<iter_difference_t<iterator_t<R>>>(ranges::size(r));
    }

    template<typename R>
    static constexpr auto impl(R &&r)
        -> ala::enable_if_t<!sized_range<R>, iter_difference_t<iterator_t<R>>> {
        return _cpo_fn::impl(ranges::begin(r), ranges::end(r));
    }

public:
    template<typename I, typename S>
    constexpr auto operator()(I first, S last) const
        -> ala::enable_if_t<input_or_output_iterator<I> && sentinel_for<S, I>,
                            iter_difference_t<I>> {
        return _cpo_fn::impl(ala::move(first), ala::move(last));
    }

    template<typename R>
    constexpr auto operator()(R &&r) const
        -> ala::enable_if_t<range<R>, iter_difference_t<iterator_t<R>>> {
        return _cpo_fn::impl(ala::forward<R>(r));
    }
};

} // namespace _distance

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto distance = _distance::_cpo_fn{};
} // namespace _cpos

namespace _next {

struct _cpo_fn {
    template<typename I>
    constexpr auto operator()(I x) const
        -> ala::enable_if_t<input_or_output_iterator<I>, I> {
        ++x;
        return x;
    }

    template<typename I>
    constexpr auto operator()(I x, iter_difference_t<I> n) const
        -> ala::enable_if_t<input_or_output_iterator<I>, I> {
        ranges::advance(x, n);
        return x;
    }

    template<typename I, typename S>
    constexpr auto operator()(I x, S bound) const
        -> ala::enable_if_t<input_or_output_iterator<I> && sentinel_for<S, I>, I> {
        ranges::advance(x, bound);
        return x;
    }

    template<typename I, typename S>
    constexpr auto operator()(I x, iter_difference_t<I> n, S bound) const
        -> ala::enable_if_t<input_or_output_iterator<I> && sentinel_for<S, I>, I> {
        ranges::advance(x, n, bound);
        return x;
    }
};

} // namespace _next

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto next = _next::_cpo_fn{};
} // namespace _cpos

namespace _prev {

struct _cpo_fn {
    template<typename I>
    constexpr auto operator()(I x) const
        -> ala::enable_if_t<bidirectional_iterator<I>, I> {
        --x;
        return x;
    }

    template<typename I>
    constexpr auto operator()(I x, iter_difference_t<I> n) const
        -> ala::enable_if_t<bidirectional_iterator<I>, I> {
        ranges::advance(x, -n);
        return x;
    }

    template<typename I, typename S>
    constexpr auto operator()(I x, iter_difference_t<I> n, S bound) const
        -> ala::enable_if_t<bidirectional_iterator<I> && sentinel_for<S, I>, I> {
        ranges::advance(x, -n, bound);
        return x;
    }
};

} // namespace _prev

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto prev = _prev::_cpo_fn{};
} // namespace _cpos

template<typename D>
class view_interface {
    static_assert(is_class<D>::value && same_as<D, remove_cv_t<D>>,
                  "view_interface requirement not satisfied");

private:
    constexpr D &derived() noexcept {
        return static_cast<D &>(*this);
    }

    constexpr const D &derived() const noexcept {
        return static_cast<const D &>(*this);
    }

public:
    template<typename R = D>
    ALA_NODISCARD constexpr auto empty() -> enable_if_t<forward_range<R>, bool> {
        return ranges::begin(derived()) == ranges::end(derived());
    }

    template<typename R = D>
    ALA_NODISCARD constexpr auto empty() const
        -> enable_if_t<forward_range<const R>, bool> {
        return ranges::begin(derived()) == ranges::end(derived());
    }

    template<typename R = D, typename = decltype(ranges::empty(ala::declval<R &>()))>
    constexpr explicit operator bool() {
        return !ranges::empty(derived());
    }

    template<typename R = D,
             typename = decltype(ranges::empty(ala::declval<const R &>()))>
    constexpr explicit operator bool() const {
        return !ranges::empty(derived());
    }

    template<typename R = D,
             typename = enable_if_t<contiguous_iterator<iterator_t<R>>>>
    constexpr auto data() {
        return ranges::empty(derived()) ? nullptr :
                                          addressof(*ranges::begin(derived()));
    }

    template<typename R = D,
             typename = enable_if_t<range<const R> &&
                                    contiguous_iterator<iterator_t<const R>>>>
    constexpr auto data() const {
        return ranges::empty(derived()) ? nullptr :
                                          addressof(*ranges::begin(derived()));
    }

    template<typename R = D,
             typename = enable_if_t<forward_range<R> &&
                                    sized_sentinel_for<sentinel_t<R>, iterator_t<R>>>>
    constexpr auto size() {
        return ranges::end(derived()) - ranges::begin(derived());
    }

    template<typename R = D,
             typename = enable_if_t<
                 forward_range<const R> &&
                 sized_sentinel_for<sentinel_t<const R>, iterator_t<const R>>>>
    constexpr auto size() const {
        return ranges::end(derived()) - ranges::begin(derived());
    }

    template<typename R = D, typename = enable_if_t<forward_range<R>>>
    constexpr decltype(auto) front() {
        return *ranges::begin(derived());
    }

    template<typename R = D, typename = enable_if_t<forward_range<const R>>>
    constexpr decltype(auto) front() const {
        return *ranges::begin(derived());
    }

    template<typename R = D,
             typename = enable_if_t<bidirectional_range<R> && common_range<R>>>
    constexpr decltype(auto) back() {
        return *ranges::prev(ranges::end(derived()));
    }

    template<typename R = D, typename = enable_if_t<bidirectional_range<const R> &&
                                                    common_range<const R>>>
    constexpr decltype(auto) back() const {
        return *ranges::prev(ranges::end(derived()));
    }

    template<typename R = D, typename = enable_if_t<random_access_range<R>>>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<R>> n) {
        return ranges::begin(derived())[n];
    }

    template<typename R = D, typename = enable_if_t<random_access_range<const R>>>
    constexpr decltype(auto)
    operator[](iter_difference_t<iterator_t<const R>> n) const {
        return ranges::begin(derived())[n];
    }

    template<typename R = D>
    constexpr auto cbegin() {
        return ranges::cbegin(derived());
    }

    template<typename R = D, typename = enable_if_t<ranges::range<const R>>>
    constexpr auto cbegin() const {
        return ranges::cbegin(derived());
    }

    template<typename R = D>
    constexpr auto cend() {
        return ranges::cend(derived());
    }

    template<typename R = D, typename = enable_if_t<ranges::range<const R>>>
    constexpr auto cend() const {
        return ranges::cend(derived());
    }
};

} // namespace ranges
} // namespace ala

#endif