#include <ala/optional.h>
#include <optional>
#include <iostream>
using namespace ala;

struct X {
    X() = delete;
    // ~X() {
    //     std::cout << "des";
    // }
    ~X() = default;
    X(int) {
        std::cout << "int";
    }
    // X(const X &) {
    //     std::cout << "cp";
    // }
    X(const X &) = default;
    X(X &&) {
        std::cout << "mv";
    }
    // X(X &&) = default;
    X &operator=(X &&) = default;
    X &operator=(const X &) = default;
};

// template<class T>
struct _optional_ba {
    // aligned_storage_t<sizeof(T), alignof(T)> _data; // exposition only
    bool _valid;
    int x;
    _optional_ba(){};
};

// cflags=-stdlib=libc++

template<class T>
struct Q {
    // char _d[sizeof(T)];
    aligned_storage_t<sizeof(T), alignof(T)> _d;
    ~Q() {
        (*(T *)&_d).~T();
    }
    Q(const T &t) {
        ::new ((void *)&_d) T(t);
    }
};

void tst() {
    Q<int> qq({});
}

template<typename S, size_t... N>
constexpr integer_sequence<char, S::get()[N]...>
prepare_impl(S, index_sequence<N...>) {
    return {};
}

template<typename S>
constexpr decltype(auto) prepare(S s) {
    return prepare_impl(s, make_index_sequence<sizeof(S::get()) - 1>{});
}

template<typename S>
struct FK;

int main() {
    auto ring = prepare([] {
        struct tmp {
            static constexpr decltype(auto) get() {
                return "1234";
            }
        };
        return tmp{};
    }());
    FK<decltype(ring)> fk;
    FK<decltype("1234")> fk;
    tst();
    int h, j;
    _optional_ba t{}; // 成员必初始化
    assert((t._valid));
    assert(!(t._valid));

    _optional_ba t1; // 使用msvc时成员没有初始化
    if ((bool)t1._valid) {
        std::cout << "shit";
    }

    bool bl{};
    ala::optional<int> opt(10);
    auto x = opt.value();
    auto y = optional<int>(9).value();

    ala::optional<X> opt1{1};
    ala::optional<X> opt2;
    ala::optional<X> opt3;
    assert(!opt2);
    assert(!opt3);
    opt1.reset();

    assert(!opt3.has_value());
    opt3 = ala::move(opt1);

    static_assert(is_trivially_copy_constructible<ala::optional<X>>::value);
    static_assert(std::is_trivially_copy_constructible<ala::optional<X>>::value);
    static_assert(std::is_trivially_copy_constructible<X>::value);

    static_assert(is_trivially_copy_constructible<ala::optional<int>>::value);
    static_assert(!is_trivially_default_constructible<ala::optional<int>>::value);
    // static_assert(is_trivially_move_constructible<ala::optional<X>>::value);
    // static_assert(is_trivially_move_assignable<ala::optional<X>>::value);
    // static_assert(!is_trivially_copy_assignable<ala::optional<X>>::value);
    return 0;
}
