#ifndef _ALA_VARIANT_H
#define _ALA_VARIANT_H

#include <ala/array.h>
#include <ala/utility.h>
#include <ala/detail/controller.h>
#include <ala/detail/monostate.h>

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

template<bool, class...>
union _variant_union;

template<bool, class... Ts>
struct _union_op;

template<class IntSeq, bool Trivial, class... Ts>
struct _make_union_helper {};

template<bool Trivial, class... Ts, size_t... Is>
struct _make_union_helper<index_sequence<Is...>, Trivial, Ts...> {
    using type = _variant_union<Trivial, type_pack_element_t<Is, Ts...>...>;
    using op = _union_op<Trivial, type_pack_element_t<Is, Ts...>...>;
};

template<class IntSeq, bool Trivial, class... Ts>
using _make_union_t = typename _make_union_helper<IntSeq, Trivial, Ts...>::type;

template<class IntSeq, bool Trivial, class... Ts>
using _make_union_op_t = typename _make_union_helper<IntSeq, Trivial, Ts...>::op;

template<bool Trivial, class... Ts>
union _variant_union {
    static constexpr size_t Size = sizeof...(Ts);
    static_assert(1 < Size, "Internal error");
    static_assert(Trivial, "Internal error");
    char _placehold;
    _make_union_t<make_integer_range<size_t, 0, (Size >> 1)>, true, Ts...> _left;
    _make_union_t<make_integer_range<size_t, (Size >> 1), Size>, true, Ts...> _rght;

    ~_variant_union() = default;
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<size_t I, class... Args, class = enable_if_t<(I < (Size >> 1))>>
    constexpr _variant_union(in_place_index_t<I>, Args &&...args)
        : _left(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<size_t I, class... Args, class = void,
             class = enable_if_t<!(I < (Size >> 1))>>
    constexpr _variant_union(in_place_index_t<I>, Args &&...args)
        : _rght(in_place_index_t<I - (Size >> 1)>{}, ala::forward<Args>(args)...) {
    }

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}
};

template<class... Ts>
union _variant_union<false, Ts...> {
    static constexpr size_t Size = sizeof...(Ts);
    static_assert(1 < Size, "Internal error");
    char _placehold;
    _make_union_t<make_integer_range<size_t, 0, (Size >> 1)>, false, Ts...> _left;
    _make_union_t<make_integer_range<size_t, (Size >> 1), Size>, false, Ts...> _rght;

    ~_variant_union(){};
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<size_t I, class... Args, class = enable_if_t<(I < (Size >> 1))>>
    constexpr _variant_union(in_place_index_t<I>, Args &&...args)
        : _left(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<size_t I, class... Args, class = void,
             class = enable_if_t<!(I < (Size >> 1))>>
    constexpr _variant_union(in_place_index_t<I>, Args &&...args)
        : _rght(in_place_index_t<I - (Size >> 1)>{}, ala::forward<Args>(args)...) {
    }

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}
};

template<class T>
union _variant_union<true, T> {
    T _value;
    char _placehold;

    ~_variant_union() = default;
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<class... Args>
    constexpr _variant_union(in_place_index_t<0>, Args &&...args)
        : _value(ala::forward<Args>(args)...) {}

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}
};

template<class T>
union _variant_union<false, T> {
    T _value;
    char _placehold;

    ~_variant_union(){};
    constexpr _variant_union(): _placehold{} {}
    constexpr _variant_union(_variant_union &&) = default;
    constexpr _variant_union(const _variant_union &) = default;
    constexpr _variant_union &operator=(const _variant_union &) = default;
    constexpr _variant_union &operator=(_variant_union &&) = default;

    template<class... Args>
    constexpr _variant_union(in_place_index_t<0>, Args &&...args)
        : _value(ala::forward<Args>(args)...) {}

    constexpr _variant_union(in_place_index_t<variant_npos>): _placehold{} {}
};

template<bool Trivial, class... Ts>
struct _union_op {
    static constexpr size_t Size = sizeof...(Ts);
    static_assert(1 < Size, "Internal error");

    using left_t =
        _make_union_op_t<make_integer_range<size_t, 0, (Size >> 1)>, Trivial, Ts...>;
    using rght_t = _make_union_op_t<make_integer_range<size_t, (Size >> 1), Size>,
                                    Trivial, Ts...>;

    template<class Fn, class Union>
    static constexpr auto fmap(size_t I, Fn &&fn, Union &&un)
        -> decltype(left_t::fmap(I, ala::forward<Fn>(fn),
                                 ala::forward<Union>(un)._left)) {
        static_assert(
            is_same<decltype(left_t::fmap(I, ala::forward<Fn>(fn),
                                          ala::forward<Union>(un)._left)),
                    decltype(rght_t::fmap(I, ala::forward<Fn>(fn),
                                          ala::forward<Union>(un)._rght))>::value,
            "Internal error");
        if (I < (Size >> 1))
            return left_t::fmap(I, ala::forward<Fn>(fn),
                                ala::forward<Union>(un)._left);
        assert(Size >> 1 <= I && I < Size);
        return rght_t::fmap(I - (Size >> 1), ala::forward<Fn>(fn),
                            ala::forward<Union>(un)._rght);
    }

    template<size_t I, class Fn, class Union>
    static constexpr auto fmap(Fn &&fn, Union &&un)
        -> enable_if_t<(I < (Size >> 1)),
                       decltype(left_t::template fmap<I>(
                           ala::forward<Fn>(fn), ala::forward<Union>(un)._left))> {
        return left_t::template fmap<I>(ala::forward<Fn>(fn),
                                        ala::forward<Union>(un)._left);
    }

    template<size_t I, class Fn, class Union>
    static constexpr auto fmap(Fn &&fn, Union &&un)
        -> enable_if_t<(Size >> 1 <= I && I < Size),
                       decltype(rght_t::template fmap<I - (Size >> 1)>(
                           ala::forward<Fn>(fn), ala::forward<Union>(un)._rght))> {
        return rght_t::template fmap<I - (Size >> 1)>(
            ala::forward<Fn>(fn), ala::forward<Union>(un)._rght);
    }
};

template<bool Trivial, class T>
struct _union_op<Trivial, T> {
    template<class Fn, class Union>
    static constexpr auto fmap(size_t I, Fn &&fn, Union &&un)
        -> decltype(ala::forward<Fn>(fn)(ala::forward<Union>(un)._value)) {
        assert(I == 0);
        return ala::forward<Fn>(fn)(ala::forward<Union>(un)._value);
    }

    template<size_t I, class Fn, class Union>
    static constexpr auto fmap(Fn &&fn, Union &&un)
        -> decltype(ala::forward<Fn>(fn)(ala::forward<Union>(un)._value)) {
        static_assert(I == 0);
        return ala::forward<Fn>(fn)(ala::forward<Union>(un)._value);
    }
};

template<bool Trivial, class... Ts>
struct _variant_destroy {
    static_assert(Trivial, "Internal error");
    _make_union_t<index_sequence_for<Ts...>, true, Ts...> _union;
    using _union_op_t = _make_union_op_t<index_sequence_for<Ts...>, true, Ts...>;
    size_t _index = variant_npos;
    ~_variant_destroy() = default;
    constexpr void _reset() {
        _index = variant_npos;
    }
    template<size_t I, class... Args>
    constexpr _variant_destroy(in_place_index_t<I>, Args &&...args)
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
    _make_union_t<index_sequence_for<Ts...>, false, Ts...> _union;
    using _union_op_t = _make_union_op_t<index_sequence_for<Ts...>, false, Ts...>;
    size_t _index = variant_npos;
    ~_variant_destroy() {
        this->_reset();
    }
    constexpr void _reset() {
        if (this->_index != variant_npos) {
            _union_op_t::fmap(
                this->_index,
                [](auto &&v) {
                    using T = remove_cvref_t<decltype(v)>;
                    v.~T();
                },
                _union);
            this->_index = variant_npos;
        }
    }
    template<size_t I, class... Args>
    constexpr _variant_destroy(in_place_index_t<I>, Args &&...args)
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
    using typename _base_t::_union_op_t;

    template<size_t I>
    constexpr const type_pack_element_t<I, Ts...> &_get() const noexcept {
        auto f = [](auto &&v) -> decltype(v) { return v; };
        return _union_op_t::template fmap<I>(f, _union);
    }

    template<size_t I>
    constexpr type_pack_element_t<I, Ts...> &_get() noexcept {
        auto f = [](auto &&v) -> decltype(v) { return v; };
        return _union_op_t::template fmap<I>(f, _union);
    }

    size_t _hash() const noexcept {
        return _union_op_t::fmap(
            this->_index,
            [](auto &&v) { return hash<remove_cvref_t<decltype(v)>>{}(v); },
            _union);
    }

    void *_address() {
        return _union_op_t::fmap(
            this->_index,
            [](auto &&v) {
                return const_cast<void *>(
                    reinterpret_cast<const volatile void *>(ala::addressof(v)));
            },
            _union);
    }

    template<size_t I, class... Args>
    void _ctor_v(in_place_index_t<I>, Args &&...args) {
        assert(this->_index == variant_npos);
        try {
            this->_index = I;
            using T = type_pack_element_t<I, Ts...>;
            ::new (this->_address()) T(ala::forward<Args>(args)...);
        } catch (...) {
            this->_index = variant_npos;
            throw;
        }
    }

    template<class VarBase>
    void _ctor(VarBase &&other) noexcept(
        conditional_t<is_lvalue_reference<VarBase>::value,
                      _and_<is_nothrow_copy_constructible<Ts>...>,
                      _and_<is_nothrow_move_constructible<Ts>...>>::value) {
        assert(this->_index == variant_npos);
        if (other._index != variant_npos) {
            try {
                this->_index = other._index;
                _union_op_t::fmap(
                    other._index,
                    [addr = _address()](auto &&v) {
                        using T = remove_cvref_t<decltype(v)>;
                        ::new (addr) T(ala::forward<decltype(v)>(v));
                    },
                    ala::forward<VarBase>(other)._union);
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
                using T = type_pack_element_t<I, Ts...>;
                *reinterpret_cast<T *>(_address()) = ala::forward<Arg>(arg);
            } catch (...) {
                throw;
            }
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
                _union_op_t::fmap(
                    other._index,
                    [addr = _address()](auto &&v) {
                        using T = remove_cvref_t<decltype(v)>;
                        *reinterpret_cast<T *>(addr) = v;
                    },
                    other._union);
            } catch (...) {
                throw;
            }
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
                _union_op_t::fmap(
                    other._index,
                    [addr = _address()](auto &&v) {
                        using T = remove_cvref_t<decltype(v)>;
                        *reinterpret_cast<T *>(addr) = ala::move(v);
                    },
                    ala::move(other)._union);
            } catch (...) {
                throw;
            }
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
                _union_op_t::fmap(
                    other._index,
                    [addr = _address()](auto &&v) {
                        using T = remove_cvref_t<decltype(v)>;
                        ala::_swap_adl(*reinterpret_cast<T *>(addr), v);
                    },
                    other._union);
            } catch (...) {
                throw;
            }
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

struct bad_variant_access: exception {
    bad_variant_access() noexcept {}
    virtual const char *what() const noexcept {
        return "bad_variant_access";
    }
};

template<size_t I, class... Us>
constexpr add_pointer_t<variant_alternative_t<I, variant<Us...>>>
get_if(variant<Us...> *v) noexcept;
template<size_t I, class... Us>
constexpr add_pointer_t<const variant_alternative_t<I, variant<Us...>>>
get_if(const variant<Us...> *v) noexcept;

template<size_t I, class... Us>
constexpr variant_alternative_t<I, variant<Us...>> &get(variant<Us...> &);
template<size_t I, class... Us>
constexpr variant_alternative_t<I, variant<Us...>> &&get(variant<Us...> &&);
template<size_t I, class... Us>
constexpr const variant_alternative_t<I, variant<Us...>> &
get(const variant<Us...> &);
template<size_t I, class... Us>
constexpr const variant_alternative_t<I, variant<Us...>> &&
get(const variant<Us...> &&);

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

    friend class hash<variant>;

    template<class Dst>
    static Dst _convert(Dst (&&)[1]);

    template<class Ti, class T, class = void>
    struct _is_no_narrow: false_type {};

    template<class Ti, class T>
    struct _is_no_narrow<Ti, T, void_t<decltype(_convert<Ti>({declval<T>()}))>>
        : true_type {};

    // is_arithmetic is necessary
    // see libcxx/test/std/utilities/variant/variant.variant/variant.ctor/T.pass.cpp
    template<size_t I, class Ti, class T,
             bool = (is_same<bool, remove_cvref_t<Ti>>::value !=
                     is_same<bool, remove_cvref_t<T>>::value) ||
                    _and_<is_arithmetic<Ti>, _not_<_is_no_narrow<Ti, T>>>::value>
    struct _overload_set_base {
        static integral_constant<size_t, I> _test(Ti);
    };

    template<size_t I, class Ti, class T>
    struct _overload_set_base<I, Ti, T, true> {
        static void _test();
    };

    template<class T, class Seq>
    struct _overload_set;

    template<class T, size_t... Is>
    struct _overload_set<T, index_sequence<Is...>>
        : _overload_set_base<Is, Ts, T>... {
        using _overload_set_base<Is, Ts, T>::_test...;
    };

    template<class T>
    using _fuzzy_i = decltype(_overload_set<T, index_sequence_for<Ts...>>::_test(
        declval<T>()));

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
    constexpr explicit variant(in_place_index_t<I>, Args &&...args)
        : _base_t(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<size_t I, class U, class... Args,
             class = enable_if_t<(I < sizeof...(Ts)) &&
                                 is_constructible<type_pack_element_t<I, Ts...>,
                                                  initializer_list<U> &, Args...>::value>>
    constexpr explicit variant(in_place_index_t<I>, initializer_list<U> il,
                               Args &&...args)
        : _base_t(in_place_index_t<I>{}, il, ala::forward<Args>(args)...) {}

    template<class T, class... Args, size_t I = type_pack_index<T, Ts...>::value,
             class = enable_if_t<is_constructible<T, Args...>::value>>
    constexpr explicit variant(in_place_type_t<T>, Args &&...args)
        : _base_t(in_place_index_t<I>{}, ala::forward<Args>(args)...) {}

    template<
        class T, class U, class... Args, size_t I = type_pack_index<T, Ts...>::value,
        class = enable_if_t<is_constructible<T, initializer_list<U> &, Args...>::value>>
    constexpr explicit variant(in_place_type_t<T>, initializer_list<U> il,
                               Args &&...args)
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
    emplace(Args &&...args) {
        this->_reset();
        this->_ctor_v(in_place_index_t<I>{}, ala::forward<Args>(args)...);
        return ala::get<I>(*this);
    }

    template<size_t I, class U, class... Args>
    enable_if_t<is_constructible<type_pack_element_t<I, Ts...>,
                                 initializer_list<U> &, Args...>::value,
                type_pack_element_t<I, Ts...> &>
    emplace(initializer_list<U> il, Args &&...args) {
        this->_reset();
        this->_ctor_v(in_place_index_t<I>{}, il, ala::forward<Args>(args)...);
        return ala::get<I>(*this);
    }

    // modifiers
    template<class T, class... Args>
    enable_if_t<is_constructible<T, Args...>::value, T &> emplace(Args &&...args) {
        return this->emplace<type_pack_index<T, Ts...>::value>(
            ala::forward<Args>(args)...);
    }

    template<class T, class U, class... Args>
    enable_if_t<is_constructible<T, initializer_list<U> &, Args...>::value, T &>
    emplace(initializer_list<U> il, Args &&...args) {
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
    return v.template _get<I>();
}

template<size_t I, class... Ts>
constexpr variant_alternative_t<I, variant<Ts...>> &&get(variant<Ts...> &&v) {
    if (I != v.index())
        throw bad_variant_access{};
    return ala::move(v.template _get<I>());
}

template<size_t I, class... Ts>
constexpr const variant_alternative_t<I, variant<Ts...>> &
get(const variant<Ts...> &v) {
    if (I != v.index())
        throw bad_variant_access{};
    return v.template _get<I>();
}

template<size_t I, class... Ts>
constexpr const variant_alternative_t<I, variant<Ts...>> &&
get(const variant<Ts...> &&v) {
    if (I != v.index())
        throw bad_variant_access{};
    return ala::move(v.template _get<I>());
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
    return ala::addressof(v->template _get<I>());
}

template<size_t I, class... Ts>
constexpr add_pointer_t<const variant_alternative_t<I, variant<Ts...>>>
get_if(const variant<Ts...> *v) noexcept {
    if (v == nullptr || I != v->index())
        return nullptr;
    return ala::addressof(v->template _get<I>());
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
        static constexpr auto _invoke_ol(int, Visitor1 &&vis, Variants1 &&...vars)
            -> decltype(ala::invoke(ala::forward<Visitor1>(vis),
                                    ala::get<Is>(ala::forward<Variants1>(vars))...)) {
            return ala::invoke(ala::forward<Visitor>(vis),
                               ala::get<Is>(ala::forward<Variants1>(vars))...);
        }

        template<class... Args>
        static constexpr void _invoke_ol(char, Args &&...) {
            assert(false);
        }

        static constexpr auto _invoke(Visitor &&vis, Variants &&...vars)
            -> decltype(_invoke_ol(0, ala::forward<Visitor>(vis),
                                   ala::forward<Variants>(vars)...)) {
            return _invoke_ol(0, ala::forward<Visitor>(vis),
                              ala::forward<Variants>(vars)...);
        }
    };

    template<size_t... Is>
    struct _dispatcher_r {
        template<class Visitor1, class... Variants1>
        static constexpr auto _invoke_ol(int, Visitor1 &&vis, Variants1 &&...vars)
            -> decltype(ala::invoke(ala::forward<Visitor1>(vis),
                                    ala::get<Is>(ala::forward<Variants1>(vars))...)) {
            return ala::invoke(ala::forward<Visitor>(vis),
                               ala::get<Is>(ala::forward<Variants1>(vars))...);
        }

        template<class... Args>
        static constexpr R _invoke_ol(char, Args &&...) {
            assert(false);
        }

        static constexpr R _invoke(Visitor &&vis, Variants &&...vars) {
            return _invoke_ol(0, ala::forward<Visitor>(vis),
                              ala::forward<Variants>(vars)...);
        }
    };

    template<size_t... Is>
    struct _dispatcher_rvoid {
        template<class Visitor1, class... Variants1>
        static constexpr auto _invoke_ol(int, Visitor1 &&vis, Variants1 &&...vars)
            -> decltype(ala::invoke(ala::forward<Visitor1>(vis),
                                    ala::get<Is>(ala::forward<Variants1>(vars))...)) {
            return ala::invoke(ala::forward<Visitor>(vis),
                               ala::get<Is>(ala::forward<Variants1>(vars))...);
        }

        template<class... Args>
        static constexpr R _invoke_ol(char, Args &&...) {
            assert(false);
        }

        static constexpr R _invoke(Visitor &&vis, Variants &&...vars) {
            _invoke_ol(0, ala::forward<Visitor>(vis),
                       ala::forward<Variants>(vars)...);
        }
    };

    template<size_t... Is>
    using dispatcher_t =
        conditional_t<is_void<R>::value,
                      _dispatcher_rvoid<Is...>,
                          conditional_t<is_same<R, _visit_no_return>::value,
                                        _dispatcher<Is...>, _dispatcher_r<Is...>>>;

    template<size_t... Is>
    static constexpr auto _make_visit(index_sequence<Is...>) {
        return dispatcher_t<Is...>::_invoke;
    }

    template<class Void, class... Ts>
    struct _has_common_type: false_type {};

    template<class... Ts>
    struct _has_common_type<void_t<common_type_t<Ts...>>, Ts...>: true_type {};

    template<class... Args>
    static constexpr auto _make_array(Args &&...args) {
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

    static constexpr auto _visit_helper(Visitor &&vis, Variants &&...vars)
        -> decltype(_visit_impl::_view(_visit_impl::_make_visit_array(),
                                       vars.index()...)(
            ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...)) {
        return _visit_impl::_view(_visit_impl::_make_visit_array(),
                                  vars.index()...)(ala::forward<Visitor>(vis),
                                                   ala::forward<Variants>(vars)...);
    }
};

template<class... Ts>
static constexpr auto _convert_to_variant(const variant<Ts...> &)
    -> variant<Ts...>;

template<class Variant, class = void>
struct _can_visit1: false_type {};

template<class Variant>
struct _can_visit1<Variant, void_t<decltype(ala::_convert_to_variant(declval<Variant>()))>>
    : true_type {
    using variant_type =
        _copy_cvref_t<Variant, decltype(ala::_convert_to_variant(declval<Variant>()))>;
};

template<class... Variants>
struct _can_visit: _and_<_can_visit1<Variants>...> {};

template<class R, class Visitor, class... Variants>
constexpr auto visit_r(Visitor &&vis, Variants &&...vars)
    -> enable_if_t<_can_visit<Variants...>::value, R> {
    return _visit_impl<R, Visitor, typename _can_visit1<Variants>::variant_type...>::_visit_helper(
        ala::forward<Visitor>(vis), ala::forward<Variants>(vars)...);
}

template<class R, class Visitor, class... Variants>
constexpr auto visit(Visitor &&vis, Variants &&...vars)
    -> enable_if_t<_can_visit<Variants...>::value, R> {
    return visit_r<R>(ala::forward<Visitor>(vis),
                      ala::forward<Variants>(vars)...);
}

template<class Visitor, class... Variants>
constexpr auto visit(Visitor &&vis, Variants &&...vars)
    -> decltype(_visit_impl<_visit_no_return, Visitor,
                            typename _can_visit1<Variants>::variant_type...>::
                    _visit_helper(ala::forward<Visitor>(vis),
                                  ala::forward<Variants>(vars)...)) {
    return _visit_impl<_visit_no_return, Visitor,
                       typename _can_visit1<Variants>::variant_type...>::
        _visit_helper(ala::forward<Visitor>(vis),
                      ala::forward<Variants>(vars)...);
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

// specialized algorithms
template<class... Ts>
enable_if_t<_and_<is_move_constructible<Ts>..., is_swappable<Ts>...>::value>
swap(variant<Ts...> &lhs, variant<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template<class... Ts>
struct hash<_sfinae_checker<
    variant<Ts...>, enable_if_t<_and_<_is_hashable<remove_const_t<Ts>>...>::value>>> {
    typedef variant<Ts...> argument_type;
    typedef size_t result_type;

    result_type operator()(const argument_type &var) const {
        using _union_op_t = typename _variant_base<Ts...>::_union_op_t;
        if (var.valueless_by_exception())
            return 0;
        else {
            size_t index_hash = hash<size_t>{}(var.index());
            size_t value_hash = var._hash();
            return index_hash ^ value_hash;
        }
    }
};

} // namespace ala

#ifdef _ALA_CLANG
    #pragma clang diagnostic pop
#endif

#endif