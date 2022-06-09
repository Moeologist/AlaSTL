#ifndef _ALA_FORWARD_LIST_H
#define _ALA_FORWARD_LIST_H

#include <ala/detail/allocator.h>
#include <ala/iterator.h>
#include <ala/detail/algorithm_base.h>

namespace ala {

struct fl_node {
    fl_node *_suc = nullptr;
};

template<class T>
struct fl_vnode: fl_node {
    T _data;
};

template<class Value, class Ptr>
struct fl_iterator {
    using iterator_category = forward_iterator_tag;
    using value_type = Value;
    using difference_type = typename pointer_traits<Ptr>::difference_type;
    using pointer = value_type *;
    using reference = value_type &;

    constexpr fl_iterator() {}
    constexpr fl_iterator(const fl_iterator &other): _ptr(other._ptr) {}
    constexpr fl_iterator(const Ptr &ptr): _ptr(ptr) {}

    template<class Value1, class Ptr1>
    constexpr fl_iterator(const fl_iterator<Value1, Ptr1> &other)
        : _ptr(other._ptr) {}

    constexpr reference operator*() const {
        fl_vnode<value_type> *vnode = static_cast<fl_vnode<value_type> *>(_ptr);
        return vnode->_data;
    }

    constexpr pointer operator->() const {
        return ala::addressof(this->operator*());
    }

    template<class Value1, class Ptr1>
    constexpr bool operator==(const fl_iterator<Value1, Ptr1> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator!=(const fl_iterator<Value1, Ptr1> &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr fl_iterator &operator++() {
        _ptr = _ptr->_suc;
        return *this;
    }

    constexpr fl_iterator operator++(int) {
        fl_iterator tmp(*this);
        ++*this;
        return tmp;
    }

protected:
    template<class, class>
    friend class fl_iterator;

    template<class, class>
    friend class forward_list;

    Ptr _ptr = nullptr;
};

template<class T, class Alloc = allocator<T>>
class forward_list {
public:
    // types:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = fl_iterator<value_type, fl_node *>;
    using const_iterator = fl_iterator<const value_type, fl_node *>;
    using allocator_type = Alloc;
    using _alloc_traits = allocator_traits<Alloc>;
    using size_type = typename _alloc_traits::size_type;
    using difference_type = typename _alloc_traits::difference_type;
    using pointer = typename _alloc_traits::pointer;
    using const_pointer = typename _alloc_traits::const_pointer;
    static_assert(is_same<value_type, typename _alloc_traits::value_type>::value,
                  "allocator::value_type mismatch");

protected:
    using _hdle_t = fl_node *;
    fl_node _guard[2];
    size_type _size = 0;
    allocator_type _alloc;

    _hdle_t head() {
        fl_node *p = _guard;
        return static_cast<_hdle_t>(p);
    }

    _hdle_t tail() {
        fl_node *p = _guard;
        return static_cast<_hdle_t>(p + 1);
    }

    void link(_hdle_t a, _hdle_t b) {
        a->_suc = b;
    }

    void cut_after(iterator bpos) noexcept {
        iterator pos = bpos._ptr->_suc;
        for (; pos != end(); --_size)
            pos = destruct_node(pos._ptr);
        link(bpos._ptr, tail());
    }

    iterator locate_before(size_type index) noexcept {
        iterator i = before_begin();
        for (; index != 0 && i != end(); --index, (void)++i)
            ;
        return i;
    }

    // insert before pos
    void attach_after(_hdle_t bpos, _hdle_t node) {
        assert(bpos != tail());
        link(node, bpos->_suc);
        link(bpos, node);
        ++_size;
    }

    _hdle_t attach_range_after(_hdle_t bpos, _hdle_t left, size_type len) {
        assert(bpos != tail());
        _hdle_t rght = left;
        for (size_type i = 1; i < len; ++i)
            rght = rght->_suc;
        link(rght, bpos->_suc);
        link(bpos, left);
        _size += len;
        return rght;
    }

    _hdle_t detach_after(_hdle_t bpos) {
        _hdle_t pos = bpos->_suc;
        assert(pos != head() && pos != tail());
        _hdle_t suc = pos->_suc;
        link(bpos, suc);
        --_size;
        return pos;
    }

    void detach_range_after(_hdle_t bleft, _hdle_t arght, size_type len) {
        assert(bleft != tail());
        assert(arght != head());
        link(bleft, arght);
        _size -= len;
    }

    void initialize() {
        link(head(), tail());
    };

    template<class... Args>
    _hdle_t construct_node(Args &&...args) {
        using holder_t = pointer_holder<fl_vnode<value_type> *, Alloc>;
        holder_t holder(_alloc, 1);
        _alloc_traits::construct(_alloc, ala::addressof(holder.get()->_data),
                                 ala::forward<Args>(args)...);
        return holder.release();
    }

    _hdle_t destruct_node(_hdle_t node) {
        assert(node != head() && node != tail());
        fl_vnode<value_type> *vnode = static_cast<fl_vnode<value_type> *>(node);
        _alloc_traits::destroy(_alloc, ala::addressof(vnode->_data));
        _hdle_t result = node->_suc;
        _alloc_traits::template deallocate_object<fl_vnode<value_type>>(_alloc,
                                                                        vnode, 1);
        return result;
    }

    void clone(const forward_list &other) {
        this->insert_after(before_begin(), other.begin(), other.end());
    }

    void clone(forward_list &&other) {
        this->insert_after(before_begin(), ala::make_move_iterator(other.begin()),
                           ala::make_move_iterator(other.end()));
    }

    void possess(forward_list &&other) {
        if (!other.empty()) {
            _hdle_t left = other.head();
            _hdle_t begn = other.head()->_suc;
            _hdle_t rght = other.tail();
            size_type n = other._size;
            other.detach_range_after(left, rght, n);
            this->attach_range_after(head(), begn, n);
        }
    }

public:
    // construct/copy/destroy:
    forward_list(): _alloc() {
        initialize();
    }

    explicit forward_list(const allocator_type &a): _alloc(a) {
        initialize();
    }

    explicit forward_list(size_type n, const allocator_type &a = allocator_type())
        : _alloc(a) {
        initialize();
        this->resize(n);
    }

    forward_list(size_type n, const value_type &v,
                 const allocator_type &a = allocator_type())
        : _alloc(a) {
        initialize();
        this->resize(n, v);
    }

    template<class InputIter,
             class = enable_if_t<is_base_of<
                 input_iterator_tag,
                 typename iterator_traits<InputIter>::iterator_category>::value>>
    forward_list(InputIter first, InputIter last,
                 const allocator_type &a = allocator_type())
        : _alloc(a) {
        initialize();
        for (iterator i = before_begin(); first != last; ++first)
            i = this->emplace_after(i, *first);
    }

    forward_list(const forward_list &other)
        : _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)) {
        initialize();
        this->clone(other);
    }

    forward_list(forward_list &&other): _alloc(ala::move(other._alloc)) {
        initialize();
        this->possess(ala::move(other));
    }

    forward_list(const forward_list &other,
                 const type_identity_t<allocator_type> &a)
        : _alloc(a) {
        initialize();
        this->clone(other);
    }

    forward_list(forward_list &&other, const type_identity_t<allocator_type> &a)
        : _alloc(a) {
        initialize();
        if (_alloc == other._alloc)
            this->possess(ala::move(other));
        else
            this->clone(ala::move(other));
    }

    forward_list(initializer_list<value_type> il,
                 const allocator_type &a = allocator_type())
        : forward_list(il.begin(), il.end(), a) {}

    ~forward_list() {
        clear();
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const forward_list &other) {
        if (_alloc != other._alloc)
            clear();
        _alloc = other._alloc;
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const forward_list &other) {
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(forward_list &&other) noexcept {
        clear();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy> move_helper(forward_list &&other) noexcept(
        _alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            clear();
            this->possess(ala::move(other));
        } else {
            this->assign(ala::make_move_iterator(other.begin()),
                         ala::make_move_iterator(other.end()));
        }
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(forward_list &other) {
        ala::_swap_adl(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy> swap_helper(forward_list &other) {
        assert(_alloc == other._alloc);
    }

public:
    forward_list &operator=(const forward_list &other) {
        if (this != ala::addressof(other))
            copy_helper(other);
        return *this;
    }

    forward_list &operator=(forward_list &&other) noexcept(
        _alloc_traits::is_always_equal::value) {
        if (this != ala::addressof(other))
            move_helper(ala::move(other));
        return *this;
    }

    forward_list &operator=(initializer_list<value_type> il) {
        this->assign(il.begin(), il.end());
        return *this;
    }

    void swap(forward_list &other) noexcept(_alloc_traits::is_always_equal::value) {
        this->swap_helper(other);
        size_type n = _size;
        _hdle_t left = head();
        _hdle_t begn = head()->_suc;
        _hdle_t rght = tail();
        if (n != 0)
            this->detach_range_after(left, rght, n);
        this->possess(ala::move(other));
        if (n != 0)
            other.attach_range_after(other.head(), begn, n);
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value>
    assign(InputIter first, InputIter last) {
        iterator bi = before_begin(), i = begin();
        for (; i != end() && first != last; ++i, (void)++bi, ++first)
            *i = *first;
        this->cut_after(bi);
        this->insert_after(bi, first, last);
    }

    void assign(size_type n, const value_type &v) {
        iterator bi = before_begin(), i = begin();
        for (; i != end() && n != 0; ++i, (void)++bi, --n)
            *i = v;
        this->cut_after(bi);
        this->insert_after(bi, n, v);
    }

    void assign(initializer_list<T> il) {
        this->assign(il.begin(), il.end());
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    // iterators:
    iterator before_begin() noexcept {
        return iterator(head());
    }

    const_iterator before_begin() const noexcept {
        return cbefore_begin();
    }

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

    const_iterator cbefore_begin() const noexcept {
        return const_iterator(const_cast<forward_list *>(this)->before_begin());
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(const_cast<forward_list *>(this)->begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(const_cast<forward_list *>(this)->end());
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
    void nv_resize(size_type sz, V &&...v) {
        static_assert(sizeof...(V) == 0 || sizeof...(V) == 1, "Internal error");
        if (size() > sz) {
            this->cut_after(this->locate_before(sz));
        } else {
            size_t n = sz - size();
            if (n == 0)
                return;
            _hdle_t head = nullptr, prev = nullptr;
            try {
                head = this->construct_node(ala::forward<V>(v)...);
                head->_suc = nullptr;
                prev = head;
                size_t i = 1;
                for (; i != n; ++i) {
                    _hdle_t node = this->construct_node(ala::forward<V>(v)...);
                    node->_suc = nullptr;
                    link(prev, node);
                    prev = node;
                }
                _hdle_t bend = this->head();
                while (bend->_suc != tail())
                    bend = bend->_suc;
                attach_range_after(bend, head, i);
            } catch (...) {
                // if (head != nullptr)
                for (_hdle_t i = head; i != nullptr;)
                    i = destruct_node(i);
                throw;
            }
        }
    }

public:
    void resize(size_type sz) {
        nv_resize(sz);
    }

    void resize(size_type sz, const value_type &v) {
        nv_resize(sz, v);
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

    // modifiers:
    template<class... Args>
    reference emplace_front(Args &&...args) {
        return *(this->emplace_after(before_begin(), ala::forward<Args>(args)...));
    }

    void push_front(const value_type &v) {
        emplace_front(v);
    }

    void push_front(value_type &&v) {
        emplace_front(ala::move(v));
    }

    void pop_front() {
        destruct_node(detach_after(head()));
    }

    template<class... Args>
    iterator emplace_after(const_iterator position, Args &&...args) {
        _hdle_t node = this->construct_node(ala::forward<Args>(args)...);
        attach_after(position._ptr, node);
        return iterator(node);
    }

    iterator insert_after(const_iterator position, const value_type &v) {
        return this->emplace(position, v);
    }

    iterator insert_after(const_iterator position, value_type &&v) {
        return this->emplace_after(position, ala::move(v));
    }

    iterator insert_after(const_iterator position, size_type n,
                          const value_type &v) {
        if (n == 0)
            return position;
        _hdle_t head = nullptr, prev = nullptr;
        try {
            head = this->construct_node(v);
            head->_suc = nullptr;
            prev = head;
            for (size_t i = 1; i != n; ++i) {
                _hdle_t node = this->construct_node(v);
                node->_suc = nullptr;
                link(prev, node);
                prev = node;
            }
            attach_range_after(position._ptr, head, n);
        } catch (...) {
            for (_hdle_t i = head; i != nullptr;)
                i = destruct_node(i);
            throw;
        }
        return iterator(prev);
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value,
                iterator>
    insert_after(const_iterator position, InputIter first, InputIter last) {
        if (first == last)
            return position;
        _hdle_t head = nullptr, prev = nullptr, ret = nullptr;
        try {
            head = this->construct_node(*first++);
            head->_suc = nullptr;
            prev = head;
            size_t i = 1;
            for (; first != last; ++first, (void)++i) {
                _hdle_t node = this->construct_node(*first);
                node->_suc = nullptr;
                link(prev, node);
                prev = node;
            }
            ret = attach_range_after(position._ptr, head, i);
        } catch (...) {
            for (_hdle_t i = head; i != nullptr;)
                i = destruct_node(i);
            throw;
            throw;
        }
        return iterator(ret);
    }

    iterator insert_after(const_iterator position,
                          initializer_list<value_type> il) {
        return this->insert_after(position, il.begin(), il.end());
    }

    iterator erase_after(const_iterator position) {
        return destruct_node(detach_after(position._ptr));
    }

    iterator erase_after(const_iterator first, const_iterator last) {
        iterator i = first;
        for (; i != last && i._ptr->_suc != last._ptr;)
            erase_after(i);
        return last;
    }

    void clear() noexcept {
        cut_after(before_begin());
    }

    // forward_list operations:
    void splice_after(const_iterator position, forward_list &other) {
        assert(_alloc == other._alloc);
        size_type len = other.size();
        if (len == 0 && this != ala::addressof(other))
            return;
        _hdle_t left = other.head();
        _hdle_t begn = other.head()->_suc;
        _hdle_t rght = other.tail();
        other.detach_range_after(left, rght, len);
        this->attach_range_after(position._ptr, begn, len);
    }

    void splice_after(const_iterator position, forward_list &&other) {
        this->splice_after(position, other);
    }

    void splice_after(const_iterator position, forward_list &other,
                      const_iterator i) {
        assert(_alloc == other._alloc);
        if (position._ptr != i._ptr && position._ptr != i._ptr->_suc)
            this->attach_after(position._ptr, other.detach_after(i._ptr));
    }

    void splice_after(const_iterator position, forward_list &&other,
                      const_iterator i) {
        this->splice_after(position, other, i);
    }

    void splice_after(const_iterator position, forward_list &other,
                      const_iterator first, const_iterator last) {
        assert(_alloc == other._alloc);
        auto len = ala::distance(first, last);
        if (len == 0 || len == 1)
            return;
        --len;
        _hdle_t left = first._ptr;
        _hdle_t begn = first._ptr->_suc;
        _hdle_t rght = last._ptr;
        other.detach_range_after(left, rght, len);
        this->attach_range_after(position._ptr, begn, len);
    }

    void splice_after(const_iterator position, forward_list &&other,
                      const_iterator first, const_iterator last) {
        this->splice_after(position, other, first, last);
    }

    size_type remove(const value_type &value) {
        return this->remove_if(
            [&](const value_type &elem) { return value == elem; });
    }

    template<class UnaryPredicate>
    size_type remove_if(UnaryPredicate pred) {
        size_type n = 0;
        _hdle_t head = nullptr;
        iterator bi = before_begin();
        for (iterator i = begin(); i != end();)
            if (pred(*i)) {
                _hdle_t h = this->detach_after(bi._ptr);
                i = bi._ptr->_suc;
                h->_suc = head;
                head = h;
                ++n;
            } else {
                ++i;
                ++bi;
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
        iterator bj = before_begin();
        iterator j = begin();
        size_type erased = 0;
        for (iterator i = j++; j != end();)
            if (pred(*i, *j)) {
                j = destruct_node(detach_after(i._ptr));
                ++erased;
            } else {
                ++j;
                ++i;
            }
        return erased;
    }

    void merge(forward_list &other) {
        this->merge(other, less<>());
    }

    void merge(forward_list &&other) {
        this->merge(other);
    }

protected:
    template<class Compare>
    iterator merge_range(iterator first, iterator mid, iterator last,
                         Compare comp) {
        iterator bi = first, bj = mid;
        iterator i = first, j = mid;
        iterator ret = nullptr;
        for (++i, (void)++j; i != j && j != last;) {
            if (comp(*j, *i)) {
                if (j._ptr->_suc == last._ptr)
                    ret = bj;
                this->attach_after(bi._ptr, detach_after(bj._ptr));
                bi = i = j;
                ++i;
                j = bj._ptr->_suc;
            } else {
                ++i;
                ++bi;
            }
        }
        return ret;
    }

    template<class Compare>
    iterator sort_range(iterator first, iterator last, Compare comp, size_type n) {
        if (n < 2)
            return ++first;
        iterator mid = ala::next(first, n >> 1);
        iterator bmid = sort_range(first, ++mid, comp, n >> 1);
        iterator bend = sort_range(bmid, last, comp, n - (n >> 1));
        iterator new_bend = merge_range(first, bmid, last, comp);
        return new_bend._ptr ? new_bend : bend;
    }

public:
    template<class Compare>
    void merge(forward_list &other, Compare comp) {
        assert(_alloc == other._alloc);
        // assert(ala::is_sorted(this->begin(), this->end()));
        // assert(ala::is_sorted(other.begin(), other.end()));
        iterator i = this->begin();
        iterator j = other.begin();
        iterator bi = this->before_begin();
        iterator bj = other.before_begin();
        while (j != other.end())
            if (i == end() || comp(*j, *i)) {
                this->attach_after(bi._ptr, other.detach_after(bj._ptr));
                bi = i = j;
                ++i;
                j = bj._ptr->_suc;
            } else {
                ++i;
                ++bi;
            }
    }

    template<class Compare>
    void merge(forward_list &&other, Compare comp) {
        this->merge(other, comp);
    }

    void sort() {
        this->sort(less<>());
    }

    template<class Compare>
    void sort(Compare comp) {
        this->sort_range(before_begin(), end(), comp, size());
    }

    void reverse() noexcept {
        _hdle_t i = head()->_suc;
        _hdle_t nhead = tail();
        for (; i != tail();) {
            _hdle_t tmp = i->_suc;
            link(i, nhead);
            nhead = i;
            i = tmp;
        }
        link(head(), nhead);
    }
};

template<class T, class Alloc>
bool operator==(const forward_list<T, Alloc> &lhs,
                const forward_list<T, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return ala::equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class T, class Alloc>
bool operator!=(const forward_list<T, Alloc> &lhs,
                const forward_list<T, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator<(const forward_list<T, Alloc> &lhs,
               const forward_list<T, Alloc> &rhs) {
    return ala::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template<class T, class Alloc>
bool operator>(const forward_list<T, Alloc> &lhs,
               const forward_list<T, Alloc> &rhs) {
    return rhs < lhs;
}

template<class T, class Alloc>
bool operator<=(const forward_list<T, Alloc> &lhs,
                const forward_list<T, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class T, class Alloc>
bool operator>=(const forward_list<T, Alloc> &lhs,
                const forward_list<T, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class T, class Alloc>
void swap(forward_list<T, Alloc> &lhs,
          forward_list<T, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class InputIter,
         class Alloc = allocator<typename iterator_traits<InputIter>::value_type>>
forward_list(InputIter, InputIter, Alloc = Alloc())
    -> forward_list<typename iterator_traits<InputIter>::value_type, Alloc>;
#endif

// C++20
template<class T, class Alloc, class U>
constexpr typename forward_list<T, Alloc>::size_type
erase(forward_list<T, Alloc> &c, const U &value) {
    return c.remove_if([&value](const auto &elem) { return elem == value; });
}

template<class T, class Alloc, class Pred>
constexpr typename forward_list<T, Alloc>::size_type
erase_if(forward_list<T, Alloc> &c, Pred pred) {
    return c.remove_if(pred);
}
} // namespace ala

#endif