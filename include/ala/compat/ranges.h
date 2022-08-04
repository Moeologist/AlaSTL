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
        noexcept(_cpo_fn::_dispatch(std::forward<T>(t), priority_tag<2>{})))
        -> decltype(_cpo_fn::_dispatch(std::forward<T>(t), priority_tag<2>{})) {
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
        noexcept(_cpo_fn::_dispatch(std::forward<T>(t), priority_tag<2>{})))
        -> decltype(_cpo_fn::_dispatch(std::forward<T>(t), priority_tag<2>{})) {
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

} // namespace ranges
} // namespace ala

#endif