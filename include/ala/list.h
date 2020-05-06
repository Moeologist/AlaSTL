#ifndef _ALA_LIST_H
#define _ALA_LIST_H

#include <ala/detail/allocator.h>
#include <ala/iterator.h>

namespace ala {

template<class T, class Alloc = allocator<T>>
class list {
public:
    // types:
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef /*implementation-defined*/ iterator;
    typedef /*implementation-defined*/ const_iterator;
    typedef /*implementation-defined*/ size_type;
    typedef /*implementation-defined*/ difference_type;
    typedef Alloc allocator_type;
    typedef typename allocator_traits<Alloc>::pointer pointer;
    typedef typename allocator_traits<Alloc>::const_pointer const_pointer;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    // construct/copy/destroy:
    explicit list(const Alloc & = Alloc());
    explicit list(size_type n);
    list(size_type n, const T &value, const Alloc & = Alloc());
    template<class InputIterator>
    list(InputIterator first, InputIterator last,
         const Alloc & = Alloc());
    list(const list<T, Alloc> &x);
    list(list &&);
    list(const list &, const Alloc &);
    list(list &&, const Alloc &);
    list(initializer_list<T>, const Alloc & = Alloc());
    ~list();
    list<T, Alloc> &operator=(const list<T, Alloc> &x);
    list<T, Alloc> &operator=(list<T, Alloc> &&x);
    list &operator=(initializer_list<T>);
    template<class InputIterator>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type n, const T &t);
    void assign(initializer_list<T>);
    allocator_type get_allocator() const noexcept;

    // iterators:
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // capacity:
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    void resize(size_type sz);
    void resize(size_type sz, const T &c);
    bool empty() const noexcept;

    // element access:
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // modifiers:
    template<class... Args>
    void emplace_front(Args &&... args);
    void pop_front();
    template<class... Args>
    void emplace_back(Args &&... args);
    void push_front(const T &x);
    void push_front(T &&x);
    void push_back(const T &x);
    void push_back(T &&x);
    void pop_back();

    template<class... Args>
    iterator emplace(const_iterator position, Args &&... args);
    iterator insert(const_iterator position, const T &x);
    iterator insert(const_iterator position, T &&x);
    iterator insert(const_iterator position, size_type n, const T &x);
    template<class InputIterator>
    iterator insert(const_iterator position, InputIterator first,
                    InputIterator last);
    iterator insert(const_iterator position, initializer_list<T>);

    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);
    void swap(list<T, Alloc> &);
    void clear() noexcept;

    // list operations:
    void splice(const_iterator position, list<T, Alloc> &x);
    void splice(const_iterator position, list<T, Alloc> &&x);
    void splice(const_iterator position, list<T, Alloc> &x, const_iterator i);
    void splice(const_iterator position, list<T, Alloc> &&x,
                const_iterator i);
    void splice(const_iterator position, list<T, Alloc> &x,
                const_iterator first, const_iterator last);
    void splice(const_iterator position, list<T, Alloc> &&x,
                const_iterator first, const_iterator last);

    void remove(const T &value);
    template<class Predicate>
    void remove_if(Predicate pred);

    void unique();
    template<class BinaryPredicate>
    void unique(BinaryPredicate binary_pred);

    void merge(list<T, Alloc> &x);
    void merge(list<T, Alloc> &&x);
    template<class Compare>
    void merge(list<T, Alloc> &x, Compare comp);
    template<class Compare>
    void merge(list<T, Alloc> &&x, Compare comp);

    void sort();
    template<class Compare>
    void sort(Compare comp);

    void reverse() noexcept;
};

} // namespace ala

#endif _ALA_LIST_H