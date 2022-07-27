#ifndef _ALA_ANY_H
#define _ALA_ANY_H

#include <ala/utility.h>

#if ALA_USE_RTTI
    #include <typeinfo>
using std::type_info;
#endif

namespace ala {

struct bad_any_cast: exception {};

enum class AnyOP { Copy, Move, Destroy, TypeInfo, Local };

template<class T>
struct _any_handle {
    static void copy(void *dst, const void *src) {
        ::new (dst) T(*reinterpret_cast<const T *>(src));
    }
    // TODO static_cast
    static void move(void *dst, void *src) {
        ::new (dst) T(ala::move(*reinterpret_cast<T *>(src)));
    }

    static void destroy(void *dst) {
        (*reinterpret_cast<T *>(dst)).~T();
    }

#if ALA_USE_RTTI
    static const type_info &typeinfo() {
        return typeid(T);
    }
#endif

    static bool local() {
        return sizeof(T) <= 2 * sizeof(size_t) &&
               is_nothrow_move_constructible<T>::value;
    }

    static void **get_vtable() {
        static void *vtable[] = {
            reinterpret_cast<void *>(&_any_handle::copy),
            reinterpret_cast<void *>(&_any_handle::move),
            reinterpret_cast<void *>(&_any_handle::destroy),
            reinterpret_cast<void *>(&_any_handle::typeinfo),
            reinterpret_cast<void *>(&_any_handle::local),
        };
        return vtable;
    };
};

class any {
protected:
    static_assert(sizeof(void *) == sizeof(size_t), "Unsupported platform");

    using copy_t = decltype(&_any_handle<int>::copy);
    using move_t = decltype(&_any_handle<int>::move);
    using destroy_t = decltype(&_any_handle<int>::destroy);
#if ALA_USE_RTTI
    using typeinfo_t = decltype(&_any_handle<int>::typeinfo);
#endif
    using local_t = decltype(&_any_handle<int>::local);

    void **_vptr = nullptr;
    size_t _placehold[2] = {};

    template<class T>
    friend const T *any_cast(const any *) noexcept;

    template<class T>
    friend T *any_cast(any *) noexcept;

    const void *_address() const noexcept {
        if (is_local())
            return reinterpret_cast<const void *>(&_placehold);
        return reinterpret_cast<const void *>(_placehold[0]);
    }

    void *_address() noexcept {
        if (is_local())
            return reinterpret_cast<void *>(&_placehold);
        return reinterpret_cast<void *>(_placehold[0]);
    }

    size_t &_size() noexcept {
        assert(!is_local());
        return _placehold[1];
    }

    const size_t &_size() const noexcept {
        assert(!is_local());
        return _placehold[1];
    }

    void *&_addref() noexcept {
        assert(!is_local());
        return *reinterpret_cast<void **>(_placehold);
    }

    template<class F>
    F _any_op(AnyOP op) const noexcept {
        return reinterpret_cast<F>(_vptr[static_cast<size_t>(op)]);
    }

    bool is_local() const noexcept {
        if (_vptr == nullptr)
            return true;
        return _any_op<local_t>(AnyOP::Local)();
    }

    void _copy(const any &other) {
        if (other.has_value()) {
            _vptr = other._vptr;
            if (other.is_local()) {
                _any_op<copy_t>(AnyOP::Copy)(this->_address(), other._address());
            } else {
                void *p = nullptr;
                try {
                    p = ::operator new(other._size());
                    if (p == nullptr) {
                        _vptr = nullptr;
                        return;
                    }
                    _any_op<copy_t>(AnyOP::Copy)(p, other._address());
                    this->_addref() = p;
                    this->_size() = other._size();
                } catch (...) {
                    ::operator delete(p);
                    _vptr = nullptr;
                    throw;
                }
            }
        }
    }

    void _move(any &&other) {
        if (other.has_value()) {
            _vptr = other._vptr;
            if (other.is_local()) {
                _any_op<move_t>(AnyOP::Move)(this->_address(), other._address());
                other.reset();
            } else {
                _addref() = other._addref();
                _size() = other._size();
                other._addref() = nullptr;
                other._size() = 0;
            }
            other._vptr = nullptr;
        }
    }

    template<class T, class... Args>
    void _emplace(Args &&...args) {
        using value_t = T;
        using handle_t = _any_handle<value_t>;
        reset();
        _vptr = handle_t::get_vtable();
        void *p = nullptr;
        try {
            if (sizeof(value_t) > sizeof(_placehold) ||
                !is_nothrow_move_constructible<value_t>::value) {
                p = ::operator new(sizeof(value_t));
                if (p == nullptr) {
                    _vptr = nullptr;
                    return;
                }
                this->_addref() = p;
                this->_size() = sizeof(value_t);
            }
            ::new (this->_address()) value_t(ala::forward<Args>(args)...);
        } catch (...) {
            ::operator delete(p);
            _vptr = nullptr;
            throw;
        }
    }

public:
    // construction and destruction
    constexpr any() noexcept {}

    any(const any &other) {
        this->_copy(other);
    }

    any(any &&other) noexcept {
        this->_move(ala::move(other));
    }

    template<class T, class... Args,
             class = enable_if_t<is_copy_constructible<decay_t<T>>::value &&
                                 is_constructible<decay_t<T>, Args...>::value>>
    explicit any(in_place_type_t<T>, Args &&...args) {
        this->_emplace<decay_t<T>>(ala::forward<Args>(args)...);
    }

    template<class T, class U, class... Args,
             class = enable_if_t<
                 is_copy_constructible<decay_t<T>>::value &&
                 is_constructible<decay_t<T>, initializer_list<U> &, Args...>::value>>
    explicit any(in_place_type_t<T> tag, initializer_list<U> il, Args &&...args) {
        this->_emplace<decay_t<T>>(il, ala::forward<Args>(args)...);
    }

    template<class T,
             class = enable_if_t<!is_same<any, decay_t<T>>::value &&
                                 !is_specification<decay_t<T>, ala::in_place_type_t>::value &&
                                 is_copy_constructible<decay_t<T>>::value>>
    any(T &&value) {
        this->_emplace<decay_t<T>>(ala::forward<T>(value));
    }

    ~any() {
        reset();
    }

    // assignments
    any &operator=(const any &other) {
        if (ala::addressof(other) != this)
            any(other).swap(*this);
        return *this;
    }

    any &operator=(any &&other) noexcept {
        if (ala::addressof(other) != this)
            any(ala::move(other)).swap(*this);
        return *this;
    }

    template<class T>
    enable_if_t<!is_same<any, decay_t<T>>::value &&
                    is_copy_constructible<decay_t<T>>::value,
                any &>
    operator=(T &&other) {
        any(ala::forward<T>(other)).swap(*this);
        return *this;
    }

    // modifiers
    template<class T, class... Args>
    enable_if_t<is_copy_constructible<decay_t<T>>::value &&
                    is_constructible<decay_t<T>, Args...>::value,
                decay_t<T> &>
    emplace(Args &&...args) {
        this->_emplace<decay_t<T>>(ala::forward<Args>(args)...);
        return *reinterpret_cast<T *>(_address());
    }

    template<class T, class U, class... Args>
    enable_if_t<is_copy_constructible<decay_t<T>>::value &&
                    is_constructible<decay_t<T>, initializer_list<U> &, Args...>::value,
                decay_t<T> &>
    emplace(initializer_list<U> il, Args &&...args) {
        this->_emplace<decay_t<T>>(il, ala::forward<Args>(args)...);
        return *reinterpret_cast<T *>(_address());
    }

    void reset() noexcept {
        if (has_value())
            _any_op<destroy_t>(AnyOP::Destroy)(this->_address());
        if (!is_local()) {
            void *p = this->_address();
            ::operator delete(p);
        }
        _vptr = nullptr;
    }

    void swap(any &other) noexcept {
        if (ala::addressof(other) == this)
            return;
        any tmp(ala::move(*this));
        this->_move(ala::move(other));
        other._move(ala::move(tmp));
    }

    // observers
    bool has_value() const noexcept {
        return _vptr != nullptr;
    }

#if ALA_USE_RTTI
    const type_info &type() const noexcept {
        if (has_value())
            return _any_op<typeinfo_t>(AnyOP::TypeInfo)();
        return typeid(void);
    }
#endif
};

// non-member functions
inline void swap(any &lhs, any &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template<class T, class... Args>
any make_any(Args &&...args) {
    return any(in_place_type_t<T>{}, ala::forward<Args>(args)...);
}

template<class T, class U, class... Args>
any make_any(initializer_list<U> il, Args &&...args) {
    return any(in_place_type_t<T>{}, il, ala::forward<Args>(args)...);
}

template<class T>
const T *any_cast(const any *operand) noexcept {
    if (operand != nullptr && operand->type() == typeid(T))
        return reinterpret_cast<const T *>(operand->_address());
    return nullptr;
}

template<class T>
T *any_cast(any *operand) noexcept {
    if (operand != nullptr && operand->type() == typeid(T))
        return reinterpret_cast<T *>(operand->_address());
    return nullptr;
}

template<class T>
T any_cast(const any &operand) {
    using value_t = remove_cvref_t<T>;
    static_assert(is_constructible<T, const value_t &>::value,
                  "any_cast requires T constructible");
    const value_t *ptr = any_cast<value_t>(&operand);
    if (ptr == nullptr)
        throw bad_any_cast{};
    return static_cast<T>(*ptr);
}

template<class T>
T any_cast(any &operand) {
    using value_t = remove_cvref_t<T>;
    static_assert(is_constructible<T, value_t &>::value,
                  "any_cast requires T constructible");
    value_t *ptr = any_cast<value_t>(&operand);
    if (ptr == nullptr)
        throw bad_any_cast{};
    return static_cast<T>(*ptr);
}

template<class T>
T any_cast(any &&operand) {
    using value_t = remove_cvref_t<T>;
    static_assert(is_constructible<T, value_t &&>::value,
                  "any_cast requires T constructible");
    value_t *ptr = any_cast<value_t>(&operand);
    if (ptr == nullptr)
        throw bad_any_cast{};
    return static_cast<T>(ala::move(*ptr));
}

} // namespace ala

#endif