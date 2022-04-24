#ifndef _ALA_DETAIL_INTEGER_SEQUENCE_H
#define _ALA_DETAIL_INTEGER_SEQUENCE_H

#include <ala/type_traits.h>

namespace ala {

template<typename Int, Int... Is>
struct integer_sequence {
    typedef Int value_type;
    static constexpr size_t size() noexcept {
        return sizeof...(Is);
    }
};

template<size_t... Is>
using index_sequence = integer_sequence<size_t, Is...>;

template<typename Void, typename... IntSeq>
struct _cat_int_seq_helper {};

template<typename Int, Int... Is>
struct _cat_int_seq_helper<void, integer_sequence<Int, Is...>> {
    using type = integer_sequence<Int, Is...>;
};

template<typename Int, Int... Is, Int... Js>
struct _cat_int_seq_helper<void, integer_sequence<Int, Is...>,
                           integer_sequence<Int, Js...>> {
    using type = integer_sequence<Int, Is..., Js...>;
};

template<typename IntSeq, typename... IntSeqs>
struct _cat_int_seq_helper<
    void_t<typename _cat_int_seq_helper<void, IntSeqs...>::type>, IntSeq, IntSeqs...>
    : _cat_int_seq_helper<void, IntSeq,
                          typename _cat_int_seq_helper<void, IntSeqs...>::type> {
};

template<typename... IntSeq>
using cat_integer_sequence = typename _cat_int_seq_helper<void, IntSeq...>::type;

template<typename IntSeq, size_t N, size_t Rem>
struct _twice_int_seq;

template<typename Int, Int... Is, size_t N>
struct _twice_int_seq<integer_sequence<Int, Is...>, N, 0> {
    using type = integer_sequence<Int, Is..., (Is + N)...>;
};

template<typename Int, Int... Is, size_t N>
struct _twice_int_seq<integer_sequence<Int, Is...>, N, 1> {
    using type = integer_sequence<Int, Is..., (Is + N)..., 2 * N>;
};

template<typename Int, size_t N>
struct _make_int_seq {
    using type = typename _twice_int_seq<typename _make_int_seq<Int, N / 2>::type,
                                         N / 2, N % 2>::type;
};

template<typename Int>
struct _make_int_seq<Int, 0> {
    using type = integer_sequence<Int>;
};

#if _ALA_ENABLE_MAKE_INTEGER_SEQ

template<typename Int, Int N>
using make_integer_sequence = __make_integer_seq<integer_sequence, Int, N>;

#else

template<typename Int, Int N>
using make_integer_sequence = typename _make_int_seq<Int, N>::type;

#endif

template<size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template<typename IntSeq, template<typename IntSeq::value_type> class... UnaryTemplt>
struct _foreach_int_seq;

template<typename Int, Int... Is>
struct _foreach_int_seq<integer_sequence<Int, Is...>> {
    using type = integer_sequence<Int, Is...>;
};

template<typename Int, Int... Is, template<Int> class UnaryTemplt,
         template<Int> class... UnaryTemplts>
struct _foreach_int_seq<integer_sequence<Int, Is...>, UnaryTemplt, UnaryTemplts...> {
    using type = typename _foreach_int_seq<
        integer_sequence<Int, UnaryTemplt<Is>::value...>, UnaryTemplts...>::type;
};

template<typename Int, Int Lhs>
struct _int_seq_op {
    template<Int Rhs>
    struct add {
        static constexpr Int value = Lhs + Rhs;
    };
    template<Int Rhs>
    struct mul {
        static constexpr Int value = Lhs * Rhs;
    };
    template<Int Rhs>
    struct sub {
        static constexpr Int value = Lhs - Rhs;
    };
};

template<typename Int, Int Start, Int End, Int Step, bool = (End < Start)>
struct _make_int_range_impl;

template<typename Int, Int Start, Int End, Int Step>
struct _make_int_range_impl<Int, Start, End, Step, true>
    : _foreach_int_seq<typename _make_int_range_impl<Int, End, Start, Step>::type,
                       _int_seq_op<Int, Start + End>::template sub> {};

template<typename Int, Int Start, Int End, Int Step>
struct _make_int_range_impl<Int, Start, End, Step, false>
    : _foreach_int_seq<make_integer_sequence<Int, (End - Start + Step - 1) / Step>,
                       _int_seq_op<Int, Step>::template mul,
                       _int_seq_op<Int, Start>::template add> {
    static_assert(Step > 0, "Step must greater than zero");
};

template<typename Int, Int Start, Int End, Int Step = 1>
using make_integer_range =
    typename _make_int_range_impl<Int, Start, End, (Step >= 0 ? Step : -Step)>::type;

template<typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

template<size_t... Is>
struct ndim_indexer;

template<size_t I, size_t... Is>
struct ndim_indexer<I, Is...> {
    static constexpr size_t ndsize =
        _prod_<integral_constant<size_t, 1>, integral_constant<size_t, Is>...>::value;

    template<size_t Flat, class IntSeq>
    struct _flat2nd_impl;

    template<size_t Flat, size_t... Idx>
    struct _flat2nd_impl<Flat, index_sequence<Idx...>> {
        static_assert(Flat / ndsize < I, "Index out of range");
        using type = typename ndim_indexer<Is...>::template _flat2nd_impl<
            Flat % ndsize, index_sequence<Idx..., Flat / ndsize>>::type;
    };

    template<size_t Flat>
    using flat2nd = typename _flat2nd_impl<Flat, index_sequence<>>::type;

    template<size_t Idx, size_t... Idxs>
    struct nd2flat
        : integral_constant<size_t, ndim_indexer<Is...>::template nd2flat<Idxs...>::value +
                                        Idx * ndsize> {
        static_assert(Idx < I, "Index out of range");
    };
};

template<>
struct ndim_indexer<> {
    template<size_t flat, class IntSeq>
    struct _flat2nd_impl;

    template<size_t flat, size_t... Idx>
    struct _flat2nd_impl<flat, index_sequence<Idx...>> {
        using type = index_sequence<Idx...>;
    };

    template<size_t flat>
    using flat2nd = typename _flat2nd_impl<flat, index_sequence<>>::type;

    template<size_t...>
    struct nd2flat: integral_constant<size_t, 0> {};
};

} // namespace ala

#endif // HEAD