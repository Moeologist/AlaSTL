#ifndef _ALA_DETAIL_UNINITIALIZED_MEMORY_H
#define _ALA_DETAIL_UNINITIALIZED_MEMORY_H

#include <ala/config.h>
#include <ala/detail/pair.h>
#include <ala/detail/memory_base.h>
#include <ala/iterator.h>

namespace ala {

template<class ForwardIter>
void uninitialized_default_construct(ForwardIter first, ForwardIter last) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    for (; first != last; ++first)
        ::new (static_cast<void *>(ala::addressof(*first))) T;
    return first;
}

template<class ForwardIter, class Size>
ForwardIter uninitialized_default_construct_n(ForwardIter first, Size count) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    for (; n > 0; ++first, (void)--n)
        ::new (static_cast<void *>(ala::addressof(*first))) T;
    return first;
}

template<class ForwardIter>
void uninitialized_value_construct(ForwardIter first, ForwardIter last) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    for (; first != last; ++first)
        ::new (static_cast<void *>(ala::addressof(*first))) T();
    return first;
}

template<class ForwardIter, class Size>
ForwardIter uninitialized_value_construct_n(ForwardIter first, Size count) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    for (; n > 0; ++first, (void)--n)
        ::new (static_cast<void *>(ala::addressof(*first))) T();
    return first;
}

template<class InputIter, class ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    for (; first != last; ++first, (void)++out)
        ::new (static_cast<void *>(ala::addressof(*out))) T(*first);
    return out;
}

template<class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size count, ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    for (; n > 0; ++out, ++first, (void)--n)
        ::new (static_cast<void *>(ala::addressof(*out))) T(*first);
    return out;
}

template<class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    for (; first != last; ++first, (void)++out)
        ::new (static_cast<void *>(ala::addressof(*out))) T(ala::move(*first));
    return out;
}

template<class InputIter, class Size, class ForwardIter>
pair<InputIter, ForwardIter> uninitialized_move_n(InputIter first, Size count,
                                                  ForwardIter out) {
    using T = typename iterator_traits<ForwardIter>::value_type;
    auto n = ala::_convert_to_integral(count);
    for (; n > 0; ++out, ++first, (void)--n)
        ::new (static_cast<void *>(ala::addressof(*out))) T(ala::move(*first));
    return out;
}

template<class ForwardIter, class T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T &x) {
    for (; first != last; ++first)
        ::new (static_cast<void *>(ala::addressof(*first))) T(x);
    return first;
}

template<class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size count, const T &x) {
    auto n = ala::_convert_to_integral(count);
    for (; n > 0; ++first, (void)--n)
        ::new (static_cast<void *>(ala::addressof(*first))) T(x);
    return first;
}

template<class T>
constexpr enable_if_t<is_array<T>::value> destroy_at(T *p) {
    for (auto &elem : *p)
        destroy_at(ala::addressof(elem));
}

template<class T>
constexpr enable_if_t<!is_array<T>::value> destroy_at(T *p) {
    p->~T();
}

template<class T, class... Args>
constexpr T *construct_at(T *p, Args &&... args) {
    return ::new (const_cast<void *>(static_cast<const volatile void *>(p)))
        T(ala::forward<Args>(args)...);
}

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

} // namespace ala

#endif // _ALA_DETAIL_UNINITIALIZED_COPY_H
