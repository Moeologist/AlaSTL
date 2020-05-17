#ifndef _ALA_VECTOR_H
#define _ALA_VECTOR_H

#include <ala/detail/algorithm_base.h>
#include <ala/detail/allocator.h>
#include <ala/detail/uninitialized_memory.h>

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

private:
    pointer _data = nullptr;
    size_type _capacity = 0;
    size_type _size = 0;
    allocator_type _alloc;

    void dealloc() {
        if (_capacity > 0) {
            assert(_data != nullptr);
            _alloc.deallocate(_data, _capacity);
            _data = nullptr;
            _capacity = 0;
        } else {
            assert(_data == nullptr);
        }
    }

    pointer alloc(size_type n) {
        return _alloc.allocator(n);
    }

    void update(pointer m, size_type capacity, size_type size) {
        assert(m != _data);
        clear_m();
        _data = m;
        _capacity = capacity;
        _size = size;
    }

    template<class... Args>
    void construct(pointer p, Args &&... args) {
        _alloc_traits::construct(_alloc, p, ala::forward<Args>(args)...);
    }

    void destroy(pointer p) {
        _alloc_traits::destroy(_alloc, p);
    }

    template<class... Args>
    pointer fill_e(pointer first, pointer last, Args &&... args) {
        for (; first != last; (void)++first)
            _alloc_traits::construct(_alloc, first, ala::forward<Args>(args)...);
        return first;
    }

    template<class InputIter>
    pointer mv(InputIter first, InputIter last, pointer out) {
        for (; first != last; ++first, (void)++out)
            _alloc_traits::construct(_alloc, out, *first);
        return out;
    }

    template<class InputIter>
    pointer cp(InputIter first, InputIter last, pointer out) {
        for (; first != last; ++first, (void)++out)
            _alloc_traits::construct(_alloc, out, ala::move(*first));
        return out;
    }

    template<class InputIter, class Dummy = value_type>
    enable_if_t<is_nothrow_move_constructible<Dummy>::value ||
                !is_copy_constructible<Dummy>::value>
    mv_cp(InputIter first, InputIter last, pointer dst) {
        return this->mv(first, last, dst);
    }

    template<class InputIter, class Dummy = value_type>
    enable_if_t<!is_nothrow_move_constructible<Dummy>::value &&
                is_copy_constructible<Dummy>::value>
    mv_cp(InputIter first, InputIter last, pointer dst) {
        return this->cp(first, last, dst);
    }

    template<class Dummy = value_type>
    enable_if_t<!is_nothrow_move_constructible<Dummy>::value &&
                is_copy_constructible<Dummy>::value>
    mv_cp(pointer dst) {
        return this->mv_cp(begin(), end(), dst);
    }

    /*
    *********************** 
    ↑          ↑           ↑
 begin()      pos        end()
    [begin, mid) move(copy) to dst1
    [mid, end)   move(copy) to dst2
    */
    void mv_cp_s(pointer mid, pointer dst1, pointer dst2) {
        this->mv_cp(begin(), mid, dst1);
        this->mv_cp(mid, end(), dst2);
    }

    /*
    *********************** 
               ↑           ↑
             start       end()
    [first, end) shift right by diff elements
    */
    void shift_right(pointer first, difference_type diff) {
        pointer mid = end() - diff;
        mid = first < mid ? mid : first;
        this->mv(mid, end(), end());
        mid = mid - 1;
        if (first < mid)
            ala::move_backward(mid, first - 1, end());
    }

    void cut(pointer position) {
        for (; position != end(); ++position)
            this->destroy(position);
        _size = position - _data;
    }

    void realloc(size_type n) {
        pointer new_data = this->allocate(n);
        this->mv_cp(begin(), size(), new_data);
        this->update(new_data, n, _size);
    }

    size_type expand() {
        size_type c = capacity();
        return c > 0 ? c << 1 : 1;
    }

    void clone(const vector &other) {
        size_type n = other.size();
        pointer new_data = this->alloc(n);
        this->cp(other.begin(), other.begin() + n, new_data);
        _capacity = n;
        _data = new_data;
        _size = n;
    }

    void possess(vector &&other) {
        _capacity = other._capacity;
        _data = other._data;
        _size = other._size;
        other._capacity = 0;
        other._data = nullptr;
        other._size = 0;
    }

    void clear_m() {
        clear();
        dealloc();
    }

public:
    // construct/copy/destroy:
    vector() noexcept(noexcept(allocator_type())) {}

    explicit vector(const allocator_type &a) noexcept {}

    explicit vector(size_type n) {
        this->resize(n);
    }

    vector(size_type n, const value_type &v,
           const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->resize(n, v);
    }

    template<class pointer,
             typename = enable_if_t<is_base_of<
                 input_iterator_tag,
                 typename iterator_traits<pointer>::iterator_category>::value>>
    vector(pointer first, pointer last,
           const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->insert(end(), first, last);
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
            this->clone(other);
    }

    vector(initializer_list<value_type> il,
           const allocator_type &a = allocator_type())
        : vector(il.begin(), il.end(), a) {}

    ~vector() {
        clear_m();
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const vector &other) {
        if (_alloc != other._alloc)
            clear_m();
        _alloc = other._alloc;
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const vector &other) {
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(vector &&other) noexcept {
        clear_m();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(vector &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            clear_m();
            this->possess(ala::move(other));
        } else {
            this->assign(other.begin(), other.end());
        }
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(vector other) {
        ala::swap(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy> swap_helper(vector &other) {
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

    void swap(vector &other) {
        this->swap_helper(other);
        swap(_data, other._data);
        swap(_capacity, other._capacity);
        swap(_size, other._size);
    }

protected:
    template<class Size, class InputIter>
    void assign_realloc(Size n, InputIter first) {
        InputIter new_data = this->allocate(n);
        this->cp(first, first + n, new_data);
        this->update(new_data, n, n);
    }

    template<class InputIter>
    void assign_norealloc(InputIter first, InputIter last) {
        size_type i = 0;
        for (; first != last && i < size(); ++first, (void)++i)
            *(_data + i) = *first;
        if (i < size())
            this->resize(i);
        if (first != last)
            this->insert(end(), first, last);
    }

    template<class Size, class InputIter>
    void assign_nv_realloc(size_type n, const value_type &v) {
        InputIter new_data = this->allocate(n);
        this->fill_e(new_data, new_data + n, v);
        this->update(new_data, n, n);
    }

    void assign_nv_norealloc(size_type n, const value_type &v) {
        size_type i = 0;
        for (; n > 0 && i != size(); --n, (void)++i)
            *(_data + i) = v;
        if (i != size())
            this->resize(i);
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
        len = distance(first, last);
        if (len > capacity())
            assign_realloc(len, first);
        else
            assign_norealloc(first, last);
    }

    template<class InputIter>
    enable_if_t<
        is_base_of<input_iterator_tag,
                   typename iterator_traits<InputIter>::iterator_category>::value &&
        !is_base_of<forward_iterator_tag,
                    typename iterator_traits<InputIter>::iterator_category>::value>
    assign(InputIter first, InputIter last) {
        assign_norealloc(first, last);
    }

    void assign(size_type n, const value_type &v) {
        if (n > capacity())
            assign_nv_realloc(n, v);
        else
            assign_nv_no_realloc(n, v);
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
        return end();
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
        return const_cast<vector *>(this)->begin();
    }

    constexpr const_iterator cend() const noexcept {
        return const_cast<vector *>(this)->end();
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_cast<vector *>(this)->rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_cast<vector *>(this)->rend();
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
    void resize_helper(size_type n, Args &&... args) {
        if (size() > n) {
            this->cut(begin() + n);
        } else if (n > capacity()) {
            pointer new_data = this->alloc(n);
            this->fill_e(new_data + size(), new_data + n,
                         ala::forward<Args>(args)...);
            this->mv_cp(new_data);
            this->update(new_data, n, n);
        } else {
            difference_type diff = n - size();
            this->fill_e(end(), end() + diff, ala::forward<Args>(args)...);
            _size += diff;
        }
    }

public:
    void resize(size_type n) {
        this->resize_helper(n);
    }

    void resize(size_type n, const value_type &v) {
        this->resize_helper(n, v);
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
        return _data;
    }

    const value_type *data() const noexcept {
        return _data;
    }

    // modifiers:
    template<class... Args>
    reference emplace_back(Args &&... args) {
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            pointer new_data = this->alloc(new_capa);
            this->construct(new_data + size(), ala::forward<Args>(args)...);
            this->mv_cp(new_data);
            this->update(new_data, new_capa, new_size);
        } else {
            this->construct(end(), ala::forward<Args>(args)...);
            ++_size;
        }
        return *end();
    }

    void push_back(const value_type &v) {
        this->emplace_back(v);
    }

    void push_back(value_type &&v) {
        this->emplace_back(ala::move(v));
    }

    void pop_back() {
        this->destroy(end() - 1);
        --_size;
    }

    template<class... Args>
    iterator emplace(const_iterator position, Args &&... args) {
        difference_type offset = position - cbegin();
        pointer pos = position;
        size_type new_size = size() + 1;
        if (new_size > capacity()) {
            size_type new_capa = expand();
            pointer new_data = this->alloc(new_capa);
            pointer new_pos = new_data + offset;
            this->construct(new_pos, ala::forward<Args>(args)...);
            this->mv_cp_s(pos, new_data, new_pos + 1);
            this->update(new_data, new_capa, new_size);
        } else {
            value_type tmp(ala::forward<Args>(args)...);
            this->shift_right(position, 1);
            this->mv_cp(&tmp, &tmp + 1, pos);
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
        pointer pos = position;
        size_type new_size = size() + n;
        if (new_size > _capacity) {
            size_type new_capa = new_size;
            pointer new_data = this->alloc(new_capa);
            pointer new_pos = new_data + offset;
            this->fill_e(new_pos, new_pos + n, v);
            this->mv_cp_s(pos, new_pos, new_pos + n);
            this->update(new_data, new_capa, new_size);
        } else {
            this->shift_right(offset, n);
            this->fill_e(pos, pos + n, v);
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
        diff_t n = distance(first, last);
        difference_type offset = position - cbegin();
        pointer pos = position;
        size_type new_size = size() + n;
        if (new_size > _capacity) {
            size_type new_capa = new_size;
            pointer new_data = this->alloc(new_capa);
            pointer new_pos = new_data + offset;
            this->cp(first, last, new_pos);
            this->mv_cp_s(pos, new_pos, new_pos + n);
            this->update(new_data, new_capa, new_size);
        } else {
            this->shift_right(offset, n);
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
        size_type n = 0;
        for (; first != last; ++first, ++position, ++n)
            position = this->emplace(position, *first);
        return position - n;
    }

    iterator insert(const_iterator position, initializer_list<value_type> il) {
        return this->insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        this->destroy(position);
        if (position + 1 < cend())
            ala::move(position + 1, cend(), position);
        --_size;
        return (iterator)position;
    }

    iterator erase(const_iterator first, const_iterator last) {
        for (const_iterator i = first; i < last; ++i, --_size)
            this->destroy(i);
        if (last < cend())
            ala::move(last, cend(), first);
        return (iterator)first;
    }

    void clear() noexcept {
        for (iterator i = begin(); i != end(); --_size, (void)++i)
            this->destroy(i);
    }
};

template<class T, class Alloc>
bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return equal(lhs.begin(), lhs.end(), rhs.begin());
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

} // namespace ala

#endif // HEAD