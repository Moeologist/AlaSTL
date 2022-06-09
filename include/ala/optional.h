#ifndef _ALA_OPTIONAL_H
#define _ALA_OPTIONAL_H

#include <ala/utility.h>
#include <ala/detail/memory_base.h>
#include <ala/detail/controller.h>
#include <ala/detail/monostate.h>
#include <ala/detail/uninitialized_memory.h>
#include <memory>

namespace ala {

struct bad_optional_access: exception {
    virtual ~bad_optional_access() noexcept = default;
    virtual const char *what() const noexcept {
        return "bad_optional_access";
    };
};

struct nullopt_t {
    struct _dummy {};
    explicit constexpr nullopt_t(_dummy) {}
};

#ifdef _ALA_ENABLE_INLINE_VAR
inline constexpr nullopt_t nullopt{nullopt_t::_dummy{}};
#else
constexpr nullopt_t nullopt{nullopt_t::_dummy{}};
#endif

template<class T, bool = is_trivially_destructible<T>::value>
struct _optional_destroy {
    union {
        char _placehold;
        T _value;
    };
    bool _valid = false;
    ~_optional_destroy() = default;
    constexpr void _reset() {
        _valid = false;
    }
    constexpr bool _has_value() const {
        return _valid;
    }
    template<class... Args>
    constexpr _optional_destroy(in_place_t, Args &&...args)
        : _value(ala::forward<Args>(args)...), _valid{true} {}
    constexpr _optional_destroy(): _placehold{} {}
    constexpr _optional_destroy(_optional_destroy &&) = default;
    constexpr _optional_destroy(const _optional_destroy &) = default;
    constexpr _optional_destroy &operator=(const _optional_destroy &) = default;
    constexpr _optional_destroy &operator=(_optional_destroy &&other) = default;
};

template<class T>
struct _optional_destroy<T, false> {
    union {
        char _placehold;
        T _value;
    };
    bool _valid = false;
    constexpr ~_optional_destroy() {
        this->_reset();
    }
    constexpr void _reset() {
        if (this->_has_value()) {
            this->_value.~T();
            this->_valid = false;
        }
    }
    constexpr bool _has_value() const {
        return _valid;
    }
    template<class... Args>
    constexpr _optional_destroy(in_place_t, Args &&...args)
        : _value(ala::forward<Args>(args)...), _valid{true} {}
    constexpr _optional_destroy(): _placehold{} {}
    constexpr _optional_destroy(_optional_destroy &&) = default;
    constexpr _optional_destroy(const _optional_destroy &) = default;
    constexpr _optional_destroy &operator=(const _optional_destroy &) = default;
    constexpr _optional_destroy &operator=(_optional_destroy &&other) = default;
};

template<class T>
struct _optional_base: _optional_destroy<T> {
    using _base_t = _optional_destroy<T>;
    using _base_t::_base_t;
    using _base_t::_has_value;
    using _base_t::_reset;

    constexpr void *_address() {
        return &this->_placehold;
    }

    template<class... Args>
    constexpr void _ctor_v(Args &&...args) {
        assert(!this->_has_value());
        // ::new (this->_address()) T(ala::forward<Args>(args)...);
        std::construct_at(&this->_value, ala::forward<Args>(args)...);
        this->_valid = true;
    }

    template<class OptBase>
    constexpr void _ctor(OptBase &&other) noexcept(
        is_nothrow_constructible<T, decltype((declval<OptBase>()._value))>::value) {
        if (other._has_value())
            this->_ctor_v(ala::forward<OptBase>(other)._value);
    }

    template<class Arg>
    constexpr void _asgn_v(Arg &&arg) {
        if (this->_has_value()) {
            this->_value = ala::forward<Arg>(arg);
        } else {
            this->_ctor_v(ala::forward<Arg>(arg));
        }
    }

    template<class OptBase>
    constexpr void _asgn(OptBase &&other) noexcept(
        is_nothrow_constructible<T, decltype((declval<OptBase>()._value))>::value &&
            is_nothrow_assignable<T &, decltype((declval<OptBase>()._value))>::value) {
        if (other._has_value()) {
            this->_asgn_v(ala::forward<OptBase>(other)._value);
        } else {
            this->_reset();
        }
    }

    ~_optional_base() = default;
    constexpr _optional_base(): _base_t{} {}
    constexpr _optional_base(_optional_base &&) = default;
    constexpr _optional_base(const _optional_base &) = default;
    constexpr _optional_base &operator=(const _optional_base &) = default;
    constexpr _optional_base &operator=(_optional_base &&other) = default;
};

template<class T>
class optional: _make_controller_t<_optional_base<T>, T> {
public:
    static_assert(!is_same<remove_cvref_t<T>, in_place_t>::value &&
                      !is_same<remove_cvref_t<T>, nullopt_t>::value &&
                      !is_array<remove_cvref_t<T>>::value &&
                      is_destructible<T>::value,
                  "N4860 [20.6.3/3]");

    using value_type = T;
    using _base_t = _make_controller_t<_optional_base<T>, T>;
    // constructors
    constexpr optional() noexcept: _base_t{} {}
    constexpr optional(nullopt_t) noexcept: optional() {}
    optional(const optional &) = default;
    optional(optional &&) = default;

    // enable_if_t make Clang's is_constructible failed
    template<class... Args /*, class = enable_if_t<is_constructible<T, Args...>::value>*/>
    constexpr explicit optional(in_place_t, Args &&...args)
        : _base_t(in_place_t(), ala::forward<Args>(args)...) {}

    template<class U, class... Args/*,
             class = enable_if_t<
                 is_constructible<T, initializer_list<U> &, Args &&...>::value>*/>
    constexpr explicit optional(in_place_t, initializer_list<U> il,
                                Args &&... args)
        : _base_t(in_place_t(), il, ala::forward<Args>(args)...) {}

    template<class U,
             class = enable_if_t<is_constructible<T, U &&>::value &&
                                 !is_same<remove_cvref_t<U>, in_place_t>::value &&
                                 !is_same<remove_cvref_t<U>, optional>::value &&
                                 is_convertible<U &&, T>::value>>
    constexpr optional(U &&u): _base_t(in_place_t(), ala::forward<U>(u)) {}

    template<class U, class = void,
             class = enable_if_t<is_constructible<T, U &&>::value &&
                                 !is_same<remove_cvref_t<U>, in_place_t>::value &&
                                 !is_same<remove_cvref_t<U>, optional>::value &&
                                 !is_convertible<U &&, T>::value>>
    explicit constexpr optional(U &&u)
        : _base_t(in_place_t(), ala::forward<U>(u)) {}

    template<class U>
    using _check = _not_<_or_<
        is_constructible<T, optional<U> &>, is_constructible<T, const optional<U> &>,
        is_constructible<T, optional<U> &&>, is_constructible<T, const optional<U> &&>,
        is_convertible<optional<U> &, T>, is_convertible<const optional<U> &, T>,
        is_convertible<optional<U> &&, T>, is_convertible<const optional<U> &&, T>>>;

    template<class U, class = enable_if_t<is_constructible<T, const U &>::value &&
                                          _check<U>::value &&
                                          is_convertible<const U &, T>::value>>
    constexpr optional(const optional<U> &other) {
        if (other)
            this->_ctor_v(*other);
    }

    template<class U, class = void,
             class = enable_if_t<is_constructible<T, const U &>::value && _check<U>::value &&
                                 !is_convertible<const U &, T>::value>>
    explicit constexpr optional(const optional<U> &other) {
        if (other)
            this->_ctor_v(*other);
    }

    template<class U,
             class = enable_if_t<is_constructible<T, U &&>::value &&
                                 _check<U>::value && is_convertible<U &&, T>::value>>
    constexpr optional(optional<U> &&other) {
        if (other)
            this->_ctor_v(ala::move(*other));
    }

    template<class U, class = void,
             class = enable_if_t<is_constructible<T, U &&>::value && _check<U>::value &&
                                 !is_convertible<U &&, T>::value>>
    explicit constexpr optional(optional<U> &&other) {
        if (other)
            this->_ctor_v(ala::move(*other));
    }

    // destructor
    ~optional() = default;
    // assignment
    constexpr optional &operator=(nullopt_t) noexcept {
        reset();
        return *this;
    }

    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;

    template<class U = T>
    constexpr enable_if_t<
        !is_same<optional, remove_cvref_t<U>>::value &&
            is_constructible<T, U>::value && is_assignable<T &, U>::value &&
            (!is_scalar<T>::value || !is_same<decay_t<U>, T>::value),
        optional &>
    operator=(U &&u) {
        this->_asgn_v(ala::forward<U>(u));
        return *this;
    }

    template<class U>
    using _check_asgn = _not_<_or_<
        is_constructible<T, optional<U> &>, is_constructible<T, const optional<U> &>,
        is_constructible<T, optional<U> &&>, is_constructible<T, const optional<U> &&>,
        is_convertible<optional<U> &, T>, is_convertible<const optional<U> &, T>,
        is_convertible<optional<U> &&, T>, is_convertible<const optional<U> &&, T>,
        is_assignable<T &, optional<U> &>, is_assignable<T &, const optional<U> &>,
        is_assignable<T &, optional<U> &&>, is_assignable<T &, const optional<U> &&>>>;

    template<class U>
    constexpr enable_if_t<_check_asgn<U>::value && is_constructible_v<T, const U &> &&
                              is_assignable_v<T &, const U &>,
                          optional> &
    operator=(const optional<U> &other) {
        if (other.has_value()) {
            this->_asgn_v(*other);
        } else {
            this->_reset();
        }
        return *this;
    }

    template<class U>
    constexpr enable_if_t<_check_asgn<U>::value && is_constructible_v<T, U> &&
                              is_assignable_v<T &, U>,
                          optional> &
    operator=(optional<U> &&other) {
        if (other.has_value()) {
            this->_asgn_v(ala::move(*other));
        } else {
            this->_reset();
        }
        return *this;
    }

    template<class... Args>
    constexpr enable_if_t<is_constructible<T, Args...>::value, T &>
    emplace(Args &&...args) {
        reset();
        this->_ctor_v(ala::forward<Args>(args)...);
        return **this;
    }

    template<class U, class... Args>
    constexpr enable_if_t<is_constructible<T, initializer_list<U> &, Args...>::value, T &>
    emplace(initializer_list<U> il, Args &&...args) {
        reset();
        this->_ctor_v(il, ala::forward<Args>(args)...);
        return **this;
    }
    // swap
    constexpr void swap(optional &other) noexcept(
        is_nothrow_move_constructible<T>::value &&is_nothrow_swappable<T>::value) {
        static_assert(is_move_constructible<T>::value && is_swappable<T>::value,
                      "optional<T>::swap requires T to be move_constructible "
                      "and swappable");
        if (!*this && !other)
            return;
        else if (!*this && other) {
            this->_ctor_v(ala::move(*other));
            other.reset();
        } else if (*this && !other)
            return other.swap(*this);
        else
            ala::_swap_adl(**this, *other);
    }
    // observers
    constexpr const T *operator->() const {
        return ala::addressof(**this);
    }

    constexpr T *operator->() {
        return ala::addressof(**this);
    }

    constexpr T &operator*() & {
        return this->_value;
    }

    constexpr const T &operator*() const & {
        return this->_value;
    }

    constexpr T &&operator*() && {
        return ala::move(this->_value);
    }

    constexpr const T &&operator*() const && {
        return ala::move(this->_value);
    }

    constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    constexpr bool has_value() const noexcept {
        return this->_has_value();
    }

    constexpr T &value() & {
        if (!*this)
            throw bad_optional_access();
        return **this;
    }

    constexpr const T &value() const & {
        if (!*this)
            throw bad_optional_access();
        return **this;
    }

    constexpr T &&value() && {
        if (!*this)
            throw bad_optional_access();
        return ala::move(**this);
    }

    constexpr const T &&value() const && {
        if (!*this)
            throw bad_optional_access();
        return ala::move(**this);
    }

    template<class U>
    constexpr T value_or(U &&def_value) const & {
        if (bool(*this))
            return **this;
        else
            return static_cast<T>(ala::forward<U>(def_value));
    }

    template<class U>
    constexpr T value_or(U &&def_value) && {
        if (bool(*this))
            return ala::move(**this);
        else
            return static_cast<T>(ala::forward<U>(def_value));
    }
    // modifiers
    constexpr void reset() noexcept {
        this->_reset();
    }

    // Impelment p0798R6
    // Monadic operations for ala::optional
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0798r6.html
    template<class F>
    constexpr invoke_result_t<F, T &> and_then(F &&f) & {
        using result_t = invoke_result_t<F, T &>;
        static_assert(is_specification<remove_cvref_t<result_t>, ala::optional>::value,
                      "F must return an optional");
        return has_value() ? ala::invoke(ala::forward<F>(f), **this) :
                             result_t(nullopt);
    }

    template<class F>
    constexpr invoke_result_t<F, T &&> and_then(F &&f) && {
        using result_t = invoke_result_t<F, T &&>;
        static_assert(is_specification<remove_cvref_t<result_t>, ala::optional>::value,
                      "F must return an optional");
        return has_value() ? ala::invoke(ala::forward<F>(f), ala::move(**this)) :
                             result_t(nullopt);
    }

    template<class F>
    constexpr invoke_result_t<F, const T &> and_then(F &&f) const & {
        using result_t = invoke_result_t<F, const T &>;
        static_assert(is_specification<remove_cvref_t<result_t>, ala::optional>::value,
                      "F must return an optional");
        return has_value() ? ala::invoke(ala::forward<F>(f), **this) :
                             result_t(nullopt);
    }

    template<class F>
    constexpr invoke_result_t<F, const T &&> and_then(F &&f) const && {
        using result_t = invoke_result_t<F, const T &&>;
        static_assert(is_specification<remove_cvref_t<result_t>, ala::optional>::value,
                      "F must return an optional");
        return has_value() ? ala::invoke(ala::forward<F>(f), ala::move(**this)) :
                             result_t(nullopt);
    }

    template<class Opt, class F, class Ret = invoke_result_t<F>,
             class = enable_if_t<!is_void<Ret>::value>>
    static constexpr optional _optional_or_else_impl(Opt &&opt, F &&f) {
        return opt.has_value() ? ala::forward<Opt>(opt) : ala::forward<F>(f)();
    }

    template<class Opt, class F, class Ret = invoke_result_t<F>,
             class = enable_if_t<is_void<Ret>::value>, class = void>
    static constexpr optional _optional_or_else_impl(Opt &&opt, F &&f) {
        if (opt.has_value())
            return ala::forward<Opt>(opt);
        ala::forward<F>(f)();
        return nullopt;
    }

    template<class F>
    constexpr optional or_else(F &&f) && {
        static_assert(is_copy_constructible<T>::value,
                      "T must return be copy_constructible");
        return _optional_or_else_impl(ala::move(*this), f);
    }

    template<class F>
    constexpr optional or_else(F &&f) const & {
        static_assert(is_copy_constructible<T>::value,
                      "T must return be copy_constructible");
        return _optional_or_else_impl(*this, f);
    }

    template<class Opt, class F,
             class Ret = invoke_result_t<F, typename Opt::value_type>,
             class = enable_if_t<!is_void<Ret>::value>>
    constexpr auto _optional_fmap_impl(Opt &&opt, F &&f) -> optional<Ret> {
        return opt.has_value() ?
                   ala::invoke(ala::forward<F>(f), *ala::forward<Opt>(opt)) :
                   nullopt;
    }

    template<class Opt, class F,
             class Ret = invoke_result_t<F, typename Opt::value_type>,
             class = enable_if_t<is_void<Ret>::value>, class = void>
    constexpr auto _optional_fmap_impl(Opt &&opt, F &&f) -> optional<monostate> {
        if (opt.has_value()) {
            ala::invoke(ala::forward<F>(f), *ala::forward<Opt>(opt));
            return optional<monostate>(monostate{});
        }
        return optional<monostate>(nullopt);
    }

    template<class F>
    constexpr auto transform(F &&f) & {
        return _optional_fmap_impl(*this, ala::forward<F>(f));
    }

    template<class F>
    constexpr auto transform(F &&f) && {
        return _optional_fmap_impl(ala::move(*this), ala::forward<F>(f));
    }

    template<class F>
    constexpr auto transform(F &&f) const & {
        return _optional_fmap_impl(*this, ala::forward<F>(f));
    }

    template<class F>
    constexpr auto transform(F &&f) const && {
        return _optional_fmap_impl(ala::move(*this), ala::forward<F>(f));
    }
};

#ifdef _ALA_ENABLE_DEDUCTION_GUIDES
template<class T>
optional(T) -> optional<T>;
#endif

template<class T, class U>
constexpr enable_if_t<
    is_convertible<decltype(declval<const T &>() == declval<const U &>()), bool>::value, bool>
operator==(const optional<T> &lhs, const optional<U> &rhs) {
    return lhs.has_value() == rhs.has_value() &&
           (!rhs.has_value() || *lhs == *rhs);
}

template<class T, class U>
constexpr enable_if_t<
    is_convertible<decltype(declval<const T &>() != declval<const U &>()), bool>::value, bool>
operator!=(const optional<T> &lhs, const optional<U> &rhs) {
    return lhs.has_value() != rhs.has_value() ||
           (lhs.has_value() && *lhs != *rhs);
}

template<class T, class U>
constexpr enable_if_t<
    is_convertible<decltype(declval<const T &>() < declval<const U &>()), bool>::value, bool>
operator<(const optional<T> &lhs, const optional<U> &rhs) {
    return rhs.has_value() && (!lhs.has_value() || *lhs < *rhs);
}

template<class T, class U>
constexpr enable_if_t<
    is_convertible<decltype(declval<const T &>() <= declval<const U &>()), bool>::value, bool>
operator<=(const optional<T> &lhs, const optional<U> &rhs) {
    return !lhs.has_value() || (rhs.has_value() && *lhs <= *rhs);
}

template<class T, class U>
constexpr enable_if_t<
    is_convertible<decltype(declval<const T &>() > declval<const U &>()), bool>::value, bool>
operator>(const optional<T> &lhs, const optional<U> &rhs) {
    return lhs.has_value() && (!rhs.has_value() || *lhs > *rhs);
}

template<class T, class U>
constexpr enable_if_t<
    is_convertible<decltype(declval<const T &>() >= declval<const U &>()), bool>::value, bool>
operator>=(const optional<T> &lhs, const optional<U> &rhs) {
    return !rhs.has_value() || (lhs.has_value() && *lhs >= *rhs);
}
// comparison with nullopt

template<class T>
constexpr bool operator==(const optional<T> &opt, nullopt_t) noexcept {
    return !opt;
}

template<class T>
constexpr bool operator==(nullopt_t, const optional<T> &opt) noexcept {
    return !opt;
}

template<class T>
constexpr bool operator!=(const optional<T> &opt, nullopt_t) noexcept {
    return bool(opt);
}

template<class T>
constexpr bool operator!=(nullopt_t, const optional<T> &opt) noexcept {
    return bool(opt);
}

template<class T>
constexpr bool operator<(const optional<T> &opt, nullopt_t) noexcept {
    return false;
}

template<class T>
constexpr bool operator<(nullopt_t, const optional<T> &opt) noexcept {
    return bool(opt);
}

template<class T>
constexpr bool operator<=(const optional<T> &opt, nullopt_t) noexcept {
    return !opt;
}

template<class T>
constexpr bool operator<=(nullopt_t, const optional<T> &opt) noexcept {
    return true;
}

template<class T>
constexpr bool operator>(const optional<T> &opt, nullopt_t) noexcept {
    return bool(opt);
}

template<class T>
constexpr bool operator>(nullopt_t, const optional<T> &opt) noexcept {
    return false;
}

template<class T>
constexpr bool operator>=(const optional<T> &opt, nullopt_t) noexcept {
    return true;
}

template<class T>
constexpr bool operator>=(nullopt_t, const optional<T> &opt) noexcept {
    return !opt;
}
// comparison with T

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() == declval<const U &>())>
operator==(const optional<T> &opt, const U &value) {
    return opt.has_value() && *opt == value;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() == declval<const U &>())>
operator==(const T &value, const optional<U> &opt) {
    return opt.has_value() && value == *opt;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() != declval<const U &>())>
operator!=(const optional<T> &opt, const U &value) {
    return !opt.has_value() || *opt != value;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() != declval<const U &>())>
operator!=(const T &value, const optional<U> &opt) {
    return !opt.has_value() || value != *opt;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() < declval<const U &>())>
operator<(const optional<T> &opt, const U &value) {
    return !opt.has_value() || *opt < value;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() < declval<const U &>())>
operator<(const T &value, const optional<U> &opt) {
    return opt.has_value() && value < *opt;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() <= declval<const U &>())>
operator<=(const optional<T> &opt, const U &value) {
    return !opt.has_value() || *opt <= value;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() <= declval<const U &>())>
operator<=(const T &value, const optional<U> &opt) {
    return opt.has_value() && value <= *opt;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() > declval<const U &>())>
operator>(const optional<T> &opt, const U &value) {
    return opt.has_value() && *opt > value;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() > declval<const U &>())>
operator>(const T &value, const optional<U> &opt) {
    return !opt.has_value() || value > *opt;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() >= declval<const U &>())>
operator>=(const optional<T> &opt, const U &value) {
    return opt.has_value() && *opt >= value;
}

template<class T, class U>
constexpr enable_if_t<true, decltype(declval<const T &>() >= declval<const U &>())>
operator>=(const T &value, const optional<U> &opt) {
    return !opt.has_value() || value >= *opt;
}

template<class T>
enable_if_t<is_move_constructible<T>::value && is_swappable<T>::value>
swap(optional<T> &lhs, optional<T> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template<class T>
constexpr optional<decay_t<T>> make_optional(T &&value) {
    return ala::optional<decay_t<T>>(ala::forward<T>(value));
}

template<class T, class... Args>
constexpr optional<T> make_optional(Args &&...args) {
    return ala::optional<T>(in_place_t(), ala::forward<Args>(args)...);
}

template<class T, class U, class... Args>
constexpr optional<T> make_optional(initializer_list<U> il, Args &&...args) {
    return ala::optional<T>(in_place_t(), il, ala::forward<Args>(args)...);
}

template<class T>
struct hash<_sfinae_checker<optional<T>,
                            enable_if_t<_is_hashable<remove_const_t<T>>::value>>> {
    using argument_type = optional<T>;
    using result_type = size_t;

    result_type operator()(const argument_type &opt) const {
        return !!opt ? hash<remove_const_t<T>>()(*opt) : 0;
    }
};

} // namespace ala

#endif