#ifndef _ALA_FUNCTIONAL_H
#define _ALA_FUNCTIONAL_H

#include <ala/detail/functional_base.h>
#include <ala/detail/allocator.h>
#include <ala/tuple.h>
#include <ala/detail/hash.h>
#include <ala/algorithm.h>

#include <new>

#if ALA_USE_RTTI
    #include <typeinfo>
#endif

namespace ala {
#if ALA_USE_RTTI
using ::std::type_info;
#endif

template<class Fn>
struct _not_fn_t {
    using _fn_t = decay_t<Fn>;
    _fn_t _fn;

    constexpr explicit _not_fn_t(Fn &&fn): _fn(ala::forward<Fn>(fn)) {}
    constexpr _not_fn_t(_not_fn_t &&fn) = default;
    constexpr _not_fn_t(const _not_fn_t &fn) = default;

    template<class... Args>
    constexpr auto operator()(Args &&...args) & noexcept(
        noexcept(!ala::invoke(_fn, ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t &, Args...>>()) {
        return !ala::invoke(_fn, ala::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator()(Args &&...args) const & noexcept(
        noexcept(!ala::invoke(_fn, ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t const &, Args...>>()) {
        return !ala::invoke(_fn, ala::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator()(Args &&...args) && noexcept(
        noexcept(!ala::invoke(ala::move(_fn), ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t, Args...>>()) {
        return !ala::invoke(ala::move(_fn), ala::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator()(Args &&...args) const && noexcept(
        noexcept(!ala::invoke(ala::move(_fn), ala::forward<Args>(args)...)))
        -> decltype(!declval<invoke_result_t<_fn_t const, Args...>>()) {
        return !ala::invoke(ala::move(_fn), ala::forward<Args>(args)...);
    }
};

template<class Fn>
constexpr _not_fn_t<Fn> not_fn(Fn &&fn) {
    return _not_fn_t<Fn>(ala::forward<Fn>(fn));
}

template<int N>
struct _placeholder_t {};

namespace placeholders {

ALA_INLINE_CONSTEXPR_V _placeholder_t<1> _1;
ALA_INLINE_CONSTEXPR_V _placeholder_t<2> _2;
ALA_INLINE_CONSTEXPR_V _placeholder_t<3> _3;
ALA_INLINE_CONSTEXPR_V _placeholder_t<4> _4;
ALA_INLINE_CONSTEXPR_V _placeholder_t<5> _5;
ALA_INLINE_CONSTEXPR_V _placeholder_t<6> _6;
ALA_INLINE_CONSTEXPR_V _placeholder_t<7> _7;
ALA_INLINE_CONSTEXPR_V _placeholder_t<8> _8;
ALA_INLINE_CONSTEXPR_V _placeholder_t<9> _9;
ALA_INLINE_CONSTEXPR_V _placeholder_t<10> _10;

} // namespace placeholders

template<class T>
struct _is_placeholder_helper: integral_constant<int, 0> {};

template<int N>
struct _is_placeholder_helper<_placeholder_t<N>>: integral_constant<int, N> {};

template<class T>
struct is_placeholder: _is_placeholder_helper<remove_cv_t<T>> {};

template<class R, class Fn, class... Args>
struct _bind_t;

template<class T>
struct _is_bind_expression_helper: false_type {};

template<class... Ts>
struct _is_bind_expression_helper<_bind_t<Ts...>>: true_type {};

template<class T>
struct is_bind_expression: _is_bind_expression_helper<remove_cv_t<T>> {};

template<class T>
ALA_INLINE_CONSTEXPR_V int is_placeholder_v = is_placeholder<T>::value;

template<class T>
ALA_INLINE_CONSTEXPR_V bool is_bind_expression_v = is_bind_expression<T>::value;

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
    using _tuple_t = tuple<decay_t<Fn>, decay_t<Args>...>;
    _tuple_t _tuple;

    template<class Tuple, class T, class T1 = remove_reference_t<T>,
             class = enable_if_t<!is_bind_expression<T1>::value &&
                                 !is_placeholder<T1>::value &&
                                 !is_reference_wrapper<T1>::value>>
    T &&_select(Tuple &&, T &&val) const {
        return ala::forward<T>(val);
    }

    template<class Tuple, class Bind,
             class = enable_if_t<is_bind_expression<remove_reference_t<Bind>>::value>>
    auto _select(Tuple &&tp, Bind &&inner_bind) const
        -> decltype(ala::forward<Bind>(inner_bind)._call(ala::forward<Tuple>(tp))) {
        return ala::forward<Bind>(inner_bind)._call(ala::forward<Tuple>(tp));
    }

    template<class Tuple, int N>
    auto &&_select(Tuple &&tp, _placeholder_t<N>) const {
        static_assert(N > 0 && N <= tuple_size<remove_cvref_t<Tuple>>::value,
                      "Illegal index");
        return ala::get<N - 1>(ala::forward<Tuple>(tp));
    }

    template<class Tuple, class T>
    T &_select(Tuple &&tp, reference_wrapper<T> refwarp) const {
        return refwarp.get();
    }

    template<class Tuple, class Bind>
    using _select_bind_t = typename Bind::template _call_result_t<Tuple>;

    template<class Tuple, class T, class T1 = remove_reference_t<T>>
    using _select_t = conditional_t<
        !is_bind_expression<T1>::value && !is_placeholder<T1>::value &&
            !is_reference_wrapper<T1>::value,
        T &&,
        conditional_t<is_bind_expression<T1>::value, _select_bind_t<Tuple, T1>,
                      conditional_t<is_placeholder<T1>::value, tuple_element_t<T1::value, Tuple>,
                                    conditional_t<is_reference_wrapper<T1>::value,
                                                  typename T1::type &, void>>>>;

    template<class Tuple, size_t... N>
    R _call_helper(false_type, Tuple &&tp, index_sequence<N...>) const {
        return static_cast<R>(
            ala::invoke(ala::get<0>(_tuple),
                        this->_select(ala::forward<Tuple>(tp),
                                      ala::get<N + 1>(_tuple))...));
    }

    template<class Tuple, size_t... N>
    R _call_helper(false_type, Tuple &&tp, index_sequence<N...>) {
        return static_cast<R>(
            ala::invoke(ala::get<0>(_tuple),
                        this->_select(ala::forward<Tuple>(tp),
                                      ala::get<N + 1>(_tuple))...));
    }

    template<class Tuple, size_t... N>
    auto _call_helper(true_type, Tuple &&tp, index_sequence<N...>) const
        -> decltype(ala::invoke(ala::get<0>(_tuple),
                                this->_select(ala::forward<Tuple>(tp),
                                              ala::get<N + 1>(_tuple))...)) {
        return ala::invoke(ala::get<0>(_tuple),
                           this->_select(ala::forward<Tuple>(tp),
                                         ala::get<N + 1>(_tuple))...);
    }

    template<class Tuple, size_t... N>
    auto _call_helper(true_type, Tuple &&tp, index_sequence<N...>)
        -> decltype(ala::invoke(ala::get<0>(_tuple),
                                this->_select(ala::forward<Tuple>(tp),
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

    _bind_t(const Fn &fn, const Args &...args): _tuple(fn, args...) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value>>
    _bind_t(const _bind_t &other): _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value>>
    _bind_t(_bind_t &&other): _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    auto operator()(Args1 &&...args) const -> decltype(this->_call(
        ala::forward_as_tuple(ala::forward<Args1>(args)...))) {
        return this->_call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }

    template<class... Args1>
    auto operator()(Args1 &&...args) -> decltype(this->_call(
        ala::forward_as_tuple(ala::forward<Args1>(args)...))) {
        return this->_call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }
};

enum class FunctionOP { Size, Copy, Move, Destroy, Local, Invoke, TypeInfo };

template<class Function, class Functor>
struct _function_handle;

template<class R, class... Args, class Functor>
struct _function_handle<R(Args...), Functor> {
    static size_t size() {
        return sizeof(Functor);
    }

    static void copy(void *dst, const void *src) {
        ::new (dst) Functor(*reinterpret_cast<const Functor *>(src));
    }

    static void move(void *dst, void *src) {
        ::new (dst) Functor(ala::move(*reinterpret_cast<Functor *>(src)));
    }

    static void destroy(void *dst) {
        (*reinterpret_cast<Functor *>(dst)).~Functor();
    }

#if ALA_USE_RTTI
    static const type_info &typeinfo() {
        return typeid(Functor);
    }
#endif

    static R invoke(void *f, Args &&...args) {
        return static_cast<R>(ala::invoke(*reinterpret_cast<Functor *>(f),
                                          ala::forward<Args>(args)...));
    }

    static bool local() {
        return sizeof(Functor) <= 2 * sizeof(size_t) &&
               is_nothrow_move_constructible<Functor>::value;
    }

    static void **get_vtable() {
        static void *vtable[] = {
            reinterpret_cast<void *>(&_function_handle::size),
            reinterpret_cast<void *>(&_function_handle::copy),
            reinterpret_cast<void *>(&_function_handle::move),
            reinterpret_cast<void *>(&_function_handle::destroy),
            reinterpret_cast<void *>(&_function_handle::local),
            reinterpret_cast<void *>(&_function_handle::invoke),
#if ALA_USE_RTTI
            reinterpret_cast<void *>(&_function_handle::typeinfo),
#else
            nullptr,
#endif
        };
        return vtable;
    };

    // clang-format off
    template<FunctionOP op>
    using _ft_table = type_pack_element_t<
        size_t(op),
        decltype(&_function_handle::size),
        decltype(&_function_handle::copy),
        decltype(&_function_handle::move),
        decltype(&_function_handle::destroy),
        decltype(&_function_handle::local),
        decltype(&_function_handle::invoke),
#if ALA_USE_RTTI
        decltype(&_function_handle::typeinfo)
#else
        nullptr_t
#endif
        >;
    // clang-format on
};

struct bad_function_call: exception {
    explicit bad_function_call() {}
};

template<class>
struct function;

template<class R, class... Args>
struct function<R(Args...)>: _function_base<R> {
private:
    static_assert(sizeof(void *) == sizeof(size_t), "Unsupported platform");

    using _buf_t = aligned_storage_t<sizeof(void *) * 2>;

    void **_vptr = nullptr;

    union {
        void *_a_p = 0;
        _buf_t _buf;
    };

    const void *_address() const noexcept {
        if (is_local())
            return reinterpret_cast<const void *>(&_buf);
        return _a_p;
    }

    void *_address() noexcept {
        if (is_local())
            return reinterpret_cast<void *>(&_buf);
        return _a_p;
    }

    void *&_addref() noexcept {
        assert(!is_local());
        return _a_p;
    }

    template<FunctionOP op>
    typename _function_handle<R(Args...), int>::template _ft_table<op>
    _any_call() const noexcept {
        return reinterpret_cast<
            typename _function_handle<R(Args...), int>::template _ft_table<op>>(
            _vptr[static_cast<size_t>(op)]);
    }

    bool is_local() const noexcept {
        if (_vptr == nullptr)
            return true;
        return _any_call<FunctionOP::Local>()();
    }

    void *_alloc(size_t sz) {
        void *p = ::operator new(sz);
        this->_addref() = p;
        return p;
    }

    void _copy(const function &other) {
        if (other) {
            _vptr = other._vptr;
            if (other.is_local()) {
                _any_call<FunctionOP::Copy>()(this->_address(), other._address());
            } else {
                this->_alloc(other._any_call<FunctionOP::Size>()());
                _any_call<FunctionOP::Copy>()(this->_address(), other._address());
            }
        }
    }

    void _move(function &&other) {
        if (other) {
            _vptr = other._vptr;
            if (other.is_local()) {
                _any_call<FunctionOP::Move>()(this->_address(), other._address());
                // other.reset();
            } else {
                _addref() = other._addref();
                other._addref() = nullptr;
                other._vptr = nullptr;
            }
        }
    }

    template<class T>
    struct _is_ala_function: false_type {};

    template<class R1, class... Args1>
    struct _is_ala_function<ala::function<R1(Args1...)>>: true_type {};

    template<class Fn>
    enable_if_t<_is_ala_function<Fn>::value, bool> is_empty_fn(const Fn &fn) {
        return !fn;
    }

    template<class Fn>
    enable_if_t<is_pointer<Fn>::value || is_member_pointer<Fn>::value, bool>
    is_empty_fn(const Fn &fn) {
        return fn == nullptr;
    }

    template<class Fn>
    enable_if_t<!_is_ala_function<Fn>::value &&
                    !(is_pointer<Fn>::value || is_member_pointer<Fn>::value),
                bool>
    is_empty_fn(const Fn &fn) {
        return false;
    }

public:
    // construct/copy/destroy:
    function() noexcept {}

    function(nullptr_t) noexcept {}

    function(const function &other) {
        this->_copy(other);
    }

    function(function &&other) noexcept {
        this->_move(ala::move(other));
    }

    template<class Fn, bool = _and_<_not_<is_same<remove_cvref_t<Fn>, function>>,
                                    is_invocable<Fn &, Args...>>::value>
    struct _is_available_fn
        : _or_<is_void<R>, is_convertible<invoke_result_t<Fn &, Args...>, R>,
               is_same<invoke_result_t<Fn &, Args...>, remove_cv_t<R>>> {};

    template<class Fn>
    struct _is_available_fn<Fn, false>: false_type {};

    template<class Fn, class = enable_if_t<_is_available_fn<Fn>::value>>
    function(Fn fn) {
        // static_assert(sizeof(Fn) <= sizeof(placehold_t),
        //               "Functor size is too large, "
        //               "ala::function have no dynamic memory allocation, ");
        // static_assert(is_nothrow_move_constructible<Fn>::value,
        //               "ala::function use small-objects optimization, "
        //               "Fn must be nothrow_move_constructible.");
        if (this->template is_empty_fn<Fn>(fn))
            return;
        using handle_t = _function_handle<R(Args...), Fn>;
        _vptr = handle_t::get_vtable();
        if (sizeof(fn) > sizeof(_buf) || !is_nothrow_move_constructible<Fn>::value)
            this->_alloc(sizeof(Fn));
        void *src = reinterpret_cast<void *>(ala::addressof(fn));
        _any_call<FunctionOP::Move>()(this->_address(), src);
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
        reset();
        return *this;
    }

    template<class Fn>
    enable_if_t<_is_available_fn<Fn>::value, function &> operator=(Fn &&fn) {
        function(ala::forward<Fn>(fn)).swap(*this);
        return *this;
    }

    template<class Fn>
    function &operator=(reference_wrapper<Fn> fn) noexcept {
        function(fn).swap(*this);
        return *this;
    }

    ~function() {
        reset();
    }

    void swap(function &other) noexcept {
        function tmp(ala::move(*this));
        this->_move(ala::move(other));
        other._move(ala::move(tmp));
    }

    explicit operator bool() const noexcept {
        return _vptr != nullptr;
    }

    R operator()(Args... args) const {
        if (!(bool)*this)
            throw bad_function_call();
        return _any_call<FunctionOP::Invoke>()(
            const_cast<function *>(this)->_address(),
            ala::forward<Args>(args)...);
    }

    void reset() {
        if (*this)
            _any_call<FunctionOP::Destroy>()(this->_address());
        if (!is_local()) {
            void *p = this->_address();
            ::operator delete(p);
        }
        _vptr = nullptr;
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
            return _any_call<FunctionOP::TypeInfo>()();
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
template<class R, class... Args> struct _function_traits<R(Args...)>                        { using type = R(Args...);      static constexpr unsigned qualify =    0b0; };
template<class R, class... Args> struct _function_traits<R(Args...) const>                  { using type = R(Args...);      static constexpr unsigned qualify =   0b01; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile>               { using type = R(Args...);      static constexpr unsigned qualify =   0b10; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile>         { using type = R(Args...);      static constexpr unsigned qualify =   0b11; };
template<class R, class... Args> struct _function_traits<R(Args...) &>                      { using type = R(Args...);      static constexpr unsigned qualify =  0b100; };
template<class R, class... Args> struct _function_traits<R(Args...) const &>                { using type = R(Args...);      static constexpr unsigned qualify =  0b101; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile &>             { using type = R(Args...);      static constexpr unsigned qualify =  0b110; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile &>       { using type = R(Args...);      static constexpr unsigned qualify =  0b111; };
template<class R, class... Args> struct _function_traits<R(Args...) &&>                     { using type = R(Args...);      static constexpr unsigned qualify = 0b1000; };
template<class R, class... Args> struct _function_traits<R(Args...) const &&>               { using type = R(Args...);      static constexpr unsigned qualify = 0b1001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile &&>            { using type = R(Args...);      static constexpr unsigned qualify = 0b1010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile &&>      { using type = R(Args...);      static constexpr unsigned qualify = 0b1011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...)>                   { using type = R(Args..., ...); static constexpr unsigned qualify =    0b0; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const>             { using type = R(Args..., ...); static constexpr unsigned qualify =   0b01; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile>          { using type = R(Args..., ...); static constexpr unsigned qualify =   0b10; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile>    { using type = R(Args..., ...); static constexpr unsigned qualify =   0b11; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) &>                 { using type = R(Args..., ...); static constexpr unsigned qualify =  0b100; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const &>           { using type = R(Args..., ...); static constexpr unsigned qualify =  0b101; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile &>        { using type = R(Args..., ...); static constexpr unsigned qualify =  0b110; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile &>  { using type = R(Args..., ...); static constexpr unsigned qualify =  0b111; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) &&>                { using type = R(Args..., ...); static constexpr unsigned qualify = 0b1000; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const &&>          { using type = R(Args..., ...); static constexpr unsigned qualify = 0b1001; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile &&>       { using type = R(Args..., ...); static constexpr unsigned qualify = 0b1010; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile &&> { using type = R(Args..., ...); static constexpr unsigned qualify = 0b1011; };
#if _ALA_ENABLE_NOEXCEPT_TYPE
template<class R, class... Args> struct _function_traits<R(Args...) noexcept>                        { using type = R(Args...);      static constexpr unsigned qualify = 0b10000; };
template<class R, class... Args> struct _function_traits<R(Args...) const noexcept>                  { using type = R(Args...);      static constexpr unsigned qualify = 0b10001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile noexcept>               { using type = R(Args...);      static constexpr unsigned qualify = 0b10010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile noexcept>         { using type = R(Args...);      static constexpr unsigned qualify = 0b10011; };
template<class R, class... Args> struct _function_traits<R(Args...) & noexcept>                      { using type = R(Args...);      static constexpr unsigned qualify = 0b10100; };
template<class R, class... Args> struct _function_traits<R(Args...) const & noexcept>                { using type = R(Args...);      static constexpr unsigned qualify = 0b10101; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile & noexcept>             { using type = R(Args...);      static constexpr unsigned qualify = 0b10110; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile & noexcept>       { using type = R(Args...);      static constexpr unsigned qualify = 0b10111; };
template<class R, class... Args> struct _function_traits<R(Args...) && noexcept>                     { using type = R(Args...);      static constexpr unsigned qualify = 0b11000; };
template<class R, class... Args> struct _function_traits<R(Args...) const && noexcept>               { using type = R(Args...);      static constexpr unsigned qualify = 0b11001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile && noexcept>            { using type = R(Args...);      static constexpr unsigned qualify = 0b11010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile && noexcept>      { using type = R(Args...);      static constexpr unsigned qualify = 0b11011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) noexcept>                   { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10000; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const noexcept>             { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10001; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile noexcept>          { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10010; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile noexcept>    { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) & noexcept>                 { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10100; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const & noexcept>           { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10101; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile & noexcept>        { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10110; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile & noexcept>  { using type = R(Args..., ...); static constexpr unsigned qualify = 0b10111; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) && noexcept>                { using type = R(Args..., ...); static constexpr unsigned qualify = 0b11000; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const && noexcept>          { using type = R(Args..., ...); static constexpr unsigned qualify = 0b11001; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile && noexcept>       { using type = R(Args..., ...); static constexpr unsigned qualify = 0b11010; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile && noexcept> { using type = R(Args..., ...); static constexpr unsigned qualify = 0b11011; };
#endif
// clang-format on

template<class Fn>
struct _function_traits_raw {};

template<class R, class... Args>
struct _function_traits_raw<R(Args...)> {
    using type = R(Args...);
    using result_type = R;
    static constexpr size_t args_count = sizeof...(Args);
};

template<class R, class... Args>
struct _function_traits_raw<R(Args..., ...)> {
    using type = R(Args..., ...);
    using result_type = R;
    static constexpr size_t args_count = size_t(-1);
};

template<class Fn, class = void>
struct _function_helper: _function_traits_raw<Fn> {};

template<class Fn>
struct _function_helper<Fn, void_t<typename _function_traits<Fn>::type>> {
    using type = typename _function_traits<Fn>::type;
    using result_type = typename _function_traits_raw<type>::result_type;
    static constexpr size_t args_count = _function_traits_raw<type>::args_count;
};

template<class MemPtr>
struct _memptr_traits {};

template<class Class, class Mem>
struct _memptr_traits<Mem Class::*> {
    using _cls_t = Class;
    using _mem_t = Mem;
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
    using _mem_t = typename _memptr_traits<decltype(&Fn::operator())>::_mem_t;
    static constexpr unsigned _qualify = _function_traits<_mem_t>::qualify;
    static_assert((_qualify & 0b1000U) == 0,
                  "ala::function can not bind to &&-qualify function object");
    using type = typename _function_helper<_mem_t>::type;
    using result_type = typename _function_helper<type>::result_type;
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
auto bind(Fn &&fn, Args &&...args)
    -> _bind_t<typename callable_traits<Fn>::result_type, Fn, Args...> {
    using R = typename callable_traits<Fn>::result_type;
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(fn),
                                   ala::forward<Args>(args)...);
}

template<class R, class Fn, class... Args>
auto bind(Fn &&fn, Args &&...args) -> _bind_t<R, Fn, Args...> {
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(fn),
                                   ala::forward<Args>(args)...);
}

// C++20

template<class Fn, class... Args>
struct _bind_front_t {
    using _tuple_t = tuple<decay_t<Fn>, decay_t<Args>...>;
    // using _tuple_t = tuple<Fn &&, Args &&...>;
    _tuple_t _tuple;

    template<size_t... N, class... Args1>
    constexpr auto _call(index_sequence<N...>, Args1 &&...args1)
        const & -> decltype(ala::invoke(ala::get<0>(_tuple),
                                        ala::get<N + 1>(_tuple)...,
                                        ala::forward<Args1>(args1)...)) {
        return ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                           ala::forward<Args1>(args1)...);
    }

    template<size_t... N, class... Args1>
    constexpr auto _call(index_sequence<N...>, Args1 &&...args1)
        & -> decltype(ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                                  ala::forward<Args1>(args1)...)) {
        return ala::invoke(ala::get<0>(_tuple), ala::get<N + 1>(_tuple)...,
                           ala::forward<Args1>(args1)...);
    }

    template<size_t... N, class... Args1>
    constexpr auto _call(index_sequence<N...>, Args1 &&...args1)
        const && -> decltype(ala::invoke(ala::get<0>(ala::move(_tuple)),
                                         ala::get<N + 1>(ala::move(_tuple))...,
                                         ala::forward<Args1>(args1)...)) {
        return ala::invoke(ala::get<0>(ala::move(_tuple)),
                           ala::get<N + 1>(ala::move(_tuple))...,
                           ala::forward<Args1>(args1)...);
    }

    template<size_t... N, class... Args1>
    constexpr auto
    _call(index_sequence<N...>, Args1 &&...args1) && -> decltype(ala::invoke(
        ala::get<0>(ala::move(_tuple)), ala::get<N + 1>(ala::move(_tuple))...,
        ala::forward<Args1>(args1)...)) {
        return ala::invoke(ala::get<0>(ala::move(_tuple)),
                           ala::get<N + 1>(ala::move(_tuple))...,
                           ala::forward<Args1>(args1)...);
    }

    constexpr _bind_front_t(Fn &&fn, Args &&...args)
        : _tuple(ala::forward<Fn>(fn), ala::forward<Args>(args)...) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value>>
    constexpr _bind_front_t(const _bind_front_t &other): _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value>>
    constexpr _bind_front_t(_bind_front_t &&other)
        : _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    constexpr auto operator()(Args1 &&...args1) const & -> decltype(this->_call(
        index_sequence_for<Args...>(), ala::forward<Args1>(args1)...)) {
        return this->_call(index_sequence_for<Args...>(),
                           ala::forward<Args1>(args1)...);
    }

    template<class... Args1>
    constexpr auto operator()(Args1 &&...args1) & -> decltype(this->_call(
        index_sequence_for<Args...>(), ala::forward<Args1>(args1)...)) {
        return this->_call(index_sequence_for<Args...>(),
                           ala::forward<Args1>(args1)...);
    }

    template<class... Args1>
    constexpr auto
    operator()(Args1 &&...args1) const && -> decltype(ala::move(*this)._call(
        index_sequence_for<Args...>(), ala::forward<Args1>(args1)...)) {
        return ala::move(*this)._call(index_sequence_for<Args...>(),
                                      ala::forward<Args1>(args1)...);
    }

    template<class... Args1>
    constexpr auto
    operator()(Args1 &&...args1) && -> decltype(ala::move(*this)._call(
        index_sequence_for<Args...>(), ala::forward<Args1>(args1)...)) {
        return ala::move(*this)._call(index_sequence_for<Args...>(),
                                      ala::forward<Args1>(args1)...);
    }
};

template<class Fn, class... Args>
constexpr enable_if_t<
    _and_<is_constructible<decay_t<Fn>, Fn>, is_move_constructible<decay_t<Fn>>,
          is_constructible<decay_t<Args>, Args>...,
          is_move_constructible<decay_t<Args>>...>::value,
    _bind_front_t<Fn, Args...>>
bind_front(Fn &&fn, Args &&...args) {
    // static_assert(sizeof...(Args) <= callable_traits<Fn>::args_count,
    //               "Arguments count too many");
    return _bind_front_t<Fn, Args...>(ala::forward<Fn>(fn),
                                      ala::forward<Args>(args)...);
}

template<class MemPtr>
constexpr auto mem_fn(MemPtr pm) noexcept {
    // return ala::_mem_fn_t<MemPtr>(pm);
    return ala::_bind_front_t<MemPtr &>(pm);
}

template<class ForwardIter, class BinPred = equal_to<>>
class default_searcher {
public:
    constexpr default_searcher(ForwardIter pat_first, ForwardIter pat_last,
                               BinPred pred = BinPred())
        : _pat_first(pat_first), _pat_last(pat_last), _pred(pred) {}

    template<class ForwardIter1>
    constexpr pair<ForwardIter1, ForwardIter1>
    operator()(ForwardIter1 first, ForwardIter1 last) const {
        ForwardIter1 loc = ala::search(first, last, _pat_first, _pat_last, _pred);
        ForwardIter1 lst = loc;
        if (lst != last)
            ala::advance(lst, ala::distance(_pat_first, _pat_last));
        return pair<ForwardIter1, ForwardIter1>(loc, lst);
    }

private:
    ForwardIter _pat_first;
    ForwardIter _pat_last;
    BinPred _pred;
};

} // namespace ala
#endif