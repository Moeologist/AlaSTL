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

    static constexpr bool _is_local = sizeof(T) <= 2 * sizeof(size_t) &&
                                      is_nothrow_move_constructible<T>::value;

    static bool local() {
        return _is_local;
    }

    static void *operate(AnyOP op) {
        switch (op) {
            case AnyOP::Copy:
                return reinterpret_cast<void *>(&_any_handle::copy);
            case AnyOP::Move:
                return reinterpret_cast<void *>(&_any_handle::move);
            case AnyOP::Destroy:
                return reinterpret_cast<void *>(&_any_handle::destroy);
            case AnyOP::TypeInfo:
                return reinterpret_cast<void *>(&_any_handle::typeinfo);
            case AnyOP::Local:
                return reinterpret_cast<void *>(&_any_handle::local);
        }
        return nullptr;
    }
};

class any {
protected:
    static_assert(sizeof(void *) == sizeof(size_t), "Unsupported platform");

    typedef void (*op_copy_t)(void *, const void *);
    typedef void (*op_move_t)(void *, void *);
    typedef void (*op_destroy_t)(void *);
#if ALA_USE_RTTI
    typedef const type_info &(*op_typeinfo_t)();
#endif
    typedef bool (*op_local_t)();

    void *(*_op_handle)(AnyOP) = nullptr;
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

    op_copy_t _op_copy() const noexcept {
        return reinterpret_cast<op_copy_t>(_op_handle(AnyOP::Copy));
    }

    op_move_t _op_move() const noexcept {
        return reinterpret_cast<op_move_t>(_op_handle(AnyOP::Move));
    }

    op_destroy_t _op_destroy() const noexcept {
        return reinterpret_cast<op_destroy_t>(_op_handle(AnyOP::Destroy));
    }

    op_typeinfo_t _op_typeinfo() const noexcept {
        return reinterpret_cast<op_typeinfo_t>(_op_handle(AnyOP::TypeInfo));
    }

    op_local_t _op_local() const noexcept {
        return reinterpret_cast<op_local_t>(_op_handle(AnyOP::Local));
    }

    bool is_local() const noexcept {
        if (_op_handle == nullptr)
            return true;
        return _op_local()();
    }

    void _copy(const any &other) {
        if (other.has_value()) {
            _op_handle = other._op_handle;
            if (other.is_local()) {
                _op_copy()(this->_address(), other._address());
            } else {
                void *p = nullptr;
                try {
                    p = ::operator new(other._size());
                    if (p == nullptr) {
                        _op_handle = nullptr;
                        return;
                    }
                    _op_copy()(p, other._address());
                    this->_addref() = p;
                    this->_size() = other._size();
                } catch (...) {
                    ::operator delete(p);
                    _op_handle = nullptr;
                    throw;
                }
            }
        }
    }

    void _move(any &&other) {
        if (other.has_value()) {
            _op_handle = other._op_handle;
            if (other.is_local()) {
                _op_move()(this->_address(), other._address());
                other.reset();
            } else {
                _addref() = other._addref();
                _size() = other._size();
                other._addref() = nullptr;
                other._size() = 0;
            }
            other._op_handle = nullptr;
        }
    }

    template<class T, class... Args>
    void _emplace(Args &&...args) {
        using value_t = T;
        using handle_t = _any_handle<value_t>;
        reset();
        _op_handle = &handle_t::operate;
        void *p = nullptr;
        try {
            if (sizeof(value_t) > sizeof(_placehold) ||
                !is_nothrow_move_constructible<value_t>::value) {
                p = ::operator new(sizeof(value_t));
                if (p == nullptr) {
                    _op_handle = nullptr;
                    return;
                }
                this->_addref() = p;
                this->_size() = sizeof(value_t);
            }
            ::new (this->_address()) value_t(ala::forward<Args>(args)...);
        } catch (...) {
            ::operator delete(p);
            _op_handle = nullptr;
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
            _op_destroy()(this->_address());
        if (!is_local()) {
            void *p = this->_address();
            ::operator delete(p);
        }
        _op_handle = nullptr;
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
        return _op_handle != nullptr;
    }

#if ALA_USE_RTTI
    const type_info &type() const noexcept {
        if (has_value())
            return _op_typeinfo()();
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