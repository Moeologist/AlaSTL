
template<typename T>
struct _class_of_memptr;

template<typename Class, typename Ret>
struct _class_of_memptr<Ret Class::*> { typedef Class type; };

template<typename T>
using _class_of_memptr_t = typename _class_of_memptr<T>::type;

template<typename Type, template<typename...> class Template>
struct is_specification: false_type {};

template<template<typename...> class Template, typename... TArgs>
struct is_specification<Template<TArgs...>, Template>: true_type {};

template<typename T>
struct reference_wrapper;

struct _invoker_mf {
    template<class Mf, class Class, class... Args>
    static decltype(auto) _call(Mf mf, Class &&ins, Args &&... args) noexcept(
        (forward<Class>(ins).*mf)(forward<Args>(args)...)) {
        return ((forward<Class>(ins).*mf)(forward<Args>(args)...));
    }
};

struct _invoker_mf_refw {
    template<class Mf, class Class, class... Args>
    static decltype(auto) _call(Mf mf, Class &&ins, Args &&... args) noexcept(
        (forward<Class>(ins).get().*mf)(forward<Args>(args)...)) {
        return ((forward<Class>(ins).get().*mf)(forward<Args>(args)...).get());
    }
};

struct _invoker_mf_deref {
    template<class Mf, class Class, class... Args>
    static decltype(auto) _call(Mf mf, Class &&ins, Args &&... args) noexcept(
        ((*forward<Class>(ins)).*mf)(forward<Args>(args)...)) {
        return (((*forward<Class>(ins)).*mf)(forward<Args>(args)...));
    }
};

struct _invoker_mo {
    template<class Mo, class Class>
    static decltype(auto) _call(Mo mo,
                                Class &&ins) noexcept(forward<Class>(ins).*mo) {
        return (forward<Class>(ins).*mo);
    }
};

struct _invoker_mo_refw {
    template<class Mo, class Class>
    static decltype(auto)
    _call(Mo mo, Class &&ins) noexcept(forward<Class>(ins).get().*mo) {
        return (forward<Class>(ins).get().*mo);
    }
};

struct _invoker_mo_deref {
    template<class Mo, class Class>
    static decltype(auto)
    _call(Mo mo, Class &&ins) noexcept((*forward<Class>(ins)).*mo) {
        return ((*forward<Class>(ins)).*mo);
    }
};

struct _invoker_functor {
    template<class Call, class... Args>
    static decltype(auto) _call(Call &&call, Args &&... args) noexcept(
        forward<Call>(call)(forward<Args>(args)...)) {
        return (forward<Call>(call)(forward<Args>(args)...));
    }
};

template<class Call, class T, class RMcvref = remove_cvref_t<Call>,
         bool IsMfp = is_member_function_pointer<RMcvref>::value,
         bool IsMop = is_member_object_pointer<RMcvref>::value>
struct _invoker_helper;

template<class Mf, class Class, class RMcvref>
struct _invoker_helper<Mf, Class, RMcvref, true, false>
    : conditional_t<
          is_base_of<_class_of_memptr_t<Mf>, remove_reference_t<Class>>::value,
          _invoker_mf,
          conditional_t<is_specification<Class, reference_wrapper>::value,
                        _invoker_mf_refw, _invoker_mf_deref>> {};

template<class Mo, class Class, class RMcvref>
struct _invoker_helper<Mo, Class, RMcvref, false, true>
    : conditional_t<
          is_base_of<_class_of_memptr_t<Mo>, remove_reference_t<Class>>::value,
          _invoker_mo,
          conditional_t<is_specification<Class, reference_wrapper>::value,
                        _invoker_mo_refw, _invoker_mo_deref>> {};

template<class Call, class T, class RMcvref>
struct _invoker_helper<Call, T, RMcvref, false, false>: _invoker_functor {};

template<class Call, class... Args>
struct _invoker;

template<class Call>
struct _invoker<Call>: _invoker_functor {};

template<class Call, class T, class... Args>
struct _invoker<Call, T, Args...>: _invoker_helper<Call, T> {};

template<class Call, class... Args>
decltype(auto) invoke(Call &&call, Args &&... args) noexcept(
    _invoker::<Call, Args...>::_call(forward<Call>(call),
                                     forward<Args>(args)...)) {
    return (_invoker<Call, Args...>::_call(forward<Call>(call),
                                           forward<Args>(args)...));
}