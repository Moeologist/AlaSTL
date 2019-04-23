#ifndef _ALA_FUNCTIONAL_H
#define _ALA_FUNCTIONAL_H

#include <typeinfo>
#include <ala/detail/functional_base.h>
#include <ala/detail/allocator.h>
#include <ala/tuple.h>

namespace ala {

template<class T>
struct reference_wrapper {
    using type = T;

    static T &_test(T &t) noexcept {
        return t;
    }
    static void _test(T &&) = delete;

    template<class U, class = decltype(_test(declval<U>())),
             class = enable_if_t<!is_same<reference_wrapper, remove_cvref_t<U>>::value>>
    reference_wrapper(U &&u) noexcept(noexcept(_test(ala::forward<U>(u))))
        : _ptr(ala::addressof(_test(ala::forward<U>(u)))) {}
    reference_wrapper(const reference_wrapper &other) noexcept = default;
    reference_wrapper &operator=(const reference_wrapper &x) = default;
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

template<int N>
struct _placeholder_t {};

namespace placeholders {
_placeholder_t<1> _1;
_placeholder_t<2> _2;
_placeholder_t<3> _3;
_placeholder_t<4> _4;
_placeholder_t<5> _5;
_placeholder_t<6> _6;
_placeholder_t<7> _7;
_placeholder_t<8> _8;
_placeholder_t<9> _9;
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
    typedef decay_t<Fn> _f_t;
    typedef tuple<decay_t<Args>...> _tuple_t;
    _f_t _f;
    _tuple_t _tuple;

    template<class Tuple, class T,
             class = enable_if_t<!is_bind_expression<remove_cvref_t<T>>::value>>
    decltype(auto) _select(Tuple &&, T &&val) {
        return ala::forward<T>(val);
    }

    template<class Tuple, class Bind, class = void,
             class = enable_if_t<is_bind_expression<remove_cvref_t<Bind>>::value>>
    decltype(auto) _select(Tuple &&tp, Bind &&inner_bind) {
        return inner_bind._call(ala::forward<Tuple>(tp));
    }

    template<class Tuple, int N>
    decltype(auto) _select(Tuple &&tp, _placeholder_t<N>) {
        static_assert(N > 0 && N <= tuple_size<remove_cvref_t<Tuple>>::value,
                      "illegal index");
        return ala::get<N - 1>(ala::forward<Tuple>(tp));
    }

    template<class Tuple, class T>
    decltype(auto) _select(Tuple &&tp, reference_wrapper<T> refwarp) {
        return refwarp.get();
    }

    template<class Tuple, size_t... N>
    decltype(auto) _call_helper(Tuple &&tp, index_sequence<N...>) {
        return ala::invoke(_f, _select(ala::forward<Tuple>(tp),
                                       ala::get<N>(_tuple))...);
    }

    template<class Tuple>
    decltype(auto) _call(Tuple &&tp) {
        return this->_call_helper(ala::forward<Tuple>(tp),
                                  ala::index_sequence_for<Args...>());
    }

public:
    _bind_t(Fn &&f, Args &&... args)
        : _f(ala::forward<Fn>(f)),
          _tuple(ala::forward_as_tuple(ala::forward<Args>(args)...)) {}

    template<bool Dummy = true,
             class = enable_if_t<Dummy && is_copy_constructible<_tuple_t>::value &&
                                 is_copy_constructible<_f_t>::value>>
    _bind_t(const _bind_t &other): _f(other._f), _tuple(other._tuple) {}

    template<bool Dummy = true, class = void,
             class = enable_if_t<Dummy && is_move_constructible<_tuple_t>::value &&
                                 is_move_constructible<_f_t>::value>>
    _bind_t(_bind_t &&other)
        : _f(ala::move(other._f)), _tuple(ala::move(other._tuple)) {}

    template<class... Args1>
    decltype(auto) operator()(Args1 &&... args) {
        return _call(ala::forward_as_tuple(ala::forward<Args1>(args)...));
    }
};

using std::type_info;

enum Operation { Copy, Move, Destroy, TypeID };

template<class Function, class Functor>
struct _function_handle;

template<class R, class... Args, class Functor>
struct _function_handle<R(Args...), Functor> {
    static R invoke(const void *Fmem, Args &&... args) {
        return ala::invoke(*(Functor *)Fmem, ala::forward<Args>(args)...);
    }
    static void *operate(void *dest, void *src, Operation op) {
        Functor &destRef = *(Functor *)dest;
        Functor &srcRef = *(Functor *)src;
        switch (op) {
            case Copy:
                ::new (dest) Functor(srcRef);
                break;
            case Move:
                ::new (dest) Functor(ala::move(srcRef));
                break;
            case Destroy: {
                destRef.~Functor();
                break;
            }
            case TypeID:
                return (void *)&typeid(Functor);
        }
        return nullptr;
    }
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
    };
    template<class Fn, class = enable_if_t<is_invocable_r<R, Fn &, Args...>::value>>
    function(Fn f) {
        static_assert(sizeof(Fn) <= sizeof(_data),
                      "functor size too large, "
                      "ala::function not use dynamic memory allocation, "
                      "set ALA_FUNCTION_MEMORY_SIZE to sizeof(largest-class)");
        typedef _function_handle<R(Args...), Fn> handle_t;
        _invoker = &handle_t::invoke;
        _operator = &handle_t::operate;
        _operator((void *)&_data, (void *)&f, Move);
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
    operator=(Fn &&f) {
        _valid = false;
        function(ala::forward<Fn>(f)).swap(*this);
        return *this;
    }
    template<class Fn>
    function &operator=(reference_wrapper<Fn>) noexcept;
    ~function() {
        if (_valid)
            _operator((void *)&_data, nullptr, Destroy);
    }
    void swap(function &other) noexcept {
        ala::swap(_data, other._data);
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
private:
    aligned_union_t<ALA_FUNCTION_MEMORY_SIZE, void (*)(), void (function::*)()> _data;
    bool _valid = false;
    typedef R (*invoker_t)(const void *, Args &&...);
    typedef void *(*operator_t)(void *, void *, Operation);
    invoker_t _invoker = nullptr;
    operator_t _operator = nullptr;
};

// clang-format off
template<class> struct _function_traits {};
template<class R, class... Args> struct _function_traits<R(Args...)>                        { typedef R type(Args...); static constexpr unsigned qualify =     0b0; };
template<class R, class... Args> struct _function_traits<R(Args...) const>                  { typedef R type(Args...); static constexpr unsigned qualify =    0b01; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile>               { typedef R type(Args...); static constexpr unsigned qualify =    0b10; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile>         { typedef R type(Args...); static constexpr unsigned qualify =    0b11; };
template<class R, class... Args> struct _function_traits<R(Args...) &>                      { typedef R type(Args...); static constexpr unsigned qualify =   0b100; };
template<class R, class... Args> struct _function_traits<R(Args...) const &>                { typedef R type(Args...); static constexpr unsigned qualify =   0b101; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile &>             { typedef R type(Args...); static constexpr unsigned qualify =   0b110; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile &>       { typedef R type(Args...); static constexpr unsigned qualify =   0b111; };
template<class R, class... Args> struct _function_traits<R(Args...) &&>                     { typedef R type(Args...); static constexpr unsigned qualify =  0b1000; };
template<class R, class... Args> struct _function_traits<R(Args...) const &&>               { typedef R type(Args...); static constexpr unsigned qualify =  0b1001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile &&>            { typedef R type(Args...); static constexpr unsigned qualify =  0b1010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile &&>      { typedef R type(Args...); static constexpr unsigned qualify =  0b1011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...)>                   { typedef R type(Args..., ...); static constexpr unsigned qualify =     0b0; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const>             { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b01; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile>          { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b10; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile>    { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b11; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) &>                 { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b100; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const &>           { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b101; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile &>        { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b110; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile &>  { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b111; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) &&>                { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1000; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const &&>          { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1001; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile &&>       { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1010; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile &&> { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1011; typedef void is_va; };
#if _ALA_ENABLE_NOEXCEPT_TYPE
template<class R, class... Args> struct _function_traits<R(Args...) noexcept>                        { typedef R type(Args...); static constexpr unsigned qualify =     0b0; };
template<class R, class... Args> struct _function_traits<R(Args...) const noexcept>                  { typedef R type(Args...); static constexpr unsigned qualify =    0b01; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile noexcept>               { typedef R type(Args...); static constexpr unsigned qualify =    0b10; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile noexcept>         { typedef R type(Args...); static constexpr unsigned qualify =    0b11; };
template<class R, class... Args> struct _function_traits<R(Args...) & noexcept>                      { typedef R type(Args...); static constexpr unsigned qualify =   0b100; };
template<class R, class... Args> struct _function_traits<R(Args...) const & noexcept>                { typedef R type(Args...); static constexpr unsigned qualify =   0b101; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile & noexcept>             { typedef R type(Args...); static constexpr unsigned qualify =   0b110; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile & noexcept>       { typedef R type(Args...); static constexpr unsigned qualify =   0b111; };
template<class R, class... Args> struct _function_traits<R(Args...) && noexcept>                     { typedef R type(Args...); static constexpr unsigned qualify =  0b1000; };
template<class R, class... Args> struct _function_traits<R(Args...) const && noexcept>               { typedef R type(Args...); static constexpr unsigned qualify =  0b1001; };
template<class R, class... Args> struct _function_traits<R(Args...) volatile && noexcept>            { typedef R type(Args...); static constexpr unsigned qualify =  0b1010; };
template<class R, class... Args> struct _function_traits<R(Args...) const volatile && noexcept>      { typedef R type(Args...); static constexpr unsigned qualify =  0b1011; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) noexcept>                   { typedef R type(Args..., ...); static constexpr unsigned qualify =     0b0; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const noexcept>             { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b01; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile noexcept>          { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b10; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile noexcept>    { typedef R type(Args..., ...); static constexpr unsigned qualify =    0b11; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) & noexcept>                 { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b100; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const & noexcept>           { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b101; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile & noexcept>        { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b110; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile & noexcept>  { typedef R type(Args..., ...); static constexpr unsigned qualify =   0b111; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) && noexcept>                { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1000; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const && noexcept>          { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1001; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) volatile && noexcept>       { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1010; typedef void is_va; };
template<class R, class... Args> struct _function_traits<R(Args..., ...) const volatile && noexcept> { typedef R type(Args..., ...); static constexpr unsigned qualify =  0b1011; typedef void is_va; };
#endif
// clang-format on

template<class Fn>
struct _function_traits_helper {};

template<class R, class... Args>
struct _function_traits_helper<R(Args...)> {
    typedef R type(Args...);
    typedef R result_type;
    static constexpr size_t arg_num = sizeof...(Args);
};

template<class Fn, class = void>
struct _function_helper: _function_traits_helper<Fn> {};

template<class Fn>
struct _function_helper<Fn, void_t<typename _function_traits<Fn>::type>> {
    typedef typename _function_traits<Fn>::type type;
    typedef typename _function_traits_helper<type>::result_type result_type;
    static constexpr size_t arg_num = _function_traits_helper<type>::arg_num;
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
    typedef typename _memptr_traits<decltype(&Class::operator())>::_mem Fn;
    static constexpr unsigned _qualify = _function_traits<Fn>::qualify;
    static_assert((_qualify & 0b1000u) == 0,
                  "function can not bind to &&-qualify functor");
    typedef typename _function_traits<Fn>::type type;
    typedef typename _function_helper<type>::result_type result_type;
    static constexpr size_t arg_num = _function_helper<type>::arg_num;
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
    typedef _mem type(typename _get_cv<_mem>::template _bind_t<_class> &);
    typedef typename _function_helper<type>::result_type result_type;
    static constexpr size_t arg_num = _function_helper<type>::arg_num;
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
    static constexpr size_t arg_num = _function_helper<type>::arg_num;
};

template<class Fn, bool = is_class<Fn>::value, bool = is_member_pointer<Fn>::value>
struct _callable_traits: _function_helper<remove_pointer_t<Fn>> {};

template<class Functor>
struct _callable_traits<Functor, true, false>: _functor_helper<Functor> {};

template<class MemPtr>
struct _callable_traits<MemPtr, false, true>: _memptr_helper<MemPtr> {};

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

template<class Fn, class... Args>
decltype(auto) bind(Fn &&f, Args &&... args) {
    using _call_traits = callable_traits<Fn>;
    static_assert(sizeof...(Args) == _call_traits::arg_num,
                  "bind size not compatible");
    using R = typename _call_traits::result_type;
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(f),
                                  ala::forward<Args>(args)...);
}

template<class R, class Fn, class... Args>
decltype(auto) bind(Fn &&f, Args &&... args) {
    using _call_traits = callable_traits<Fn>;
    static_assert(sizeof...(Args) == _call_traits::arg_num,
                  "bind size not compatible");
    return _bind_t<R, Fn, Args...>(ala::forward<Fn>(f),
                                  ala::forward<Args>(args)...);
}

} // namespace ala
#endif