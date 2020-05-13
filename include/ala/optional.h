#ifndef _ALA_OPTIONAL_H
#define _ALA_OPTIONAL_H

#include <ala/utility.h>

namespace ala {

struct bad_optional_access: logic_error {
    explicit bad_optional_access(const char *what_arg): logic_error(what_arg) {}
};

struct nullopt_t {
    explicit constexpr nullopt_t(int) {}
};

template<class T>
struct _optional_base {
    aligned_storage_t<sizeof(T), alignof(T)> _data; // exposition only
    bool _valid = false; // maybe is_trivially_default_constructible?
};

template<class T, bool = is_trivially_destructible<T>::value>
struct _optional_destroy: _optional_base<T> {};

template<class T>
struct _optional_destroy<T, false>: _optional_base<T> {
    ~_optional_destroy() {
        if (this->_valid)
            (*(T *)&(this->_data)).~T();
    }
    constexpr _optional_destroy() = default;
    constexpr _optional_destroy(const _optional_destroy &other) = default;
    constexpr _optional_destroy(_optional_destroy &&) = default;
    _optional_destroy &operator=(const _optional_destroy &) = default;
    _optional_destroy &operator=(_optional_destroy &&) = default;
};

template<class T, bool = is_copy_constructible<T>::value,
         bool = is_trivially_copy_constructible<T>::value>
struct _optional_copy: _optional_destroy<T> {};

template<class T>
struct _optional_copy<T, false, false>: _optional_destroy<T> {
    ~_optional_copy() = default;
    constexpr _optional_copy() = default;
    constexpr _optional_copy(const _optional_copy &other) = delete;
    constexpr _optional_copy(_optional_copy &&) = default;
    _optional_copy &operator=(const _optional_copy &) = default;
    _optional_copy &operator=(_optional_copy &&) = default;
};

template<class T>
struct _optional_copy<T, true, false>: _optional_destroy<T> {
    ~_optional_copy() = default;
    constexpr _optional_copy() = default;
    constexpr _optional_copy(const _optional_copy &other) {
        if (other._valid)
            ::new ((void *)&(this->_data)) T(*(T *)&(other._data));
        this->_valid = other._valid;
    }
    constexpr _optional_copy(_optional_copy &&) = default;
    _optional_copy &operator=(const _optional_copy &) = default;
    _optional_copy &operator=(_optional_copy &&) = default;
};

template<class T, bool = is_move_constructible<T>::value,
         bool = is_trivially_move_constructible<T>::value>
struct _optional_move: _optional_copy<T> {};

template<class T>
struct _optional_move<T, false, false>: _optional_copy<T> {
    ~_optional_move() = default;
    constexpr _optional_move() = default;
    constexpr _optional_move(const _optional_move &) = default;
    constexpr _optional_move(_optional_move &&other) = delete;
    _optional_move &operator=(const _optional_move &) = default;
    _optional_move &operator=(_optional_move &&) = default;
};

template<class T>
struct _optional_move<T, true, false>: _optional_copy<T> {
    ~_optional_move() = default;
    constexpr _optional_move() = default;
    constexpr _optional_move(const _optional_move &) = default;
    constexpr _optional_move(_optional_move &&other) {
        if (other._valid)
            ::new ((void *)&(this->_data)) T(ala::move(*(T *)&(other._data)));
        this->_valid = other._valid;
    }
    _optional_move &operator=(const _optional_move &) = default;
    _optional_move &operator=(_optional_move &&) = default;
};

template<class T, bool = is_copy_constructible<T>::value &&is_copy_assignable<T>::value,
         bool = is_trivially_copy_constructible<T>::value &&
             is_trivially_copy_assignable<T>::value &&is_trivially_destructible<T>::value>
struct _optional_copy_asgn: _optional_move<T> {};

template<class T>
struct _optional_copy_asgn<T, false, false>: _optional_move<T> {
    ~_optional_copy_asgn() = default;
    constexpr _optional_copy_asgn() = default;
    constexpr _optional_copy_asgn(_optional_copy_asgn &&) = default;
    constexpr _optional_copy_asgn(const _optional_copy_asgn &) = default;
    _optional_copy_asgn &operator=(const _optional_copy_asgn &other) = delete;
    _optional_copy_asgn &operator=(_optional_copy_asgn &&) = default;
};

template<class T>
struct _optional_copy_asgn<T, true, false>: _optional_move<T> {
    ~_optional_copy_asgn() = default;
    constexpr _optional_copy_asgn() = default;
    constexpr _optional_copy_asgn(_optional_copy_asgn &&) = default;
    constexpr _optional_copy_asgn(const _optional_copy_asgn &) = default;
    _optional_copy_asgn &operator=(const _optional_copy_asgn &other) {
        if (this->_valid && other._valid) {
            *(T *)&(this->_data) = *(T *)&(other._data);
        } else if (other._valid) {
            ::new ((void *)&(this->_data)) T(*(T *)&(other._data));
            this->_valid = true;
        } else if (this->_valid) {
            (*(T *)&(this->_data)).~T();
        }
    }
    _optional_copy_asgn &operator=(_optional_copy_asgn &&) = default;
};

template<class T, bool = is_move_constructible<T>::value &&is_move_assignable<T>::value,
         bool = is_trivially_move_constructible<T>::value &&
             is_trivially_move_assignable<T>::value &&is_trivially_destructible<T>::value>
struct _optional_move_asgn: _optional_copy_asgn<T> {};

template<class T>
struct _optional_move_asgn<T, false, false>: _optional_copy_asgn<T> {
    ~_optional_move_asgn() = default;
    constexpr _optional_move_asgn() = default;
    constexpr _optional_move_asgn(_optional_move_asgn &&) = default;
    constexpr _optional_move_asgn(const _optional_move_asgn &) = default;
    _optional_move_asgn &operator=(const _optional_move_asgn &) = default;
    _optional_move_asgn &operator=(_optional_move_asgn &&other) = delete;
};

template<class T>
struct _optional_move_asgn<T, true, false>: _optional_copy_asgn<T> {
    ~_optional_move_asgn() = default;
    constexpr _optional_move_asgn() = default;
    constexpr _optional_move_asgn(_optional_move_asgn &&) = default;
    constexpr _optional_move_asgn(const _optional_move_asgn &) = default;
    _optional_move_asgn &operator=(const _optional_move_asgn &) = default;
    _optional_move_asgn &operator=(_optional_move_asgn &&other) noexcept(
        is_nothrow_move_assignable<T>::value &&is_nothrow_move_constructible<T>::value) {
        if (this->_valid && other._valid) {
            *(T *)&(this->_data) = ala::move(*(T *)&(other._data));
        } else if (other._valid) {
            ::new ((void *)&(this->_data)) T(ala::move(*(T *)&(other._data)));
            this->_valid = true;
        } else if (this->_valid) {
            (*(T *)&(this->_data)).~T();
        }
        return *this;
    }
};

template<class T>
class optional: _optional_move_asgn<T> {
public:
    using value_type = T;
    // constructors
    constexpr optional() = default;
    constexpr optional(nullopt_t) noexcept: optional() {}
    optional(const optional &) = default;
    optional(optional &&) = default;

    template<class... Args, class = enable_if_t<is_constructible<T, Args...>::value>>
    constexpr explicit optional(in_place_t, Args &&... args) {
        ::new ((void *)&(this->_data)) T(ala::forward<Args>(args)...);
        this->_valid = true;
    }

    template<class U, class... Args,
             class = enable_if_t<
                 is_constructible<T, initializer_list<U> &, Args &&...>::value>>
    constexpr explicit optional(in_place_t, initializer_list<U> il,
                                Args &&... args) {
        ::new ((void *)&(this->_data)) T(il, ala::forward<Args>(args)...);
        this->_valid = true;
    }

    template<class U = value_type,
             class = enable_if_t<is_constructible<T, U &&>::value &&
                                 !is_same<remove_cvref_t<U>, in_place_t>::value &&
                                 !is_same<remove_cvref_t<U>, optional>::value &&
                                 is_convertible<U &&, T>::value>>
    constexpr optional(U &&u) {
        ::new ((void *)&(this->_data)) T(ala::forward<U>(u));
        this->_valid = true;
    }

    template<class U = value_type, class = void,
             class = enable_if_t<is_constructible<T, U &&>::value &&
                                 !is_same<remove_cvref_t<U>, in_place_t>::value &&
                                 !is_same<remove_cvref_t<U>, optional>::value &&
                                 !is_convertible<U &&, T>::value>>
    explicit constexpr optional(U &&u) {
        ::new ((void *)&(this->_data)) T(ala::forward<U>(u));
        this->_valid = true;
    }

    template<class U>
    using _check = _not_<_or_<
        is_constructible<T, optional<U> &>, is_constructible<T, const optional<U> &>,
        is_constructible<T, optional<U> &&>, is_constructible<T, const optional<U> &&>,
        is_convertible<optional<U> &, T>, is_convertible<const optional<U> &, T>,
        is_convertible<optional<U> &&, T>, is_convertible<const optional<U> &&, T>>>;

    template<class U, class = enable_if_t<is_constructible<T, const U &>::value &&
                                          _check<U>::value &&
                                          is_convertible<const U &, T>::value>>
    optional(const optional<U> &other): optional(*other) {}

    template<class U, class = void,
             class = enable_if_t<is_constructible<T, const U &>::value && _check<U>::value &&
                                 !is_convertible<const U &, T>::value>>
    explicit optional(const optional<U> &other): optional(*other) {}

    template<class U,
             class = enable_if_t<is_constructible<T, U &&>::value &&
                                 _check<U>::value && is_convertible<U &&, T>::value>>
    optional(optional<U> &&other): optional(ala::move(*other)) {}

    template<class U, class = void,
             class = enable_if_t<is_constructible<T, U &&>::value && _check<U>::value &&
                                 !is_convertible<U &&, T>::value>>
    explicit optional(optional<U> &&other): optional(ala::move(*other)) {}

    // destructor
    ~optional() = default;
    // assignment
    optional &operator=(nullopt_t) noexcept {
        reset();
    }

    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;

    template<class U = T>
    enable_if_t<!is_same<optional, remove_cvref_t<U>>::value &&
                    is_constructible<T, U>::value && is_assignable<T &, U>::value &&
                    (!is_scalar<T>::value || !is_same<decay_t<U>, T>::value),
                optional> &
    operator=(U &&u) {
        this->emplace(forward<U>(u));
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
    enable_if_t<_check_asgn<U>::value && is_constructible_v<T, const U &> &&
                    is_assignable_v<T &, const U &>,
                optional> &
    operator=(const optional<U> &other) {
        if (this->_valid && other._valid) {
            *(T *)&(this->_data) = *other;
        } else if (other._valid) {
            ::new ((void *)&(this->_data)) T(*other);
            this->_valid = true;
        } else if (this->_valid) {
            (*(T *)&(this->_data)).~T();
        }
        return *this;
    }

    template<class U>
    enable_if_t<_check_asgn<U>::value && is_constructible_v<T, U> &&
                    is_assignable_v<T &, U>,
                optional> &
    operator=(optional<U> &&other) {
        if (this->_valid && other._valid) {
            *(T *)&(this->_data) = ala::move(*other);
        } else if (other._valid) {
            ::new ((void *)&(this->_data)) T(ala::move(*other));
            this->_valid = true;
        } else if (this->_valid) {
            (*(T *)&(this->_data)).~T();
        }
        return *this;
    }

    template<class... Args>
    enable_if_t<is_constructible<T, Args...>::value> emplace(Args &&... args) {
        reset();
        ::new ((void *)&(this->_data)) T(ala::forward<Args>(args)...);
        this->_valid = true;
    }

    template<class U, class... Args>
    enable_if_t<is_constructible<T, initializer_list<U> &, Args...>::value>
    emplace(initializer_list<U> il, Args &&... args) {
        reset();
        ::new ((void *)&(this->_data)) T(il, ala::forward<Args>(args)...);
        this->_valid = true;
    }
    // swap
    void swap(optional &other) noexcept(
        is_nothrow_move_constructible<T>::value &&is_nothrow_swappable<T>::value) {
        if (!*this && !other)
            return;
        else if (!*this && other)
            return other.swap(*this);
        else if (*this && !other)
            other = ala::move(*this);
        else
            ala::swap(**this, *other);
        swap(this->_valid, other._valid);
    }
    // observers
    constexpr const T *operator->() const {
        return (const T *)&(this->_data);
    }

    constexpr T *operator->() {
        return (T *)&(this->_data);
    }

    constexpr const T &operator*() const & {
        return static_cast<const T &>(*(T *)&(this->_data));
    }

    constexpr T &operator*() & {
        return static_cast<T &>(*(T *)&(this->_data));
    }

    constexpr T &&operator*() && {
        return static_cast<T &&>(*(T *)&(this->_data));
    }

    constexpr const T &&operator*() const && {
        return static_cast<const T &&>(*(T *)&(this->_data));
    }

    constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    constexpr bool has_value() const noexcept {
        return this->_valid;
    }

    constexpr const T &value() const & {
        if (!*this)
            throw bad_optional_access("ala::optional has no value");
        return static_cast<const T &>(*(T *)&(this->_data));
    }

    constexpr T &value() & {
        if (!*this)
            throw bad_optional_access("ala::optional has no value");
        return static_cast<T &>(*(T *)&(this->_data));
    }

    constexpr T &&value() && {
        if (!*this)
            throw bad_optional_access("ala::optional has no value");
        return static_cast<T &&>(*(T *)&(this->_data));
    }

    constexpr const T &&value() const && {
        if (!*this)
            throw bad_optional_access("ala::optional has no value");
        return static_cast<const T &&>(*(T *)&(this->_data));
    }

    template<class U>
    constexpr T value_or(U &&def_value) const & {
        return bool(*this) ? **this : static_cast<T>(ala::forward<U>(def_value));
    }

    template<class U>
    constexpr T value_or(U &&def_value) && {
        return bool(*this) ? ala::move(**this) :
                             static_cast<T>(ala::forward<U>(def_value));
    }
    // modifiers
    void reset() noexcept {
        if (*this)
            (**this).~T();
        this->_valid = false;
    }
};

template<class T, class U>
constexpr bool operator==(const optional<T> &lhs, const optional<U> &rhs) {
    if (bool(lhs) != bool(rhs))
        return false;
    else if (bool(lhs) || bool(rhs))
        return true;
    return *lhs == *rhs;
}

template<class T, class U>
constexpr bool operator!=(const optional<T> &lhs, const optional<U> &rhs) {
    if (bool(lhs) != bool(rhs))
        return true;
    else if (bool(lhs) || bool(rhs))
        return false;
    return *lhs != *rhs;
}

template<class T, class U>
constexpr bool operator<(const optional<T> &lhs, const optional<U> &rhs) {
    if (bool(rhs) == false)
        return false;
    else if (bool(lhs) == false)
        return true;
    return *lhs < *rhs;
}

template<class T, class U>
constexpr bool operator<=(const optional<T> &lhs, const optional<U> &rhs) {
    if (bool(lhs) == false)
        return true;
    else if (bool(rhs) == false)
        return false;
    return *lhs <= *rhs;
}

template<class T, class U>
constexpr bool operator>(const optional<T> &lhs, const optional<U> &rhs) {
    if (bool(lhs) == false)
        return false;
    else if (bool(rhs) == false)
        return true;
    return *lhs > *rhs;
}

template<class T, class U>
constexpr bool operator>=(const optional<T> &lhs, const optional<U> &rhs) {
    if (bool(rhs) == false)
        return true;
    else if (bool(lhs) == false)
        return false;
    return *lhs >= *rhs;
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

template<class T>
constexpr bool operator==(const optional<T> &opt, const T &value) {
    return bool(opt) ? *opt == value : false;
}

template<class T>
constexpr bool operator==(const T &value, const optional<T> &opt) {
    return bool(opt) ? value == *opt : false;
}

template<class T>
constexpr bool operator!=(const optional<T> &opt, const T &value) {
    return bool(opt) ? *opt != value : true;
}

template<class T>
constexpr bool operator!=(const T &value, const optional<T> &opt) {
    return bool(opt) ? value != *opt : true;
}

template<class T>
constexpr bool operator<(const optional<T> &opt, const T &value) {
    return bool(opt) ? *opt < value : true;
}

template<class T>
constexpr bool operator<(const T &value, const optional<T> &opt) {
    return bool(opt) ? value < *opt : false;
}

template<class T>
constexpr bool operator<=(const optional<T> &opt, const T &value) {
    return bool(opt) ? *opt <= value : true;
}

template<class T>
constexpr bool operator<=(const T &value, const optional<T> &opt) {
    return bool(opt) ? value <= *opt : false;
}

template<class T>
constexpr bool operator>(const optional<T> &opt, const T &value) {
    return bool(opt) ? *opt > value : false;
}

template<class T>
constexpr bool operator>(const T &value, const optional<T> &opt) {
    return bool(opt) ? value > *opt : true;
}

template<class T>
constexpr bool operator>=(const optional<T> &opt, const T &value) {
    return bool(opt) ? *opt >= value : false;
}

template<class T>
constexpr bool operator>=(const T &value, const optional<T> &opt) {
    return bool(opt) ? value >= *opt : true;
}

template<class T>
void swap(optional<T> &lhs, optional<T> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template<class T>
constexpr optional<decay_t<T>> make_optional(T &&value) {
    return optional<decay_t<T>>(forward<T>(value));
}

template<class T, class... Args>
constexpr optional<T> make_optional(Args &&... args) {
    return optional<T>(in_place, forward<Args>(args)...);
}

template<class T, class U, class... Args>
constexpr optional<T> make_optional(initializer_list<U> il, Args &&... args) {
    return optional<T>(in_place, il, forward<Args>(args)...);
}

} // namespace ala

#endif