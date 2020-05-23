#ifndef _ALA_LIST_H
#define _ALA_LIST_H

#include <ala/detail/linked_list.h>
#include <ala/detail/algorithm_base.h>

namespace ala {

template<class T, class Alloc = allocator<T>>
class list {
public:
    // types:
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef l_iterator<value_type, l_node<value_type> *, false> iterator;
    typedef l_iterator<value_type, l_node<value_type> *, true> const_iterator;
    typedef Alloc allocator_type;
    typedef allocator_traits<Alloc> _alloc_traits;
    typedef typename _alloc_traits::size_type size_type;
    typedef typename _alloc_traits::difference_type difference_type;
    typedef typename _alloc_traits::pointer pointer;
    typedef typename _alloc_traits::const_pointer const_pointer;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;
    static_assert(is_same<value_type, typename _alloc_traits::value_type>::value,
                  "allocator::value_type mismatch");

protected:
    typedef l_node<value_type> _node_t;
    typedef _node_t *_hdle_t; // node handle type
    _hdle_t _guard = nullptr;
    size_type _size = 0;
    allocator_type _alloc;

    _hdle_t head() {
        return _guard;
    }

    _hdle_t tail() {
        return _guard + 1;
    }

    void link(_hdle_t a, _hdle_t b) {
        a->_suc = b;
        b->_pre = a;
    }

    void cut(iterator pos) noexcept {
        _hdle_t e = pos._ptr->_pre;
        for (; pos != end(); --_size)
            pos = destruct_node(pos._ptr);
        link(e, tail());
    }

    iterator locate(size_type index) noexcept {
        iterator i = begin();
        for (; index != 0 && i != end(); --index, (void)++i)
            ;
        return i;
    }

    // insert before pos
    void attach(_hdle_t pos, _hdle_t node) {
        assert(pos != head());
        link(pos->_pre, node);
        link(node, pos);
        ++_size;
    }

    // [left, rght], not [)
    void attach_range(_hdle_t pos, _hdle_t left, _hdle_t rght, size_type len) {
        assert(pos != head());
        link(pos->_pre, left);
        link(rght, pos);
        _size += len;
    }

    _hdle_t detach(_hdle_t pos) {
        assert(pos != head() && pos != tail());
        _hdle_t pre = pos->_pre, suc = pos->_suc;
        link(pre, suc);
        --_size;
        return pos;
    }

    void detach_range(_hdle_t left, _hdle_t rght, size_type len) {
        assert(left != head() && left != tail());
        assert(rght != head() && rght != tail());
        _hdle_t pre = left->_pre, suc = rght->_suc;
        link(pre, suc);
        _size -= len;
    }

    void initialize() {
        _guard = _alloc_traits::template allocate_object<_node_t>(_alloc, 2);
        head()->_pre = nullptr;
        link(head(), tail());
        tail()->_suc = nullptr;
    };

    template<class... Args>
    _hdle_t construct_node(Args &&... args) {
        _hdle_t node = _alloc_traits::template allocate_object<_node_t>(_alloc,
                                                                        1);
        _alloc_traits::construct(_alloc, ala::addressof(node->_data),
                                 ala::forward<Args>(args)...);
        return node;
    }

    _hdle_t destruct_node(_hdle_t node) {
        _alloc_traits::destroy(_alloc, ala::addressof(node->_data));
        _hdle_t rv = node->_suc;
        _alloc_traits::template deallocate_object<_node_t>(_alloc, node, 1);
        return rv;
    }

    void clone(const list &other) {
        this->insert(end(), other.begin(), other.end());
    }

    void clone(list &&other) {
        this->insert(end(), ala::make_move_iterator(other.begin()),
                     ala::make_move_iterator(other.end()));
    }

    void possess(list &&other) {
        if (!other.empty()) {
            _hdle_t bgn = other.head()->_suc;
            _hdle_t end = other.tail()->_pre;
            size_type n = other._size;
            other.detach_range(bgn, end, n);
            this->attach_range(tail(), bgn, end, n);
        }
    }

public:
    // construct/copy/destroy:
    list(): _alloc() {
        initialize();
    }

    explicit list(const allocator_type &a): _alloc(a) {
        initialize();
    }

    explicit list(size_type n, const allocator_type &a = allocator_type())
        : _alloc(a) {
        initialize();
        this->resize(n);
    }

    list(size_type n, const value_type &v,
         const allocator_type &a = allocator_type())
        : _alloc(a) {
        initialize();
        this->resize(n, v);
    }

    template<class InputIter,
             class = enable_if_t<is_base_of<
                 input_iterator_tag,
                 typename iterator_traits<InputIter>::iterator_category>::value>>
    list(InputIter first, InputIter last,
         const allocator_type &a = allocator_type())
        : _alloc(a) {
        initialize();
        for (; first != last; ++first)
            this->emplace_back(*first);
    }

    list(const list &other)
        : _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)) {
        initialize();
        this->clone(other);
    }

    list(list &&other): _alloc(ala::move(other._alloc)) {
        initialize();
        this->possess(ala::move(other));
    }

    list(const list &other, const allocator_type &a): _alloc(a) {
        initialize();
        this->clone(other);
    }

    list(list &&other, const allocator_type &a): _alloc(a) {
        initialize();
        if (_alloc == other._alloc)
            this->possess(ala::move(other));
        else
            this->clone(ala::move(other));
    }

    list(initializer_list<value_type> il,
         const allocator_type &a = allocator_type())
        : list(il.begin(), il.end(), a) {}

    ~list() {
        clear();
        //FIXME deinie guard
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const list &other) {
        if (_alloc != other._alloc)
            clear();
        _alloc = other._alloc;
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const list &other) {
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(list &&other) noexcept {
        clear();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(list &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            clear();
            this->possess(ala::move(other));
        } else {
            this->assign(ala::make_move_iterator(other.begin()),
                         ala::make_move_iterator(other.end()));
        }
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(list &other) {
        ala::swap(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy> swap_helper(list &other) {
        assert(_alloc == other._alloc);
    }

public:
    list &operator=(const list &other) {
        if (this != ala::addressof(other))
            copy_helper(other);
        return *this;
    }

    list &operator=(list &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (this != ala::addressof(other))
            move_helper(ala::move(other));
        return *this;
    }

    list &operator=(initializer_list<value_type> il) {
        this->assign(il.begin(), il.end());
        return *this;
    }

    void swap(list &other) noexcept(_alloc_traits::is_always_equal::value) {
        this->swap_helper(other);
        ala::swap(_guard, other._guard);
        ala::swap(_size, other._size);
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value>
    assign(InputIter first, InputIter last) {
        iterator i = begin();
        for (; i != end() && first != last; ++i, (void)++first)
            *i = *first;
        this->cut(i);
        this->insert(end(), first, last);
    }

    void assign(size_type n, const value_type &v) {
        iterator i = begin();
        for (; i != end() && n != 0; ++i, (void)--n)
            *i = v;
        this->cut(i);
        this->insert(end(), n, v);
    }

    void assign(initializer_list<T> il) {
        this->assign(il.begin(), il.end());
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    // iterators:
    iterator begin() noexcept {
        return iterator(head()->_suc);
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    iterator end() noexcept {
        return iterator(tail());
    }

    const_iterator end() const noexcept {
        return cend();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return crbegin();
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return crend();
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(const_cast<list *>(this)->begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(const_cast<list *>(this)->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // capacity:
    size_type size() const noexcept {
        return _size;
    }

    size_type max_size() const noexcept {
        return _alloc_traits::max_size(_alloc);
    }

protected:
    template<class... Args>
    void resize_helper(size_type sz, Args &&... args) {
        if (size() > sz) {
            iterator pos = this->locate(sz);
            this->cut(pos);
        } else {
            for (int i = size(); i < sz; ++i)
                this->emplace_back(ala::forward<Args>(args)...);
        }
    }

public:
    void resize(size_type sz) {
        resize_helper(sz);
    }

    void resize(size_type sz, const value_type &v) {
        resize_helper(sz, v);
    }

    ALA_NODISCARD bool empty() const noexcept {
        return size() == 0;
    }

    // element access:
    reference front() {
        return *begin();
    }

    const_reference front() const {
        return *cbegin();
    }

    reference back() {
        return *rbegin();
    }

    const_reference back() const {
        return *crbegin();
    }

    // modifiers:
    template<class... Args>
    reference emplace_front(Args &&... args) {
        return *(this->emplace(begin(), ala::forward<Args>(args)...));
    }

    template<class... Args>
    reference emplace_back(Args &&... args) {
        return *(this->emplace(end(), ala::forward<Args>(args)...));
    }

    void push_front(const value_type &v) {
        emplace_front(v);
    }

    void push_front(value_type &&v) {
        emplace_front(ala::move(v));
    }

    void push_back(const value_type &v) {
        emplace_back(v);
    }

    void push_back(value_type &&v) {
        emplace_back(ala::move(v));
    }

    void pop_front() {
        destruct_node(detach(head()->_suc));
    }

    void pop_back() {
        destruct_node(detach(tail()->_pre));
    }

    template<class... Args>
    iterator emplace(const_iterator position, Args &&... args) {
        _hdle_t node = construct_node(ala::forward<Args>(args)...);
        attach(position._ptr, node);
        return iterator(node);
    }

    iterator insert(const_iterator position, const value_type &v) {
        return this->emplace(position, v);
    }

    iterator insert(const_iterator position, value_type &&v) {
        return this->emplace(position, ala::move(v));
    }

    iterator insert(const_iterator position, size_type n, const value_type &v) {
        size_type in = 0;
        try {
            for (; in < n; ++in)
                position = this->insert(position, v);
        } catch (...) {
            if (in == 1) {
                this->destruct_node(this->detach(position._ptr));
            } else if (in > 1) {
                iterator last = ala::next(position, in - 1);
                this->detach_range(position._ptr, last._ptr, in);
                for (_hdle_t h = position._ptr; in != 0; --in)
                    h = this->destruct_node(h);
            }
            throw;
        }
        return position;
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value,
                iterator>
    insert(const_iterator position, InputIter first, InputIter last) {
        iterator rv = position;
        size_type in = 0;
        try {
            if (first != last) {
                rv = position = this->insert(position, *first++);
                ++position;
                ++in;
            }
            for (; first != last; ++position, (void)++in)
                position = this->insert(position, *first++);
        } catch (...) {
            if (in == 1) {
                this->destruct_node(this->detach(rv._ptr));
            } else if (in > 1) {
                iterator last = ala::next(rv, in - 1);
                this->detach_range(rv._ptr, last._ptr, in);
                for (_hdle_t h = rv._ptr; in != 0; --in)
                    h = this->destruct_node(h);
            }
            throw;
        }
        return rv;
    }

    iterator insert(const_iterator position, initializer_list<value_type> il) {
        return this->insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        return destruct_node(detach(position._ptr));
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator i = first;
        for (; i != last;)
            i = erase(i);
        return i;
    }

    void clear() noexcept {
        cut(begin());
    }

    // list operations:
    void splice(const_iterator position, list &other) {
        assert(_alloc == other._alloc);
        size_type len = other.size();
        if (len == 0 && this != ala::addressof(other))
            return;
        _hdle_t left = other.head()->_suc;
        _hdle_t rght = other.tail()->_pre;
        other.detach_range(left, rght, len);
        this->attach_range(position._ptr, left, rght, len);
    }

    void splice(const_iterator position, list &&other) {
        this->splice(position, other);
    }

    void splice(const_iterator position, list &other, const_iterator i) {
        assert(_alloc == other._alloc);
        if (position._ptr != i._ptr)
            this->attach(position._ptr, other.detach(i._ptr));
    }

    void splice(const_iterator position, list &&other, const_iterator i) {
        this->splice(position, other, i);
    }

    void splice(const_iterator position, list &other, const_iterator first,
                const_iterator last) {
        assert(_alloc == other._alloc);
        auto len = ala::distance(first, last);
        if (len == 0)
            return;
        _hdle_t left = first._ptr;
        _hdle_t rght = (--last)._ptr;
        other.detach_range(left, rght, len);
        this->attach_range(position._ptr, left, rght, len);
    }

    void splice(const_iterator position, list &&other, const_iterator first,
                const_iterator last) {
        this->splice(position, other, first, last);
    }

    size_type remove(const value_type &value) {
        return this->remove_if(
            [&](const value_type &elem) { return value == elem; });
    }

    template<class UnaryPredicate>
    size_type remove_if(UnaryPredicate pred) {
        size_type n = 0;
        _hdle_t rmh = nullptr;
        for (iterator i = begin(); i != end();)
            if (pred(*i)) {
                _hdle_t h = this->detach(i._ptr);
                ++i;
                h->_suc = rmh;
                rmh = h;
                ++n;
            } else {
                ++i;
            }
        for (_hdle_t i = rmh; i != nullptr;)
            i = destruct_node(i);
        return n;
    }

    void unique() {
        this->unique(
            [](const value_type &a, const value_type &b) { return a == b; });
    }

    template<class BinaryPredicate>
    void unique(BinaryPredicate pred) {
        if (size() < 2)
            return;
        iterator j = begin();
        for (iterator i = j++; j != end();)
            if (pred(*i, *j))
                j = destruct_node(detach(j._ptr));
            else {
                ++j;
                ++i;
            }
    }

    void merge(list &other) {
        this->merge(other, less<>());
    }

    void merge(list &&other) {
        this->merge(other);
    }

protected:
    template<class Compare>
    iterator merge_range(iterator first, iterator mid, iterator last,
                         Compare comp) {
        iterator i = first, j = mid;
        while (i != j && j != last) {
            if (comp(*j, *i)) {
                if (i == first)
                    first = j;
                this->attach(i._ptr, detach(j++._ptr));
            } else
                ++i;
        }
        return first;
    }

    iterator middle(iterator first, iterator last) {
        iterator tmp = last++;
        for (iterator _guard = first; _guard != tmp && _guard != last;
             ++first, ++++_guard)
            ;
        return first;
    }

    template<class Compare>
    iterator sort_range(iterator first, iterator last, Compare comp) {
        iterator tmp = first++;
        if (tmp == last || first == last)
            return tmp;
        first = tmp;
        iterator mid = middle(first, last);
        first = sort_range(first, mid, comp);
        mid = sort_range(mid, last, comp);
        return merge_range(first, mid, last, comp);
    }

public:
    template<class Compare>
    void merge(list &other, Compare comp) {
        assert(_alloc == other._alloc);
        // assert(ala::is_sorted(this->begin(), this->end()));
        // assert(ala::is_sorted(other.begin(), other.end()));
        iterator i = this->begin();
        iterator j = other.begin();
        while (j != other.end())
            if (i == end() || comp(*j, *i))
                this->attach(i._ptr, other.detach(j++._ptr));
            else
                ++i;
    }

    template<class Compare>
    void merge(list &&other, Compare comp) {
        this.merge(other, comp);
    }

    void sort() {
        this->sort(less<>());
    }

    template<class Compare>
    void sort(Compare comp) {
        this->sort_range(begin(), end(), comp);
    }

    void reverse() noexcept {
        _hdle_t first = head()->_suc, last = tail()->_pre;
        for (iterator i = begin(); i != end(); --i)
            ala::swap(i._ptr->_suc, i._ptr->_pre);
        link(head(), last);
        link(first, tail());
    }
};

template<class T, class Alloc>
bool operator==(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return ala::equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class T, class Alloc>
bool operator!=(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator<(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return ala::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template<class T, class Alloc>
bool operator>(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return rhs < lhs;
}

template<class T, class Alloc>
bool operator<=(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class T, class Alloc>
bool operator>=(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class T, class Alloc>
void swap(list<T, Alloc> &lhs,
          list<T, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class InputIter,
         class Alloc = allocator<typename iterator_traits<InputIter>::value_type>>
list(InputIter, InputIter, Alloc = Alloc())
    -> list<typename iterator_traits<InputIter>::value_type, Alloc>;
#endif

// C++20
template<class T, class Alloc, class U>
constexpr typename list<T, Alloc>::size_type erase(list<T, Alloc> &c,
                                                   const U &value) {
    return c.remove_if([&value](const auto &elem) { return elem == value; });
}

template<class T, class Alloc, class Pred>
constexpr typename list<T, Alloc>::size_type erase_if(list<T, Alloc> &c,
                                                      Pred pred) {
    return c.remove_if(pred);
}
} // namespace ala

#endif