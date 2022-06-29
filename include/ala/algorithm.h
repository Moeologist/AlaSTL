#ifndef _ALA_ALGORITHM_H
#define _ALA_ALGORITHM_H

#include <ala/utility.h>
#include <ala/random.h>
#include <ala/detail/algorithm_base.h>
#include <ala/detail/sort.h>
#include <ala/detail/allocator.h>
#include <ala/detail/uninitialized_memory.h>

namespace ala {

// Non-modifying sequence operations
template<class InputIter, class UnaryPred>
constexpr InputIter find_if_not(InputIter first, InputIter last, UnaryPred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            return first;
    return last;
}

template<class InputIter, class UnaryPred>
constexpr bool all_of(InputIter first, InputIter last, UnaryPred pred) {
    return ala::find_if_not(first, last, pred) == last;
}

template<class InputIter, class UnaryPred>
constexpr bool any_of(InputIter first, InputIter last, UnaryPred pred) {
    return ala::find_if(first, last, pred) != last;
}

template<class InputIter, class UnaryPred>
constexpr bool none_of(InputIter first, InputIter last, UnaryPred pred) {
    return ala::find_if(first, last, pred) == last;
}

template<class InputIter, class Fn>
constexpr Fn for_each(InputIter first, InputIter last, Fn f) {
    for (; first != last; ++first)
        f(*first);
    return f;
}

template<class InputIter, class Size, class Fn>
constexpr InputIter for_each_n(InputIter first, Size count, Fn f) {
    auto n = ala::_convert_to_integral(count);
    for (decltype(n) i = 0; i < n; ++first, (void)++i)
        f(*first);
    return first;
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2,
                              BinPred pred) {
    for (;; ++first1) {
        ForwardIter1 i = first1;
        for (ForwardIter2 j = first2;; ++i, (void)++j) {
            if (j == last2)
                return first1;
            if (i == last1)
                return last1;
            if (!pred(*i, *j))
                break;
        }
    }
}

template<class ForwardIter1, class ForwardIter2>
constexpr ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2) {
    return ala::search(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIter, class Searcher>
constexpr ForwardIter search(ForwardIter first, ForwardIter last,
                             const Searcher &searcher) {
    return searcher(first, last).first;
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                                ForwardIter2 first2, ForwardIter2 last2,
                                BinPred pred) {
    if (first2 == last2)
        return last1;
    ForwardIter1 tmp = last1;
    while (true) {
        ForwardIter1 pos = ala::search(first1, last1, first2, last2, pred);
        if (pos == last1) {
            break;
        } else {
            tmp = first1 = pos;
            ++first1;
        }
    }
    return tmp;
}

template<class ForwardIter1, class ForwardIter2>
constexpr ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                                ForwardIter2 first2, ForwardIter2 last2) {
    return ala::find_end(first1, last1, first2, last2, equal_to<>());
}

template<class InputIter, class ForwardIter, class BinPred>
constexpr InputIter find_first_of(InputIter first1, InputIter last1,
                                  ForwardIter first2, ForwardIter last2,
                                  BinPred pred) {
    for (; first1 != last1; ++first1)
        for (ForwardIter i = first2; i != last2; ++i)
            if (pred(*first1, *i))
                return first1;
    return last1;
}

template<class InputIter, class ForwardIter>
constexpr InputIter find_first_of(InputIter first1, InputIter last1,
                                  ForwardIter first2, ForwardIter last2) {
    return ala::find_first_of(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIter, class BinPred>
constexpr ForwardIter adjacent_find(ForwardIter first, ForwardIter last,
                                    BinPred pred) {
    if (first == last)
        return last;
    ForwardIter next = first;
    for (++next; next != last; ++next, (void)++first)
        if (pred(*first, *next))
            return first;
    return last;
}

template<class ForwardIter>
constexpr ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
    return ala::adjacent_find(first, last, ala::equal_to<>());
}

template<class InputIter, class T>
constexpr typename iterator_traits<InputIter>::difference_type
count(InputIter first, InputIter last, const T &value) {
    using diff_t = typename iterator_traits<InputIter>::difference_type;
    diff_t n = 0;
    for (; first != last; ++first)
        if (*first == value)
            ++n;
    return n;
}

template<class InputIter, class UnaryPred>
constexpr typename iterator_traits<InputIter>::difference_type
count_if(InputIter first, InputIter last, UnaryPred pred) {
    using diff_t = typename iterator_traits<InputIter>::difference_type;
    diff_t n = 0;
    for (; first != last; ++first)
        if (pred(*first))
            n++;
    return n;
}

template<class InputIter1, class InputIter2, class BinPred>
constexpr pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, BinPred pred) {
    while (first1 != last1 && pred(*first1, *first2))
        ++first1, (void)++first2;
    return ala::make_pair(first1, first2);
}

template<class InputIter1, class InputIter2>
constexpr pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
    return ala::mismatch(first1, last1, first2, equal_to<>());
}

template<class InputIter1, class InputIter2, class BinPred>
constexpr pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2,
         InputIter2 last2, BinPred pred) {
    while (first1 != last1 && first2 != last2 && pred(*first1, *first2))
        ++first1, (void)++first2;
    return ala::make_pair(first1, first2);
}

template<class InputIter1, class InputIter2>
constexpr pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2,
         InputIter2 last2) {
    return ala::mismatch(first1, last1, first2, last2, equal_to<>());
}

template<class ForwardIter, class Size, class T, class BinPred>
constexpr ForwardIter search_n(ForwardIter first, ForwardIter last, Size count,
                               const T &value, BinPred pred) {
    auto n = ala::_convert_to_integral(count);
    if (n < 1)
        return first;
    for (; first != last; ++first) {
        if (!pred(*first, value))
            continue;
        ForwardIter tmp = first;
        decltype(n) i = 0;
        while (true) {
            ++i;
            if (!(i < n))
                return tmp;
            ++first;
            if (first == last)
                return last;
            if (!pred(*first, value))
                break;
        }
    }
    return last;
}

template<class ForwardIter, class Size, class T>
constexpr ForwardIter search_n(ForwardIter first, ForwardIter last, Size count,
                               const T &value) {
    return ala::search_n(first, last, count, value, equal_to<>());
}

// Modifying sequence operations

template<class InputIter, class OutputIter, class UnaryPred>
constexpr OutputIter copy_if(InputIter first, InputIter last, OutputIter out,
                             UnaryPred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            *out++ = *first;
    return out;
}

template<class InputIter, class OutputIter, class UnaryOperation>
constexpr OutputIter transform(InputIter first, InputIter last, OutputIter out,
                               UnaryOperation unary_op) {
    while (first != last)
        *out++ = unary_op(*first++);
    return out;
}

template<class InputIter1, class InputIter2, class OutputIter, class BinaryOperation>
constexpr OutputIter transform(InputIter1 first1, InputIter1 last1,
                               InputIter2 first2, OutputIter out,
                               BinaryOperation binary_op) {
    while (first1 != last1)
        *out++ = binary_op(*first1++, *first2++);
    return out;
}

template<class ForwardIter, class T>
constexpr void replace(ForwardIter first, ForwardIter last, const T &old_value,
                       const T &new_value) {
    for (; first != last; ++first)
        if (*first == old_value)
            *first = new_value;
}

template<class ForwardIter, class UnaryPred, class T>
constexpr void replace_if(ForwardIter first, ForwardIter last, UnaryPred pred,
                          const T &new_value) {
    for (; first != last; ++first)
        if (pred(*first))
            *first = new_value;
}

template<class InputIter, class OutputIter, class T>
constexpr OutputIter replace_copy(InputIter first, InputIter last, OutputIter out,
                                  const T &old_value, const T &new_value) {
    for (; first != last; ++first)
        *out++ = (*first == old_value) ? new_value : *first;
    return out;
}

template<class InputIter, class OutputIter, class UnaryPred, class T>
constexpr OutputIter replace_copy_if(InputIter first, InputIter last,
                                     OutputIter out, UnaryPred pred,
                                     const T &new_value) {
    for (; first != last; ++first)
        *out++ = pred(*first) ? new_value : *first;
    return out;
}

template<class OutputIter, class Size, class T>
constexpr OutputIter fill_n(OutputIter first, Size count, const T &value) {
    auto n = ala::_convert_to_integral(count);
    for (; 0 < n; --n)
        *first++ = value;
    return first;
}

template<class ForwardIter, class Generator>
constexpr void generate(ForwardIter first, ForwardIter last, Generator gen) {
    for (; first != last; ++first)
        *first = gen();
}

template<class OutputIter, class Size, class Generator>
constexpr OutputIter generate_n(OutputIter first, Size count, Generator gen) {
    auto n = ala::_convert_to_integral(count);
    for (; 0 < n; --n)
        *first++ = gen();
    return first;
}

template<class InputIter, class OutputIter, class T>
constexpr OutputIter remove_copy(InputIter first, InputIter last,
                                 OutputIter out, const T &value) {
    for (; first != last; ++first)
        if (!(*first == value))
            *out++ = *first;
    return out;
}

template<class InputIter, class OutputIter, class UnaryPred>
constexpr OutputIter remove_copy_if(InputIter first, InputIter last,
                                    OutputIter out, UnaryPred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            *out++ = *first;
    return out;
}

template<class ForwardIter, class BinPred>
constexpr ForwardIter unique(ForwardIter first, ForwardIter last, BinPred pred) {
    if (first == last)
        return last;
    for (ForwardIter next = first; ++next != last;)
        if (!pred(*first, *next) && ++first != next)
            *first = ala::move(*next);
    return ++first;
}

template<class ForwardIter>
constexpr ForwardIter unique(ForwardIter first, ForwardIter last) {
    return ala::unique(first, last, equal_to<>());
}

template<class InputIter, class ForwardIter, class BinPred>
constexpr ForwardIter _unique_copy_dispatch(InputIter first, InputIter last,
                                            ForwardIter out, BinPred pred,
                                            false_type, true_type) {
    if (first != last) {
        *out = *first;
        while (++first != last)
            if (!pred(*out, *first))
                *++out = *first;
        ++out;
    }
    return out;
}

template<class ForwardIter, class OutputIter, class BinPred>
constexpr OutputIter _unique_copy_dispatch(ForwardIter first, ForwardIter last,
                                           OutputIter out, BinPred pred,
                                           true_type, ...) {
    if (first != last) {
        ForwardIter t = first;
        *out++ = *t;
        while (++first != last) {
            if (!pred(*t, *first)) {
                t = first;
                *out++ = *first;
            }
        }
    }
    return out;
}

template<class InputIter, class OutputIter, class BinPred>
constexpr OutputIter _unique_copy_dispatch(InputIter first, InputIter last,
                                           OutputIter out, BinPred pred,
                                           false_type, false_type) {
    using T = typename iterator_traits<InputIter>::value_type;
    if (first != last) {
        T tmp(*first);
        *out++ = tmp;
        while (++first != last) {
            if (!pred(tmp, *first)) {
                tmp = *first;
                *out++ = tmp;
            }
        }
    }
    return out;
}

template<class InputIter, class OutputIter, class BinPred>
constexpr OutputIter unique_copy(InputIter first, InputIter last,
                                 OutputIter out, BinPred pred) {
    using itag_t = is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>;
    using otag_t = is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>;
    return ala::_unique_copy_dispatch(first, last, out, pred, itag_t(), otag_t());
}

template<class ForwardIter, class OutputIter>
constexpr OutputIter unique_copy(ForwardIter first, ForwardIter last,
                                 OutputIter out) {
    return ala::unique_copy(first, last, out, equal_to<>());
}

template<class BidirIter>
constexpr void reverse(BidirIter first, BidirIter last) {
    while ((first != last) && (first != --last))
        ala::iter_swap(first++, last);
}

template<class BidirIter, class OutputIter>
constexpr OutputIter reverse_copy(BidirIter first, BidirIter last,
                                  OutputIter out) {
    while (first != last)
        *out++ = *--last;
    return out;
}

template<class ForwardIter, class OutputIter>
constexpr OutputIter rotate_copy(ForwardIter first, ForwardIter middle,
                                 ForwardIter last, OutputIter out) {
    return ala::copy(first, middle, ala::copy(middle, last, out));
}

template<class RandomIter, class URBG>
constexpr void shuffle(RandomIter first, RandomIter last, URBG &&g) {
    using diff_t = typename iterator_traits<RandomIter>::difference_type;
    using ud_t = uniform_int_distribution<diff_t>;
    using param_t = typename ud_t::param_type;
    diff_t len = last - first;
    if (len < 2)
        return;
    ud_t uid;
    for (--last, (void)--len; first < last; ++first, (void)--len) {
        diff_t offset = uid(g, param_t(0, len));
        if (offset != diff_t(0))
            ala::iter_swap(first, first + offset);
    }
}

template<class ForwardIter, class OutputIter, class Distance, class URBG>
constexpr OutputIter _sample_dispatch(ForwardIter first, ForwardIter last,
                                      OutputIter out, Distance n, URBG &&g,
                                      input_iterator_tag) {
    Distance i = 0;
    for (; first != last && i < n; ++first, (void)++i)
        *(out + i) = *first;
    for (Distance len = i; first != last; ++first, (void)++i) {
        Distance r = ala::uniform_int_distribution<Distance>(0, i)(g);
        if (r < len)
            *(out + r) = *first;
    }
    return out + ala::min(n, i);
}

template<class ForwardIter, class OutputIter, class Distance, class URBG>
constexpr OutputIter _sample_dispatch(ForwardIter first, ForwardIter last,
                                      OutputIter out, Distance n, URBG &&g,
                                      forward_iterator_tag) {
    Distance len = ala::distance(first, last);
    for (n = ala::min(n, len); n != 0; ++first) {
        Distance r = ala::uniform_int_distribution<Distance>(0, --len)(g);
        if (r < n) {
            *out++ = *first;
            --n;
        }
    }
    return out;
}

template<class ForwardIter, class OutputIter, class Distance, class URBG>
constexpr OutputIter sample(ForwardIter first, ForwardIter last, OutputIter out,
                            Distance n, URBG &&g) {
    using tag_t = _iter_tag_t<ForwardIter>;
    return ala::_sample_dispatch(first, last, out, n, ala::forward<URBG>(g),
                                 tag_t{});
}

// Partitioning operations
template<class InputIter, class UnaryPred>
constexpr bool is_partitioned(InputIter first, InputIter last, UnaryPred pred) {
    for (; first != last; ++first)
        if (!pred(*first)) {
            ++first;
            break;
        }
    for (; first != last; ++first)
        if (pred(*first))
            return false;
    return true;
}

template<class ForwardIter, class UnaryPred>
constexpr ForwardIter partition(ForwardIter first, ForwardIter last,
                                UnaryPred pred) {
    first = ala::find_if_not(first, last, pred);
    if (first == last)
        return last;
    ForwardIter i = first;
    for (++i; i != last; ++i)
        if (pred(*i))
            ala::iter_swap(i, first++);
    return first;
}

template<class BidirIter, class UnaryPred>
constexpr BidirIter stable_partition(BidirIter first, BidirIter last,
                                     UnaryPred pred) {
    using T = typename iterator_traits<BidirIter>::value_type;
    auto len = ala::distance(first, last);
    allocator<T> alloc;
    pointer_holder<T *, allocator<T>> ph(alloc, len);
    T *tmp = ph.get();
    auto out = tmp;

    first = ala::find_if_not(first, last, pred);
    if (first == last)
        return first;

    BidirIter i = first;
    ala::construct_at(out++, ala::move(*first));
    for (++i; i != last; ++i)
        if (pred(*i))
            *first++ = ala::move(*i);
        else
            ala::construct_at(out++, ala::move(*i));

    ala::move(tmp, out, first);
    ala::destroy(tmp, out);
    return first;
}

template<class InputIter, class OutputIter1, class OutputIter2, class UnaryPred>
constexpr pair<OutputIter1, OutputIter2>
partition_copy(InputIter first, InputIter last, OutputIter1 out_true,
               OutputIter2 out_false, UnaryPred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            *out_true++ = *first;
        else
            *out_false++ = *first;
    return ala::pair<OutputIter1, OutputIter2>(out_true, out_false);
}

template<class ForwardIter, class UnaryPred>
constexpr ForwardIter partition_point(ForwardIter first, ForwardIter last,
                                      UnaryPred pred) {
    using diff_t = typename iterator_traits<ForwardIter>::difference_type;
    diff_t len = ala::distance(first, last);

    while (len > 0) {
        ForwardIter i = first;
        diff_t step = len / 2;
        ala::advance(i, step);
        if (pred(*i)) {
            first = ++i;
            len -= step + 1;
        } else
            len = step;
    }
    return first;
}

// Sorting operations (see ala/detail/sort.h)
template<class ForwardIter, class Comp>
constexpr ForwardIter is_sorted_until(ForwardIter first, ForwardIter last,
                                      Comp comp) {
    if (first != last) {
        ForwardIter next = first;
        while (++next != last) {
            if (comp(*next, *first))
                return next;
            first = next;
        }
    }
    return last;
}

template<class ForwardIter>
constexpr ForwardIter is_sorted_until(ForwardIter first, ForwardIter last) {
    return ala::is_sorted_until(first, last, less<>());
}

template<class ForwardIter, class Comp>
constexpr bool is_sorted(ForwardIter first, ForwardIter last, Comp comp) {
    return ala::is_sorted_until(first, last, comp) == last;
}

template<class ForwardIter>
constexpr bool is_sorted(ForwardIter first, ForwardIter last) {
    return ala::is_sorted(first, last, less<>());
}

// Set operations (on sorted ranges)
template<class InputIter1, class InputIter2, class Comp>
constexpr bool includes(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                        InputIter2 last2, Comp comp) {
    for (; first2 != last2; ++first1) {
        if (first1 == last1 || comp(*first2, *first1))
            return false;
        if (!comp(*first1, *first2))
            ++first2;
    }
    return true;
}

template<class InputIter1, class InputIter2>
constexpr bool includes(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                        InputIter2 last2) {
    return ala::includes(first1, last1, first2, last2, less<>());
}

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter set_union(InputIter1 first1, InputIter1 last1,
                               InputIter2 first2, InputIter2 last2,
                               OutputIter out, Comp comp) {
    for (; first1 != last1; ++out) {
        if (first2 == last2)
            return ala::copy(first1, last1, out);
        if (comp(*first2, *first1)) {
            *out = *first2++;
        } else {
            if (!comp(*first1, *first2))
                ++first2;
            *out = *first1++;
        }
    }
    return ala::copy(first2, last2, out);
}

template<class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_union(InputIter1 first1, InputIter1 last1,
                               InputIter2 first2, InputIter2 last2,
                               OutputIter out) {
    return ala::set_union(first1, last1, first2, last2, out, less<>());
}

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
                                      InputIter2 first2, InputIter2 last2,
                                      OutputIter out, Comp comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } else {
            if (!comp(*first2, *first1)) {
                *out++ = *first1++;
            }
            ++first2;
        }
    }
    return out;
}

template<class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
                                      InputIter2 first2, InputIter2 last2,
                                      OutputIter out) {
    return ala::set_intersection(first1, last1, first2, last2, out, less<>());
}

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                                    InputIter2 first2, InputIter2 last2,
                                    OutputIter out, Comp comp) {
    while (first1 != last1) {
        if (first2 == last2)
            return ala::copy(first1, last1, out);
        if (comp(*first1, *first2)) {
            *out++ = *first1++;
        } else {
            if (!comp(*first2, *first1)) {
                ++first1;
            }
            ++first2;
        }
    }
    return out;
}

template<class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                                    InputIter2 first2, InputIter2 last2,
                                    OutputIter out) {
    return ala::set_difference(first1, last1, first2, last2, out, less<>());
}

template<class InputIter1, class InputIter2, class OutputIter, class Comp>
constexpr OutputIter
set_symmetric_difference(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                         InputIter2 last2, OutputIter out, Comp comp) {
    while (first1 != last1) {
        if (first2 == last2)
            return ala::copy(first1, last1, out);
        if (comp(*first1, *first2)) {
            *out++ = *first1++;
        } else {
            if (comp(*first2, *first1)) {
                *out++ = *first2;
            } else {
                ++first1;
            }
            ++first2;
        }
    }
    return ala::copy(first2, last2, out);
}

template<class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter
set_symmetric_difference(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                         InputIter2 last2, OutputIter out) {
    return ala::set_symmetric_difference(first1, last1, first2, last2, out,
                                         less<>());
}

template<class T, class Comp>
constexpr pair<const T &, const T &> minmax(const T &a, const T &b, Comp comp) {
    return comp(b, a) ? ala::pair<const T &, const T &>(b, a) :
                        ala::pair<const T &, const T &>(a, b);
}

template<class T>
constexpr pair<const T &, const T &> minmax(const T &a, const T &b) {
    return ala::minmax(a, b, less<>());
}

template<class ForwardIter, class Comp>
constexpr ForwardIter min_element(ForwardIter first, ForwardIter last, Comp comp) {
    static_assert(is_base_of<forward_iterator_tag, _iter_tag_t<ForwardIter>>::value,
                  "ala::min_element need Forward Iterator");
    if (first == last)
        return last;
    ForwardIter min = first;
    for (++first; first != last; ++first) {
        if (comp(*first, *min))
            min = first;
    }
    return min;
}

template<class ForwardIter>
constexpr ForwardIter min_element(ForwardIter first, ForwardIter last) {
    return ala::min_element(first, last, less<>());
}

template<class ForwardIter, class Comp>
constexpr ForwardIter max_element(ForwardIter first, ForwardIter last, Comp comp) {
    static_assert(is_base_of<forward_iterator_tag, _iter_tag_t<ForwardIter>>::value,
                  "ala::max_element need Forward Iterator");
    if (first == last)
        return last;
    ForwardIter max = first;
    for (++first; first != last; ++first) {
        if (comp(*max, *first))
            max = first;
    }
    return max;
}

template<class ForwardIter>
constexpr ForwardIter max_element(ForwardIter first, ForwardIter last) {
    return ala::max_element(first, last, less<>());
}

template<class ForwardIter, class Comp>
constexpr pair<ForwardIter, ForwardIter>
minmax_element(ForwardIter first, ForwardIter last, Comp comp) {
    static_assert(is_base_of<forward_iterator_tag, _iter_tag_t<ForwardIter>>::value,
                  "ala::minmax_element need Forward Iterator");
    if (first == last)
        return pair<ForwardIter, ForwardIter>(last, last);
    ForwardIter min = first;
    ForwardIter max = first;
    for (++first; first != last; ++first) {
        ForwardIter prev = first;
        if (++first == last) {
            if (comp(*prev, *min))
                min = prev;
            else if (!(comp(*prev, *max)))
                max = prev;
            break;
        } else {
            if (comp(*first, *prev)) {
                if (comp(*first, *min))
                    min = first;
                if (!(comp(*prev, *max)))
                    max = prev;
            } else {
                if (comp(*prev, *min))
                    min = prev;
                if (!(comp(*first, *max)))
                    max = first;
            }
        }
    }
    return ala::make_pair(min, max);
}

template<class ForwardIter>
constexpr pair<ForwardIter, ForwardIter> minmax_element(ForwardIter first,
                                                        ForwardIter last) {
    return ala::minmax_element(first, last, less<>());
}

template<class T, class Comp>
constexpr T min(initializer_list<T> t, Comp comp) {
    return *ala::min_element(t.begin(), t.end(), comp);
}

template<class T>
constexpr T min(initializer_list<T> t) {
    return ala::min(t, less<>());
}

template<class T, class Comp>
constexpr T max(initializer_list<T> t, Comp comp) {
    return *ala::max_element(t.begin(), t.end(), comp);
}

template<class T>
constexpr T max(initializer_list<T> t) {
    return ala::max(t, less<>());
}

template<class T, class Comp>
constexpr pair<T, T> minmax(initializer_list<T> t, Comp comp) {
    using iter_t = typename initializer_list<T>::iterator;
    pair<iter_t, iter_t> pr = ala::minmax_element(t.begin(), t.end(), comp);
    return pair<T, T>(*pr.first, *pr.second);
}

template<class T>
constexpr pair<T, T> minmax(initializer_list<T> t) {
    return ala::minmax(t, less<>());
}

template<class T, class Compare>
constexpr const T &clamp(const T &v, const T &lo, const T &hi, Compare comp) {
    return assert(!comp(hi, lo)), comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
    return ala::clamp(v, lo, hi, less<>());
}

// Permutation operations
template<class BidirIter, class Comp>
constexpr bool prev_permutation(BidirIter first, BidirIter last, Comp comp) {
    BidirIter i = last;
    if (first == last || first == --i)
        return false;
    while (true) {
        BidirIter i1 = i;
        if (comp(*i1, *--i)) {
            BidirIter j = last;
            while (!comp(*--j, *i))
                ;
            ala::_swap_adl(*i, *j);
            ala::reverse(i1, last);
            return true;
        }
        if (i == first) {
            ala::reverse(first, last);
            return false;
        }
    }
}

template<class BidirIter>
constexpr bool prev_permutation(BidirIter first, BidirIter last) {
    return ala::prev_permutation(first, last, less<>());
}

template<class BidirIter, class Comp>
constexpr bool next_permutation(BidirIter first, BidirIter last, Comp comp) {
    BidirIter i = last;
    if (first == last || first == --i)
        return false;
    while (true) {
        BidirIter i1 = i;
        if (comp(*--i, *i1)) {
            BidirIter j = last;
            while (!comp(*i, *--j))
                ;
            ala::_swap_adl(*i, *j);
            ala::reverse(i1, last);
            return true;
        }
        if (i == first) {
            ala::reverse(first, last);
            return false;
        }
    }
}

template<class BidirIter>
constexpr bool next_permutation(BidirIter first, BidirIter last) {
    return ala::next_permutation(first, last, less<>());
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool _is_permutation_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                                        ForwardIter2 first2, ForwardIter2 last2,
                                        BinPred pred, false_type) {
    using T = typename iterator_traits<ForwardIter1>::value_type;
    auto p = ala::mismatch(first1, last1, first2, last2, pred);
    first1 = p.first;
    first2 = p.second;
    ForwardIter1 i = first1;
    ForwardIter2 j = first2;
    for (; i != last1 && j != last2; ++i, (void)++j) {
        auto unary = [&](const T &x) { return pred(x, *i); };
        if (i != ala::find_if(first1, i, unary))
            continue;
        auto c = ala::count_if(first2, last2, unary);
        if (c == 0 || c != ala::count_if(first1, last1, unary))
            return false;
    }
    if (i == last1 && j == last2)
        return true;
    return false;
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool _is_permutation_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                                        ForwardIter2 first2, ForwardIter2 last2,
                                        BinPred pred, true_type) {
    if (ala::distance(first1, last1) == ala::distance(first2, last2))
        return ala::_is_permutation_dispatch(first1, last1, first2, last2, pred,
                                             false_type{});
    return false;
}

template<class ForwardIter1, class ForwardIter2>
constexpr bool _is_permutation_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                                        ForwardIter2 first2, ForwardIter2 last2,
                                        false_type) {
    using T = typename iterator_traits<ForwardIter1>::value_type;
    auto p = ala::mismatch(first1, last1, first2, last2);
    first1 = p.first;
    first2 = p.second;
    ForwardIter1 i = first1;
    ForwardIter2 j = first2;
    for (; i != last1 && j != last2; ++i, (void)++j) {
        if (i != ala::find(first1, i, *i))
            continue;
        auto c = ala::count(first2, last2, *i);
        if (c == 0 || c != ala::count(first1, last1, *i))
            return false;
    }
    if (i == last1 && j == last2)
        return true;
    return false;
}

template<class ForwardIter1, class ForwardIter2>
constexpr bool _is_permutation_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                                        ForwardIter2 first2, ForwardIter2 last2,
                                        true_type) {
    if (ala::distance(first1, last1) == ala::distance(first2, last2))
        return ala::_is_permutation_dispatch(first1, last1, first2, last2,
                                             false_type{});
    return false;
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2,
                              BinPred pred) {
    using tag_t =
        _and_<is_base_of<random_access_iterator_tag, _iter_tag_t<ForwardIter1>>,
              is_base_of<random_access_iterator_tag, _iter_tag_t<ForwardIter2>>>;
    return ala::_is_permutation_dispatch(first1, last1, first2, last2, pred,
                                         tag_t{});
}

template<class ForwardIter1, class ForwardIter2>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2) {
    using tag_t =
        _and_<is_base_of<random_access_iterator_tag, _iter_tag_t<ForwardIter1>>,
              is_base_of<random_access_iterator_tag, _iter_tag_t<ForwardIter2>>>;
    return ala::_is_permutation_dispatch(first1, last1, first2, last2, tag_t{});
}

template<class ForwardIter1, class ForwardIter2, class BinPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, BinPred pred) {
    ForwardIter2 last2 = first2;
    ala::advance(last2, ala::distance(first1, last1));
    return ala::is_permutation(first1, last1, first2, last2, pred);
}

template<class ForwardIter1, class ForwardIter2>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2) {
    ForwardIter2 last2 = first2;
    ala::advance(last2, ala::distance(first1, last1));
    return ala::is_permutation(first1, last1, first2, last2);
}

template<class ForwardIter>
constexpr ForwardIter
shift_left(ForwardIter first, ForwardIter last,
           typename iterator_traits<ForwardIter>::difference_type n) {
    using diff_t = typename iterator_traits<ForwardIter>::difference_type;
    diff_t len = ala::distance(first, last);
    if (!(0 < n))
        return last;
    if (!(n < len))
        return first;
    ForwardIter mid = first;
    ala::advance(mid, n);
    return ala::move(mid, last, first);
}

template<class BidirIter>
constexpr BidirIter
_shift_right_dispatch(BidirIter first, BidirIter last,
                      typename iterator_traits<BidirIter>::difference_type n,
                      bidirectional_iterator_tag) {
    using diff_t = typename iterator_traits<BidirIter>::difference_type;
    diff_t len = ala::distance(first, last);
    if (!(0 < n))
        return first;
    if (!(n < len))
        return last;
    BidirIter mid = first;
    ala::advance(mid, len - n);
    return ala::move_backward(first, mid, last);
}

template<class ForwardIter>
constexpr ForwardIter
_shift_right_dispatch(ForwardIter first, ForwardIter last,
                      typename iterator_traits<ForwardIter>::difference_type n,
                      forward_iterator_tag) {
    using diff_t = typename iterator_traits<ForwardIter>::difference_type;
    diff_t len = ala::distance(first, last);
    if (!(0 < n))
        return first;
    if (!(n < len))
        return last;
    ForwardIter mid = first;
    ala::advance(mid, n);

    auto left = first;
    auto right = mid;
    while (left != mid) {
        if (right == last) {
            ala::move(first, left, mid);
            return mid;
        }
        ++left;
        ++right;
    }

    ForwardIter i = first;
    while (true) {
        if (right == last) {
            left = ala::move(i, mid, left);
            ala::move(first, i, left);
            return mid;
        }
        ala::_swap_adl(*i++, *left++);
        ++right;
        if (i == mid)
            i = first;
    }
}

template<class ForwardIter>
constexpr ForwardIter
shift_right(ForwardIter first, ForwardIter last,
            typename iterator_traits<ForwardIter>::difference_type n) {
    using tag_t = _iter_tag_t<ForwardIter>;
    return ala::_shift_right_dispatch(first, last, n, tag_t{});
}

} // namespace ala

#endif // HEAD