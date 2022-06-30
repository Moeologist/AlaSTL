#ifndef _ALA_STACK_H
#define _ALA_STACK_H

#include <ala/ring.h>

namespace ala {
template<class T, class Container = ring<T>>
class stack {
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
    stack(): stack(Container()) {}
    explicit stack(const Container &d): c(d) {}
    explicit stack(Container &&d): c(ala::move(d)) {}
    template<class InputIter>
    stack(InputIter first, InputIter last): c(first, last) {}

    // stack(const stack &other): c(other.c) {}
    // stack(stack &&other): c(ala::move(other.c)) {}

    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    explicit stack(const Alloc &a): c(a) {}
    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    stack(const Container &d, const Alloc &a): c(d, a) {}
    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    stack(Container &&d, const Alloc &a): c(ala::move(d), a) {}

    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    stack(const stack &other, const Alloc &a): c(other.c, a) {}
    template<class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    stack(stack &&other, const Alloc &a): c(ala::move(other.c), a) {}

    template<class InputIter, class Alloc,
             class Dummy = enable_if_t<uses_allocator<Container, Alloc>::value>>
    stack(InputIter first, InputIter last, const Alloc &a): c(first, last, a) {}

    // stack &operator=(const stack &other) noexcept(
    //     is_nothrow_copy_assignable<Container>::value) {
    //     this->c = other.c;
    //     return *this;
    // }

    // stack &operator=(stack &&other) noexcept(
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
    reference top() {
        return c.back();
    }
    const_reference top() const {
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
        c.pop_back();
    }
    void swap(stack &s) noexcept(is_nothrow_swappable<Container>::value) {
        using ala::swap;
        swap(c, s.c);
    }

    template<class T1, class Container1>
    friend bool operator==(const stack<T1, Container1> &lhs,
                           const stack<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator!=(const stack<T1, Container1> &lhs,
                           const stack<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator<(const stack<T1, Container1> &lhs,
                          const stack<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator<=(const stack<T1, Container1> &lhs,
                           const stack<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator>(const stack<T1, Container1> &lhs,
                          const stack<T1, Container1> &rhs);
    template<class T1, class Container1>
    friend bool operator>=(const stack<T1, Container1> &lhs,
                           const stack<T1, Container1> &rhs);
};

template<class T, class Container>
bool operator==(const stack<T, Container> &x, const stack<T, Container> &y) {
    return x.c == y.c;
}

template<class T, class Container>
bool operator!=(const stack<T, Container> &x, const stack<T, Container> &y) {
    return x.c != y.c;
}

template<class T, class Container>
bool operator<(const stack<T, Container> &x, const stack<T, Container> &y) {
    return x.c < y.c;
}

template<class T, class Container>
bool operator>(const stack<T, Container> &x, const stack<T, Container> &y) {
    return x.c > y.c;
}

template<class T, class Container>
bool operator<=(const stack<T, Container> &x, const stack<T, Container> &y) {
    return x.c <= y.c;
}

template<class T, class Container>
bool operator>=(const stack<T, Container> &x, const stack<T, Container> &y) {
    return x.c >= y.c;
}

template<class T, class Container>
void swap(stack<T, Container> &x,
          stack<T, Container> &y) noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES

template<class Container>
stack(Container) -> stack<typename Container::value_type, Container>;

template<class InputIter>
stack(InputIter, InputIter)
    -> stack<typename iterator_traits<InputIter>::value_type>;

template<class Container, class Allocator>
stack(Container, Allocator) -> stack<typename Container::value_type, Container>;

template<class InputIter, class Allocator>
stack(InputIter, InputIter, Allocator)
    -> stack<typename iterator_traits<InputIter>::value_type,
             ring<typename iterator_traits<InputIter>::value_type, Allocator>>;
#endif

template<class T, class Container, class Alloc>
struct uses_allocator<stack<T, Container>, Alloc>
    : uses_allocator<Container, Alloc>::type {};
} // namespace ala

#endif