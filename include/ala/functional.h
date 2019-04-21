#ifndef _ALA_FUNCTIONAL_H
#define _ALA_FUNCTIONAL_H

#include <ala/detail/functional_base.h>
#include <typeinfo>

namespace ala {

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

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class R, class... Args>
function(R (*)(Args...))->function<R(Args...)>;

template<class>
struct _function_helper {};

template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...)> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) volatile> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const volatile> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) &> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const &> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) volatile &> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const volatile &> {
    typedef R type(Args...);
};

#if _ALA_ENABLE_NOEXCEPT_TYPE
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) volatile noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const volatile noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) & noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const &noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) volatile &noexcept> {
    typedef R type(Args...);
};
template<class F, class R, class... Args>
struct _function_helper<R (F::*)(Args...) const volatile &noexcept> {
    typedef R type(Args...);
};
#endif

template<class Functor>
function(Functor)
    ->function<class _function_helper<decltype(&Functor::operator())>::type>;
#endif

} // namespace ala
#endif