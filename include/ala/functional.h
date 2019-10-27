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
using std::type_info;
#endif

struct identity {
    using is_transparent = int;
    template<class T>
    constexpr T &&operator()(T &&t) const noexcept {
        return ala::forward<T>(t);
    }
};

template<class T>
struct reference_wrapper {
    using type = T;

    static void _test(T &t) noexcept;
    static void _test(T &&) = delete;

    template<class U, class = decltype(_test(declval<U>())),
             class = enable_if_t<!is_same<remove_cvref_t<U>, reference_wrapper>::value>>
    reference_wrapper(U &&u) noexcept(noexcept(_test(declval<U>())))
        : _ptr(ala::addressof(ala::forward<U>(u))) {}
    reference_wrapper(const reference_wrapper &) noexcept = default;
    reference_wrapper &operator=(const reference_wrapper &) noexcept = default;
    operator T &() const noexcept {
        return *_ptr;
    }

    T &get() const noexcept {
        return *_ptr;
    }

    template<class... Args>
    enable_if_t<is_invocable<T &, Args...>::value, invoke_result_t<T &, Args...>>
    operator()(Args &&... args) const {
        return ala::invoke(get(), ala::forward<Args>(args)...);
    }

    T *_ptr;
};

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class T>
reference_wrapper(T &)->reference_wrapper<T>;
#endif

template<class T>
reference_wrapper<T> ref(T &t) noexcept {
    return reference_wrapper<T>(t);
}

template<class T>
reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
    return ref(t.get());
}

template<class T>
void ref(const T &&) = delete;

template<class T>
reference_wrapper<const T> cref(const T &t) noexcept {
    return reference_wrapper<const T>(t);
}

template<class T>
reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept {
    return cref(t.get());
}

template<class T>
void cref(const T &&) = delete;

template<class Fn>
struct _not_fn_t {
    typedef decay_t<Fn> _fn_t;
    _fn_t _fn;

    explicit _not_fn_t(Fn &&fn): _fn(forward<Fn>(fn)) {}
    _not_fn_t(_not_fn_t &&fn) = default;
    _not_fn_t(const _not_fn_t &fn) = default;

    template<class... Args>
    auto operator()(Args &&... args) & -> decltype(
        !declval<invoke_result_t<_fn_t &, Args...>>()) {
        return !invoke(_fn, forward<Args>(args)...);
    }

    template<class... Args>
    auto operator()(Args &&... args) const & -> decltype(
        !declval<invoke_result_t<_fn_t const &, Args...>>()) {
        return !invoke(_fn, forward<Args>(args)...);
    }

    template<class... Args>
    auto operator()(Args &&... args) && -> decltype(
        !declval<invoke_result_t<_fn_t, Args...>>()) {
        return !invoke(ala::move(_fn), forward<Args>(args)...);
    }

    template<class... Args>
    auto operator()(Args &&... args) const && -> decltype(
        !declval<invoke_result_t<_fn_t const, Args...>>()) {
        return !invoke(ala::move(_fn), forward<Args>(args)...);
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

template<class R, class Fn, class... Args>
struct _bind_t {
    typedef R result_type;
    typedef decay_t<Fn> _fn_t;
    typedef tuple<decay_t<Args>...> _tuple_t;
    _fn_t _fn;
    _tuple_t _tuple;

    template<class Tuple, class T,
             class = enable_if_t<!is_bind_expression<remove_cvref_t<T>>::value>>
    T &&_select(Tuple &&, T &&val) {
        return ala::forward<T>(val);
    }

    template<class Tuple, class Bind, class = void,
             class = enable_if_t<is_bind_expression<remove_cvref_t<Bind>>::value>>
    auto _select(Tuple &&tp, Bind &&inner_bind)
        -> decltype(ala::forward<Bind>(inner_bind)._call(ala::forward<Tuple>(tp))) {
        return ala::forward<Bind>(inner_bind)._call(ala::forward<Tuple>(tp));
    }

    template<class Tuple, int N>
    auto _select(Tuple &&tp, _placeholder_t<N>)
        -> decltype(ala::get<N - 1>(ala::forward<Tuple>(tp))) {
        static_assert(N > 0 && N <= tuple_size<remove_cvref_t<Tuple>>::value,
                      "illegal index");
        return ala::get<N - 1>(ala::forward<Tuple>(tp));
    }

    template<class Tuple, class T>
    auto _select(Tuple &&tp, reference_wrapper<T> refwarp)
        -> decltype(refwarp.get()) {
        return refwarp.get();
    }

    template<class Tuple, size_t... N>
    auto _call_helper(Tuple &&tp, index_sequence<N...>)
        -> decltype(ala::invoke(_fn, _select(ala::forward<Tuple>(tp),
                                             ala::get<N>(_tuple))...)) {
        return ala::invoke(_fn, _select(ala::forward<Tuple>(tp),
                                        ala::get<N>(_tuple))...);
    }

    template<class Tuple>
    auto _call(Tuple &&tp)
        -> decltype(this->_call_helper(ala::forward<Tuple>(tp),
                                       ala::index_sequence_for<Args...>())) {
        return this->_call_helper(ala::forward<Tuple>(tp),
                                  ala::index_sequence_for<Args...>());
    }

    _bind_t(Fn &&fn, Args &&... args)
        : _fn(ala::forward<Fn>(fn)), _tuple(ala::forward<Args>(args)...) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value &&
                                 is_copy_constructible<_fn_t>::value>>
    _bind_t(const _bind_t &other): _fn(other._fn), _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value &&
                                 is_move_constructible<_fn_t>::value>>
    _bind_t(_bind_t &&other)
        : _fn(ala::move(other._fn)), _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    result_type operator()(Args1 &&... args) {
        return _call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }
};

enum Operation { Copy, Move, Destroy, TypeID };

template<class Function, class Functor>
struct _function_handle;

template<class R, class... Args, class Functor>
struct _function_handle<R(Args...), Functor> {
    static R invoke(const void *Fmem, Args &&... args) {
        return ala::invoke(*(Functor *)Fmem, ala::forward<Args>(args)...);
    }

    static void *operate(void *dst, void *src, Operation op) {
        Functor &dstRef = *(Functor *)dst;
        Functor &srcRef = *(Functor *)src;
        switch (op) {
            case Copy:
                ::new (dst) Functor(srcRef);
                break;
            case Move:
                ::new (dst) Functor(ala::move(srcRef));
                break;
            case Destroy:
                dstRef.~Functor();
                break;
            case TypeID:
                return (void *)&typeid(Functor);
        }
        return nullptr;
    }
    static constexpr void *handle[2] = {&invoke, &operate};
};

struct bad_function_call: exception {
    explicit bad_function_call() {}
};

template<class>
struct function;

template<class R, class... Args>
struct function<R(Args...)> {
    using result_type = R;
    // construct/copy/destroy:
    function() noexcept {}

    function(nullptr_t) noexcept {}

    function(const function &other)
        : _invoker(other._invoker), _operator(other._operator) {
        if (other._valid) {
            _operator((void *)&_data, (void *)&(other._data), Copy);
            _valid = true;
        }
    }

    function(function &&other)
        : _invoker(other._invoker), _operator(other._operator) {
        if (other._valid) {
            _operator((void *)&_data, (void *)&(other._data), Move);
            _valid = true;
        }
    }

    template<class Fn, class = enable_if_t<is_invocable_r<R, Fn &, Args...>::value>>
    function(Fn fn) {
        static_assert(sizeof(Fn) <= sizeof(_data),
                      "functor size is too large, "
                      "ala::function have no dynamic memory allocation, "
                      "set ALA_FUNCTION_MEMORY_SIZE to sizeof(largest-class).");
        static_assert(is_nothrow_move_constructible<Fn>::value,
                      "ala::function use small-objects optimization, "
                      "Fn must be nothrow_move_constructible.");
        typedef _function_handle<R(Args...), Fn> handle_t;
        _invoker = &handle_t::invoke;
        _operator = &handle_t::operate;
        _operator((void *)&_data, (void *)&fn, Move);
        _valid = true;
    }

    function &operator=(const function &other) {
        _valid = false;
        function(other).swap(*this);
        return *this;
    }

    function &operator=(function &&other) {
        _valid = false;
        function(ala::move(other)).swap(*this);
        return *this;
    }

    function &operator=(nullptr_t) noexcept {
        if (_valid)
            _operator((void *)&_data, nullptr, Destroy);
        _valid = false;
    }

    template<class Fn>
    enable_if_t<is_invocable_r<R, Fn, Args...>::value, function> &
    operator=(Fn &&fn) {
        _valid = false;
        function(ala::forward<Fn>(fn)).swap(*this);
        return *this;
    }

    template<class Fn>
    function &operator=(reference_wrapper<Fn>) noexcept;
    ~function() {
        if (_valid)
            _operator((void *)&_data, nullptr, Destroy);
    }

    void swap(function &other) noexcept {
        aligned_storage_t<sizeof(_data), alignof(decltype(_data))> _tmp;
        if (_valid)
            _operator((void *)&_tmp, (void *)&_data, Move);
        if (other._valid)
            other._operator((void *)&_data, (void *)&other._data, Move);
        if (_valid)
            _operator((void *)&other._data, (void *)&_tmp, Move);
        ala::swap(_valid, other._valid);
        ala::swap(_invoker, other._invoker);
        ala::swap(_operator, other._operator);
    }

    explicit operator bool() const noexcept {
        return _valid;
    }

    result_type operator()(Args... args) const {
        if (!_valid)
            throw bad_function_call();
        return _invoker((void *)&_data, ala::forward<Args>(args)...);
    }

#if ALA_USE_RTTI
    template<class T>
    T *target() noexcept {
        if (_valid && typeid(T) == target_type())
            return (T *)&_data;
        return nullptr;
    }

    template<class T>
    const T *target() const noexcept {
        return const_cast<function *>(this)->target<T>();
    }

    const type_info &target_type() const noexcept {
        if (_valid)
            return *(type_info *)_operator(nullptr, nullptr, TypeID);
        return typeid(void);
    }

#endif
    // private:
    aligned_union_t<ALA_FUNCTION_MEMORY_SIZE, void (*)(), void (function::*)()> _data;
    bool _valid = false;
    typedef R (*invoker_t)(const void *, Args &&...);
    typedef void *(*operator_t)(void *, void *, Operation);
    invoker_t _invoker = nullptr;
    operator_t _operator = nullptr;
};

template<class R, class... Args>
bool operator==(const function<R(Args...)> &lhs, const function<R(Args...)> &rhs) {
    return lhs._eq(rhs);
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
struct _function_traits_helper {};

template<class R, class... Args>
struct _function_traits_helper<R(Args...)> {
    typedef R type(Args...);
    typedef R result_type;
    static constexpr size_t args_count = sizeof...(Args);
};

template<class R, class... Args>
struct _function_traits_helper<R(Args..., ...)> {
    typedef R type(Args..., ...);
    typedef R result_type;
    static constexpr size_t args_count = sizeof...(Args);
};

template<class Fn, class = void>
struct _function_helper: _function_traits_helper<Fn> {};

template<class Fn>
struct _function_helper<Fn, void_t<typename _function_traits<Fn>::type>> {
    typedef typename _function_traits<Fn>::type type;
    typedef typename _function_traits_helper<type>::result_type result_type;
    static constexpr size_t args_count = _function_traits_helper<type>::args_count;
};

template<class MemPtr>
struct _memptr_traits {};

template<class Class, class Mem>
struct _memptr_traits<Mem Class::*> {
    typedef Class _class;
    typedef Mem _mem;
};

template<class Class, class = void>
struct _functor_helper {};

template<class Class>
struct _functor_helper<Class, void_t<decltype(&Class::operator())>> {
    typedef typename _memptr_traits<decltype(&Class::operator())>::_mem _fn;
    static constexpr unsigned _qualify = _function_traits<_fn>::qualify;
    static_assert((_qualify & 0b1000u) == 0,
                  "function can not bind to &&-qualify functor");
    typedef typename _function_traits<_fn>::type type;
    typedef typename _function_helper<type>::result_type result_type;
    static constexpr size_t args_count = _function_helper<type>::args_count;
};

template<class Fn, class This>
struct _add_this_param {};

template<class R, class This, class... Args>
struct _add_this_param<R(Args...), This> {
    typedef R type(This, Args...);
};

template<class MemPtr, bool = is_member_object_pointer<MemPtr>::value,
         bool = is_member_function_pointer<MemPtr>::value>
struct _memptr_helper {};

template<class MemPtr>
struct _memptr_helper<MemPtr, true, false> {
    typedef typename _memptr_traits<MemPtr>::_mem _mem;
    typedef typename _memptr_traits<MemPtr>::_class _class;
    typedef _mem type(const _class &);
    typedef typename _function_helper<type>::result_type result_type;
    static constexpr size_t args_count = _function_helper<type>::args_count;
};

template<class MemPtr>
struct _memptr_helper<MemPtr, false, true> {
    typedef typename _memptr_traits<MemPtr>::_mem _mem;
    typedef typename _memptr_traits<MemPtr>::_class _class;
    typedef typename _function_traits<_mem>::type raw;
    static constexpr unsigned qualify = _function_traits<_mem>::qualify;
    typedef conditional_t<(qualify & 0b1u) == 0, _class, add_const_t<_class>> c_type;
    typedef conditional_t<(qualify & 0b10u) == 0, c_type, add_volatile_t<c_type>> cv_type;
    typedef conditional_t<(qualify & 0b1000u) == 0, add_lvalue_reference_t<cv_type>,
                          add_rvalue_reference_t<cv_type>>
        rcv_type;
    typedef typename _add_this_param<raw, rcv_type>::type type;
    typedef typename _function_helper<type>::result_type result_type;
    static constexpr size_t args_count = _function_helper<type>::args_count;
};

template<class Fn, bool = is_class<Fn>::value, bool = is_member_pointer<Fn>::value>
struct _callable_traits: _function_helper<remove_pointer_t<Fn>> {};

template<class Functor>
struct _callable_traits<Functor, true, false>: _functor_helper<Functor> {};

template<class MemPtr>
struct _callable_traits<MemPtr, false, true>
    : _memptr_helper<remove_cv_t<MemPtr>> {};

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
function(Callable)->function<typename callable_traits<Callable>::type>;
#endif

template<class MemPtr>
enable_if_t<is_member_pointer<MemPtr>::value,
            function<typename callable_traits<MemPtr>::type>>
mem_fn(MemPtr pm) noexcept {
    return function<typename _memptr_helper<remove_reference_t<MemPtr>>::type>(pm);
}

template<class Fn, class... Args>
auto bind(Fn &&fn, Args &&... args)
    -> _bind_t<typename callable_traits<Fn>::result_type, Fn, Args...> {
    using _call_traits = callable_traits<Fn>;
    static_assert(sizeof...(Args) == _call_traits::args_count,
                  "bind size not compatible");
    using R = typename _call_traits::result_type;
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(fn),
                                   ala::forward<Args>(args)...);
}

template<class R, class Fn, class... Args>
auto bind(Fn &&fn, Args &&... args) -> _bind_t<R, Fn, Args...> {
    using _call_traits = callable_traits<Fn>;
    static_assert(sizeof...(Args) == _call_traits::args_count,
                  "bind size not compatible");
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(fn),
                                   ala::forward<Args>(args)...);
}

// C++20

template<class Fn, class... Args>
struct _bind_front_t {
    typedef decay_t<Fn> _fn_t;
    typedef tuple<unwrap_ref_decay_t<Args>...> _tuple_t;
    _fn_t _fn;
    _tuple_t _tuple;

    template<size_t... N, class... Args1>
    auto _call(index_sequence<N...>, Args1... args1)
        -> decltype(ala::invoke(_fn, ala::get<N>(_tuple)...,
                                ala::forward<Args1>(args1)...)) {
        return ala::invoke(_fn, ala::get<N>(_tuple)...,
                           ala::forward<Args1>(args1)...);
    }

    _bind_front_t(Fn &&fn, Args &&... args)
        : _fn(ala::forward<Fn>(fn)), _tuple(ala::forward<Args>(args)...) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value &&
                                 is_copy_constructible<_fn_t>::value>>
    _bind_front_t(const _bind_front_t &other)
        : _fn(other._fn), _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value &&
                                 is_move_constructible<_fn_t>::value>>
    _bind_front_t(_bind_front_t &&other)
        : _fn(ala::move(other._fn)), _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    auto operator()(Args1 &&... args1)
        -> decltype(this->_call(index_sequence_for<Args...>(),
                                ala::forward<Args1>(args1)...)) {
        static_assert(sizeof...(Args) + sizeof...(Args1) ==
                          callable_traits<_fn_t>::args_count,
                      "arguments count not compatible");
        return this->_call(index_sequence_for<Args...>(),
                           ala::forward<Args1>(args1)...);
    }
};

template<class Fn, class... Args>
_bind_front_t<Fn, Args...> bind_front(Fn &&fn, Args &&... args) {
    static_assert(sizeof...(Args) <= callable_traits<Fn>::args_count,
                  "arguments count too many");
    return _bind_front_t<Fn, Args...>(ala::forward<Fn>(fn),
                                      ala::forward<Args>(args)...);
}

} // namespace ala
#endif