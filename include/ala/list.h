#ifndef _ALA_LIST_H
#define _ALA_LIST_H

#include <ala/detail/allocator.h>
#include <ala/iterator.h>
#include <ala/detail/algorithm_base.h>

namespace ala {

struct l_node {
    l_node *_pre = nullptr;
    l_node *_suc = nullptr;
};

template<class T>
struct l_vnode: l_node {
    T _data;
};

template<class Value, class Ptr>
struct l_iterator {
    using iterator_category = bidirectional_iterator_tag;
    using value_type = Value;
    using difference_type = typename pointer_traits<Ptr>::difference_type;
    using pointer = value_type *;
    using reference = value_type &;

    constexpr l_iterator() {}
    constexpr l_iterator(const l_iterator &other): _ptr(other._ptr) {}
    constexpr l_iterator(const Ptr &ptr): _ptr(ptr) {}

    template<class Value1, class Ptr1>
    constexpr l_iterator(const l_iterator<Value1, Ptr1> &other)
        : _ptr(other._ptr) {}

    constexpr reference operator*() const {
        l_vnode<value_type> *vnode = static_cast<l_vnode<value_type> *>(_ptr);
        return vnode->_data;
    }

    constexpr pointer operator->() const {
        return ala::addressof(this->operator*());
    }

    template<class Value1, class Ptr1>
    constexpr bool operator==(const l_iterator<Value1, Ptr1> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator!=(const l_iterator<Value1, Ptr1> &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr l_iterator &operator++() {
        _ptr = _ptr->_suc;
        return *this;
    }

    constexpr l_iterator operator++(int) {
        l_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr l_iterator &operator--() {
        _ptr = _ptr->_pre;
        return *this;
    }

    constexpr l_iterator operator--(int) {
        l_iterator tmp(*this);
        --*this;
        return tmp;
    }

protected:
    template<class, class>
    friend class l_iterator;

    template<class, class>
    friend class list;

    Ptr _ptr = nullptr;
};

template<class T, class Alloc = allocator<T>>
class list {
public:
    // types:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = l_iterator<value_type, l_node *>;
    using const_iterator = l_iterator<const value_type, l_node *>;
    using allocator_type = Alloc;
    using _alloc_traits = allocator_traits<Alloc>;
    using size_type = typename _alloc_traits::size_type;
    using difference_type = typename _alloc_traits::difference_type;
    using pointer = typename _alloc_traits::pointer;
    using const_pointer = typename _alloc_traits::const_pointer;
    using reverse_iterator = ala::reverse_iterator<iterator>;
    using const_reverse_iterator = ala::reverse_iterator<const_iterator>;
    static_assert(is_same<value_type, typename _alloc_traits::value_type>::value,
                  "allocator::value_type mismatch");

protected:
    using _hdle_t = l_node *;
    l_node _guard[2];
    size_type _size = 0;
    allocator_type _alloc;

    _hdle_t head() {
        l_node *p = _guard;
        return static_cast<_hdle_t>(p);
    }

    _hdle_t tail() {
        l_node *p = _guard;
        return static_cast<_hdle_t>(p + 1);
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
        link(head(), tail());
    };

    template<class... Args>
    _hdle_t construct_node(Args &&...args) {
        using holder_t = pointer_holder<l_vnode<value_type> *, Alloc>;
        holder_t holder(_alloc, 1);
        _alloc_traits::construct(_alloc, ala::addressof(holder.get()->_data),
                                 ala::forward<Args>(args)...);
        return holder.release();
    }

    _hdle_t destruct_node(_hdle_t node) {
        assert(node != head() && node != tail());
        l_vnode<value_type> *vnode = static_cast<l_vnode<value_type> *>(node);
        _alloc_traits::destroy(_alloc, ala::addressof(vnode->_data));
        _hdle_t result = vnode->_suc;
        _alloc_traits::template deallocate_object<l_vnode<value_type>>(_alloc,
                                                                       vnode, 1);
        return result;
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
             class = enable_if_t<
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
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

    list(const list &other, const type_identity_t<allocator_type> &a)
        : _alloc(a) {
        initialize();
        this->clone(other);
    }

    list(list &&other, const type_identity_t<allocator_type> &a): _alloc(a) {
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
        ala::_swap_adl(_alloc, other._alloc);
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
        size_type n = _size;
        _hdle_t bgn = head()->_suc;
        _hdle_t end = tail()->_pre;
        if (n != 0)
            this->detach_range(bgn, end, n);
        this->possess(ala::move(other));
        if (n != 0)
            other.attach_range(other.tail(), bgn, end, n);
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>
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
    template<class... V>
    void v_resize(size_type n, V &&...v) {
        static_assert(sizeof...(V) == 0 || sizeof...(V) == 1, "Internal error");
        if (size() > n) {
            iterator pos = this->locate(n);
            this->cut(pos);
        } else {
            size_t m = n - size();
            if (m == 0)
                return;
            _hdle_t head = nullptr, prev = nullptr;
            try {
                head = this->construct_node(ala::forward<V>(v)...);
                head->_pre = head->_suc = nullptr;
                prev = head;
                size_t i = 1;
                for (; i != m; ++i) {
                    _hdle_t node = this->construct_node(ala::forward<V>(v)...);
                    node->_suc = nullptr;
                    link(prev, node);
                    prev = node;
                }
                attach_range(tail(), head, prev, i);
            } catch (...) {
                // if (head != nullptr)
                for (_hdle_t i = head; i != nullptr;)
                    i = destruct_node(i);
                throw;
            }
        }
    }

public:
    void resize(size_type n) {
        v_resize(n);
    }

    void resize(size_type n, const value_type &v) {
        v_resize(n, v);
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
    reference emplace_front(Args &&...args) {
        return *(this->emplace(begin(), ala::forward<Args>(args)...));
    }

    template<class... Args>
    reference emplace_back(Args &&...args) {
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
    iterator emplace(const_iterator position, Args &&...args) {
        _hdle_t node = this->construct_node(ala::forward<Args>(args)...);
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
        if (n == 0)
            return position;
        _hdle_t head = nullptr, prev = nullptr;
        try {
            head = this->construct_node(v);
            head->_pre = head->_suc = nullptr;
            prev = head;
            for (size_t i = 1; i != n; ++i) {
                _hdle_t node = this->construct_node(v);
                node->_suc = nullptr;
                link(prev, node);
                prev = node;
            }
            attach_range(position._ptr, head, prev, n);
        } catch (...) {
            for (_hdle_t i = head; i != nullptr;)
                i = destruct_node(i);
            throw;
        }
        return iterator(head);
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value, iterator>
    insert(const_iterator position, InputIter first, InputIter last) {
        if (first == last)
            return position;
        _hdle_t head = nullptr, prev = nullptr;
        try {
            head = this->construct_node(*first++);
            head->_pre = head->_suc = nullptr;
            prev = head;
            size_t i = 1;
            for (; first != last; ++first, (void)++i) {
                _hdle_t node = this->construct_node(*first);
                node->_suc = nullptr;
                link(prev, node);
                prev = node;
            }
            attach_range(position._ptr, head, prev, i);
        } catch (...) {
            for (_hdle_t i = head; i != nullptr;)
                i = destruct_node(i);
            throw;
            throw;
        }
        return iterator(head);
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
        _hdle_t head = nullptr;
        for (iterator i = begin(); i != end();)
            if (pred(*i)) {
                _hdle_t h = this->detach(i._ptr);
                ++i;
                h->_suc = head;
                head = h;
                ++n;
            } else {
                ++i;
            }
        for (_hdle_t i = head; i != nullptr;)
            i = destruct_node(i);
        return n;
    }

    size_type unique() {
        return this->unique(
            [](const value_type &a, const value_type &b) { return a == b; });
    }

    template<class BinaryPredicate>
    size_type unique(BinaryPredicate pred) {
        if (size() < 2)
            return 0;
        iterator j = begin();
        size_type erased = 0;
        for (iterator i = j++; j != end();)
            if (pred(*i, *j)) {
                j = destruct_node(detach(j._ptr));
                ++erased;
            } else {
                ++j;
                ++i;
            }
        return erased;
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

    template<class Compare>
    iterator sort_range(iterator first, iterator last, Compare comp, size_type n) {
        if (n < 2)
            return first;
        iterator mid = ala::next(first, n >> 1);
        first = sort_range(first, mid, comp, n >> 1);
        mid = sort_range(mid, last, comp, n - (n >> 1));
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
        this->merge(other, comp);
    }

    void sort() {
        this->sort(less<>());
    }

    template<class Compare>
    void sort(Compare comp) {
        this->sort_range(begin(), end(), comp, size());
    }

    void reverse() noexcept {
        _hdle_t first = head()->_suc, last = tail()->_pre;
        for (iterator i = begin(); i != end(); --i)
            ala::_swap_adl(i._ptr->_suc, i._ptr->_pre);
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