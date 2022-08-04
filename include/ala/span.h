#ifndef _ALA_SPAN_H
#define _ALA_SPAN_H

#include <ala/detail/stddef.h>
#include <ala/iterator.h>
#include <ala/ranges.h>

namespace ala {

template<class T, size_t N>
struct array;

// constants
ALA_INLINE_CONSTEXPR_V size_t dynamic_extent = numeric_limits<size_t>::max();

template<class T, size_t Extent = dynamic_extent>
class span {
public:
    // constants and types
    using element_type = T;
    using value_type = remove_cv_t<T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = element_type *;
    using const_pointer = const element_type *;
    using reference = element_type &;
    using const_reference = const element_type &;
    using iterator = pointer;
    using reverse_iterator = reverse_iterator<iterator>;
    static constexpr size_type extent = Extent;

    // constructors, copy, and assignment
    template<size_t E = Extent, class = enable_if_t<E == dynamic_extent || E == 0>>
    constexpr span() noexcept: _data(nullptr), _size(0) {}

    template<class It,
             class = enable_if_t<
                 contiguous_iterator<It> &&
                 is_convertible<remove_reference_t<iter_reference_t<It>> (*)[],
                                element_type (*)[]>::value &&
                 Extent == dynamic_extent>>
    constexpr span(It first, size_type count)
        : _data(ala::to_address(first)), _size(count) {}

    template<class It, class End,
             class = enable_if_t<
                 contiguous_iterator<It> && sized_sentinel_for<End, It> &&
                 is_convertible<remove_reference_t<iter_reference_t<It>> (*)[],
                                element_type (*)[]>::value &&
                 !is_convertible<End, size_t>::value && Extent == dynamic_extent>>
    constexpr span(It first, End last)
        : _data(ala::to_address(first)), _size(last - first) {}

    template<class It,
             class = enable_if_t<
                 contiguous_iterator<It> &&
                 is_convertible<remove_reference_t<iter_reference_t<It>> (*)[],
                                element_type (*)[]>::value &&
                 Extent != dynamic_extent>,
             char = 0>
    constexpr explicit span(It first, size_type count)
        : _data(ala::to_address(first)), _size(count) {}

    template<class It, class End,
             class = enable_if_t<
                 contiguous_iterator<It> && sized_sentinel_for<End, It> &&
                 is_convertible<remove_reference_t<iter_reference_t<It>> (*)[],
                                element_type (*)[]>::value &&
                 !is_convertible<End, size_t>::value && Extent != dynamic_extent>,
             char = 0>
    constexpr explicit span(It first, End last)
        : _data(ala::to_address(first)), _size(last - first) {}

    template<
        size_t N,
        class = enable_if_t<
            is_convertible<remove_pointer_t<decltype(ala::data(
                               declval<type_identity_t<element_type> (&)[N]>()))> (*)[],
                           element_type (*)[]>::value &&
            (extent == dynamic_extent || N == extent)>>
    constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
        : _data(ala::data(arr)), _size(N) {}

    template<
        class T1, size_t N,
        class = enable_if_t<
            is_convertible<remove_pointer_t<decltype(ala::data(
                               declval<type_identity_t<element_type> (&)[N]>()))> (*)[],
                           element_type (*)[]>::value &&
            (extent == dynamic_extent || N == extent)>>
    constexpr span(array<T1, N> &arr) noexcept
        : _data(ala::data(arr)), _size(N) {}

    template<
        class T1, size_t N,
        class = enable_if_t<
            is_convertible<remove_pointer_t<decltype(ala::data(
                               declval<type_identity_t<element_type> (&)[N]>()))> (*)[],
                           element_type (*)[]>::value &&
            (extent == dynamic_extent || N == extent)>>
    constexpr span(const array<T1, N> &arr) noexcept
        : _data(ala::data(arr)), _size(N) {}

    template<class T1>
    struct _array_or_span: false_type {};

    template<class T1, size_t Size>
    struct _array_or_span<array<T1, Size>>: true_type {};

    template<class T1, size_t Size>
    struct _array_or_span<span<T1, Size>>: true_type {};

#if ALA_USE_CONCEPTS
    template<class R>
        requires ranges::contiguous_range<R> && ranges::sized_range<R> &&
                 (ranges::borrowed_range<R> || is_const_v<element_type>) &&
                 (!_array_or_span<remove_cvref_t<R>>::value) &&
                 (!is_array_v<remove_cvref_t<R>>) &&
                 is_convertible_v<
                     remove_reference_t<ranges::range_reference_t<R>> (*)[],
                     element_type (*)[]>
    constexpr explicit(extent != dynamic_extent) span(R &&r)
        : span(ranges::data(r), ranges::size(r)) {
        (void)(extent != dynamic_extent || ranges::size(r) == Extent);
    }
#else
    template<class R,
             class = enable_if_t<
                 ranges::contiguous_range<R> && ranges::sized_range<R> &&
                 (ranges::borrowed_range<R> || is_const_v<element_type>)&&(
                     !_array_or_span<remove_cvref_t<R>>::value) &&
                 (!is_array_v<remove_cvref_t<R>>)&&is_convertible_v<
                     remove_reference_t<ranges::range_reference_t<R>> (*)[],
                     element_type (*)[]>>,
             size_type Dummy = extent, class = enable_if_t<(Dummy != dynamic_extent)>>
    constexpr explicit span(R &&r): span(ranges::data(r), ranges::size(r)) {
        (void)(extent != dynamic_extent || ranges::size(r) == Extent);
    }

    template<class R,
             class = enable_if_t<
                 ranges::contiguous_range<R> && ranges::sized_range<R> &&
                 (ranges::borrowed_range<R> || is_const_v<element_type>)&&(
                     !_array_or_span<remove_cvref_t<R>>::value) &&
                 (!is_array_v<remove_cvref_t<R>>)&&is_convertible_v<
                     remove_reference_t<ranges::range_reference_t<R>> (*)[],
                     element_type (*)[]>>,
             size_type Dummy = extent,
             class = enable_if_t<(Dummy == dynamic_extent)>, class = void>
    constexpr span(R &&r): span(ranges::data(r), ranges::size(r)) {
        (void)(extent != dynamic_extent || ranges::size(r) == Extent);
    }
#endif

    constexpr span(const span &other) noexcept = default;

    template<class T1, size_t Extent1,
             class = enable_if_t<
                 (extent == dynamic_extent || Extent1 == dynamic_extent || extent == Extent1) &&
                 is_convertible<typename span<T1, Extent1>::element_type (*)[],
                                element_type (*)[]>::value &&
                 !(extent != dynamic_extent && Extent1 == dynamic_extent)>,
             char = 0>
    constexpr span(const span<T1, Extent1> &s) noexcept
        : _data(s.data()), _size(s.size()) {}

    template<class T1, size_t Extent1,
             class = enable_if_t<
                 (extent == dynamic_extent || Extent1 == dynamic_extent || extent == Extent1) &&
                 is_convertible<typename span<T1, Extent1>::element_type (*)[],
                                element_type (*)[]>::value &&
                 (extent != dynamic_extent && Extent1 == dynamic_extent)>>
    constexpr explicit span(const span<T1, Extent1> &s) noexcept
        : _data(s.data()), _size(s.size()) {}

    ~span() noexcept = default;

    constexpr span &operator=(const span &other) noexcept = default;

    // subviews
    template<size_t Count>
    constexpr span<element_type, Count> first() const {
        static_assert(Count <= extent);
        return span<element_type, Count>{data(), Count};
    }

    template<size_t Count>
    constexpr span<element_type, Count> last() const {
        static_assert(Count <= extent);
        return span<element_type, Count>{data() + (size() - Count), Count};
    }

    template<size_t Offset, size_t Count = dynamic_extent>
    constexpr span<element_type, (Count != dynamic_extent ?
                                      Count :
                                      (Extent != dynamic_extent ? Extent - Offset :
                                                                  dynamic_extent))>
    subspan() const noexcept {
        static_assert(Offset <= Extent &&
                      (Count == dynamic_extent || Count <= Extent - Offset));
        return span<element_type, (Count != dynamic_extent ?
                                       Count :
                                       (Extent != dynamic_extent ? Extent - Offset :
                                                                   dynamic_extent))>{
            data() + Offset,
            (Count != dynamic_extent ? Count : this->size() - Offset)};
    }

    constexpr span<element_type, dynamic_extent> first(size_type count) const {
        assert(count <= extent);
        return span<element_type, dynamic_extent>{data(), count};
    }

    constexpr span<element_type, dynamic_extent> last(size_type count) const {
        assert(count <= extent);
        return span<element_type, dynamic_extent>{data() + (size() - count),
                                                  count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(size_type offset, size_type count = dynamic_extent) const noexcept {
        assert(offset <= Extent &&
               (count == dynamic_extent || count <= Extent - offset));
        return span<element_type, dynamic_extent>{data() + offset,
                                                  (count != dynamic_extent ?
                                                       count :
                                                       this->size() - offset)};
    }

    // observers
    constexpr size_type size() const noexcept {
        return _size;
    }

    constexpr size_type size_bytes() const noexcept {
        return size() * sizeof(element_type);
    }

    ALA_NODISCARD constexpr bool empty() const noexcept {
        return size() == 0;
    }

    // element access
    constexpr reference operator[](size_type idx) const {
        return *(data() + idx);
    }

    constexpr reference front() const {
        return *data();
    }

    constexpr reference back() const {
        return *(data() + (size() - 1));
    }

    constexpr pointer data() const noexcept {
        return _data;
    }
    // iterator support
    constexpr iterator begin() const noexcept {
        return data();
    }
    constexpr iterator end() const noexcept {
        return data() + size();
    }
    constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }
    constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
    }

private:
    pointer _data;   // exposition only
    size_type _size; // exposition only
};

template<class T, size_t Extent>
ALA_INLINE_CONSTEXPR_V bool ranges::enable_borrowed_range<span<T, Extent>> = true;

// views of object representation
template<class T, size_t Extent>
span<const byte, Extent == dynamic_extent ? dynamic_extent : sizeof(T) * Extent>
as_bytes(span<T, Extent> s) noexcept {
    using R = span<const byte, Extent == dynamic_extent ? dynamic_extent :
                                                          sizeof(T) * Extent>;
    return R{reinterpret_cast<const byte *>(s.data()), s.size_bytes()};
}

template<class T, size_t Extent>
span<byte, Extent == dynamic_extent ? dynamic_extent : sizeof(T) * Extent>
as_writable_bytes(span<T, Extent> s) noexcept {
    using R =
        span<byte, Extent == dynamic_extent ? dynamic_extent : sizeof(T) * Extent>;
    return R{reinterpret_cast<byte *>(s.data()), s.size_bytes()};
}

#if _ALA_ENABLE_DEDUCTION_GUIDES

template<class It, class EndOrSize>
span(It, EndOrSize) -> span<remove_reference_t<iter_reference_t<It>>>;
template<class R>
span(R &&) -> span<remove_reference_t<ranges::range_reference_t<R>>>;

template<class T, size_t N>
span(T (&)[N]) -> span<T, N>;
template<class T, size_t N>
span(array<T, N> &) -> span<T, N>;
template<class T, size_t N>
span(const array<T, N> &) -> span<const T, N>;

#endif

} // namespace ala

#endif