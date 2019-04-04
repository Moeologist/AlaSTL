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
template<size_t I, typename Int, Int... Is>
struct _get_integer_sequence_impl {};

template<size_t I, typename Int, Int I1, Int... Is>
struct _get_integer_sequence_impl<I, Int, I1, Is...> {
    typedef typename _get_integer_sequence_impl<I - 1, Int, Is...>::type type;
};

template<typename Int, Int I1>
struct _get_integer_sequence_impl<0, Int, I1> {
    typedef integral_constant<Int, I1> type;
};

template<size_t I, typename Seq>
struct get_integer_sequence;

template<size_t I, typename Int, Int... Is>
struct get_integer_sequence<I, integer_sequence<Int, Is...>>
    : _get_integer_sequence_impl<sizeof...(Is) - 1 - I, Int, Is...>::type {
    static_assert(I + 1 <= sizeof...(Is), "out of range");
};

template<typename Int, typename IntSeq, Int... Is>
struct _reverse_integer_sequence_impl;

template<typename Int, Int I, Int... Is, Int... SIs>
struct _reverse_integer_sequence_impl<Int, integer_sequence<Int, SIs...>, I, Is...> {
    typedef typename _reverse_integer_sequence_impl<
        Int, integer_sequence<Int, I, SIs...>, Is...>::type type;
};

template<typename Int, Int... SIs>
struct _reverse_integer_sequence_impl<Int, integer_sequence<Int, SIs...>> {
    typedef integer_sequence<Int, SIs...> type;
};

template<typename Int, Int... Is>
struct _reverse_integer_sequence_helper;

template<typename Int, Int... Is>
struct _reverse_integer_sequence_helper<integer_sequence<Int, Is...>> {
    typedef typename _reverse_integer_sequence_impl<Int, integer_sequence<Int>,
                                                    Is...>::type type;
};

template<typename IntSeq>
using reverse_integer_sequence =
    typename _reverse_integer_sequence_helper<IntSeq>::type;

// [start, end]
template<typename Int, Int Start, Int End, int Step, typename IntSeq>
struct _make_int_range_impl;

template<typename Int, Int Start, Int End, int Step, Int... Is>
struct _make_int_range_impl<Int, Start, End, Step, integer_sequence<Int, Is...>> {
    static_assert(Start != End, "internal error");
    static constexpr Int Next = Start > End ? Start - Step : Start + Step;
    typedef typename _make_int_range_impl<
        Int, Next, End, Step, integer_sequence<Int, Is..., Start>>::type type;
};

template<typename Int, Int End, int Step, Int... Is>
struct _make_int_range_impl<Int, End, End, Step, integer_sequence<Int, Is...>> {
    typedef integer_sequence<Int, Is..., End> type;
};

// [start, end)
template<typename Int, Int Start, Int End, Int Step>
struct _make_int_range_helper {
    static constexpr Int Dis = End > Start ? End - Start : Start - End;
    static constexpr Int StepAbs = Step > 0 ? Step : -Step;
    static constexpr Int Fix = Dis % StepAbs == 0 ? StepAbs : Dis % StepAbs;
    // shrink range rather than expand range
    static constexpr Int EndFix = End > Start ? End - Fix : End + Fix;
    typedef typename _make_int_range_impl<Int, Start, EndFix, StepAbs,
                                          integer_sequence<Int>>::type type;
};

template<typename Int, Int Start, Int Step>
struct _make_int_range_helper<Int, Start, Start, Step> {
    typedef integer_sequence<Int> type;
};

template<typename Int, Int Start, Int End, Int Step = 1>
using make_integer_range =
    typename _make_int_range_helper<Int, Start, End, Step>::type;

// make_integer_sequence ...
// template<typename Int, Int N, typename IntSeq>
// struct _make_integer_sequence_impl;

// template<typename Int, Int N, Int... Is>
// struct _make_integer_sequence_impl<Int, N, integer_sequence<Int, Is...>> {
//     typedef typename _make_integer_sequence_impl<
//         Int, N - 1, integer_sequence<Int, N - 1, Is...>>::type type;
// };

// template<typename Int, Int... Is>
// struct _make_integer_sequence_impl<Int, 0, integer_sequence<Int, Is...>> {
//     typedef integer_sequence<Int, Is...> type;
// };

template<typename Int, Int N>
using make_integer_sequence = make_integer_range<Int, 0, N>;

template<size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template<typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

} // namespace ala

#endif // HEAD