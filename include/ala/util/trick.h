#include <ala/functional.h>
#include <ala/optional.h>
#include <ala/tuple.h>
#include <string>
#include <iostream>

template<class... Args>
constexpr void _dummy(Args &&...) noexcept {};

template<class F, size_t... Is>
constexpr void _unroll(F &&f, char *data, ala::index_sequence<Is...>) noexcept {
    _dummy((f(data[Is]), 0)...);
}

template<class Lambda>
struct lazy {
    using type = ala::invoke_result_t<Lambda>;
    Lambda lambda;
    ala::optional<type> result;
    constexpr lazy(const Lambda &l): lambda(l) {}

    void reset() {
        result.reset();
    }

    constexpr type get() {
        if (!result)
            result = lambda();
        return *result;
    }

    constexpr operator type() {
        return get();
    }
};

template<class Lambda>
struct delay {
    Lambda lambda;
    constexpr delay(const Lambda &l): lambda(l) {}

    ~delay() {
        lambda();
    }
};

template<class Get, class Set>
struct property {
    Get getter;
    Set setter;

    constexpr property(const Get &g, const Set &s): getter(g), setter(s) {}

    template<class T>
    constexpr decltype(auto) operator=(T &&other) {
        return setter(ala::forward<T>(other));
    }

    constexpr operator ala::invoke_result_t<Get>() {
        return getter();
    }
};

template<class F>
constexpr auto make_lazy(F &&f) {
    return lazy< ala::remove_cvref_t    < F>>(f);
}

template<class F>
constexpr auto make_delay(F &&f) {
    return delay< ala::remove_cvref_t   < F>>(f);
}

template<class Get, class Set>
constexpr auto make_property(Get &&g, Set &&s) {
    return property<ala::remove_cvref_t< Get>, ala::remove_cvref_t< Set>>(g, s);
}

#define LAZY(exp) make_lazy([&]() { return exp; })
#define DELAY(exp) auto ALA_COUNTER_ID = make_delay([&]() { exp; })

// #if _ALA_ENABLE_DEDUCTION_GUIDES
// template<class F>
// lazy(F) -> lazy<F>;

// #endif

