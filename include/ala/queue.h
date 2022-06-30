#ifndef _ALA_QUEUE_H
#define _ALA_QUEUE_H

#include <ala/ring.h>
#include <ala/vector.h>
#include <ala/detail/sort.h>

namespace ala {
template<class T, class Container = ring<T>>
class queue {
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;
    static_assert((is_same<T, value_type>::value),
                  "Container::value_type not same as T");

protected:
    Container c;

public:
    queue(): queue(Container()) {}
    explicit queue(const Container &d): c(d) {}
    explicit queue(Container &&d): c(ala::move(d)) {}
    template<class InputIter>
    queue(InputIter first, InputIter last): c(first, last) {}

    // queue(const queue &other): c(other.c) {}
    // queue(queue &&other): c(ala::move(other.c)) {}

    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    explicit queue(const Alloc &a): c(a) {}
    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    queue(const Container &d, const Alloc &a): c(d, a) {}
    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    queue(Container &&d, const Alloc &a): c(ala::move(d), a) {}

    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    queue(const queue &other, const Alloc &a): c(other.c, a) {}
    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    queue(queue &&other, const Alloc &a): c(ala::move(other.c), a) {}

    template<class InputIter, class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    queue(InputIter first, InputIter last, const Alloc &a): c(first, last, a) {}

    // queue &operator=(const queue &other) noexcept(
    //     is_nothrow_copy_assignable<Container>::value) {
    //     this->c = other.c;
    //     return *this;
    // }

    // queue &operator=(queue &&other) noexcept(
    //     is_nothrow_move_assignable<Container>::value) {
    //     this->c = ala::move(other.c);
    //     return *this;
    // }

    ALA_NODISCARD bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }
    reference front() {
        return c.front();
    }
    const_reference front() const {
        return c.front();
    }
    reference back() {
        return c.back();
    }
    const_reference back() const {
        return c.back();
    }
    void push(const value_type &x) {
        c.push_back(x);
    }
    void push(value_type &&x) {
        c.push_back(ala::move(x));
    }
    template<class... Args>
    decltype(auto) emplace(Args &&...args) {
        return c.emplace_back(ala::forward<Args>(args)...);
    }
    void pop() {
        c.pop_front();
    }
    void swap(queue &s) noexcept(is_nothrow_swappable<Container>::value) {
        using ala::swap;
        swap(c, s.c);
    }

    template<class T1, class Container1>
    friend bool operator==(const queue<T1, Container1> &lhs,
                           const queue<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator!=(const queue<T1, Container1> &lhs,
                           const queue<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator<(const queue<T1, Container1> &lhs,
                          const queue<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator<=(const queue<T1, Container1> &lhs,
                           const queue<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator>(const queue<T1, Container1> &lhs,
                          const queue<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator>=(const queue<T1, Container1> &lhs,
                           const queue<T1, Container1> &rhs);
};

template<class T, class Container>
bool operator==(const queue<T, Container> &x, const queue<T, Container> &y) {
    return x.c == y.c;
}

template<class T, class Container>
bool operator!=(const queue<T, Container> &x, const queue<T, Container> &y) {
    return x.c != y.c;
}

template<class T, class Container>
bool operator<(const queue<T, Container> &x, const queue<T, Container> &y) {
    return x.c < y.c;
}

template<class T, class Container>
bool operator>(const queue<T, Container> &x, const queue<T, Container> &y) {
    return x.c > y.c;
}

template<class T, class Container>
bool operator<=(const queue<T, Container> &x, const queue<T, Container> &y) {
    return x.c <= y.c;
}

template<class T, class Container>
bool operator>=(const queue<T, Container> &x, const queue<T, Container> &y) {
    return x.c >= y.c;
}

template<class T, class Container>
void swap(queue<T, Container> &x,
          queue<T, Container> &y) noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES

template<class Container>
queue(Container) -> queue<typename Container::value_type, Container>;

template<class InputIter>
queue(InputIter, InputIter)
    -> queue<typename iterator_traits<InputIter>::value_type>;

template<class Container, class Alloc>
queue(Container, Alloc) -> queue<typename Container::value_type, Container>;

template<class InputIter, class Alloc>
queue(InputIter, InputIter, Alloc)
    -> queue<typename iterator_traits<InputIter>::value_type,
             ring<typename iterator_traits<InputIter>::value_type, Alloc>>;
#endif

template<class T, class Container, class Alloc>
struct uses_allocator<queue<T, Container>, Alloc>
    : uses_allocator<Container, Alloc>::type {};

template<class T, class Container = vector<T>,
         class Compare = less<typename Container::value_type>>
class priority_queue {
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;
    using value_compare = Compare;
    static_assert((is_same<T, value_type>::value),
                  "Container::value_type not same as T");

protected:
    Container c;
    Compare comp;

public:
    priority_queue(): priority_queue(Compare()) {}
    explicit priority_queue(const Compare &x): priority_queue(x, Container()) {}
    priority_queue(const Compare &x, const Container &d): comp(x), c(d) {
        ala::make_heap(c.begin(), c.end(), comp);
    }
    priority_queue(const Compare &x, Container &&d): comp(x), c(ala::move(d)) {
        ala::make_heap(c.begin(), c.end(), comp);
    }

    // priority_queue( const priority_queue& other ):c(other.c) {}
    // priority_queue( priority_queue&& other ):c(ala::move(other.c)) {}

    template<class InputIter,
             typename = enable_if_t<
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Compare &x = Compare())
        : c(first, last), comp(x) {
        ala::make_heap(c.begin(), c.end(), comp);
    }
    template<class InputIter,
             typename = enable_if_t<
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Compare &x,
                   const Container &d)
        : c(d), comp(x) {
        c.insert(c.end(), first, last);
        ala::make_heap(c.begin(), c.end(), comp);
    }
    template<class InputIter,
             typename = enable_if_t<
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Compare &x,
                   Container &&d)
        : c(ala::move(d)), comp(x) {
        c.insert(c.end(), first, last);
        ala::make_heap(c.begin(), c.end(), comp);
    }

    template<class Alloc>
    explicit priority_queue(const Alloc &a): c(a), comp() {}
    template<class Alloc>
    priority_queue(const Compare &x, const Alloc &a): c(a), comp(x) {}
    template<class Alloc>
    priority_queue(const Compare &x, const Container &d, const Alloc &a)
        : c(d, a), comp(x) {
        ala::make_heap(c.begin(), c.end(), comp);
    }
    template<class Alloc>
    priority_queue(const Compare &x, Container &&d, const Alloc &a)
        : c(ala::move(d), a), comp(x) {
        ala::make_heap(c.begin(), c.end(), comp);
    }
    template<class Alloc>
    priority_queue(const priority_queue &other, const Alloc &a)
        : c(other.c, a), comp(other.comp) {}
    template<class Alloc>
    priority_queue(priority_queue &&other, const Alloc &a)
        : c(ala::move(other.c), a), comp(ala::move(other.comp)) {}

    template<class InputIter, class Alloc,
             typename = enable_if_t<
                 uses_allocator<Container, Alloc>::value &&
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Alloc &a)
        : c(first, last), comp() {
        ala::make_heap(c.begin(), c.end(), comp);
    }

    template<class InputIter, class Alloc,
             typename = enable_if_t<
                 uses_allocator<Container, Alloc>::value &&
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Compare &x,
                   const Alloc &a)
        : c(first, last), comp(x) {
        ala::make_heap(c.begin(), c.end(), comp);
    }

    template<class InputIter, class Alloc,
             typename = enable_if_t<
                 uses_allocator<Container, Alloc>::value &&
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Compare &x,
                   const Container &d, const Alloc &a)
        : c(d, a), comp(x) {
        c.insert(c.end(), first, last);
        ala::make_heap(c.begin(), c.end(), comp);
    }

    template<class InputIter, class Alloc,
             typename = enable_if_t<
                 uses_allocator<Container, Alloc>::value &&
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    priority_queue(InputIter first, InputIter last, const Compare &x,
                   Container &&d, const Alloc &a)
        : c(ala::move(d), a), comp(x) {
        c.insert(c.end(), first, last);
        ala::make_heap(c.begin(), c.end(), comp);
    }

    [[nodiscard]] bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }
    const_reference top() const {
        return c.front();
    }

    void push(const value_type &x) {
        c.push_back(x);
        ala::push_heap(c.begin(), c.end(), comp);
    }

    void push(value_type &&x) {
        c.push_back(ala::move(x));
        ala::push_heap(c.begin(), c.end(), comp);
    }

    template<class... Args>
    void emplace(Args &&...args) {
        c.emplace_back(ala::forward<Args>(args)...);
        ala::push_heap(c.begin(), c.end(), comp);
    }
    void pop() {
        ala::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
    void swap(priority_queue &q) noexcept(
        is_nothrow_swappable_v<Container> &&is_nothrow_swappable_v<Compare>) {
        using ala::swap;
        swap(c, q.c);
        swap(comp, q.comp);
    }
};

#if _ALA_ENABLE_DEDUCTION_GUIDES

template<class Compare, class Container>
priority_queue(Compare, Container) -> priority_queue<
    enable_if_t<!_is_allocator<Compare>::value && !_is_allocator<Container>::value,
                typename Container::value_type>,
    Container, Compare>;

template<class InputIter,
         class Compare = less<typename iterator_traits<InputIter>::value_type>,
         class Container = vector<typename iterator_traits<InputIter>::value_type>>
priority_queue(InputIter, InputIter, Compare = Compare(), Container = Container())
    -> priority_queue<
        enable_if_t<!_is_allocator<Compare>::value && !_is_allocator<Container>::value,
                    typename iterator_traits<InputIter>::value_type>,
        Container, Compare>;

template<class Compare, class Container, class Alloc>
priority_queue(Compare, Container, Alloc) -> priority_queue<
    enable_if_t<!_is_allocator<Compare>::value && !_is_allocator<Container>::value &&
                    uses_allocator<Container, Alloc>::value,
                typename Container::value_type>,
    Container, Compare>;

template<class InputIter, class Alloc>
priority_queue(InputIter, InputIter, Alloc)
    -> priority_queue<enable_if_t<_is_allocator<Alloc>::value,
                                  typename iterator_traits<InputIter>::value_type>,
                      vector<typename iterator_traits<InputIter>::value_type, Alloc>,
                      less<typename iterator_traits<InputIter>::value_type>>;

template<class InputIter, class Compare, class Alloc>
priority_queue(InputIter, InputIter, Compare, Alloc) -> priority_queue<
    typename iterator_traits<InputIter>::value_type,
    vector<enable_if_t<!_is_allocator<Compare>::value && _is_allocator<Alloc>::value,
                       typename iterator_traits<InputIter>::value_type>,
           Alloc>,
    Compare>;

template<class InputIter, class Compare, class Container, class Alloc>
priority_queue(InputIter, InputIter, Compare, Container, Alloc) -> priority_queue<
    enable_if_t<!_is_allocator<Compare>::value && !_is_allocator<Container>::value &&
                    _is_allocator<Alloc>::value && uses_allocator<Container, Alloc>::value,
                typename Container::value_type>,
    Container, Compare>;

#endif

template<class T, class Container, class Compare, class Alloc>
struct uses_allocator<priority_queue<T, Container, Compare>, Alloc>
    : uses_allocator<Container, Alloc>::type {};

} // namespace ala

#endif