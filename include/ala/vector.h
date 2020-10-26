#ifndef _ALA_VECTOR_H
#define _ALA_VECTOR_H

#include <ala/detail/algorithm_base.h>
#include <ala/detail/allocator.h>

namespace ala {

template<class T, class Alloc = allocator<T>>
class vector {
public:
    // types:
    typedef T value_type;
    typedef Alloc allocator_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef allocator_traits<allocator_type> _alloc_traits;
    typedef typename _alloc_traits::size_type size_type;
    typedef typename _alloc_traits::difference_type difference_type;
    typedef typename _alloc_traits::pointer pointer;
    typedef typename _alloc_traits::const_pointer const_pointer;
    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;
    static_assert(is_same<value_type, typename _alloc_traits::value_type>::value,
                  "allocator::value_type mismatch");

protected:
    pointer _data = nullptr;
    size_type _capacity = 0;
    size_type _size = 0;
    allocator_type _alloc;
    using holder_t = pointer_holder<pointer, Alloc>;

    void update(pointer m, size_type capacity, size_type size) {
        assert(m != _data);
        _data = m;
        _capacity = capacity;
        _size = size;
    }

    template<class... V>
    pointer v_fill(pointer first, pointer last, V &&...v) {
        static_assert(sizeof...(V) == 0 || sizeof...(V) == 1, "Internal error");
        for (; first != last; (void)++first)
            _alloc_traits::construct(_alloc, first, ala::forward<V>(v)...);
        return first;
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
    void migrate2(pointer mid, pointer dst1, pointer dst2) {
        this->migrate(begin(), mid, dst1);
        this->migrate(mid, end(), dst2);
    }

    /*
    *********************** 
               ↑           ↑
             first       end()
    [first, end) shift right by diff elements
    */
    void shift_right(pointer first, difference_type diff) {
        pointer mid = end() - diff;
        mid = first < mid ? mid : first;
        this->mv(mid, end(), end());
        mid = mid - 1;
        if (first < mid)
            ala::move_backward(first, mid, end());
    }

    void cut(pointer position) noexcept {
        pointer e = end();
        for (; position != e; ++position, (void)--_size)
            _alloc_traits::destroy(_alloc, position);
    }

    void realloc(size_type n) {
        holder_t holder(_alloc, n);
        this->migrate(holder.get());
        size_t sz = size();
        this->destroy();
        this->update(holder.release(), n, sz);
    }

    size_type expand() {
        size_type c = capacity();
        return c + (c >> 1) + 1;
    }

    size_type expand2() {
        size_type c = capacity();
        return (c << 1) + 1;
    }

    void clone(const vector &other) {
        if (other.empty())
            return;
        size_type n = other.size();
        holder_t holder(_alloc, n);
        this->cp(other.begin(), other.begin() + n, holder.get());
        this->update(holder.release(), n, n);
    }

    void clone(vector &&other) {
        if (other.empty())
            return;
        size_type n = other.size();
        holder_t holder(_alloc, n);
        this->mv(other.begin(), other.begin() + n, holder.get());
        this->update(holder.release(), n, n);
    }

    void possess(vector &&other) {
        _capacity = other._capacity;
        _data = other._data;
        _size = other._size;
        other._capacity = 0;
        other._data = nullptr;
        other._size = 0;
    }

    void destroy() {
        clear();
        if (_capacity > 0) {
            _alloc.deallocate(_data, _capacity);
            _data = nullptr;
            _capacity = 0;
        }
    }

    template<class InputIter>
    enable_if_t<!is_base_of<forward_iterator_tag,
                            typename iterator_traits<InputIter>::iterator_category>::value>
    iter_ctor_helper(InputIter first, InputIter last) {
        for (; first != last; ++first) {
            size_type new_size = size() + 1;
            if (new_size > capacity()) {
                size_type new_capa = expand2();
                holder_t holder(_alloc, new_capa);
                _alloc_traits::construct(_alloc, holder.get() + size(), *first);
                this->migrate(holder.get());
                this->update(holder.release(), new_capa, new_size);
            } else {
                _alloc_traits::construct(_alloc, end(), *first);
                ++_size;
            }
        }
    }

    template<class ForwardIter>
    enable_if_t<is_base_of<forward_iterator_tag,
                           typename iterator_traits<ForwardIter>::iterator_category>::value>
    iter_ctor_helper(ForwardIter first, ForwardIter last) {
        if (ala::distance(first, last) < 1)
            return;
        size_type new_size = ala::distance(first, last);
        size_type new_capa = new_size;
        holder_t holder(_alloc, new_capa);
        this->cp(first, last, holder.get());
        this->update(holder.release(), new_capa, new_size);
    }

    template<class... V>
    void v_ctor_helper(size_type n, V &&...v) {
        if (n < 1)
            return;
        size_type new_size = n;
        size_type new_capa = new_size;
        holder_t holder(_alloc, new_capa);
        this->v_fill(holder.get(), holder.get() + n, ala::forward<V>(v)...);
        this->update(holder.release(), new_capa, new_size);
    }

public:
    // construct/copy/destroy:
    vector() noexcept(is_nothrow_default_constructible<allocator_type>::value)
        : _alloc() {}

    explicit vector(const allocator_type &a) noexcept: _alloc(a) {}

    explicit vector(size_type n, const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->v_ctor_helper(n);
    }

    vector(size_type n, const value_type &v,
           const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->v_ctor_helper(n, v);
    }

    template<class InputIter,
             typename = enable_if_t<is_base_of<
                 input_iterator_tag,
                 typename iterator_traits<InputIter>::iterator_category>::value>>
    vector(InputIter first, InputIter last,
           const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->iter_ctor_helper(first, last);
    }

    vector(const vector &other)
        : _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)) {
        this->clone(other);
    }

    vector(vector &&other) noexcept: _alloc(ala::move(other._alloc)) {
        this->possess(ala::move(other));
    }

    vector(const vector &other, const allocator_type &a): _alloc(a) {
        this->clone(other);
    }

    vector(vector &&other, const allocator_type &a): _alloc(a) {
        if (_alloc == other._alloc)
            this->possess(ala::move(other));
        else
            this->clone(ala::move(other));
    }

    vector(initializer_list<value_type> il,
           const allocator_type &a = allocator_type())
        : vector(il.begin(), il.end(), a) {}

    ~vector() {
        destroy();
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const vector &other) {
        if (_alloc != other._alloc)
            destroy();
        _alloc = other._alloc;
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const vector &other) {
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(vector &&other) noexcept {
        destroy();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(vector &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            destroy();
            this->possess(ala::move(other));
        } else {
            this->assign(ala::make_move_iterator(other.begin()),
                         ala::make_move_iterator(other.end()));
        }
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(vector &other) noexcept {
        ala::swap(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy>
    swap_helper(vector &other) noexcept(_alloc_traits::is_always_equal::value) {
        assert(_alloc == other._alloc);
    }

public:
    vector &operator=(const vector &other) {
        if (this != addressof(other))
            copy_helper(other);
        return *this;
    }

    vector &operator=(vector &&other) noexcept(
        _alloc_traits::propagate_on_container_move_assignment::value ||
        _alloc_traits::is_always_equal::value) {
        if (this != addressof(other))
            move_helper(ala::move(other));
        return *this;
    }

    vector &operator=(initializer_list<value_type> il) {
        this->assign(il.begin(), il.end());
        return *this;
    }

    void
    swap(vector &other) noexcept(_alloc_traits::propagate_on_container_swap::value ||
                                 _alloc_traits::is_always_equal::value) {
        this->swap_helper(other);
        ala::swap(_data, other._data);
        ala::swap(_capacity, other._capacity);
        ala::swap(_size, other._size);
    }

protected:
    template<class Size, class InputIter>
    void assign_realloc(Size n, InputIter first, InputIter last) {
        holder_t holder(_alloc, n);
        this->cp(first, last, holder.get());
        this->destroy();
        this->update(holder.release(), n, n);
    }

    template<class InputIter>
    void assign_norealloc(InputIter first, InputIter last) {
        size_type i = 0;
        for (; first != last && i < size(); ++first, (void)++i)
            *(_data + i) = *first;
        if (i < size())
            this->cut(begin() + i);
        if (first != last)
            this->insert(cend(), first, last);
    }

    void assign_nv_realloc(size_type n, const value_type &v) {
        holder_t holder(_alloc, n);
        this->v_fill(holder.get(), holder.get() + n, v);
        this->destroy();
        this->update(holder.release(), n, n);
    }

    void assign_nv_norealloc(size_type n, const value_type &v) {
        size_type i = 0;
        for (; n > 0 && i != size(); --n, (void)++i)
            *(_data + i) = v;
        if (i != size())
            this->cut(begin() + i);
        if (n > 0)
            this->insert(end(), n, v);
    }

public:
    template<class InputIter>
    enable_if_t<
        is_base_of<input_iterator_tag,
                   typename iterator_traits<InputIter>::iterator_category>::value &&
        is_base_of<forward_iterator_tag,
                   typename iterator_traits<InputIter>::iterator_category>::value>
    assign(InputIter first, InputIter last) {
        typename iterator_traits<InputIter>::difference_type len;
        len = ala::distance(first, last);
        if (len > capacity())
            this->assign_realloc(len, first, last);
        else
            this->assign_norealloc(first, last);
    }

    template<class InputIter>
    enable_if_t<
        is_base_of<input_iterator_tag,
                   typename iterator_traits<InputIter>::iterator_category>::value &&
        !is_base_of<forward_iterator_tag,
                    typename iterator_traits<InputIter>::iterator_category>::value>
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
        return _data;
    }

    constexpr const_iterator begin() const noexcept {
        return cbegin();
    }

    constexpr iterator end() noexcept {
        return _data + size();
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
        return const_iterator(const_cast<vector *>(this)->begin());
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(const_cast<vector *>(this)->end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crend() const noexcept {
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
        if (size() > n) {
            this->cut(begin() + n);
        } else if (n > capacity()) {
            holder_t holder(_alloc, n);
            this->v_fill(holder.get() + size(), holder.get() + n,
                         ala::forward<V>(v)...);
            this->migrate(holder.get());
            this->destroy();
            this->update(holder.release(), n, n);
        } else {
            difference_type diff = n - size();
            this->v_fill(end(), end() + diff, ala::forward<V>(v)...);
            _size += diff;
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
        return _capacity;
    }

    ALA_NODISCARD bool empty() const noexcept {
        return _size == 0;
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
        return _data[n];
    }

    const_reference operator[](size_type n) const {
        return _data[n];
    }

    constexpr reference at(size_type n) {
        if (n >= _size)
            throw out_of_range("ala::vector index out of range");
        return _data[n];
    }

    constexpr const_reference at(size_type n) const {
        if (n >= _size)
            throw out_of_range("ala::vector index out of range");
        return _data[n];
    }

    reference front() {
        return _data[0];
    }

    const_reference front() const {
        return _data[0];
    }

    reference back() {
        return _data[_size - 1];
    }

    const_reference back() const {
        return _data[_size - 1];
    }

    // data access:
    value_type *data() noexcept {
        if (empty())
            return nullptr;
        return ala::addressof(*_data);
    }

    const value_type *data() const noexcept {
        if (empty())
            return nullptr;
        return ala::addressof(*_data);
    }

    // modifiers:
    template<class... Args>
    reference emplace_back(Args &&...args) {
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            holder_t holder(_alloc, new_capa);
            _alloc_traits::construct(_alloc, holder.get() + size(),
                                     ala::forward<Args>(args)...);
            this->migrate(holder.get());
            this->destroy();
            this->update(holder.release(), new_capa, new_size);
        } else {
            _alloc_traits::construct(_alloc, end(), ala::forward<Args>(args)...);
            ++_size;
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
        _alloc_traits::destroy(_alloc, end() - 1);
        --_size;
    }

    template<class... Args>
    iterator emplace(const_iterator position, Args &&...args) {
        difference_type offset = position - cbegin();
        pointer pos = begin() + offset;
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            holder_t holder(_alloc, new_capa);
            pointer new_pos = holder.get() + offset;
            _alloc_traits::construct(_alloc, new_pos,
                                     ala::forward<Args>(args)...);
            this->migrate2(pos, holder.get(), new_pos + 1);
            this->destroy();
            this->update(holder.release(), new_capa, new_size);
        } else {
            value_type tmp(ala::forward<Args>(args)...);
            this->shift_right(pos, 1);
            this->migrate(&tmp, &tmp + 1, pos);
            ++_size;
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
        pointer pos = begin() + offset;
        size_type new_size = size() + n;
        if (new_size > capacity()) {
            size_type new_capa = new_size;
            holder_t holder(_alloc, new_capa);
            pointer new_pos = holder.get() + offset;
            this->v_fill(new_pos, new_pos + n, v);
            this->migrate2(pos, holder.get(), new_pos + n);
            this->destroy();
            this->update(holder.release(), new_capa, new_size);
        } else {
            this->shift_right(pos, n);
            this->v_fill(pos, pos + n, v);
            _size += n;
        }
        return begin() + offset;
    }

    template<class InputIter>
    enable_if_t<
        is_base_of<input_iterator_tag,
                   typename iterator_traits<InputIter>::iterator_category>::value &&
            is_base_of<forward_iterator_tag,
                       typename iterator_traits<InputIter>::iterator_category>::value,
        iterator>
    insert(const_iterator position, InputIter first, InputIter last) {
        typedef typename iterator_traits<InputIter>::difference_type diff_t;
        diff_t n = ala::distance(first, last);
        difference_type offset = position - cbegin();
        pointer pos = begin() + offset;
        size_type new_size = size() + n;
        if (new_size > _capacity) {
            size_type new_capa = new_size;
            holder_t holder(_alloc, new_capa);
            pointer new_pos = holder.get() + offset;
            this->cp(first, last, new_pos);
            this->migrate2(pos, holder.get(), new_pos + n);
            this->destroy();
            this->update(holder.release(), new_capa, new_size);
        } else {
            this->shift_right(pos, n);
            this->cp(first, last, pos);
            _size += n;
        }
        return begin() + offset;
    }

    template<class InputIter>
    enable_if_t<
        is_base_of<input_iterator_tag,
                   typename iterator_traits<InputIter>::iterator_category>::value &&
            !is_base_of<forward_iterator_tag,
                        typename iterator_traits<InputIter>::iterator_category>::value,
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
        pointer pos = begin() + (position - cbegin());
        if (pos == end())
            return end();
        ala::move(pos + 1, end(), pos);
        pop_back();
        return pos;
    }

    iterator erase(const_iterator first, const_iterator last) {
        pointer left = begin() + (first - cbegin());
        pointer rght = begin() + (last - cbegin());
        difference_type n = rght - left;
        ala::move(rght, end(), left);
        this->cut(end() - n);
        return left;
    }

    void clear() noexcept {
        this->cut(begin());
    }
};

template<class T, class Alloc>
bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return ala::equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class T, class Alloc>
bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator<(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return ala::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template<class T, class Alloc>
bool operator>(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return rhs < lhs;
}

template<class T, class Alloc>
bool operator<=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class T, class Alloc>
bool operator>=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class T, class Alloc>
void swap(vector<T, Alloc> &lhs,
          vector<T, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class InputIter,
         class Alloc = allocator<typename iterator_traits<InputIter>::value_type>>
vector(InputIter, InputIter, Alloc = Alloc())
    -> vector<typename iterator_traits<InputIter>::value_type, Alloc>;
#endif

// C++20
template<class T, class Alloc, class U>
constexpr typename vector<T, Alloc>::size_type erase(vector<T, Alloc> &c,
                                                     const U &value) {
    using iter_t = typename vector<T, Alloc>::iterator;
    using diff_t = typename vector<T, Alloc>::difference_type;
    iter_t i = ala::remove(c.begin(), c.end(), value);
    diff_t n = ala::distance(i, c.end());
    c.erase(i, c.end());
    return n;
}

template<class T, class Alloc, class Pred>
constexpr typename vector<T, Alloc>::size_type erase_if(vector<T, Alloc> &c,
                                                        Pred pred) {
    using iter_t = typename vector<T, Alloc>::iterator;
    using diff_t = typename vector<T, Alloc>::difference_type;
    iter_t i = ala::remove_if(c.begin(), c.end(), pred);
    diff_t n = ala::distance(i, c.end());
    c.erase(i, c.end());
    return n;
}

} // namespace ala

#endif // HEAD