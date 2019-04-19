#include <ala/optional.h>
#include <optional>
#include <iostream>
using namespace ala;

struct X {
    X() = delete;
    X(int) {
        std::cout << "int";
    }
    // X(const X &) {
    //     std::cout << "cp";
    // }
    // X(const X &) = default;
    X(X &&) {
        std::cout << "mv";
    }
    // X(X &&) = default;
    X &operator=(X &&) = default;
    X &operator=(const X &) = default;
};

struct Bind {
    template<class T>
    using rebind = int;
};

template<typename T, typename = void>
struct _has_rebind_template: false_type {};

template<typename T>
struct _has_rebind_template<T, void_t<typename T::template rebind<T>>>
    : true_type {};

template<template<typename...> class...>
using void_template = void;

template<typename T, typename = void_template<>>
struct _has_rebind_template1: false_type {};

template<typename T>
struct _has_rebind_template1<T, void_template<T::template rebind>>: true_type {};

template<bool B, typename T>
struct ab {};
template<typename T>
struct ab<true, T> {
    typedef T type;
};

static_assert(_has_rebind_template<Bind>::value);
static_assert(_has_rebind_template1<Bind>::value);

// template<class T>
struct _optional_ba {
    // aligned_storage_t<sizeof(T), alignof(T)> _data; // exposition only
    bool _valid = false;
    int x;
    // constexpr _optional_ba() = default;
};

//@cflags=-stdlib=libc++

int main() {
    _optional_ba t{}; // 成员必初始化
    assert(!t._valid);

    _optional_ba t1; // 使用msvc时成员没有初始化
    assert(!t1._valid); // Assertion failed

    bool bl{};
    const ala::optional<int> opt(10);
    auto x = opt.value();
    auto y = optional<int>(9).value();

    ala::optional<X> opt1{1};
    // ala::optional<X> opt2 = ala::move(opt1);
    ala::optional<X> opt2;
    ala::optional<X> opt3;
    assert(!opt2);
    assert(!opt3);

    assert(opt3.has_value());
    // opt3 = opt1;
    // opt3 = ala::move(opt1);

    static_assert(!is_trivially_copy_constructible<ala::optional<X>>::value);
    static_assert(is_trivially_copy_constructible<ala::optional<int>>::value);
    static_assert(!is_trivially_default_constructible<ala::optional<int>>::value);
    // static_assert(is_trivially_move_constructible<ala::optional<X>>::value);
    // static_assert(is_trivially_move_assignable<ala::optional<X>>::value);
    // static_assert(!is_trivially_copy_assignable<ala::optional<X>>::value);
    return 0;
}
