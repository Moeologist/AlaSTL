#ifndef _ALA_VARIANT_H
#define _ALA_VARIANT_H

#include <ala/array.h>
#include <ala/utility.h>
#include <ala/functional.h>
#include <ala/detail/macro.h>
#include <ala/detail/controller.h>

#ifdef _ALA_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreturn-type"
#endif

namespace ala {

#ifdef _ALA_INLINE_VAR
inline constexpr size_t variant_npos = -1;
#else
constexpr size_t variant_npos = -1;
#endif

template<class... Ts>
class variant;

template<class T>
struct variant_size; // not defined

template<size_t I, class T>
struct variant_alternative; // not defined

template<class... Ts>
struct variant_size<variant<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

template<class T>
struct variant_size<const T>: variant_size<T> {};
template<class T>
struct variant_size<volatile T>: variant_size<T> {};
template<class T>
struct variant_size<const volatile T>: variant_size<T> {};

#ifdef _ALA_ENABLE_INLINE_VAR
template<class T>
inline constexpr size_t variant_size_v = variant_size<T>::value;
#endif

template<size_t I, class... Ts>
struct variant_alternative<I, variant<Ts...>>: type_pack_element<I, Ts...> {};

template<size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template<size_t I, class T>
struct variant_alternative<I, const T> {
    using type = add_const_t<variant_alternative_t<I, T>>;
};

template<size_t I, class T>
struct variant_alternative<I, volatile T> {
    using type = add_volatile_t<variant_alternative_t<I, T>>;
};

template<size_t I, class T>
struct variant_alternative<I, const volatile T> {
    using type = add_const_t<add_volatile_t<variant_alternative_t<I, T>>>;
};

template<bool, size_t Begin, size_t Size, class... Ts>
union _variant_union {
    static_assert(1 < Size, "Internal error");
    char _placehold;
    _variant_union<true, Begin, (Size >> 1), Ts...> _left;
    _variant_union<true, Begin + (Size >> 1), Size - (Size >> 1), Ts...> _rght;

    ~_variant_union() = default;
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<size_t I, class... Args,
             class = enable_if_t<(Begin <= I && I < Begin + (Size >> 1))>>
    constexpr _variant_union(in_place_index_t<I>, Args &&... args)
        : _left(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<size_t I, class... Args, class = void,
             class = enable_if_t<(Begin + (Size >> 1) <= I && I < Begin + Size)>>
    constexpr _variant_union(in_place_index_t<I>, Args &&... args)
        : _rght(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}

    template<size_t I>
    constexpr auto get() const
        -> enable_if_t<(Begin <= I && I < Begin + (Size >> 1)),
                       decltype(_left.template get<I>())> {
        return _left.template get<I>();
    }

    template<size_t I>
    constexpr auto get() const
        -> enable_if_t<(Begin + (Size >> 1) <= I && I < Begin + Size),
                       decltype(_rght.template get<I>())> {
        return _rght.template get<I>();
    }

    template<class Fn>
    constexpr auto fmap(size_t I, Fn &&fn) const
        -> common_type_t<decltype(_left.fmap(I, ala::forward<Fn>(fn))),
                         decltype(_rght.fmap(I, ala::forward<Fn>(fn)))> {
        if (Begin <= I && I < Begin + (Size >> 1))
            return _left.fmap(I, ala::forward<Fn>(fn));
        if (Begin + (Size >> 1) <= I && I < Begin + Size)
            return _rght.fmap(I, ala::forward<Fn>(fn));
        assert(false);
    }

    template<size_t I, class Fn>
    constexpr auto fmap(in_place_index_t<I>, Fn &&fn)
        -> enable_if_t<(Begin <= I && I < Begin + (Size >> 1)),
                       decltype(_left.fmap(in_place_index_t<I>{},
                                           ala::forward<Fn>(fn)))> {
        return _left.fmap(in_place_index_t<I>{}, ala::forward<Fn>(fn));
    }

    template<size_t I, class Fn>
    constexpr auto fmap(in_place_index_t<I>, Fn &&fn)
        -> enable_if_t<(Begin + (Size >> 1) <= I && I < Begin + Size),
                       decltype(_rght.fmap(in_place_index_t<I>{},
                                           ala::forward<Fn>(fn)))> {
        return _rght.fmap(in_place_index_t<I>{}, ala::forward<Fn>(fn));
    }
};

template<size_t Begin, size_t Size, class... Ts>
union _variant_union<false, Begin, Size, Ts...> {
    static_assert(1 < Size, "Internal error");
    char _placehold;
    _variant_union<false, Begin, (Size >> 1), Ts...> _left;
    _variant_union<false, Begin + (Size >> 1), Size - (Size >> 1), Ts...> _rght;

    ~_variant_union(){};
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<size_t I, class... Args,
             class = enable_if_t<(Begin <= I && I < Begin + (Size >> 1))>>
    constexpr _variant_union(in_place_index_t<I>, Args &&... args)
        : _left(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<size_t I, class... Args, class = void,
             class = enable_if_t<(Begin + (Size >> 1) <= I && I < Begin + Size)>>
    constexpr _variant_union(in_place_index_t<I>, Args &&... args)
        : _rght(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}

    template<size_t I>
    constexpr auto get() const
        -> enable_if_t<(Begin <= I && I < Begin + (Size >> 1)),
                       decltype(_left.template get<I>())> {
        return _left.template get<I>();
    }

    template<size_t I>
    constexpr auto get() const
        -> enable_if_t<(Begin + (Size >> 1) <= I && I < Begin + Size),
                       decltype(_rght.template get<I>())> {
        return _rght.template get<I>();
    }

    template<class Fn>
    constexpr auto fmap(size_t I, Fn &&fn) const
        -> common_type_t<decltype(_left.fmap(I, ala::forward<Fn>(fn))),
                         decltype(_rght.fmap(I, ala::forward<Fn>(fn)))> {
        if (Begin <= I && I < Begin + (Size >> 1))
            return _left.fmap(I, ala::forward<Fn>(fn));
        if (Begin + (Size >> 1) <= I && I < Begin + Size)
            return _rght.fmap(I, ala::forward<Fn>(fn));
        assert(false);
    }

    template<size_t I, class Fn>
    constexpr auto fmap(in_place_index_t<I>, Fn &&fn)
        -> enable_if_t<(Begin <= I && I < Begin + (Size >> 1)),
                       decltype(_left.fmap(in_place_index_t<I>{},
                                           ala::forward<Fn>(fn)))> {
        return _left.fmap(in_place_index_t<I>{}, ala::forward<Fn>(fn));
    }

    template<size_t I, class Fn>
    constexpr auto fmap(in_place_index_t<I>, Fn &&fn)
        -> enable_if_t<(Begin + (Size >> 1) <= I && I < Begin + Size),
                       decltype(_rght.fmap(in_place_index_t<I>{},
                                           ala::forward<Fn>(fn)))> {
        return _rght.fmap(in_place_index_t<I>{}, ala::forward<Fn>(fn));
    }
};

template<size_t Begin, class... Ts>
union _variant_union<true, Begin, 1, Ts...> {
    type_pack_element_t<Begin, Ts...> _value;
    char _placehold;

    ~_variant_union() = default;
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<class... Args>
    constexpr _variant_union(in_place_index_t<Begin>, Args &&... args)
        : _value(ala::forward<Args>(args)...) {}

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}

    template<size_t I>
    constexpr const type_pack_element_t<Begin, Ts...> &get() const {
        static_assert(I == Begin, "Internal error");
        return this->_value;
    }

    template<class Fn>
    constexpr auto fmap(size_t I, Fn &&fn) const
        -> decltype(ala::forward<Fn>(fn)(_value)) {
        assert(I == Begin);
        return ala::forward<Fn>(fn)(_value);
    }

    template<size_t I, class Fn>
    constexpr auto fmap(in_place_index_t<I>, Fn &&fn)
        -> decltype(ala::forward<Fn>(fn)(_value)) {
        static_assert(I == Begin, "Internal error");
        return ala::forward<Fn>(fn)(_value);
    }
};

template<size_t Begin, class... Ts>
union _variant_union<false, Begin, 1, Ts...> {
    type_pack_element_t<Begin, Ts...> _value;
    char _placehold;

    ~_variant_union(){};
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<class... Args>
    constexpr _variant_union(in_place_index_t<Begin>, Args &&... args)
        : _value(ala::forward<Args>(args)...) {}

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}

    template<size_t I>
    constexpr const type_pack_element_t<Begin, Ts...> &get() const {
        static_assert(I == Begin, "Internal error");
        return this->_value;
    }

    template<class Fn>
    constexpr auto fmap(size_t I, Fn &&fn) const
        -> decltype(ala::forward<Fn>(fn)(_value)) {
        assert(I == Begin);
        return ala::forward<Fn>(fn)(_value);
    }

    template<size_t I, class Fn>
    constexpr auto fmap(in_place_index_t<I>, Fn &&fn)
        -> decltype(ala::forward<Fn>(fn)(_value)) {
        static_assert(I == Begin, "Internal error");
        return ala::forward<Fn>(fn)(_value);
    }
};

template<bool, class... Ts>
struct _variant_destroy {
    _variant_union<true, 0, sizeof...(Ts), Ts...> _union;
    size_t _index = variant_npos;
    ~_variant_destroy() = default;
    constexpr void _reset() {
        _index = variant_npos;
    }
    template<size_t I, class... Args>
    constexpr _variant_destroy(in_place_index_t<I>, Args &&... args)
        : _union(in_place_index_t<I>{}, ala::forward<Args>(args)...),
          _index(I) {}
    constexpr _variant_destroy() = default;
    constexpr _variant_destroy(_variant_destroy &&) = default;
    constexpr _variant_destroy(const _variant_destroy &) = default;
    constexpr _variant_destroy &operator=(const _variant_destroy &) = default;
    constexpr _variant_destroy &operator=(_variant_destroy &&other) = default;
};

template<class... Ts>
struct _variant_destroy<false, Ts...> {
    _variant_union<false, 0, sizeof...(Ts), Ts...> _union;
    size_t _index = variant_npos;
    ~_variant_destroy() {
        this->_reset();
    }
    constexpr void _reset() {
        if (this->_index != variant_npos) {
            this->_union.fmap(this->_index, [](auto &&v) {
                using T = remove_cvref_t<decltype(v)>;
                v.~T();
            });
            this->_index = variant_npos;
        }
    }
    template<size_t I, class... Args>
    constexpr _variant_destroy(in_place_index_t<I>, Args &&... args)
        : _union(in_place_index_t<I>{}, ala::forward<Args>(args)...),
          _index(I) {}

    constexpr _variant_destroy() = default;
    constexpr _variant_destroy(_variant_destroy &&) = default;
    constexpr _variant_destroy(const _variant_destroy &) = default;
    constexpr _variant_destroy &operator=(const _variant_destroy &) = default;
    constexpr _variant_destroy &operator=(_variant_destroy &&other) = default;
};

template<class... Ts>
struct _variant_base
    : _variant_destroy<_and_<is_trivially_destructible<Ts>...>::value, Ts...> {
    using _base_t =
        _variant_destroy<_and_<is_trivially_destructible<Ts>...>::value, Ts...>;
    using _base_t::_base_t;
    using _base_t::_union;
    using _base_t::_index;
    using _base_t::_reset;

    template<size_t I>
    constexpr const type_pack_element_t<I, Ts...> &get() const {
        return _union.template get<I>();
    }

    void *_address() {
        return this->_union.fmap(this->_index, [](auto &&v) {
            return const_cast<void *>(
                reinterpret_cast<const volatile void *>(ala::addressof(v)));
        });
    }

    template<size_t I, class... Args>
    void _ctor_v(in_place_index_t<I>, Args &&... args) {
        assert(this->_index == variant_npos);
        try {
            this->_index = I;
            ::new (this->_address())
                type_pack_element_t<I, Ts...>(ala::forward<Args>(args)...);
        } catch (...) {
            this->_index = variant_npos;
            throw;
        }
    }

    void _ctor(_variant_base &&other) noexcept(
        _and_<is_nothrow_move_constructible<Ts>...>::value) {
        assert(this->_index == variant_npos);
        if (other._index != variant_npos) {
            try {
                this->_index = other._index;
                other._union.fmap(other._index, [addr = _address()](auto &&v) {
                    ::new (addr) remove_cvref_t<decltype(v)>(
                        const_cast<remove_cvref_t<decltype(v)> &&>(v));
                });
            } catch (...) {
                this->_index = variant_npos;
                throw;
            }
        }
    }

    void _ctor(const _variant_base &other) noexcept(
        _and_<is_nothrow_copy_constructible<Ts>...>::value) {
        assert(this->_index == variant_npos);
        if (other._index != variant_npos) {
            try {
                this->_index = other._index;
                other._union.fmap(other._index, [addr = _address()](auto &&v) {
                    ::new (addr) remove_cvref_t<decltype(v)>(v);
                });
            } catch (...) {
                this->_index = variant_npos;
                throw;
            }
        }
    }

    template<size_t I, class Arg>
    void _asgn_v(in_place_index_t<I>, Arg &&arg) {
        if (_index == I) {
            try {
                _union.fmap(in_place_index_t<I>{},
                            [&](auto &v) { v = ala::forward<Arg>(arg); });
            } catch (...) { throw; }
        } else if (is_nothrow_constructible<type_pack_element_t<I, Ts...>, Arg>::value ||
                   !is_nothrow_move_constructible<type_pack_element_t<I, Ts...>>::value) {
            this->_reset();
            this->_ctor_v(in_place_index_t<I>{}, ala::forward<Arg>(arg));
        } else {
            _variant_base tmp(in_place_index_t<variant_npos>{});
            tmp._ctor_v(in_place_index_t<I>{}, ala::forward<Arg>(arg));
            this->_asgn(ala::move(tmp));
        }
    }

    void _asgn(const _variant_base &other) {
        bool a[] = {(is_nothrow_copy_constructible<Ts>::value ||
                     !is_nothrow_move_constructible<Ts>::value)...};
        if (this->_index == variant_npos && other._index == variant_npos) {
            return;
        } else if (other._index == variant_npos) {
            this->_reset();
        } else if (this->_index == other._index) {
            try {
                other._union.fmap(other._index, [addr = _address()](auto &&v) {
                    *reinterpret_cast<remove_cvref_t<decltype(v)> *>(addr) = v;
                });
            } catch (...) { throw; }
        } else if (a[other._index]) {
            this->_reset();
            this->_ctor(other);
        } else {
            _variant_base tmp(in_place_index_t<variant_npos>{});
            tmp._ctor(other);
            this->_asgn(ala::move(tmp));
        }
    }

    void _asgn(_variant_base &&other) noexcept(
        _and_<is_nothrow_move_constructible<Ts>...,
              is_nothrow_move_assignable<Ts>...>::value) {
        if (this->_index == variant_npos && other._index == variant_npos) {
            return;
        } else if (other._index == variant_npos) {
            this->_reset();
        } else if (this->_index == other._index) {
            try {
                other._union.fmap(other._index, [addr = _address()](auto &&v) {
                    *reinterpret_cast<remove_cvref_t<decltype(v)> *>(
                        addr) = const_cast<remove_cvref_t<decltype(v)> &&>(v);
                });
            } catch (...) { throw; }
        } else {
            this->_reset();
            this->_ctor(ala::move(other));
        }
    }

    void _swap_move(true_type, _variant_base &other) noexcept(
        _and_<is_nothrow_move_constructible<Ts>...,
              is_nothrow_move_assignable<Ts>...>::value) {
        _variant_base tmp(in_place_index_t<variant_npos>{});
        tmp._ctor(ala::move(other));
        other._asgn(ala::move(*this));
        this->_asgn(ala::move(tmp));
    }

    void _swap_move(false_type, _variant_base &other) noexcept {
        abort();
    }

    void _swap(_variant_base &other) noexcept(
        _and_<is_nothrow_move_constructible<Ts>..., is_nothrow_swappable<Ts>...>::value) {
        if (this->_index == variant_npos && other._index == variant_npos) {
            return;
        } else if (this->_index == other._index) {
            try {
                other._union.fmap(other._index, [addr = _address()](auto &&v) {
                    ala::_swap_adl(
                        *reinterpret_cast<remove_cvref_t<decltype(v)> *>(addr),
                        const_cast<remove_cvref_t<decltype(v)> &>(v));
                });
            } catch (...) { throw; }
        } else {
            using tag_t = bool_constant<_and_<is_move_constructible<Ts>...,
                                              is_move_assignable<Ts>...>::value>;
            this->_swap_move(tag_t{}, other);
        }
    }

    ~_variant_base() = default;
    constexpr _variant_base() = default;
    constexpr _variant_base(_variant_base &&) = default;
    constexpr _variant_base(const _variant_base &) = default;
    constexpr _variant_base &operator=(const _variant_base &) = default;
    constexpr _variant_base &operator=(_variant_base &&other) = default;
};

struct monostate {};

struct bad_variant_access: exception {
    bad_variant_access() noexcept {}
    virtual const char *what() const noexcept {
        return "bad_variant_access";
    }
};

template<class... Ts>
class variant: _make_controller_t<_variant_base<Ts...>, Ts...> {
    static_assert(_and_<_not_<is_array<Ts>>...>::value &&
                      _and_<_not_<is_reference<Ts>>...>::value &&
                      _and_<_not_<is_void<Ts>>...>::value &&
                      _and_<is_destructible<Ts>...>::value,
                  "N4860 [20.7.3/2]");
    static_assert(sizeof...(Ts) > 0, "N4860 [20.7.3/3]");
    using _base_t = _make_controller_t<_variant_base<Ts...>, Ts...>;
    using _first_t = type_pack_element_t<0, Ts...>;

    template<size_t I, class... Us>
    constexpr friend add_pointer_t<variant_alternative_t<I, variant<Us...>>>
    get_if(variant<Us...> *v) noexcept;
    template<size_t I, class... Us>
    constexpr friend add_pointer_t<const variant_alternative_t<I, variant<Us...>>>
    get_if(const variant<Us...> *v) noexcept;

    template<size_t I, class... Us>
    constexpr friend variant_alternative_t<I, variant<Us...>> &
    get(variant<Us...> &);
    template<size_t I, class... Us>
    constexpr friend variant_alternative_t<I, variant<Us...>> &&
    get(variant<Us...> &&);
    template<size_t I, class... Us>
    constexpr friend const variant_alternative_t<I, variant<Us...>> &
    get(const variant<Us...> &);
    template<size_t I, class... Us>
    constexpr friend const variant_alternative_t<I, variant<Us...>> &&
    get(const variant<Us...> &&);

    template<size_t I, class Ti, class T,
             bool = is_same<bool, remove_cvref_t<Ti>>::value, class = void>
    struct _overload_set_base {
        void _test();
    };

    // avoid narrowing
    template<size_t I, class Ti, class T>
    struct _overload_set_base<I, Ti, T, false,
                              void_t<decltype(ala::array<Ti, 1>{{declval<T>()}})>> {
        using F = integral_constant<size_t, I> (&)(Ti);
        operator F();
        static integral_constant<size_t, I> _test(Ti);
    };

    template<size_t I, class Ti, class T>
    struct _overload_set_base<
        I, Ti, T, true, enable_if_t<is_same<bool, remove_cvref_t<T>>::value>> {
        using F = integral_constant<size_t, I> (&)(Ti);
        operator F();
        static integral_constant<size_t, I> _test(Ti);
    };

    template<class T, class Seq>
    struct _overload_set;

    template<class T, size_t... Is>
    struct _overload_set<T, index_sequence<Is...>>
        : _overload_set_base<Is, Ts, T>... {
        // using _overload_set_base<Is, Ts, T>::_test...;
    };

    template<class T>
    using _fuzzy_i = decltype(
        _overload_set<T, index_sequence_for<Ts...>>{}(declval<T>()));
    // using _fuzzy_i = decltype(
    //     _overload_set<T, index_sequence_for<Ts...>>::_test(declval<T>()));

    template<class T>
    using _fuzzy_t = type_pack_element_t<_fuzzy_i<T>::value, Ts...>;

    template<class T>
    struct _is_tag: false_type {};

    template<size_t I>
    struct _is_tag<in_place_index_t<I>>: true_type {};

    template<class T>
    struct _is_tag<in_place_type_t<T>>: true_type {};

public:
    template<class Dummy = _first_t,
             class = enable_if_t<is_default_constructible<Dummy>::value>>
    constexpr variant() noexcept(is_nothrow_default_constructible<Dummy>::value)
        : _base_t(in_place_index_t<0>{}) {}

    constexpr variant(const variant &) = default;
    constexpr variant(variant &&) = default;

    template<class T,
             class = enable_if_t<_and_<bool_constant<sizeof...(Ts) != 0>,
                                       _not_<is_same<remove_cvref_t<T>, variant>>,
                                       _not_<_is_tag<remove_cvref_t<T>>>,
                                       is_constructible<_fuzzy_t<T>, T>>::value>>
    constexpr variant(T &&v) noexcept(is_nothrow_constructible<_fuzzy_t<T>, T>::value)
        : _base_t(in_place_index_t<_fuzzy_i<T>::value>{}, ala::forward<T>(v)) {}

    template<size_t I, class... Args,
             class = enable_if_t<
                 (I < sizeof...(Ts)) &&
                 is_constructible<type_pack_element_t<I, Ts...>, Args...>::value>>
    constexpr explicit variant(in_place_index_t<I>, Args &&... args)
        : _base_t(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<size_t I, class U, class... Args,
             class = enable_if_t<(I < sizeof...(Ts)) &&
                                 is_constructible<type_pack_element_t<I, Ts...>,
                                                  initializer_list<U> &, Args...>::value>>
    constexpr explicit variant(in_place_index_t<I>, initializer_list<U> il,
                               Args &&... args)
        : _base_t(in_place_index_t<I>{}, il, ala::forward<Args>(args)...) {}

    template<class T, class... Args, size_t I = type_pack_index<T, Ts...>::value,
             class = enable_if_t<is_constructible<T, Args...>::value>>
    constexpr explicit variant(in_place_type_t<T>, Args &&... args)
        : _base_t(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<
        class T, class U, class... Args, size_t I = type_pack_index<T, Ts...>::value,
        class = enable_if_t<is_constructible<T, initializer_list<U> &, Args...>::value>>
    constexpr explicit variant(in_place_type_t<T>, initializer_list<U> il,
                               Args &&... args)
        : _base_t(in_place_index_t<I>{}, il, ala::forward<Args>(args)...) {}

    // destructor
    ~variant() = default;
    // assignment
    constexpr variant &operator=(const variant &) = default;
    constexpr variant &operator=(variant &&) = default;

    template<class T>
    enable_if_t<_and_<_not_<is_same<remove_cvref_t<T>, variant>>,
                      is_assignable<_fuzzy_t<T> &, T>,
                      is_constructible<_fuzzy_t<T>, T>>::value,
                variant &>
    operator=(T &&value) noexcept(
        is_nothrow_assignable<_fuzzy_t<T> &, T>::value
            &&is_nothrow_constructible<_fuzzy_t<T>, T>::value) {
        this->_asgn_v(in_place_index_t<_fuzzy_i<T>::value>{},
                      ala::forward<T>(value));
        return *this;
    }

    template<size_t I, class... Args>
    enable_if_t<is_constructible<type_pack_element_t<I, Ts...>, Args...>::value,
                type_pack_element_t<I, Ts...> &>
    emplace(Args &&... args) {
        this->_reset();
        this->_ctor_v(in_place_index_t<I>{}, ala::forward<Args>(args)...);
        return ala::get<I>(*this);
    }

    template<size_t I, class U, class... Args>
    enable_if_t<is_constructible<type_pack_element_t<I, Ts...>,
                                 initializer_list<U> &, Args...>::value,
                type_pack_element_t<I, Ts...> &>
    emplace(initializer_list<U> il, Args &&... args) {
        this->_reset();
        this->_ctor_v(in_place_index_t<I>{}, il, ala::forward<Args>(args)...);
        return ala::get<I>(*this);
    }

    // modifiers
    template<class T, class... Args>
    enable_if_t<is_constructible<T, Args...>::value, T &> emplace(Args &&... args) {
        return this->emplace<type_pack_index<T, Ts...>::value>(
            ala::forward<Args>(args)...);
    }

    template<class T, class U, class... Args>
    enable_if_t<is_constructible<T, initializer_list<U> &, Args...>::value, T &>
    emplace(initializer_list<U> il, Args &&... args) {
        return this->emplace<type_pack_index<T, Ts...>::value>(
            il, ala::forward<Args>(args)...);
    }

    // value status
    constexpr bool valueless_by_exception() const noexcept {
        return index() == variant_npos;
    }

    constexpr size_t index() const noexcept {
        return this->_index;
    }

    // swap
    void swap(variant &other) noexcept(_and_<is_nothrow_move_constructible<Ts>...,
                                             is_nothrow_swappable<Ts>...>::value) {
        static_assert(_and_<is_move_constructible<Ts>...>::value,
                      "Not all of types are move constructible");
        static_cast<_base_t *>(this)->_swap(static_cast<_base_t &>(other));
    }
};

// value access
template<class T, class... Ts>
constexpr bool holds_alternative(const variant<Ts...> &v) noexcept {
    return type_pack_index<T, Ts...>::value == v.index();
}

template<size_t I, class... Ts>
constexpr variant_alternative_t<I, variant<Ts...>> &get(variant<Ts...> &v) {
    if (I != v.index())
        throw bad_variant_access{};
    return const_cast<variant_alternative_t<I, variant<Ts...>> &>(
        v.template get<I>());
}

template<size_t I, class... Ts>
constexpr variant_alternative_t<I, variant<Ts...>> &&get(variant<Ts...> &&v) {
    if (I != v.index())
        throw bad_variant_access{};
    return ala::move(const_cast<variant_alternative_t<I, variant<Ts...>> &>(
        v.template get<I>()));
}

template<size_t I, class... Ts>
constexpr const variant_alternative_t<I, variant<Ts...>> &
get(const variant<Ts...> &v) {
    if (I != v.index())
        throw bad_variant_access{};
    return v.template get<I>();
}

template<size_t I, class... Ts>
constexpr const variant_alternative_t<I, variant<Ts...>> &&
get(const variant<Ts...> &&v) {
    if (I != v.index())
        throw bad_variant_access{};
    return ala::move(v.template get<I>());
}

template<class T, class... Ts>
constexpr T &get(variant<Ts...> &v) {
    return ala::get<type_pack_index<T, Ts...>::value>(v);
}

template<class T, class... Ts>
constexpr T &&get(variant<Ts...> &&v) {
    return ala::get<type_pack_index<T, Ts...>::value>(ala::move(v));
}

template<class T, class... Ts>
constexpr const T &get(const variant<Ts...> &v) {
    return ala::get<type_pack_index<T, Ts...>::value>(v);
}

template<class T, class... Ts>
constexpr const T &&get(const variant<Ts...> &&v) {
    return ala::get<type_pack_index<T, Ts...>::value>(ala::move(v));
}

template<size_t I, class... Ts>
constexpr add_pointer_t<variant_alternative_t<I, variant<Ts...>>>
get_if(variant<Ts...> *v) noexcept {
    if (v == nullptr || I != v->index())
        return nullptr;
    return const_cast<add_pointer_t<variant_alternative_t<I, variant<Ts...>>>>(
        ala::addressof(v->template get<I>()));
}

template<size_t I, class... Ts>
constexpr add_pointer_t<const variant_alternative_t<I, variant<Ts...>>>
get_if(const variant<Ts...> *v) noexcept {
    if (v == nullptr || I != v->index())
        return nullptr;
    return ala::addressof(v->template get<I>());
}

template<class T, class... Ts>
constexpr add_pointer_t<T> get_if(variant<Ts...> *v) noexcept {
    return ala::get_if<type_pack_index<T, Ts...>::value>(v);
}

template<class T, class... Ts>
constexpr add_pointer_t<const T> get_if(const variant<Ts...> *v) noexcept {
    return ala::get_if<type_pack_index<T, Ts...>::value>(v);
}

// visitation

struct _visit_no_return {};

template<class R, class Visitor, class... Variants>
struct _visit_impl {
    template<size_t... Is>
    struct _dispatcher {
        template<class Visitor1, class... Variants1>
        static constexpr auto _invoke_ol(Visitor1 &&vis, Variants1 &&... vars)
            -> decltype(ala::invoke(ala::forward<Visitor1>(vis),
                                    ala::get<Is>(ala::forward<Variants1>(vars))...)) {
            return ala::invoke(ala::forward<Visitor>(vis),
                               ala::get<Is>(ala::forward<Variants1>(vars))...);
        }

        static constexpr auto _invoke(Visitor &&vis, Variants &&... vars)
            -> decltype(_invoke_ol(ala::forward<Visitor>(vis),
                                   ala::forward<Variants>(vars)...)) {
            return _invoke_ol(ala::forward<Visitor>(vis),
                              ala::forward<Variants>(vars)...);
        }
    };

    template<size_t... Is>
    struct _dispatcher_no_return {
        template<class Visitor1, class... Variants1>
        static constexpr R _invoke_ol(char, Visitor1 &&vis, Variants1 &&... vars) {
            // never run into here;
            return R();
        }

        template<class Visitor1, class... Variants1>
        static constexpr auto _invoke_ol(int, Visitor1 &&vis, Variants1 &&... vars)
            -> decltype(ala::invoke(ala::forward<Visitor1>(vis),
                                    ala::get<Is>(ala::forward<Variants1>(vars))...)) {
            return ala::invoke(ala::forward<Visitor>(vis),
                               ala::get<Is>(ala::forward<Variants1>(vars))...);
        }

        static constexpr R _invoke(Visitor &&vis, Variants &&... vars) {
            return _invoke_ol(0, ala::forward<Visitor>(vis),
                              ala::forward<Variants>(vars)...);
        }
    };

    template<size_t... Is>
    using dispatcher =
        conditional_t<is_same<R, _visit_no_return>::value, _dispatcher<Is...>,
                      _dispatcher_no_return<Is...>>;

    template<size_t... Is>
    static constexpr auto _make_visit(index_sequence<Is...>) {
        return dispatcher<Is...>::_invoke;
    }

    template<class Void, class... Ts>
    struct _has_common_type: false_type {};

    template<class... Ts>
    struct _has_common_type<void_t<common_type_t<Ts...>>, Ts...>: true_type {};

    template<class... Args>
    static constexpr auto _make_array(Args &&... args) {
        static_assert(
            _has_common_type<void, remove_cvref_t<Args>...>::value,
            "The call is ill-formed if the invocation above is not a valid "
            "expression of the same type and value category, for all "
            "combinations of alternative types of all variants.");
        return ala::array<common_type_t<remove_cvref_t<Args>...>, sizeof...(Args)>{
            {ala::forward<Args>(args)...}};
    }

    template<size_t... Is>
    static constexpr auto _make_visit_array(index_sequence<Is...>) {
        return _visit_impl::_make_visit(index_sequence<Is...>{});
    }

    template<size_t... Is, size_t... Js, class... Seqs>
    static constexpr auto _make_visit_array(index_sequence<Is...>,
                                            index_sequence<Js...>, Seqs... rest) {
        return _visit_impl::_make_array(
            _visit_impl::_make_visit_array(index_sequence<Is..., Js>{},
                                           rest...)...);
    }

    static constexpr auto _make_visit_array() {
        return _visit_impl::_make_visit_array(
            index_sequence<>{},
            make_index_sequence<variant_size<remove_cvref_t<Variants>>::value>{}...);
    }

    template<class Invoker>
    static constexpr auto _view(Invoker &&a) {
        return a;
    }

    template<class Array, class... Size_t>
    static constexpr auto _view(Array &&a, size_t i, Size_t... ii) {
        if (i == variant_npos)
            throw bad_variant_access{};
        return _view(a[i], ii...);
    }

    static constexpr auto _visit_helper(Visitor &&vis, Variants &&... vars)
        -> decltype(_visit_impl::_view(_visit_impl::_make_visit_array(),
                                       vars.index()...)(
            ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...)) {
        return _visit_impl::_view(_visit_impl::_make_visit_array(),
                                  vars.index()...)(ala::forward<Visitor>(vis),
                                                   ala::forward<Variants>(vars)...);
    }
};

template<class Visitor, class... Variants>
constexpr auto visit(Visitor &&vis, Variants &&... vars)
    -> decltype(_visit_impl<_visit_no_return, Visitor, Variants...>::_visit_helper(
        ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...)) {
    return _visit_impl<_visit_no_return, Visitor, Variants...>::_visit_helper(
        ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...);
}

template<class R, class Visitor, class... Variants>
constexpr R visit(Visitor &&vis, Variants &&... vars) {
    return _visit_impl<R, Visitor, Variants...>::_visit_helper(
        ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...);
}

template<class R, class Visitor, class... Variants>
constexpr R visit_r(Visitor &&vis, Variants &&... vars) {
    return _visit_impl<R, Visitor, Variants...>::_visit_helper(
        ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...);
}

// relational operators
template<class... Ts>
constexpr bool operator==(const variant<Ts...> &lhs, const variant<Ts...> &rhs) {
    if (lhs.index() != rhs.index())
        return false;
    if (lhs.valueless_by_exception())
        return true;
    return ala::visit_r<bool>(equal_to<>{}, lhs, rhs);
}

template<class... Ts>
constexpr bool operator!=(const variant<Ts...> &lhs, const variant<Ts...> &rhs) {
    if (lhs.index() != rhs.index())
        return true;
    if (lhs.valueless_by_exception())
        return false;
    return ala::visit_r<bool>(not_equal_to<>{}, lhs, rhs);
}

template<class... Ts>
constexpr bool operator<(const variant<Ts...> &lhs, const variant<Ts...> &rhs) {
    if (rhs.valueless_by_exception())
        return false;
    if (lhs.valueless_by_exception())
        return true;
    if (lhs.index() < rhs.index())
        return true;
    if (rhs.index() < lhs.index())
        return false;
    return ala::visit_r<bool>(less<>{}, lhs, rhs);
}

template<class... Ts>
constexpr bool operator>(const variant<Ts...> &lhs, const variant<Ts...> &rhs) {
    if (lhs.valueless_by_exception())
        return false;
    if (rhs.valueless_by_exception())
        return true;
    if (lhs.index() > rhs.index())
        return true;
    if (rhs.index() > lhs.index())
        return false;
    return ala::visit_r<bool>(greater<>{}, lhs, rhs);
}

template<class... Ts>
constexpr bool operator<=(const variant<Ts...> &lhs, const variant<Ts...> &rhs) {
    if (lhs.valueless_by_exception())
        return true;
    if (rhs.valueless_by_exception())
        return false;
    if (lhs.index() < rhs.index())
        return true;
    if (rhs.index() < lhs.index())
        return false;
    return ala::visit_r<bool>(less_equal<>{}, lhs, rhs);
}

template<class... Ts>
constexpr bool operator>=(const variant<Ts...> &lhs, const variant<Ts...> &rhs) {
    if (rhs.valueless_by_exception())
        return true;
    if (lhs.valueless_by_exception())
        return false;
    if (lhs.index() > rhs.index())
        return true;
    if (rhs.index() > lhs.index())
        return false;
    return ala::visit_r<bool>(greater_equal<>{}, lhs, rhs);
}

// monostate relational operators
constexpr bool operator<(monostate, monostate) noexcept {
    return false;
}

constexpr bool operator>(monostate, monostate) noexcept {
    return false;
}

constexpr bool operator<=(monostate, monostate) noexcept {
    return true;
}

constexpr bool operator>=(monostate, monostate) noexcept {
    return true;
}

constexpr bool operator==(monostate, monostate) noexcept {
    return true;
}

constexpr bool operator!=(monostate, monostate) noexcept {
    return false;
}

// specialized algorithms
template<class... Ts>
enable_if_t<_and_<is_move_constructible<Ts>..., is_swappable<Ts>...>::value>
swap(variant<Ts...> &lhs, variant<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#ifdef _ALA_CLANG
    #pragma clang diagnostic pop
#endif

} // namespace ala

#endif