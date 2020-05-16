#ifndef _ALA_LIST_H
#define _ALA_LIST_H

#include <ala/detail/linked_list.h>
#include <ala/detail/functional_base.h>

namespace ala {

template<class T, class Alloc = allocator<T>>
class list {
public:
    // types:
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef l_iterator<l_node<value_type> *, false> iterator;
    typedef l_iterator<l_node<value_type> *, true> const_iterator;
    typedef Alloc allocator_type;
    typedef allocator_traits<Alloc> _alloc_traits;
    typedef typename _alloc_traits::size_type size_type;
    typedef typename _alloc_traits::difference_type difference_type;
    typedef typename _alloc_traits::pointer pointer;
    typedef typename _alloc_traits::const_pointer const_pointer;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;

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
        for (; pos != end();) {
            destruct_node(pos++._ptr);
            --_size;
        }
    }

    iterator locate(size_type index) noexcept {
        iterator i = begin();
        for (size_type id = 0; id != index && i != end(); ++i)
            ;
        return i;
    }

    // insert before pos
    void attach_to(_hdle_t pos, _hdle_t node) {
        assert(pos != head());
        link(pos->_pre, node);
        link(node, pos);
        ++_size;
    }

    void attach_range_to(_hdle_t pos, _hdle_t first, _hdle_t last, size_type len) {
        assert(pos != head());
        link(pos->_pre, first);
        link(last, pos);
        _size += len;
    }

    _hdle_t detach(_hdle_t pos) {
        assert(pos != head() && pos != tail());
        _hdle_t pre = pos->_pre, suc = pos->_suc;
        link(pre, suc);
        --_size;
        return pos;
    }

    void detach_range(_hdle_t first, _hdle_t last, size_type len) {
        assert(first != head() && first != tail());
        assert(last != head());
        _hdle_t pre = first->_pre, suc = last;
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

    void destruct_node(_hdle_t node) {
        _alloc_traits::destroy(_alloc, ala::addressof(node->_data));
        _alloc_traits::template deallocate_object<_node_t>(_alloc, node, 1);
        node = nullptr;
    }

    void clone(const list &other) {
        this->insert(end(), other.begin(), other.end());
    }

    void possess(list &&other) {
        if (other._size > 0) {
            _hdle_t bgn = other.head()->_suc;
            _hdle_t end = other.tail();
            size_type n = other._size;
            other.detach_range(bgn, end, other._size);
            this->attach_range_to(tail(), bgn, end, other._size);
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

    explicit list(size_type n): _alloc() {
        initialize();
        for (size_type i = 0; i < n; ++i)
            this->emplace_back();
    }

    list(size_type n, const value_type &value, const Alloc &a = Alloc())
        : _alloc(a) {
        initialize();
        this->insert(end(), n, value);
    }

    template<class InputIter,
             class = enable_if_t<is_base_of<
                 input_iterator_tag,
                 typename iterator_traits<InputIter>::iterator_category>::value>>
    list(InputIter first, InputIter last, const Alloc &a = Alloc()): _alloc(a) {
        initialize();
        this->insert(end(), first, last);
    }

    list(const list &other)
        : _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)) {
        initialize();
        this->clone(other);
    }

    list(list &&other): _alloc(ala::move(other._alloc)) {
        initialize();
        this->possess(other);
    }

    list(const list &other, const Alloc &a): _alloc(a) {
        initialize();
        this->clone(other);
    }

    list(list &&other, const Alloc &a): _alloc(a) {
        initialize();
        this->possess(other);
    }

    list(initializer_list<value_type> il, const Alloc &a = Alloc())
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
        this->possess(other);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(list &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            clear();
            this->possess(ala::move(other));
        } else {
            this->assign(other.begin(), other.end());
        }
    }

public:
    list &operator=(const list &other) {
        if (this != ala::addressof(other))
            copy_helper(other);
        return *this;
    }

    // TODO:check same obj
    list &operator=(list &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (this != ala::addressof(other))
            move_helper(ala::move(other));
        return *this;
    }

    list &operator=(initializer_list<value_type> il) {
        clear();
        this->insert(end(), il);
        return *this;
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value>
    assign(InputIter first, InputIter last) {
        iterator i = begin();
        for (; i != end() && first != last; ++i, ++first)
            *i = *first;
        this->cut(i);
        this->insert(end(), first, last);
    }

    void assign(size_type n, const value_type &v) {
        iterator i = begin();
        for (; i != end() && n != 0; ++i, --n)
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
        return const_iterator(const_cast<list *>(this)->rbegin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_iterator(const_cast<list *>(this)->rend());
    }

    // capacity:
    size_type size() const noexcept {
        return _size;
    }

    size_type max_size() const noexcept {
        return numeric_limits<difference_type>::max();
    }

protected:
    template<class... Args>
    void resize_helper(size_type sz, Args &&... args) {
        if (size() > sz) {
            iterator pos = this->locate(sz);
            _hdle_t end = pos._ptr->_pre;
            this->cut(pos);
            link(end, tail());
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

    bool empty() const noexcept {
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
    void emplace_front(Args &&... args) {
        this->emplace(begin(), ala::forward<Args>(args)...);
    }

    template<class... Args>
    void emplace_back(Args &&... args) {
        this->emplace(end(), ala::forward<Args>(args)...);
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
        attach_to(position._ptr, node);
        return iterator(node);
    }

    iterator insert(const_iterator position, const value_type &v) {
        return this->emplace(position, v);
    }

    iterator insert(const_iterator position, value_type &&v) {
        return this->emplace(position, ala::move(v));
    }

    iterator insert(const_iterator position, size_type n, const value_type &v) {
        for (size_type i = 0; i < n; ++i)
            position = this.insert(position, v);
        return position;
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value,
                iterator>
    insert(const_iterator position, InputIter first, InputIter last) {
        iterator rv = position;
        if (first != last)
            rv = this->insert(position, *first++);
        for (; first != last;)
            this->insert(position, *first++);
        return rv;
    }

    iterator insert(const_iterator position, initializer_list<value_type> il) {
        return this->insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        _hdle_t next = position._ptr->_suc;
        destruct_node(detach(position._ptr));
        return iterator(next);
    }

    iterator erase(const_iterator first, const_iterator last) {
        for (iterator i = first; i != last;)
            i = erase(i);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> _swap_helper(list other) {
        ala::swap(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy> _swap_helper(list &other) {
        assert(_alloc == other._alloc);
    }

    void swap(list &other) noexcept(_alloc_traits::is_always_equal::value) {
        ala::swap(_guard, other._guard);
        ala::swap(_size, other._size);
        _swap_helper(other);
    }

    void clear() noexcept {
        cut(begin());
    }

    // list operations:
    void splice(const_iterator position, list &other) {
        assert(_alloc == other._alloc);
        size_type len = other.size();
        other.detach_range(other.begin(), other.end(), len);
        this->attach_range_to(position, other.begin(), other.end(), len);
    }

    void splice(const_iterator position, list &&other) {
        this->splice(position, other);
    }

    void splice(const_iterator position, list &other, const_iterator i) {
        assert(_alloc == other._alloc);
        destruct_node(other.detach(i._ptr));
        this->attach_to(position->_ptr, i._ptr);
    }

    void splice(const_iterator position, list &&other, const_iterator i) {
        this->splice(position, other, i);
    }

    void splice(const_iterator position, list &other, const_iterator first,
                const_iterator last) {
        assert(_alloc == other._alloc);
        size_type len = ala::distance(first, last);
        other.detach_range(first, last, len);
        this->attach_range_to(position, first, last, len);
    }

    void splice(const_iterator position, list &&other, const_iterator first,
                const_iterator last) {
        this->splice(position, other, first, last);
    }

    void remove(const value_type &value) {
        this->remove([&](const value_type &v) { return v == value; });
    }

    template<class UnaryPredicate>
    void remove_if(UnaryPredicate pred) {
        for (iterator i = begin(); i != end();++i)
            if (pred(*i))
                destruct_node(detach(i._ptr));
    }

    void unique() {
        this->unique(
            [](const value_type &a, const value_type &b) { return a == b; });
    }

    template<class BinaryPredicate>
    void unique(BinaryPredicate pred) {
        if (size() < 2)
            return;
        for (iterator i = begin(), j = ++begin(); j != end(); ++i, ++j)
            if (pred(*i, *j))
                destruct_node(detach(j._ptr));
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
            if (comp(*i, *j))
                ++i;
            else {
                if (i == first)
                    first = j;
                this->attach_to(i._ptr, detach(j++._ptr));
            }
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
        assert(ala::is_sorted(this->begin(), this->end()));
        assert(ala::is_sorted(other.begin(), other.end()));
        iterator i = this->begin();
        iterator j = other.begin();
        while (j != other.end())
            if (i == end() || comp(*j, *i))
                this->attach_to(i._ptr, other.detach(j++._ptr));
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
} // namespace ala

#endif