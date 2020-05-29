#ifndef _ALA_FUNCTIONAL_H
#define _ALA_FUNCTIONAL_H

#include <ala/detail/functional_base.h>
#include <ala/detail/allocator.h>
#include <ala/tuple.h>

#if ALA_USE_RTTI
    #include <typeinfo>
#endif

namespace ala {
#if ALA_USE_RTTI
using ::std::type_info;
#endif

struct identity {
    using is_transparent = int;
    template<class T>
    constexpr T &&operator()(T &&t) const noexcept {
        return ala::forward<T>(t);
    }
};

template<class Fn>
struct _not_fn_t {
    typedef decay_t<Fn> _fn_t;
    _fn_t _fn;

    constexpr explicit _not_fn_t(Fn &&fn): _fn(ala::forward<Fn>(fn)) {}
    constexpr _not_fn_t(_not_fn_t &&fn) = default;
    constexpr _not_fn_t(const _not_fn_t &fn) = default;

    template<class... Args>
    constexpr auto operator()(Args &&... args) &noexcept(
        noexcept(!ala::invoke(_fn, ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t &, Args...>>()) {
        return !ala::invoke(_fn, ala::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator()(Args &&... args) const &noexcept(
        noexcept(!ala::invoke(_fn, ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t const &, Args...>>()) {
        return !ala::invoke(_fn, ala::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator()(Args &&... args) &&noexcept(
        noexcept(!ala::invoke(ala::move(_fn), ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t, Args...>>()) {
        return !ala::invoke(ala::move(_fn), ala::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator()(Args &&... args) const &&noexcept(
        noexcept(!ala::invoke(ala::move(_fn), ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t const, Args...>>()) {
        return !ala::invoke(ala::move(_fn), ala::forward<Args>(args)...);
    }
};

template<class Fn>
_not_fn_t<Fn> not_fn(Fn &&fn) {
    return _not_fn_t<Fn>(ala::forward<Fn>(fn));
}

template<int N>
struct _placeholder_t {};

namespace placeholders {
#if _ALA_ENABLE_INLINE_VAR
inline constexpr _placeholder_t<1> _1;
inline constexpr _placeholder_t<2> _2;
inline constexpr _placeholder_t<3> _3;
inline constexpr _placeholder_t<4> _4;
inline constexpr _placeholder_t<5> _5;
inline constexpr _placeholder_t<6> _6;
inline constexpr _placeholder_t<7> _7;
inline constexpr _placeholder_t<8> _8;
inline constexpr _placeholder_t<9> _9;
inline constexpr _placeholder_t<10> _10;
#else
const _placeholder_t<1> _1;
const _placeholder_t<2> _2;
const _placeholder_t<3> _3;
const _placeholder_t<4> _4;
const _placeholder_t<5> _5;
const _placeholder_t<6> _6;
const _placeholder_t<7> _7;
const _placeholder_t<8> _8;
const _placeholder_t<9> _9;
const _placeholder_t<10> _10;
#endif
} // namespace placeholders

template<class T>
struct is_placeholder: integral_constant<int, 0> {};

template<int N>
struct is_placeholder<_placeholder_t<N>>: integral_constant<int, N> {};

template<class R, class Fn, class... Args>
struct _bind_t;

template<class T>
struct is_bind_expression: false_type {};

template<class... Ts>
struct is_bind_expression<_bind_t<Ts...>>: true_type {};

#if _ALA_ENABLE_INLINE_VAR
template<class T>
inline constexpr int is_placeholder_v = is_placeholder<T>::value;

template<class T>
inline constexpr bool is_bind_expression_v = is_bind_expression<T>::value;
#endif

struct _unknown_result {};
struct _unknown_arg {};

template<class R>
struct _function_base {
    using result_type = R;
};

template<>
struct _function_base<_unknown_result> {};

template<class R, class Fn, class... Args>
struct _bind_t: _function_base<R> {
    typedef tuple<decay_t<Fn>, decay_t<Args>...> _tuple_t;
    _tuple_t _tuple;

    template<class Tuple, class T,
             class = enable_if_t<!is_bind_expression<remove_cvref_t<T>>::value>>
    T &&_select(Tuple &&, T &&val) const {
        return ala::forward<T>(val);
    }

    template<class Tuple, class Bind, class = void,
             class = enable_if_t<is_bind_expression<remove_cvref_t<Bind>>::value>>
    auto _select(Tuple &&tp, Bind &&inner_bind) const
        -> decltype(ala::forward<Bind>(inner_bind)._call(ala::forward<Tuple>(tp))) {
        return ala::forward<Bind>(inner_bind)._call(ala::forward<Tuple>(tp));
    }

    template<class Tuple, int N>
    auto _select(Tuple &&tp, _placeholder_t<N>) const
        -> decltype(ala::get<N - 1>(ala::forward<Tuple>(tp))) {
        static_assert(N > 0 && N <= tuple_size<remove_cvref_t<Tuple>>::value,
                      "Illegal index");
        return ala::get<N - 1>(ala::forward<Tuple>(tp));
    }

    template<class Tuple, class T>
    auto _select(Tuple &&tp, reference_wrapper<T> refwarp) const
        -> decltype(refwarp.get()) {
        return refwarp.get();
    }

    template<class Tuple, size_t... N>
    R _call_helper(false_type, Tuple &&tp, index_sequence<N...>) const {
        return static_cast<R>(ala::invoke(ala::get<0>(_tuple),
                                          _select(ala::forward<Tuple>(tp),
                                                  ala::get<N + 1>(_tuple))...));
    }

    template<class Tuple, size_t... N>
    R _call_helper(false_type, Tuple &&tp, index_sequence<N...>) {
        return static_cast<R>(ala::invoke(ala::get<0>(_tuple),
                                          _select(ala::forward<Tuple>(tp),
                                                  ala::get<N + 1>(_tuple))...));
    }

    template<class Tuple, size_t... N>
    auto _call_helper(true_type, Tuple &&tp, index_sequence<N...>) const
        -> decltype(ala::invoke(ala::get<0>(_tuple),
                                _select(ala::forward<Tuple>(tp),
                                        ala::get<N + 1>(_tuple))...)) {
        return ala::invoke(ala::get<0>(_tuple),
                           _select(ala::forward<Tuple>(tp),
                                   ala::get<N + 1>(_tuple))...);
    }

    template<class Tuple, size_t... N>
    auto _call_helper(true_type, Tuple &&tp, index_sequence<N...>)
        -> decltype(ala::invoke(ala::get<0>(_tuple),
                                _select(ala::forward<Tuple>(tp),
                                        ala::get<N + 1>(_tuple))...)) {
        return ala::invoke(ala::get<0>(_tuple),
                           _select(ala::forward<Tuple>(tp),
                                   ala::get<N + 1>(_tuple))...);
    }

    template<class Tuple>
    auto _call(Tuple &&tp) const
        -> decltype(this->_call_helper(is_same<R, _unknown_result>(),
                                       ala::forward<Tuple>(tp),
                                       ala::index_sequence_for<Args...>())) {
        return this->_call_helper(is_same<R, _unknown_result>(),
                                  ala::forward<Tuple>(tp),
                                  ala::index_sequence_for<Args...>());
    }

    template<class Tuple>
    auto _call(Tuple &&tp)
        -> decltype(this->_call_helper(is_same<R, _unknown_result>(),
                                       ala::forward<Tuple>(tp),
                                       ala::index_sequence_for<Args...>())) {
        return this->_call_helper(is_same<R, _unknown_result>(),
                                  ala::forward<Tuple>(tp),
                                  ala::index_sequence_for<Args...>());
    }

    _bind_t(const Fn &fn, const Args &... args): _tuple(fn, args...) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value>>
    _bind_t(const _bind_t &other): _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value>>
    _bind_t(_bind_t &&other): _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    auto operator()(Args1 &&... args) const -> decltype(
        this->_call(ala::forward_as_tuple(ala::forward<Args1>(args)...))) {
        return this->_call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }

    template<class... Args1>
    auto operator()(Args1 &&... args) -> decltype(
        this->_call(ala::forward_as_tuple(ala::forward<Args1>(args)...))) {
        return this->_call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }
};

enum Operation { Copy, Move, Destroy, TypeInfo, Invoke, Local };

template<class Function, class Functor>
struct _function_handle;

template<class R, class... Args, class Functor>
struct _function_handle<R(Args...), Functor> {
    static void copy(void *dst, const void *src) {
        ::new (dst) Functor(*(Functor *)src);
    }

    static void move(void *dst, const void *src) {
        ::new (dst) Functor(ala::move(*(Functor *)src));
    }

    static void destroy(void *dst) {
        (*(Functor *)dst).~Functor();
    }

    static const type_info &typeinfo() {
        return typeid(Functor);
    }

    static R invoke(void *f, Args &&... args) {
        return static_cast<R>(
            ala::invoke(*(Functor *)f, ala::forward<Args>(args)...));
    }

    static bool local() {
        return sizeof(Functor) <= 2 * sizeof(size_t) &&
               is_nothrow_move_constructible<Functor>::value;
    }

    static void *operate(Operation op) {
        switch (op) {
            case Copy:
                return reinterpret_cast<void *>(&_function_handle::copy);
            case Move:
                return reinterpret_cast<void *>(&_function_handle::move);
            case Destroy:
                return reinterpret_cast<void *>(&_function_handle::destroy);
            case TypeInfo:
                return reinterpret_cast<void *>(&_function_handle::typeinfo);
            case Invoke:
                return reinterpret_cast<void *>(&_function_handle::invoke);
            case Local:
                return reinterpret_cast<void *>(&_function_handle::local);
        }
        return nullptr;
    };
};

struct bad_function_call: exception {
    explicit bad_function_call() {}
};

template<class>
struct function;

template<class T>
struct _is_ala_function: false_type {};

template<class R, class... Args>
struct _is_ala_function<function<R(Args...)>>: true_type {};

template<class R, class... Args>
struct function<R(Args...)>: _function_base<R> {
    static_assert(sizeof(void *) == sizeof(size_t), "Unsupported platform");

    typedef R (*op_invoke_t)(void *, Args &&...);
    typedef void (*op_copy_t)(void *, const void *);
    typedef void (*op_move_t)(void *, const void *);
    typedef void (*op_destroy_t)(void *);
    typedef const type_info &(*op_typeinfo_t)();
    typedef bool (*op_local_t)();

    void *(*_op_handle)(Operation) = nullptr;
    size_t _placehold[2] = {};

    op_local_t _op_local() const noexcept {
        return reinterpret_cast<op_local_t>(_op_handle(Local));
    }

    bool is_local() const noexcept {
        if (_op_handle == nullptr)
            return true;
        return _op_local()();
    }

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
        assert(!is_local() || !*this);
        return _placehold[1];
    }

    const size_t &_size() const noexcept {
        assert(!is_local() || !*this);
        return _placehold[1];
    }

    void *&_addref() noexcept {
        assert(!is_local() || !*this);
        return *reinterpret_cast<void **>(_placehold);
    }

    op_invoke_t _op_invoke() const noexcept {
        return reinterpret_cast<op_invoke_t>(_op_handle(Invoke));
    }

    op_copy_t _op_copy() const noexcept {
        return reinterpret_cast<op_copy_t>(_op_handle(Copy));
    }

    op_move_t _op_move() const noexcept {
        return reinterpret_cast<op_move_t>(_op_handle(Move));
    }

    op_destroy_t _op_destroy() const noexcept {
        return reinterpret_cast<op_destroy_t>(_op_handle(Destroy));
    }

    op_typeinfo_t _op_typeinfo() const noexcept {
        return reinterpret_cast<op_typeinfo_t>(_op_handle(TypeInfo));
    }

    void *_alloc(size_t sz) {
        void *p = ::operator new(sz);
        this->_addref() = p;
        this->_size() = sz;
        return p;
    }

    void _dealloc() {
        if (!is_local()) {
            void *p = this->_address();
            ::operator delete(p);
        }
    }

    void _copy(const function &other) {
        _op_handle = other._op_handle;
        if (other) {
            if (other.is_local()) {
                _op_copy()(this->_address(), other._address());
            } else {
                this->_alloc(other._size());
                _op_copy()(this->_address(), other._address());
            }
        }
    }

    void _move(function &&other) {
        if (other) {
            if (other.is_local()) {
                _op_handle = other._op_handle;
                _op_move()(this->_address(), other._address());
            } else {
                ala::swap(_addref(), other._addref());
                ala::swap(_size(), other._size());
                _op_handle = other._op_handle;
            }
            other._op_handle = nullptr;
        }
    }

    // construct/copy/destroy:
    function() noexcept {}

    function(nullptr_t) noexcept {}

    function(const function &other) {
        this->_copy(other);
    }

    function(function &&other) noexcept {
        this->_move(ala::move(other));
    }

    template<class Fn>
    enable_if_t<_is_ala_function<Fn>::value, bool> _is_empty_fn(const Fn &fn) {
        return !fn;
    }

    template<class Fn>
    enable_if_t<is_pointer<Fn>::value || is_member_pointer<Fn>::value, bool>
    _is_empty_fn(const Fn &fn) {
        return fn == nullptr;
    }

    template<class Fn>
    enable_if_t<!_is_ala_function<Fn>::value &&
                    !(is_pointer<Fn>::value || is_member_pointer<Fn>::value),
                bool>
    _is_empty_fn(const Fn &fn) {
        return false;
    }

    template<class Fn, class = enable_if_t<is_invocable_r<R, Fn &, Args...>::value>>
    function(Fn fn) {
        // static_assert(sizeof(Fn) <= sizeof(placehold_t),
        //               "Functor size is too large, "
        //               "ala::function have no dynamic memory allocation, ");
        // static_assert(is_nothrow_move_constructible<Fn>::value,
        //               "ala::function use small-objects optimization, "
        //               "Fn must be nothrow_move_constructible.");
        if (this->template _is_empty_fn<Fn>(fn))
            return;
        typedef _function_handle<R(Args...), Fn> handle_t;
        _op_handle = &handle_t::operate;
        if (sizeof(fn) > sizeof(_placehold) ||
            !is_nothrow_move_constructible<Fn>::value)
            this->_alloc(sizeof(Fn));
        void *src = reinterpret_cast<void *>(ala::addressof(fn));
        _op_move()(this->_address(), src);
    }

    function &operator=(const function &other) {
        if (ala::addressof(other) != this)
            function(other).swap(*this);
        return *this;
    }

    function &operator=(function &&other) noexcept {
        if (ala::addressof(other) != this)
            function(ala::move(other)).swap(*this);
        return *this;
    }

    function &operator=(nullptr_t) noexcept {
        function(ala::move(*this));
        return *this;
    }

    template<class Fn>
    enable_if_t<is_invocable_r<R, Fn &, Args...>::value, function &>
    operator=(Fn &&fn) {
        function(ala::forward<Fn>(fn)).swap(*this);
        return *this;
    }

    template<class Fn>
    function &operator=(reference_wrapper<Fn>) noexcept;
    ~function() {
        if (*this)
            _op_destroy()(this->_address());
        _dealloc();
    }

    void swap(function &other) noexcept {
        if (!this->is_local() && !other.is_local()) {
            ala::swap(this->_addref(), other._addref());
            ala::swap(this->_size(), other._size());
            ala::swap(this->_op_handle, other._op_handle);
        } else {
            function tmp(ala::move(*this));
            this->_move(ala::move(other));
            other._move(ala::move(tmp));
        }
    }

    explicit operator bool() const noexcept {
        return _op_handle != nullptr;
    }

    R operator()(Args... args) const {
        if (!*this)
            throw bad_function_call();
        return _op_invoke()(const_cast<function *>(this)->_address(),
                            ala::forward<Args>(args)...);
    }

#if ALA_USE_RTTI
    template<class T>
    T *target() noexcept {
        if (*this && typeid(T) == target_type())
            return (T *)this->_address();
        return nullptr;
    }

    template<class T>
    const T *target() const noexcept {
        return const_cast<function *>(this)->target<T>();
    }

    const type_info &target_type() const noexcept {
        if (*this)
            return _op_typeinfo()();
        return typeid(void);
    }
#endif
}; // namespace ala

template<class R, class... Args>
void swap(function<R(Args...)> &lhs, function<R(Args...)> &rhs) noexcept {
    lhs.swap(rhs);
}
template<class R, class... Args>
bool operator==(const function<R(Args...)> &f, nullptr_t) noexcept {
    return !f;
}

template<class R, class... Args>
bool operator==(nullptr_t, const function<R(Args...)> &f) noexcept {
    return !f;
}

template<class R, class... Args>
bool operator!=(const function<R(Args...)> &f, nullptr_t) noexcept {
    return !!f;
}

template<class R, class... Args>
bool operator!=(nullptr_t, const function<R(Args...)> &f) noexcept {
    return !!f;
}

// clang-format off
template<class> struct _function_traits {};
//                                                                                                                          | noexcept | rref | lref | volatile | const |
template<class R, class... Args> struct _function_traits<R(Args...)>                        { typedef R type(Args...);      static constexpr unsigned qualify =    0b0; };
template<class R, class... Args> struct _function_traits<R(Args...) const>                  { typedef R type(Args...);      static constexpr unsigned qualify =   0b01; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile>               { typedef R type(Args...);      static constexpr unsigned qualify =   0b10; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile>         { typedef R type(Args...);      static constexpr unsigned qualify =   0b11; };
template<class R, class... Args> struct _function_traits<R(Args...) &>                      { typedef R type(Args...);      static constexpr unsigned qualify =  0b100; };
template<class R, class... Args> struct _function_traits<R(Args...) const &>                { typedef R type(Args...);      static constexpr unsigned qualify =  0b101; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile &>             { typedef R type(Args...);      static constexpr unsigned qualify =  0b110; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile &>       { typedef R type(Args...);      static constexpr unsigned qualify =  0b111; };
template<class R, class... Args> struct _function_traits<R(Args...) &&>                     { typedef R type(Args...);      static constexpr unsigned qualify = 0b1000; };
template<class R, class... Args> struct _function_traits<R(Args...) const &&>               { typedef R type(Args...);      static constexpr unsigned qualify = 0b1001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile &&>            { typedef R type(Args...);      static constexpr unsigned qualify = 0b1010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile &&>      { typedef R type(Args...);      static constexpr unsigned qualify = 0b1011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...)>                   { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b0; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const>             { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b01; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile>          { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b10; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile>    { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b11; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) &>                 { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b100; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const &>           { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b101; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile &>        { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b110; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile &>  { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b111; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) &&>                { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b1000; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const &&>          { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b1001; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile &&>       { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b1010; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile &&> { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b1011; };
#if _ALA_ENABLE_NOEXCEPT_TYPE
template<class R, class... Args> struct _function_traits<R(Args...) noexcept>                        { typedef R type(Args...);      static constexpr unsigned qualify = 0b10000; };
template<class R, class... Args> struct _function_traits<R(Args...) const noexcept>                  { typedef R type(Args...);      static constexpr unsigned qualify = 0b10001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile noexcept>               { typedef R type(Args...);      static constexpr unsigned qualify = 0b10010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile noexcept>         { typedef R type(Args...);      static constexpr unsigned qualify = 0b10011; };
template<class R, class... Args> struct _function_traits<R(Args...) & noexcept>                      { typedef R type(Args...);      static constexpr unsigned qualify = 0b10100; };
template<class R, class... Args> struct _function_traits<R(Args...) const & noexcept>                { typedef R type(Args...);      static constexpr unsigned qualify = 0b10101; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile & noexcept>             { typedef R type(Args...);      static constexpr unsigned qualify = 0b10110; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile & noexcept>       { typedef R type(Args...);      static constexpr unsigned qualify = 0b10111; };
template<class R, class... Args> struct _function_traits<R(Args...) && noexcept>                     { typedef R type(Args...);      static constexpr unsigned qualify = 0b11000; };
template<class R, class... Args> struct _function_traits<R(Args...) const && noexcept>               { typedef R type(Args...);      static constexpr unsigned qualify = 0b11001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile && noexcept>            { typedef R type(Args...);      static constexpr unsigned qualify = 0b11010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile && noexcept>      { typedef R type(Args...);      static constexpr unsigned qualify = 0b11011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) noexcept>                   { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10000; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const noexcept>             { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10001; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile noexcept>          { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10010; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile noexcept>    { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) & noexcept>                 { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10100; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const & noexcept>           { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10101; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile & noexcept>        { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10110; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile & noexcept>  { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b10111; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) && noexcept>                { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b11000; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const && noexcept>          { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b11001; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile && noexcept>       { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b11010; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile && noexcept> { typedef R type(Args..., ...); static constexpr unsigned qualify = 0b11011; };
#endif
// clang-format on

template<class Fn>
struct _function_traits_raw {};

template<class R, class... Args>
struct _function_traits_raw<R(Args...)> {
    typedef R type(Args...);
    typedef R result_type;
    static constexpr size_t args_count = sizeof...(Args);
};

template<class R, class... Args>
struct _function_traits_raw<R(Args..., ...)> {
    typedef R type(Args..., ...);
    typedef R result_type;
    static constexpr size_t args_count = size_t(-1);
};

template<class Fn, class = void>
struct _function_helper: _function_traits_raw<Fn> {};

template<class Fn>
struct _function_helper<Fn, void_t<typename _function_traits<Fn>::type>> {
    typedef typename _function_traits<Fn>::type type;
    typedef typename _function_traits_raw<type>::result_type result_type;
    static constexpr size_t args_count = _function_traits_raw<type>::args_count;
};

template<class MemPtr>
struct _memptr_traits {};

template<class Class, class Mem>
struct _memptr_traits<Mem Class::*> {
    typedef Class _cls_t;
    typedef Mem _mem_t;
};

template<class Fn, class = void>
struct _functor_result {
    using result_type = _unknown_result;
};

template<class Fn>
struct _functor_result<Fn, void_t<typename Fn::result_type>> {
    using result_type = typename Fn::result_type;
};

template<class Fn, class = void>
struct _functor_helper: _functor_result<Fn> {
    static constexpr size_t args_count = size_t(-1);
};

template<class Fn>
struct _functor_helper<Fn, void_t<decltype(&Fn::operator())>> {
    typedef typename _memptr_traits<decltype(&Fn::operator())>::_mem_t _mem_t;
    static constexpr unsigned _qualify = _function_traits<_mem_t>::qualify;
    static_assert((_qualify & 0b1000u) == 0,
                  "ala::function can not bind to &&-qualify function object");
    typedef typename _function_helper<_mem_t>::type type;
    typedef typename _function_helper<type>::result_type result_type;
    static constexpr size_t args_count = _function_helper<type>::args_count;
};

template<class MemPtr, bool = is_member_object_pointer<MemPtr>::value,
         bool = is_member_function_pointer<MemPtr>::value>
struct _memptr_helper;

template<class MemPtr>
struct _memptr_helper<MemPtr, true, false> {
    using _mem_t = typename _memptr_traits<MemPtr>::_mem_t;
    using _cls_t = typename _memptr_traits<MemPtr>::_cls_t;
    using result_type = _unknown_result;
    static constexpr size_t args_count = 1;
};

template<class MemPtr>
struct _memptr_helper<MemPtr, false, true> {
    using _mem_t = typename _memptr_traits<MemPtr>::_mem_t;
    using _cls_t = typename _memptr_traits<MemPtr>::_cls_t;
    using raw_t = typename _function_traits<_mem_t>::type;
    static constexpr unsigned qualify = _function_traits<_mem_t>::qualify;
    using result_type = typename _function_helper<raw_t>::result_type;
    static constexpr size_t _raw_args_count = _function_helper<raw_t>::args_count;
    static constexpr size_t args_count = _raw_args_count == size_t(-1) ?
                                             size_t(-1) :
                                             _raw_args_count;
};

template<class Fn, bool = is_class<Fn>::value, bool = is_member_pointer<Fn>::value>
struct _callable_traits: _function_helper<remove_pointer_t<Fn>> {};

template<class Functor>
struct _callable_traits<Functor, true, false>: _functor_helper<Functor> {};

template<class MemPtr>
struct _callable_traits<MemPtr, false, true>
    : _memptr_helper<remove_cv_t<MemPtr>> {};

// callable_traits can not process transparent function object (operator() is template)
template<class Callable>
struct callable_traits: _callable_traits<remove_reference_t<Callable>> {};

#if _ALA_ENABLE_DEDUCTION_GUIDES
// template<class R, class... Args>
// function(R (*)(Args...))->function<R(Args...)>;

// template<class Functor>
// function(Functor)
//     ->function<typename _functor_helper<decltype(&Functor::operator())>::type>;

// template<class MemPtr>
// function(MemPtr)->function<typename _memptr_helper<MemPtr>::type>;

template<class Callable>
function(Callable) -> function<typename callable_traits<Callable>::type>;
#endif

template<class Fn, class... Args>
auto bind(Fn &&fn, Args &&... args)
    -> _bind_t<typename callable_traits<Fn>::result_type, Fn, Args...> {
    using R = typename callable_traits<Fn>::result_type;
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(fn),
                                   ala::forward<Args>(args)...);
}

template<class R, class Fn, class... Args>
auto bind(Fn &&fn, Args &&... args) -> _bind_t<R, Fn, Args...> {
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(fn),
                                   ala::forward<Args>(args)...);
}

// C++20
template<class...>
struct Fucker;

template<auto...>
struct FuckerV;

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)

#define FUCK(x) Fucker<decltype(x)> DEFER_3(fuck)
#define FUCT(x) Fucker<x> DEFER_3(fuck)
#define FUCV(x) FuckerV<x> DEFER_3(fuck)
template<class Fn, class... Args>
struct _bind_front_t {
    typedef tuple<decay_t<Fn>, decay_t<Args>...> _tuple_t;
    _tuple_t _tuple;

    template<size_t... N, class... Args1>
    auto _call(index_sequence<N...>, Args1 &&... args1) const
        -> decltype(ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                                ala::forward<Args1>(args1)...)) {
        return ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                           ala::forward<Args1>(args1)...);
    }

    template<size_t... N, class... Args1>
    auto _call(index_sequence<N...>, Args1 &&... args1)
        -> decltype(ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                                ala::forward<Args1>(args1)...)) {
        return ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                           ala::forward<Args1>(args1)...);
    }

    _bind_front_t(Fn &&fn, Args &&... args)
        : _tuple(ala::forward<Fn>(fn), ala::forward<Args>(args)...) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value>>
    _bind_front_t(const _bind_front_t &other): _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value>>
    _bind_front_t(_bind_front_t &&other): _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    auto operator()(Args1 &&... args1) const
        -> decltype(this->_call(index_sequence_for<Args...>(),
                                ala::forward<Args1>(args1)...)) {
        return this->_call(index_sequence_for<Args...>(),
                           ala::forward<Args1>(args1)...);
    }

    template<class... Args1>
    auto operator()(Args1 &&... args1)
        -> decltype(this->_call(index_sequence_for<Args...>(),
                                ala::forward<Args1>(args1)...)) {
        return this->_call(index_sequence_for<Args...>(),
                           ala::forward<Args1>(args1)...);
    }
};

template<class Fn, class... Args>
_bind_front_t<Fn, Args...> bind_front(Fn &&fn, Args &&... args) {
    // static_assert(sizeof...(Args) <= callable_traits<Fn>::args_count,
    //               "Arguments count too many");
    return _bind_front_t<Fn, Args...>(ala::forward<Fn>(fn),
                                      ala::forward<Args>(args)...);
}

template<class MemPtr>
auto mem_fn(MemPtr pm) noexcept {
    // return ala::_mem_fn_t<MemPtr>(pm);
    return ala::_bind_front_t<MemPtr &>(pm);
}

} // namespace ala
#endif