#ifndef _ALA_RANGES_H
#define _ALA_RANGES_H

#include <ala/concepts.h>
#include <ala/iterator.h>

namespace ala {

namespace ranges {

template<class R>
ALA_INLINE_CONSTEXPR_V bool enable_borrowed_range = false;

template<class T>
concept __can_borrow = is_lvalue_reference_v<T> ||
    enable_borrowed_range<remove_cvref_t<T>>;

template<class T>
concept __workaround_52970 = is_class_v<remove_cvref_t<T>> ||
    is_union_v<remove_cvref_t<T>>;

namespace _begin {
template<class T>
concept __member_begin = __can_borrow<T> && __workaround_52970<T> &&
    requires(T &&t) {
    {
        static_cast<decay_t<decltype((t.begin()))>>(t.begin())
        } -> input_or_output_iterator;
};

void begin(auto &) = delete;
void begin(const auto &) = delete;

template<class T>
concept __adl_begin = !__member_begin<T> && __can_borrow<T> &&
                              __class_or_enum<remove_cvref_t<T>> &&
                              requires(T && t) {
    {
        static_cast<decay_t<decltype((begin(t)))>>(begin(t))
        } -> input_or_output_iterator;
};

struct _cpo_fn {
    template<class T>
    ALA_NODISCARD constexpr auto operator()(T (&t)[]) const noexcept
        requires(sizeof(T) != 0) // Disallow incomplete element types.
    {
        return t + 0;
    }

    template<class T, size_t _Np>
    ALA_NODISCARD constexpr auto operator()(T (&t)[_Np]) const noexcept
        requires(sizeof(T) != 0) // Disallow incomplete element types.
    {
        return t + 0;
    }

    template<class T>
    requires __member_begin<T> ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((t.begin()))>>(t.begin()))) {
        return static_cast<decay_t<decltype((t.begin()))>>(t.begin());
    }

    template<class T>
    requires __adl_begin<T> ALA_NODISCARD constexpr auto
    operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((begin(t)))>>(begin(t)))) {
        return static_cast<decay_t<decltype((begin(t)))>>(begin(t));
    }

    void operator()(auto &&) const = delete;
};
} // namespace _begin

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto begin = _begin::_cpo_fn{};
} // namespace _cpos

template<class T>
using iterator_t = decltype(ranges::begin(declval<T &>()));

namespace _end {
template<class T>
concept __member_end = __can_borrow<T> && __workaround_52970<T> &&
    requires(T &&t) {
    typename iterator_t<T>;
    {
        static_cast<decay_t<decltype((t.end()))>>(t.end())
        } -> sentinel_for<iterator_t<T>>;
};

void end(auto &) = delete;
void end(const auto &) = delete;

template<class T>
concept __adl_end = !__member_end<T> && __can_borrow<T> &&
                            __class_or_enum<remove_cvref_t<T>> &&
                            requires(T && t) {
    typename iterator_t<T>;
    {
        static_cast<decay_t<decltype((end(t)))>>(end(t))
        } -> sentinel_for<iterator_t<T>>;
};

class _cpo_fn {
public:
    template<class T, size_t _Np>
    ALA_NODISCARD constexpr auto operator()(T (&t)[_Np]) const noexcept
        requires(sizeof(T) != 0) // Disallow incomplete element types.
    {
        return t + _Np;
    }

    template<class T>
    requires __member_end<T> ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((t.end()))>>(t.end()))) {
        return static_cast<decay_t<decltype((t.end()))>>(t.end());
    }

    template<class T>
    requires __adl_end<T> ALA_NODISCARD constexpr auto operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((end(t)))>>(end(t)))) {
        return static_cast<decay_t<decltype((end(t)))>>(end(t));
    }

    void operator()(auto &&) const = delete;
};
} // namespace _end

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto end = _end::_cpo_fn{};
} // namespace _cpos

namespace _cbegin {
struct _cpo_fn {
    template<class T>
    requires is_lvalue_reference_v<T &&> ALA_NODISCARD constexpr auto
    operator()(T &&t) const noexcept(
        noexcept(ranges::begin(static_cast<const remove_reference_t<T> &>(t))))
        -> decltype(ranges::begin(static_cast<const remove_reference_t<T> &>(t))) {
        return ranges::begin(static_cast<const remove_reference_t<T> &>(t));
    }

    template<class T>
    requires is_rvalue_reference_v<T &&> ALA_NODISCARD constexpr auto
    operator()(T &&t) const
        noexcept(noexcept(ranges::begin(static_cast<const T &&>(t))))
            -> decltype(ranges::begin(static_cast<const T &&>(t))) {
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
    requires is_lvalue_reference_v<T &&> ALA_NODISCARD constexpr auto
    operator()(T &&t) const noexcept(
        noexcept(ranges::end(static_cast<const remove_reference_t<T> &>(t))))
        -> decltype(ranges::end(static_cast<const remove_reference_t<T> &>(t))) {
        return ranges::end(static_cast<const remove_reference_t<T> &>(t));
    }

    template<class T>
    requires is_rvalue_reference_v<T &&> ALA_NODISCARD constexpr auto
    operator()(T &&t) const
        noexcept(noexcept(ranges::end(static_cast<const T &&>(t))))
            -> decltype(ranges::end(static_cast<const T &&>(t))) {
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
void size(auto &) = delete;
void size(const auto &) = delete;

template<class T>
concept __size_enabled = !disable_sized_range<remove_cvref_t<T>>;

template<class T>
concept __member_size = __size_enabled<T> && __workaround_52970<T> &&
    requires(T &&t) {
    { static_cast<decay_t<decltype((t.size()))>>(t.size()) } -> __integeral_arithmetic;
};

template<class T>
concept __adl_size = __size_enabled<T> && !__member_size<T> &&
                             __class_or_enum<remove_cvref_t<T>> &&
                             requires(T && t) {
    { static_cast<decay_t<decltype((size(t)))>>(size(t)) } -> __integeral_arithmetic;
};

template<class T>
concept __difference = !__member_size<T> && !__adl_size<T> &&
                       __class_or_enum<remove_cvref_t<T>> && requires(T && t) {
    { ranges::begin(t) } -> forward_iterator;
    {
        ranges::end(t)
        } -> sized_sentinel_for<decltype(ranges::begin(declval<T>()))>;
};

struct _cpo_fn {
    template<class T, size_t Sz>
    ALA_NODISCARD constexpr size_t operator()(T (&&)[Sz]) const noexcept {
        return Sz;
    }

    template<class T, size_t Sz>
    ALA_NODISCARD constexpr size_t operator()(T (&)[Sz]) const noexcept {
        return Sz;
    }

    template<__member_size T>
    ALA_NODISCARD constexpr __integeral_arithmetic auto operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((t.size()))>>(t.size()))) {
        return static_cast<decay_t<decltype((t.size()))>>(t.size());
    }

    template<__adl_size T>
    ALA_NODISCARD constexpr __integeral_arithmetic auto operator()(T &&t) const
        noexcept(noexcept(static_cast<decay_t<decltype((size(t)))>>(size(t)))) {
        return static_cast<decay_t<decltype((size(t)))>>(size(t));
    }

    template<__difference T>
    ALA_NODISCARD constexpr __integeral_arithmetic auto operator()(T &&t) const
        noexcept(noexcept(ranges::end(t) - ranges::begin(t))) {
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
    template<class T>
    requires requires(T &&t) {
        ranges::size(t);
    }
    ALA_NODISCARD constexpr integral auto operator()(T &&t) const
        noexcept(noexcept(ranges::size(t))) {
        using _Signed = make_signed_t<decltype(ranges::size(t))>;
        if constexpr (sizeof(ptrdiff_t) > sizeof(_Signed))
            return static_cast<ptrdiff_t>(ranges::size(t));
        else
            return static_cast<_Signed>(ranges::size(t));
    }
};
} // namespace _ssize

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto ssize = _ssize::_cpo_fn{};
} // namespace _cpos

template<class T>
concept range = requires(T &t) {
    ranges::begin(t); // equality-preserving for forward iterators
    ranges::end(t);
};

template<class T>
concept sized_range = ranges::range<T> && requires(T &t) {
    ranges::size(t);
};

template<ranges::range R>
using sentinel_t = decltype(ranges::end(declval<R &>()));

template<ranges::sized_range R>
using range_size_t = decltype(ranges::size(declval<R &>()));

template<ranges::range R>
using range_difference_t = iter_difference_t<ranges::iterator_t<R>>;

template<ranges::range R>
using range_value_t = iter_value_t<ranges::iterator_t<R>>;

template<ranges::range R>
using range_reference_t = iter_reference_t<ranges::iterator_t<R>>;

template<ranges::range R>
using range_rvalue_reference_t = iter_rvalue_reference_t<ranges::iterator_t<R>>;

template<class R>
concept borrowed_range = ranges::range<R> && is_lvalue_reference_v<R> ||
    enable_borrowed_range<remove_cvref_t<R>>;

template<class T>
concept input_range = ranges::range<T> && input_iterator<ranges::iterator_t<T>>;

template<class R, class T>
concept output_range = ranges::range<R> &&
    output_iterator<ranges::iterator_t<R>, T>;

template<class T>
concept forward_range = ranges::input_range<T> &&
    forward_iterator<ranges::iterator_t<T>>;

template<class T>
concept bidirectional_range = ranges::forward_range<T> &&
    bidirectional_iterator<ranges::iterator_t<T>>;

template<class T>
concept random_access_range = ranges::bidirectional_range<T> &&
    random_access_iterator<ranges::iterator_t<T>>;

namespace _data {
template<class T>
concept __ptr_to_object = is_pointer_v<T> && is_object_v<remove_pointer_t<T>>;

template<class T>
concept __member_data = __can_borrow<T> && __workaround_52970<T> &&
    requires(T &&t) {
    { static_cast<decay_t<decltype((t.data()))>>(t.data()) } -> __ptr_to_object;
};

template<class T>
concept __ranges_begin_invocable = !__member_data<T> && __can_borrow<T> &&
                                   requires(T && t) {
    { ranges::begin(t) } -> contiguous_iterator;
};

struct _cpo_fn {
    template<__member_data T>

    constexpr auto operator()(T &&t) const noexcept(noexcept(t.data())) {
        return t.data();
    }

    template<__ranges_begin_invocable T>

    constexpr auto operator()(T &&t) const
        noexcept(noexcept(ala::to_address(ranges::begin(t)))) {
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
    requires is_lvalue_reference_v<T &&> ALA_NODISCARD constexpr auto
    operator()(T &&t) const noexcept(
        noexcept(ranges::data(static_cast<const remove_reference_t<T> &>(t))))
        -> decltype(ranges::data(static_cast<const remove_reference_t<T> &>(t))) {
        return ranges::data(static_cast<const remove_reference_t<T> &>(t));
    }

    template<class T>
    requires is_rvalue_reference_v<T &&> ALA_NODISCARD constexpr auto
    operator()(T &&t) const
        noexcept(noexcept(ranges::data(static_cast<const T &&>(t))))
            -> decltype(ranges::data(static_cast<const T &&>(t))) {
        return ranges::data(static_cast<const T &&>(t));
    }
};
} // namespace _cdata

inline namespace _cpos {
ALA_INLINE_CONSTEXPR_V auto cdata = _cdata::_cpo_fn{};
} // namespace _cpos

template<class T>
concept contiguous_range = ranges::random_access_range<T> &&
    contiguous_iterator<ranges::iterator_t<T>> && requires(T &t) {
    { ranges::data(t) } -> same_as<add_pointer_t<ranges::range_reference_t<T>>>;
};

} // namespace ranges
} // namespace ala

#endif