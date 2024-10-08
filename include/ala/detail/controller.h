#ifndef _ALA_DETAIL_CONTROLLER_CTL
#define _ALA_DETAIL_CONTROLLER_CTL

#include <ala/type_traits.h>

namespace ala {

template<class Base, bool, bool>
struct _copy_ctor_ctl: Base {
    using Base::Base;
    ~_copy_ctor_ctl() = default;
    constexpr _copy_ctor_ctl() = default;
    constexpr _copy_ctor_ctl(const _copy_ctor_ctl &) = default;
    constexpr _copy_ctor_ctl(_copy_ctor_ctl &&) = default;
    constexpr _copy_ctor_ctl &operator=(const _copy_ctor_ctl &) = default;
    constexpr _copy_ctor_ctl &operator=(_copy_ctor_ctl &&) = default;
};

template<class Base>
struct _copy_ctor_ctl<Base, false, false>: Base {
    using Base::Base;
    ~_copy_ctor_ctl() = default;
    constexpr _copy_ctor_ctl() = default;
    constexpr _copy_ctor_ctl(const _copy_ctor_ctl &) = delete;
    constexpr _copy_ctor_ctl(_copy_ctor_ctl &&) = default;
    constexpr _copy_ctor_ctl &operator=(const _copy_ctor_ctl &) = default;
    constexpr _copy_ctor_ctl &operator=(_copy_ctor_ctl &&) = default;
};

template<class Base>
struct _copy_ctor_ctl<Base, true, false>: Base {
    using Base::Base;
    ~_copy_ctor_ctl() = default;
    constexpr _copy_ctor_ctl() = default;
    constexpr _copy_ctor_ctl(const _copy_ctor_ctl &other) noexcept(
        noexcept(declval<Base>()._ctor(static_cast<const Base &>(other)))) {
        Base::_ctor(static_cast<const Base &>(other));
    }
    constexpr _copy_ctor_ctl(_copy_ctor_ctl &&) = default;
    constexpr _copy_ctor_ctl &operator=(const _copy_ctor_ctl &) = default;
    constexpr _copy_ctor_ctl &operator=(_copy_ctor_ctl &&) = default;
};

template<class Base, class... Ts>
using _copy_ctor_ctl_t =
    _copy_ctor_ctl<Base, _and_<is_copy_constructible<Ts>...>::value,
                   _and_<is_trivially_copy_constructible<Ts>...>::value>;

template<class Base, bool, bool>
struct _move_ctor_ctl: Base {
    using Base::Base;
    ~_move_ctor_ctl() = default;
    constexpr _move_ctor_ctl() = default;
    constexpr _move_ctor_ctl(const _move_ctor_ctl &) = default;
    constexpr _move_ctor_ctl(_move_ctor_ctl &&) = default;
    constexpr _move_ctor_ctl &operator=(const _move_ctor_ctl &) = default;
    constexpr _move_ctor_ctl &operator=(_move_ctor_ctl &&) = default;
};

template<class Base>
struct _move_ctor_ctl<Base, false, false>: Base {
    using Base::Base;
    ~_move_ctor_ctl() = default;
    constexpr _move_ctor_ctl() = default;
    constexpr _move_ctor_ctl(const _move_ctor_ctl &) = default;
    constexpr _move_ctor_ctl(_move_ctor_ctl &&) = delete;
    constexpr _move_ctor_ctl &operator=(const _move_ctor_ctl &) = default;
    constexpr _move_ctor_ctl &operator=(_move_ctor_ctl &&) = default;
};

template<class Base>
struct _move_ctor_ctl<Base, true, false>: Base {
    using Base::Base;
    ~_move_ctor_ctl() = default;
    constexpr _move_ctor_ctl() = default;
    constexpr _move_ctor_ctl(const _move_ctor_ctl &) = default;
    constexpr _move_ctor_ctl(_move_ctor_ctl &&other) noexcept(
        noexcept(declval<Base>()._ctor(static_cast<Base &&>(other)))) {
        Base::_ctor(static_cast<Base &&>(other));
    }
    constexpr _move_ctor_ctl &operator=(const _move_ctor_ctl &) = default;
    constexpr _move_ctor_ctl &operator=(_move_ctor_ctl &&) = default;
};

template<class Base, class... Ts>
using _move_ctor_ctl_t =
    _move_ctor_ctl<Base, _and_<is_move_constructible<Ts>...>::value,
                   _and_<is_trivially_move_constructible<Ts>...>::value>;

template<class Base, bool, bool>
struct _copy_asgn_ctl: Base {
    using Base::Base;
    ~_copy_asgn_ctl() = default;
    constexpr _copy_asgn_ctl() = default;
    constexpr _copy_asgn_ctl(_copy_asgn_ctl &&) = default;
    constexpr _copy_asgn_ctl(const _copy_asgn_ctl &) = default;
    constexpr _copy_asgn_ctl &operator=(const _copy_asgn_ctl &) = default;
    constexpr _copy_asgn_ctl &operator=(_copy_asgn_ctl &&) = default;
};

template<class Base>
struct _copy_asgn_ctl<Base, false, false>: Base {
    using Base::Base;
    ~_copy_asgn_ctl() = default;
    constexpr _copy_asgn_ctl() = default;
    constexpr _copy_asgn_ctl(_copy_asgn_ctl &&) = default;
    constexpr _copy_asgn_ctl(const _copy_asgn_ctl &) = default;
    constexpr _copy_asgn_ctl &operator=(const _copy_asgn_ctl &) = delete;
    constexpr _copy_asgn_ctl &operator=(_copy_asgn_ctl &&) = default;
};

template<class Base>
struct _copy_asgn_ctl<Base, true, false>: Base {
    using Base::Base;
    ~_copy_asgn_ctl() = default;
    constexpr _copy_asgn_ctl() = default;
    constexpr _copy_asgn_ctl(_copy_asgn_ctl &&) = default;
    constexpr _copy_asgn_ctl(const _copy_asgn_ctl &) = default;
    constexpr _copy_asgn_ctl &operator=(const _copy_asgn_ctl &other) noexcept(
        noexcept(declval<Base>()._asgn(static_cast<const Base &>(other)))) {
        Base::_asgn(static_cast<const Base &>(other));
        return *this;
    }
    constexpr _copy_asgn_ctl &operator=(_copy_asgn_ctl &&) = default;
};

template<class Base, class... Ts>
using _copy_asgn_ctl_t = _copy_asgn_ctl<
    Base, _and_<is_copy_constructible<Ts>..., is_copy_assignable<Ts>...>::value,
    _and_<is_trivially_copy_constructible<Ts>..., is_trivially_copy_assignable<Ts>...,
          is_trivially_destructible<Ts>...>::value>;

template<class Base, bool, bool>
struct _move_asgn_ctl: Base {
    using Base::Base;
    ~_move_asgn_ctl() = default;
    constexpr _move_asgn_ctl() = default;
    constexpr _move_asgn_ctl(_move_asgn_ctl &&) = default;
    constexpr _move_asgn_ctl(const _move_asgn_ctl &) = default;
    constexpr _move_asgn_ctl &operator=(const _move_asgn_ctl &) = default;
    constexpr _move_asgn_ctl &operator=(_move_asgn_ctl &&) = default;
};

template<class Base>
struct _move_asgn_ctl<Base, false, false>: Base {
    using Base::Base;
    ~_move_asgn_ctl() = default;
    constexpr _move_asgn_ctl() = default;
    constexpr _move_asgn_ctl(_move_asgn_ctl &&) = default;
    constexpr _move_asgn_ctl(const _move_asgn_ctl &) = default;
    constexpr _move_asgn_ctl &operator=(const _move_asgn_ctl &) = default;
    constexpr _move_asgn_ctl &operator=(_move_asgn_ctl &&) = delete;
};

template<class Base>
struct _move_asgn_ctl<Base, true, false>: Base {
    using Base::Base;
    ~_move_asgn_ctl() = default;
    constexpr _move_asgn_ctl() = default;
    constexpr _move_asgn_ctl(_move_asgn_ctl &&) = default;
    constexpr _move_asgn_ctl(const _move_asgn_ctl &) = default;
    constexpr _move_asgn_ctl &operator=(const _move_asgn_ctl &) = default;
    constexpr _move_asgn_ctl &operator=(_move_asgn_ctl &&other) noexcept(
        noexcept(declval<Base>()._asgn(static_cast<Base &&>(other)))) {
        Base::_asgn(static_cast<Base &&>(other));
        return *this;
    }
};

template<class Base, class... Ts>
using _move_asgn_ctl_t = _move_asgn_ctl<
    Base, _and_<is_move_constructible<Ts>..., is_move_assignable<Ts>...>::value,
    _and_<is_trivially_move_constructible<Ts>..., is_trivially_move_assignable<Ts>...,
          is_trivially_destructible<Ts>...>::value>;

template<class Base, class... Ts>
using _make_controller_t = _move_asgn_ctl_t<
    _copy_asgn_ctl_t<_move_ctor_ctl_t<_copy_ctor_ctl_t<Base, Ts...>, Ts...>, Ts...>,
    Ts...>;

#define ALA_F4_EQ_DEFAULT(T) \
    constexpr T(T &&) = default; \
    constexpr T(const T &) = default; \
    constexpr T &operator=(const T &) = default; \
    constexpr T &operator=(T &&other) = default;

} // namespace ala

#endif