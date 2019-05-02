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

// extra feature

template<size_t I, typename Seq>
struct _get_int_seq_helper {};

template<size_t I, typename Int, Int I1, Int... Is>
struct _get_int_seq_helper<I, integer_sequence<Int, I1, Is...>>
    : _get_int_seq_helper<I - 1, integer_sequence<Int, Is...>> {};

template<typename Int, Int I1, Int... Is>
struct _get_int_seq_helper<0, integer_sequence<Int, I1, Is...>> {
    typedef integral_constant<Int, I1> type;
};

template<size_t I, typename IntSeq>
using get_integer_sequence = typename _get_int_seq_helper<I, IntSeq>::type;

template<typename Int, typename IntSeq, Int... Is>
struct _reverse_int_seq_impl {};

template<typename Int, Int I, Int... Is, Int... SIs>
struct _reverse_int_seq_impl<Int, integer_sequence<Int, SIs...>, I, Is...> {
    typedef typename _reverse_int_seq_impl<Int, integer_sequence<Int, I, SIs...>,
                                           Is...>::type type;
};

template<typename Int, Int... SIs>
struct _reverse_int_seq_impl<Int, integer_sequence<Int, SIs...>> {
    typedef integer_sequence<Int, SIs...> type;
};

template<typename Int, Int... Is>
struct _reverse_int_seq_helper {};

template<typename Int, Int... Is>
struct _reverse_int_seq_helper<integer_sequence<Int, Is...>> {
    typedef
        typename _reverse_int_seq_impl<Int, integer_sequence<Int>, Is...>::type type;
};

template<typename IntSeq>
using reverse_integer_sequence = typename _reverse_int_seq_helper<IntSeq>::type;

template<typename Void, typename... IntSeq>
struct _cat_int_seq_helper {};

template<typename Int, Int... Is>
struct _cat_int_seq_helper<void, integer_sequence<Int, Is...>> {
    typedef integer_sequence<Int, Is...> type;
};

template<typename Int, Int... Is, Int... Js>
struct _cat_int_seq_helper<void, integer_sequence<Int, Is...>,
                           integer_sequence<Int, Js...>> {
    typedef integer_sequence<Int, Is..., Js...> type;
};

template<typename IntSeq, typename... IntSeqs>
struct _cat_int_seq_helper<void_t<typename _cat_int_seq_helper<IntSeqs...>::type>,
                           IntSeq, IntSeqs...>
    : _cat_int_seq_helper<IntSeq, typename _cat_int_seq_helper<IntSeqs...>::type> {
    static_assert(sizeof...(IntSeqs) > 1, "internal error");
};

template<typename... IntSeq>
using cat_integer_sequence = typename _cat_int_seq_helper<void, IntSeq...>::type;

// [start, end]
template<typename Int, Int Start, Int End, Int Step, typename IntSeq>
struct _make_int_range_impl;

template<typename Int, Int Start, Int End, Int Step, Int... Is>
struct _make_int_range_impl<Int, Start, End, Step, integer_sequence<Int, Is...>> {
    static_assert(Start != End, "internal error");
    static constexpr Int Next = Start > End ? Start - Step : Start + Step;
    typedef typename _make_int_range_impl<
        Int, Next, End, Step, integer_sequence<Int, Is..., Start>>::type type;
};

template<typename Int, Int End, Int Step, Int... Is>
struct _make_int_range_impl<Int, End, End, Step, integer_sequence<Int, Is...>> {
    typedef integer_sequence<Int, Is..., End> type;
};

template<typename, typename>
struct _cat2_int_seq;

template<typename Int, Int... Is, Int... Js>
struct _cat2_int_seq<integer_sequence<Int, Is...>, integer_sequence<Int, Js...>> {
    typedef integer_sequence<Int, Is..., Js...> type;
};

// [start, end)
template<typename Int, Int Start, Int End, Int Step, bool = (End - Start > Step)>
struct _make_int_range_helper {
    // static constexpr Int Dis = End > Start ? End - Start : Start - End;
    // static constexpr Int Fix = Dis % Step == 0 ? Step : Dis % Step;
    // shrink range rather than expand range
    // static constexpr Int EndFix = End > Start ? End - Fix : End + Fix;
    // typedef typename _make_int_range_impl<Int, Start, EndFix, Step,
    //                                       integer_sequence<Int>>::type type;
    // typedef integer_sequence<Int, Start> type;
    static constexpr Int Dis = (End - Start) / 2;
    static constexpr Int Mid = Start + Dis;
    static constexpr Int MidFix = Mid + (Dis % Step);
    typedef typename _cat2_int_seq<
        typename _make_int_range_helper<Int, Start, Mid, Step>::type,
        typename _make_int_range_helper<Int, MidFix, End, Step>::type>::type
        type;
};

template<typename Int, Int Start, Int End, Int Step>
struct _make_int_range_helper<Int, Start, End, Step, false> {
    typedef conditional_t<(End > Start), integer_sequence<Int, Start>, integer_sequence<Int>> type;
};

// template<typename Int, Int Start, Int End, Int Step>
// struct _make_int_range_helper<Int, Start, End, Step> {
//     static constexpr Int Dis = (End > Start ? End - Start : Start - End) / 2;
//     static constexpr Int Mid = End > Start ? Start + Dis : Start - Dis;
//     static constexpr Int Fix = Dis % Step == 0 ? Step : Dis % Step;
//     static constexpr Int MidFix = End > Start ? Mid - Fix : Mid + Fix;
//     typedef cat_integer_sequence<
//         typename _make_int_range_helper<Int, Start, MidFix, Step>::type,
//         typename _make_int_range_helper<Int, MidFix, End, Step>::type>
//         type;
// };

// template<typename Int, Int Start, Int End, Int Step>
// struct _make_int_range_helper<Int, Start, End, Step, true> {

//     typedef typename _cat2_int_seq<
//         typename _make_int_range_helper<Int, Start, (Start + End) / 2, Step>::type,
//         typename _make_int_range_helper<Int, (Start + End) / 2, End, Step>::type>::type
//         type;
// };

template<typename Int, Int Start, Int End, Int Step = 1>
using make_integer_range =
    typename _make_int_range_helper<Int, Start, End, (Step > 0 ? Step : -Step)>::type;

// make_integer_sequence
// template<typename Int>
// struct _make_int_seq_impl {
//     template<Int N, typename IntSeq>
//     struct _impl;

//     template<Int N, Int... Is>
//     struct _impl<N, integer_sequence<Int, Is...>> {
//         typedef
//             typename _impl<N - 1, integer_sequence<Int, N - 1, Is...>>::type type;
//     };

//     template<Int... Is>
//     struct _impl<0, integer_sequence<Int, Is...>> {
//         typedef integer_sequence<Int, Is...> type;
//     };
// };

// template<typename Int, Int N>
// using make_integer_sequence = typename _make_int_seq_impl<
//     Int>::template _impl<N, integer_sequence<Int>>::type;

template<typename Int, Int N>
using make_integer_sequence = make_integer_range<Int, 0, N>;

template<size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template<typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

} // namespace ala

#endif // HEAD