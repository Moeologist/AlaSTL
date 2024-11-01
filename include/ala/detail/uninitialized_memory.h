#ifndef _ALA_DETAIL_UNINITIALIZED_MEMORY_H
#define _ALA_DETAIL_UNINITIALIZED_MEMORY_H

#include <ala/config.h>
#include <ala/detail/pair.h>
#include <ala/detail/memory_base.h>
#include <ala/iterator.h>

namespace ala {

template<class ForwardIter>
constexpr void destroy(ForwardIter first, ForwardIter last) {
    for (; first != last; ++first)
        ala::destroy_at(ala::addressof(*first));
}

template<class ForwardIter, class Size>
ForwardIter destroy_n(ForwardIter first, Size count) {
    auto n = ala::_convert_to_integral(count);
    for (; n > 0; (void)++first, --n)
        ala::destroy_at(ala::addressof(*first));
    return first;
}

template<class ForwardIter>
void uninitialized_default_construct(ForwardIter first, ForwardIter last) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    ForwardIter i = first;
    try {
        for (; i != last; ++i)
            ::new (ala::_voidify(*i)) T;
    } catch (...) {
        ala::destroy(first, i);
        throw;
    }
    return i;
}

template<class ForwardIter, class Size>
ForwardIter uninitialized_default_construct_n(ForwardIter first, Size count) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    ForwardIter i = first;
    try {
        for (; n > 0; ++i, (void)--n)
            ::new (ala::_voidify(*i)) T;
    } catch (...) {
        ala::destroy(first, i);
        throw;
    }
    return i;
}

template<class ForwardIter>
void uninitialized_value_construct(ForwardIter first, ForwardIter last) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    ForwardIter i = first;
    try {
        for (; i != last; ++i)
            ::new (ala::_voidify(*i)) T();
    } catch (...) {
        ala::destroy(first, i);
        throw;
    }
    return i;
}

template<class ForwardIter, class Size>
ForwardIter uninitialized_value_construct_n(ForwardIter first, Size count) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    ForwardIter i = first;
    try {
        for (; n > 0; ++i, (void)--n)
            ::new (ala::_voidify(*i)) T();
    } catch (...) {
        ala::destroy(first, i);
        throw;
    }
    return i;
}

template<class ForwardIter, class T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T &x) {
    ForwardIter i = first;
    try {
        for (; i != last; ++i)
            ::new (ala::_voidify(*i)) T(x);
    } catch (...) {
        ala::destroy(first, i);
        throw;
    }
    return i;
}

template<class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size count, const T &x) {
    auto n = ala::_convert_to_integral(count);
    ForwardIter i = first;
    try {
        for (; n > 0; ++i, (void)--n)
            ::new (ala::_voidify(*i)) T(x);
    } catch (...) {
        ala::destroy(first, i);
        throw;
    }
    return i;
}

template<class InputIter, class ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    ForwardIter i = out;
    try {
        for (; first != last; ++first, (void)++i)
            ::new (ala::_voidify(*i)) T(*first);
    } catch (...) {
        ala::destroy(out, i);
        throw;
    }
    return i;
}

template<class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size count, ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    ForwardIter i = out;
    try {
        for (; n > 0; ++i, ++first, (void)--n)
            ::new (ala::_voidify(*i)) T(*first);
    } catch (...) {
        ala::destroy(out, i);
        throw;
    }
    return i;
}

template<class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    ForwardIter i = out;
    try {
        for (; first != last; ++first, (void)++i)
            ::new (ala::_voidify(*i)) T(ala::move(*first));
    } catch (...) {
        ala::destroy(out, i);
        throw;
    }
    return i;
}

template<class InputIter, class Size, class ForwardIter>
pair<InputIter, ForwardIter> uninitialized_move_n(InputIter first, Size count,
                                                  ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    ForwardIter i = out;
    try {
        for (; n > 0; ++i, ++first, (void)--n)
            ::new (ala::_voidify(*i)) T(ala::move(*first));
    } catch (...) {
        ala::destroy(out, i);
        throw;
    }
    return i;
}

} // namespace ala

#endif // _ALA_DETAIL_UNINITIALIZED_COPY_H
