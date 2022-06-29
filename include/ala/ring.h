#ifndef _ALA_RING_H
#define _ALA_RING_H

#include <ala/detail/algorithm_base.h>
#include <ala/detail/allocator.h>
#include <ala/iterator.h>

namespace ala {

template<class Value, class Ring>
struct ring_iterator {
#if ALA_API_VER >= 20
    using iterator_concept = random_access_iterator_tag;
#endif
    using iterator_category = random_access_iterator_tag;
    using value_type = Value;
    using difference_type = typename Ring::difference_type;
    using size_type = typename Ring::size_type;
    using pointer = typename Ring::pointer;
    using reference = value_type &;

    constexpr ring_iterator() {}

    constexpr ring_iterator(const ring_iterator &other)
        : _ref(other._ref), _ptr(other._ptr) {}

    template<class Value1, class Ptr1>
    constexpr ring_iterator(const ring_iterator<Value1, Ptr1> &other)
        : _ref(other._ref), _ptr(other._ptr) {}

    constexpr pointer operator->() const {
        return _ptr;
    }

    constexpr reference operator*() const {
        return *(this->operator->());
    }

    template<class Value1, class Ptr1>
    constexpr bool operator==(const ring_iterator<Value1, Ptr1> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator!=(const ring_iterator<Value1, Ptr1> &rhs) const {
        return !(*this == rhs);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator<(const ring_iterator<Value1, Ptr1> &rhs) const {
        if (!_ref)
            return _ptr < rhs._ptr;
        return _ref->_ptr2idx(_ptr) < rhs._ref->_ptr2idx(rhs._ptr);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator>(const ring_iterator<Value1, Ptr1> &rhs) const {
        return rhs < *this;
    }

    template<class Value1, class Ptr1>
    constexpr bool operator<=(const ring_iterator<Value1, Ptr1> &rhs) const {
        return !(rhs < *this);
    }

    template<class Value1, class Ptr1>
    constexpr bool operator>=(const ring_iterator<Value1, Ptr1> &rhs) const {
        return !(*this < rhs);
    }

    constexpr void _adv(difference_type diff) {
        if (!_ref)
            return;
        size_type idx = _ref->_ptr2idx(_ptr);
        idx = _ref->_diff(idx, diff);
        _ptr = _ref->_idx2ptr(idx);
    }

    constexpr ring_iterator &operator++() {
        _adv(1);
        return *this;
    }

    constexpr ring_iterator operator++(int) {
        ring_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr ring_iterator &operator--() {
        _adv(-1);
        return *this;
    }

    constexpr ring_iterator operator--(int) {
        ring_iterator tmp(*this);
        --*this;
        return tmp;
    }

    constexpr ring_iterator &operator+=(difference_type n) {
        _adv(n);
        return *this;
    }

    constexpr ring_iterator &operator-=(difference_type n) {
        _adv(-n);
        return *this;
    }

    constexpr ring_iterator operator+(difference_type n) const {
        ring_iterator tmp(*this);
        tmp += n;
        return tmp;
    }

    constexpr ring_iterator operator-(difference_type n) const {
        ring_iterator tmp(*this);
        tmp -= n;
        return tmp;
    }

    template<class Value1, class Ptr1>
    constexpr difference_type
    operator-(const ring_iterator<Value1, Ptr1> &rhs) const {
        if (!_ref)
            return 0;
        difference_type l = (difference_type)_ref->_ptr2idx(_ptr);
        difference_type r = (difference_type)rhs._ref->_ptr2idx(rhs._ptr);
        return l - r;
    }

    friend constexpr ring_iterator operator+(difference_type lhs,
                                             ring_iterator rhs) {
        return rhs + lhs;
    }

    constexpr reference operator[](difference_type n) const {
        return *(*this + n);
    }

protected:
    template<class, class>
    friend class ring_iterator;

    template<class, class>
    friend class ring;

    constexpr ring_iterator(const Ring *ref, pointer ptr)
        : _ref(ref), _ptr(ptr) {}

    const Ring *_ref = nullptr;
    pointer _ptr = nullptr;
};

template<class T, class Alloc = allocator<T>>
class ring {
public:
    // types:
    using value_type = T;
    using allocator_type = Alloc;
    using reference = value_type &;
    using const_reference = const value_type &;
    using _alloc_traits = allocator_traits<allocator_type>;
    using size_type = typename _alloc_traits::size_type;
    using difference_type = typename _alloc_traits::difference_type;
    using pointer = typename _alloc_traits::pointer;
    using const_pointer = typename _alloc_traits::const_pointer;
    using iterator = ring_iterator<value_type, ring>;
    using const_iterator = ring_iterator<const value_type, ring>;
    using reverse_iterator = ala::reverse_iterator<iterator>;
    using const_reverse_iterator = ala::reverse_iterator<const_iterator>;
    static_assert(is_same<value_type, typename _alloc_traits::value_type>::value,
                  "allocator::value_type mismatch");

protected:
    pointer _data = nullptr;
    size_type _circ = 1;
    size_type _head = 0;
    size_type _tail = 0;
    allocator_type _alloc;
    using holder_t = pointer_holder<pointer, Alloc>;

    pointer _idx2ptr(size_type idx) const {
        return _data + ((_head + idx) % _circ);
    }

    size_type _ptr2idx(pointer ptr) const {
        return (ptr - _data + _circ - _head) % _circ;
    }

    iterator _idx2it(size_type idx) const {
        return iterator(this, _idx2ptr(idx));
    }

    size_type _diff(size_type idx, difference_type diff) const {
        return (idx + _circ + diff) % _circ;
    }

    template<class, class>
    friend class ring_iterator;

    void update(pointer m, size_type l, size_type h, size_type t) {
        assert(m != _data);
        _data = m;
        _circ = l;
        _head = h;
        _tail = t;
    }

    template<class... V>
    pointer v_fill(pointer first, pointer last, V &&...v) {
        static_assert(sizeof...(V) == 0 || sizeof...(V) == 1, "Internal error");
        pointer i = first;
        try {
            for (; i != last; ++i)
                _alloc_traits::construct(_alloc, i, ala::forward<V>(v)...);
        } catch (...) {
            for (; i != first;)
                _alloc_traits::destroy(_alloc, --i);
            throw;
        }
        return i;
    }

    template<class InputIter>
    void mv(InputIter first, InputIter last, pointer out) {
        InputIter i = first;
        try {
            for (; i != last; ++i, (void)++out)
                _alloc_traits::construct(_alloc, out, ala::move(*i));
        } catch (...) {
            for (; i != first; ++first, (void)--out)
                _alloc_traits::destroy(_alloc, out);
            throw;
        }
    }

    template<class InputIter>
    void cp(InputIter first, InputIter last, pointer out) {
        InputIter i = first;
        try {
            for (; i != last; ++i, (void)++out)
                _alloc_traits::construct(_alloc, out, *i);
        } catch (...) {
            for (; i != first; ++first, (void)--out)
                _alloc_traits::destroy(_alloc, out);
            throw;
        }
    }

    template<class... V>
    iterator v_fill(iterator first, iterator last, V &&...v) {
        static_assert(sizeof...(V) == 0 || sizeof...(V) == 1, "Internal error");
        iterator i = first;
        try {
            for (; i != last; ++i)
                _alloc_traits::construct(_alloc, i.operator->(),
                                         ala::forward<V>(v)...);
        } catch (...) {
            for (; i != first;)
                _alloc_traits::destroy(_alloc, (--i).operator->());
            throw;
        }
        return i;
    }

    template<class InputIter>
    void mv(InputIter first, InputIter last, iterator out) {
        InputIter i = first;
        try {
            for (; i != last; ++i, (void)++out)
                _alloc_traits::construct(_alloc, out.operator->(), ala::move(*i));
        } catch (...) {
            for (; i != first; ++first, (void)--out)
                _alloc_traits::destroy(_alloc, out.operator->());
            throw;
        }
    }

    template<class InputIter>
    void cp(InputIter first, InputIter last, iterator out) {
        InputIter i = first;
        try {
            for (; i != last; ++i, (void)++out)
                _alloc_traits::construct(_alloc, out.operator->(), *i);
        } catch (...) {
            for (; i != first; ++first, (void)--out)
                _alloc_traits::destroy(_alloc, out.operator->());
            throw;
        }
    }

    template<class InputIter, class Dummy = value_type>
    enable_if_t<(is_nothrow_move_constructible<Dummy>::value ||
                 !is_copy_constructible<Dummy>::value)>
    migrate(InputIter first, InputIter last, pointer dst) {
        return this->mv(first, last, dst);
    }

    template<class InputIter, class Dummy = value_type>
    enable_if_t<!(is_nothrow_move_constructible<Dummy>::value ||
                  !is_copy_constructible<Dummy>::value)>
    migrate(InputIter first, InputIter last, pointer dst) {
        return this->cp(first, last, dst);
    }

    void migrate(pointer dst) {
        return this->migrate(begin(), end(), dst);
    }

    /*
    *********************** 
    ↑          ↑           ↑
 begin()      pos        end()
    [begin, mid) move(copy) to dst1
    [mid, end)   move(copy) to dst2
    */
    void migrate2(iterator mid, pointer dst1, pointer dst2) {
        this->migrate(begin(), mid, dst1);
        this->migrate(mid, end(), dst2);
    }

    void cut(iterator position) noexcept {
        difference_type n = 0;
        for (; position != end(); ++position, (void)++n)
            _alloc_traits::destroy(_alloc, position._ptr);
        if (n != 0)
            _tail = this->_diff(_tail, -n);
    }

    void cut_head(iterator position) noexcept {
        difference_type n = 0;
        for (; position != begin(); (void)++n)
            _alloc_traits::destroy(_alloc, (--position)._ptr);
        if (n != 0)
            _head = this->_diff(_head, n);
    }

    void realloc(size_type n) {
        holder_t holder(_alloc, n + 1);
        this->migrate(holder.get());
        size_t sz = size();
        this->destroy();
        this->update(holder.release(), n + 1, 0, sz);
    }

    size_type expand() {
        size_type c = capacity();
        return c + (c >> 1) + 1;
    }

    void clone(const ring &other) {
        if (other.empty())
            return;
        size_type n = other.size();
        holder_t holder(_alloc, n + 1);
        this->cp(other.begin(), other.end(), holder.get());
        this->update(holder.release(), n + 1, 0, n);
    }

    void clone(ring &&other) {
        if (other.empty())
            return;
        size_type n = other.size();
        holder_t holder(_alloc, n + 1);
        this->mv(other.begin(), other.end(), holder.get());
        this->update(holder.release(), n + 1, 0, n);
    }

    void possess(ring &&other) {
        _circ = other._circ;
        _data = other._data;
        _head = other._head;
        _tail = other._tail;
        other._circ = 1;
        other._data = nullptr;
        other._head = 0;
        other._tail = 0;
    }

    void destroy() {
        clear();
        if (_data) {
            _alloc.deallocate(_data, _circ);
            _data = nullptr;
            _circ = 1;
        }
    }

    template<class InputIter>
    enable_if_t<!is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>::value>
    iter_ctor_helper(InputIter first, InputIter last) {
        for (; first != last; ++first)
            this->emplace_back(*first);
    }

    template<class ForwardIter>
    enable_if_t<is_base_of<forward_iterator_tag, _iter_tag_t<ForwardIter>>::value>
    iter_ctor_helper(ForwardIter first, ForwardIter last) {
        size_type new_size = ala::distance(first, last);
        if (new_size < 1)
            return;
        size_type new_capa = new_size;
        holder_t holder(_alloc, new_capa + 1);
        this->cp(first, last, holder.get());
        this->update(holder.release(), new_capa + 1, 0, new_size);
    }

    template<class... V>
    void v_ctor_helper(size_type n, V &&...v) {
        if (n < 1)
            return;
        size_type new_size = n;
        size_type new_capa = new_size;
        holder_t holder(_alloc, new_capa + 1);
        this->v_fill(holder.get(), holder.get() + n, ala::forward<V>(v)...);
        this->update(holder.release(), new_capa + 1, 0, new_size);
    }

public:
    // construct/copy/destroy:
    ring() noexcept(is_nothrow_default_constructible<allocator_type>::value)
        : _alloc() {}

    explicit ring(const allocator_type &a) noexcept: _alloc(a) {}

    explicit ring(size_type n, const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->v_ctor_helper(n);
    }

    ring(size_type n, const value_type &v,
         const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->v_ctor_helper(n, v);
    }

    template<class InputIter,
             typename = enable_if_t<
                 is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value>>
    ring(InputIter first, InputIter last,
         const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->iter_ctor_helper(first, last);
    }

    ring(const ring &other)
        : _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)) {
        this->clone(other);
    }

    ring(ring &&other) noexcept: _alloc(ala::move(other._alloc)) {
        this->possess(ala::move(other));
    }

    ring(const ring &other, const type_identity_t<allocator_type> &a)
        : _alloc(a) {
        this->clone(other);
    }

    ring(ring &&other, const type_identity_t<allocator_type> &a): _alloc(a) {
        if (_alloc == other._alloc)
            this->possess(ala::move(other));
        else
            this->clone(ala::move(other));
    }

    ring(initializer_list<value_type> il,
         const allocator_type &a = allocator_type())
        : ring(il.begin(), il.end(), a) {}

    ~ring() {
        destroy();
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const ring &other) {
        if (_alloc != other._alloc)
            destroy();
        _alloc = other._alloc;
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const ring &other) {
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(ring &&other) noexcept {
        destroy();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(ring &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            destroy();
            this->possess(ala::move(other));
        } else {
            this->assign(ala::make_move_iterator(other.begin()),
                         ala::make_move_iterator(other.end()));
        }
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(ring &other) noexcept {
        ala::_swap_adl(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy>
    swap_helper(ring &other) noexcept(_alloc_traits::is_always_equal::value) {
        assert(_alloc == other._alloc);
    }

public:
    ring &operator=(const ring &other) {
        if (this != addressof(other))
            copy_helper(other);
        return *this;
    }

    ring &operator=(ring &&other) noexcept(
        _alloc_traits::propagate_on_container_move_assignment::value ||
        _alloc_traits::is_always_equal::value) {
        if (this != addressof(other))
            move_helper(ala::move(other));
        return *this;
    }

    ring &operator=(initializer_list<value_type> il) {
        this->assign(il.begin(), il.end());
        return *this;
    }

    void
    swap(ring &other) noexcept(_alloc_traits::propagate_on_container_swap::value ||
                               _alloc_traits::is_always_equal::value) {
        this->swap_helper(other);
        ala::_swap_adl(_data, other._data);
        ala::_swap_adl(_circ, other._circ);
        ala::_swap_adl(_head, other._head);
        ala::_swap_adl(_tail, other._tail);
    }

protected:
    template<class Size, class InputIter>
    void assign_realloc(Size n, InputIter first, InputIter last) {
        holder_t holder(_alloc, n + 1);
        this->cp(first, last, holder.get());
        this->destroy();
        this->update(holder.release(), n + 1, 0, n);
    }

    template<class InputIter>
    void assign_norealloc(InputIter first, InputIter last) {
        size_type i = 0;
        for (; first != last && i < size(); ++first, (void)++i)
            *(begin() + i) = *first;
        if (i < size())
            this->cut(begin() + i);
        if (first != last)
            this->insert(cend(), first, last);
    }

    void assign_nv_realloc(size_type n, const value_type &v) {
        holder_t holder(_alloc, n + 1);
        this->v_fill(holder.get(), holder.get() + n, v);
        this->destroy();
        this->update(holder.release(), n + 1, 0, n);
    }

    void assign_nv_norealloc(size_type n, const value_type &v) {
        size_type i = 0;
        for (; n > 0 && i != size(); --n, (void)++i)
            *(begin() + i) = v;
        if (i != size())
            this->cut(begin() + i);
        if (n > 0)
            this->insert(end(), n, v);
    }

public:
    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value &&
                is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>::value>
    assign(InputIter first, InputIter last) {
        typename iterator_traits<InputIter>::difference_type len;
        len = ala::distance(first, last);
        if (len > capacity())
            this->assign_realloc(len, first, last);
        else
            this->assign_norealloc(first, last);
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value &&
                !is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>::value>
    assign(InputIter first, InputIter last) {
        this->assign_norealloc(first, last);
    }

    void assign(size_type n, const value_type &v) {
        if (n > capacity())
            this->assign_nv_realloc(n, v);
        else
            this->assign_nv_norealloc(n, v);
    }

    void assign(initializer_list<value_type> il) {
        this->assign(il.begin(), il.end());
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    // iterator:
    constexpr iterator begin() noexcept {
        return this->_idx2it(0);
    }

    constexpr const_iterator begin() const noexcept {
        return cbegin();
    }

    constexpr iterator end() noexcept {
        return this->_idx2it(size());
    }

    constexpr const_iterator end() const noexcept {
        return cend();
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return crbegin();
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return crend();
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(const_cast<ring *>(this)->begin());
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(const_cast<ring *>(this)->end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // capacity:
    size_type size() const noexcept {
        return (_tail + _circ - _head) % _circ;
    }

    size_type max_size() const noexcept {
        return _alloc_traits::max_size(_alloc);
    }

protected:
    template<class... V>
    void v_resize(size_type n, V &&...v) {
        if (size() > n) {
            this->cut(this->_idx2it(n));
        } else if (n > capacity()) {
            holder_t holder(_alloc, n + 1);
            this->v_fill(holder.get() + size(), holder.get() + n,
                         ala::forward<V>(v)...);
            this->migrate(holder.get());
            this->destroy();
            this->update(holder.release(), n + 1, 0, n);
        } else {
            difference_type diff = n - size();
            this->v_fill(end(), end() + diff, ala::forward<V>(v)...);
            _tail = this->_diff(_tail, diff);
        }
    }

public:
    void resize(size_type n) {
        this->v_resize(n);
    }

    void resize(size_type n, const value_type &v) {
        this->v_resize(n, v);
    }

    size_type capacity() const noexcept {
        return _circ - 1;
    }

    ALA_NODISCARD bool empty() const noexcept {
        return _head == _tail;
    }

    void reserve(size_type n) {
        if (n > capacity())
            this->realloc(n);
    }

    void shrink_to_fit() {
        if (capacity() > size())
            this->realloc(size());
    }

    // element access:
    reference operator[](size_type n) {
        return _data[(_head + n) % _circ];
    }

    const_reference operator[](size_type n) const {
        return _data[(_head + n) % _circ];
    }

    constexpr reference at(size_type n) {
        if (!(n < size()))
            throw out_of_range("ala::ring index out of range");
        return _data[n];
    }

    constexpr const_reference at(size_type n) const {
        if (!(n < size()))
            throw out_of_range("ala::ring index out of range");
        return _data[n];
    }

    reference front() {
        return _data[_head];
    }

    const_reference front() const {
        return _data[_head];
    }

    reference back() {
        return _data[this->_diff(_tail, -1)];
    }

    const_reference back() const {
        return _data[this->_diff(_tail, -1)];
    }

    // modifiers:
    template<class... Args>
    reference emplace_back(Args &&...args) {
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            holder_t holder(_alloc, new_capa + 1);
            _alloc_traits::construct(_alloc, holder.get() + size(),
                                     ala::forward<Args>(args)...);
            this->migrate(holder.get());
            this->destroy();
            this->update(holder.release(), new_capa + 1, 0, new_size);
        } else {
            _alloc_traits::construct(_alloc, this->_idx2ptr(size()),
                                     ala::forward<Args>(args)...);
            _tail = this->_diff(_tail, 1);
        }
        return back();
    }

    void push_back(const value_type &v) {
        this->emplace_back(v);
    }

    void push_back(value_type &&v) {
        this->emplace_back(ala::move(v));
    }

    void pop_back() {
        _alloc_traits::destroy(_alloc, this->_idx2ptr(size() - 1));
        _tail = this->_diff(_tail, -1);
    }

    template<class... Args>
    reference emplace_front(Args &&...args) {
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            holder_t holder(_alloc, new_capa + 1);
            _alloc_traits::construct(_alloc, holder.get(),
                                     ala::forward<Args>(args)...);
            this->migrate(holder.get() + 1);
            this->destroy();
            this->update(holder.release(), new_capa + 1, 0, new_size);
        } else {
            _head = this->_diff(_head, -1);
            _alloc_traits::construct(_alloc, this->_idx2ptr(0),
                                     ala::forward<Args>(args)...);
        }
        return front();
    }

    void push_front(const value_type &v) {
        this->emplace_front(v);
    }

    void push_front(value_type &&v) {
        this->emplace_front(ala::move(v));
    }

    void pop_front() {
        _alloc_traits::destroy(_alloc, this->_idx2ptr(0));
        _head = this->_diff(_head, 1);
    }

    template<class... Args>
    iterator emplace(const_iterator position, Args &&...args) {
        difference_type offset = position - cbegin();
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            holder_t holder(_alloc, new_capa + 1);
            pointer new_pos = holder.get() + offset;
            _alloc_traits::construct(_alloc, new_pos,
                                     ala::forward<Args>(args)...);
            this->migrate2(begin() + offset, holder.get(), new_pos + 1);
            this->destroy();
            this->update(holder.release(), new_capa + 1, 0, new_size);
        } else {
            _alloc_traits::construct(_alloc, this->_idx2ptr(size()),
                                     ala::forward<Args>(args)...);
            _tail = this->_diff(_tail, 1);
            ala::rotate(begin() + offset, end() - 1, end());
        }
        return begin() + offset;
    }

    iterator insert(const_iterator position, const value_type &v) {
        return this->emplace(position, v);
    }

    iterator insert(const_iterator position, value_type &&v) {
        return this->emplace(position, ala::move(v));
    }

    iterator insert(const_iterator position, size_type n, const value_type &v) {
        difference_type offset = position - cbegin();
        size_type new_size = size() + n;
        if (new_size > capacity()) {
            size_type new_capa = new_size;
            holder_t holder(_alloc, new_capa + 1);
            pointer new_pos = holder.get() + offset;
            this->v_fill(new_pos, new_pos + n, v);
            this->migrate2(begin() + offset, holder.get(), new_pos + n);
            this->destroy();
            this->update(holder.release(), new_capa + 1, 0, new_size);
        } else {
            this->v_fill(end(), end() + n, v);
            _tail = this->_diff(_tail, n);
            ala::rotate(begin() + offset, end() - n, end());
        }
        return begin() + offset;
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value &&
                    is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>::value,
                iterator>
    insert(const_iterator position, InputIter first, InputIter last) {
        using diff_t = typename iterator_traits<InputIter>::difference_type;
        diff_t n = ala::distance(first, last);
        difference_type offset = position - cbegin();
        size_type new_size = size() + n;
        if (new_size > capacity()) {
            size_type new_capa = new_size;
            holder_t holder(_alloc, new_capa + 1);
            pointer new_pos = holder.get() + offset;
            this->cp(first, last, new_pos);
            this->migrate2(begin() + offset, holder.get(), new_pos + n);
            this->destroy();
            this->update(holder.release(), new_capa + 1, 0, new_size);
        } else {
            this->cp(first, last, end());
            _tail = this->_diff(_tail, n);
            ala::rotate(begin() + offset, end() - n, end());
        }
        return begin() + offset;
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag, _iter_tag_t<InputIter>>::value &&
                    !is_base_of<forward_iterator_tag, _iter_tag_t<InputIter>>::value,
                iterator>
    insert(const_iterator position, InputIter first, InputIter last) {
        difference_type offset = position - cbegin();
        for (; first != last; ++first, (void)++position)
            position = this->emplace(position, *first);
        return begin() + offset;
    }

    iterator insert(const_iterator position, initializer_list<value_type> il) {
        return this->insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        difference_type offset = position - cbegin();
        iterator pos = begin() + offset;
        if (pos == end())
            return end();

        if (offset < end() - (pos + 1)) {
            ala::move_backward(begin(), pos, pos + 1);
            pop_front();
            return pos + 1;
        }

        ala::move(pos + 1, end(), pos);
        pop_back();
        return pos;
    }

    iterator erase(const_iterator first, const_iterator last) {
        difference_type offset1 = first - cbegin();
        difference_type offset2 = last - cbegin();
        iterator left = begin() + offset1;
        iterator rght = begin() + offset2;
        if (first == last)
            return left;

        difference_type n = rght - left;

        if (offset1 < cend() - last) {
            ala::move_backward(begin(), left, rght);
            this->cut_head(this->_idx2it(n));
            return rght;
        }

        ala::move(rght, end(), left);
        this->cut(this->_idx2it(size() - n));
        return left;
    }

    void clear() noexcept {
        this->cut(begin());
        _head = _tail = 0;
    }
};

template<class T, class Alloc>
bool operator==(const ring<T, Alloc> &lhs, const ring<T, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return ala::equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class T, class Alloc>
bool operator!=(const ring<T, Alloc> &lhs, const ring<T, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator<(const ring<T, Alloc> &lhs, const ring<T, Alloc> &rhs) {
    return ala::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template<class T, class Alloc>
bool operator>(const ring<T, Alloc> &lhs, const ring<T, Alloc> &rhs) {
    return rhs < lhs;
}

template<class T, class Alloc>
bool operator<=(const ring<T, Alloc> &lhs, const ring<T, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class T, class Alloc>
bool operator>=(const ring<T, Alloc> &lhs, const ring<T, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class T, class Alloc>
void swap(ring<T, Alloc> &lhs,
          ring<T, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class InputIter,
         class Alloc = allocator<typename iterator_traits<InputIter>::value_type>>
ring(InputIter, InputIter, Alloc = Alloc())
    -> ring<typename iterator_traits<InputIter>::value_type, Alloc>;
#endif

// C++20
template<class T, class Alloc, class U>
constexpr typename ring<T, Alloc>::size_type erase(ring<T, Alloc> &c,
                                                   const U &value) {
    using iter_t = typename ring<T, Alloc>::iterator;
    using diff_t = typename ring<T, Alloc>::difference_type;
    iter_t i = ala::remove(c.begin(), c.end(), value);
    diff_t n = ala::distance(i, c.end());
    c.erase(i, c.end());
    return n;
}

template<class T, class Alloc, class Pred>
constexpr typename ring<T, Alloc>::size_type erase_if(ring<T, Alloc> &c,
                                                      Pred pred) {
    using iter_t = typename ring<T, Alloc>::iterator;
    using diff_t = typename ring<T, Alloc>::difference_type;
    iter_t i = ala::remove_if(c.begin(), c.end(), pred);
    diff_t n = ala::distance(i, c.end());
    c.erase(i, c.end());
    return n;
}

} // namespace ala

#endif // HEAD