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

private:
    pointer _data = nullptr;
    size_type _capacity = 0;
    size_type _size = 0;
    allocator_type _alloc;

    void dealloc() {
        assert(_data != nullptr && _capacity > 0);
        _alloc.deallocate(_data, _capacity);
        _capacity = 0;
    }

    void alloc(size_type n) {
        assert(_alloc == nullptr && _capacity == 0);
        _data = _alloc.allocator(n);
        _capacity = n;
    }

    enable_if_t<is_trivial<value_type>::value>
    cp_elements(pointer dst, pointer first, pointer last) {
        ala::memmove((void *)(dst), (void *)(first),
                     sizeof(value_type) * (last - first));
    }

    enable_if_t<!is_trivial<value_type>::value>
    cp_elements(pointer dst, pointer first, pointer last) {
        for (; first < last; ++first, ++dst)
            _alloc_traits::construct(_alloc, dst, *first);
    }

    enable_if_t<!is_trivial<value_type>::value>
    mv_elements(pointer dst, pointer first, pointer last) {
        ala::memmove((void *)(dst), (void *)(first),
                     sizeof(value_type) * (last - first));
    }

    enable_if_t<!is_trivial<value_type>::value>
    mv_elements(pointer dst, pointer first, pointer last) {
        for (; first < last; ++first, ++dst)
            _alloc_traits::construct(_alloc, dst++, ala::move(*first));
    }

    void resize_alloc(size_type n) {
        pointer new_data = _alloc.allocate(n);
        this->move_to(_data, _data + _size, new_data);
        _capacity = n;
        _data = new_data;
    }

    template<class Dummy = value_type>
    enable_if_t<is_nothrow_move_constructible<Dummy>::value ||
                !is_copy_constructible<Dummy>::value>
    mv_or_cp(pointer dst, pointer first, pointer last) {
        mv_elements(dst, first, last);
    }

    template<class Dummy = value_type>
    enable_if_t<!is_nothrow_move_constructible<Dummy>::value &&
                is_copy_constructible<Dummy>::value>
    mv_or_cp(pointer dst, pointer first, pointer last) {
        cp_elements(dst, first, last);
    }

    pointer expand() {
        _capacity = _capacity > 0 ? _capacity << 1 : 1;
        return _alloc.allocate(_capacity);
    }

    void clone(const vector &other) {
        this->alloc(other._size);
        cp_elements(_data, other.begin(), other.end());
        _size = other._size;
    }

    void possess(vector &&other) {
        _capacity = other._capacity;
        _data = other._data;
        _size = other._size;
        other._capacity = 0;
        other._data = nullptr;
        other._size = 0;
    }

    void mclear() {
        clear();
        dealloc();
    }

public:
    // construct/copy/destroy:
    vector() noexcept(noexcept(allocator_type())) {}

    explicit vector(const allocator_type &a = allocator_type()) noexcept {}

    explicit vector(size_type n) {
        this->alloc(n);
        this->resize(n);
    }

    vector(size_type n, const value_type &v,
           const allocator_type &a = allocator_type())
        : _alloc(a) {
        this->alloc(n);
        this->resize(n, v);
    }

    template<class InputIter,
             typename = enable_if_t<is_base_of<
                 input_iterator_tag,
                 typename iterator_traits<InputIter>::iterator_category>::value>>
    vector(InputIter first, InputIter last,
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

    vector(initializer_vector<value_type> il,
           const allocator_type &a = allocator_type())
        : vector(il.begin(), il.end(), a) {}

    ~vector() {
        mclear();
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const vector &other) {
        if (_alloc != other._alloc)
            mclear();
        _alloc = other._alloc;
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const vector &other) {
        this->assign(other.begin(), other.end());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(vector &&other) noexcept {
        mclear();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(vector &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            mclear();
            this->possess(ala::move(other));
        } else {
            this->assign(other.begin(), other.end());
        }
    }

public:
    vector &operator=(const vector &other) {
        copy_helper(other);
        return *this;
    }

    vector &operator=(vector &&other) noexcept(
        _alloc_traits::propagate_on_container_move_assignment::value ||
        _alloc_traits::is_always_equal::value) {
        move_helper(ala::move(other));
        return *this;
    }

    vector &operator=(initializer_vector<value_type> il) {
        clear();
        this->insert(end(), il);
        return *this;
    }

    template<class InputIter>
    enable_if_t<is_base_of<input_iterator_tag,
                           typename iterator_traits<InputIter>::iterator_category>::value>
    assign(InputIter first, InputIter last) {
        clear();
        this->insert(end(), first, last);
    }

    void assign(size_type n, const value_type &v) {
        clear();
        this->insert(end(), n, v);
    }

    void assign(initializer_vector<value_type> il) {
        *this = il;
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
        return _data + _size;
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

    void resize(size_type sz) {
        value_type v = value_type();
        this->resize(sz, v);
    }

    void resize(size_type sz, const value_type &v) {
        if (sz < _size) {
            for (size_type i = sz; i < _size; ++i)
                _alloc_traits::destroy(_data + i);
        } else if (_size < sz) {
            if (_capacity < sz) {
                pointer new_data = _alloc.allocate(sz);
                _capacity = sz;
                for (size_type i = _size; i < sz; ++i)
                    _alloc_traits::construct(new_data + i);
                move_to(_data, _data + _size, new_data);
                _data = new_data;
            } else {
                for (size_type i = _size; i < sz; ++i)
                    _alloc_traits::construct(_data + i);
            }
        }
        _size = sz;
    }

    size_type capacity() const noexcept {
        return _capacity;
    }

    ALA_NODISCARD bool empty() const noexcept {
        return _size == 0;
    }

    void reserve(size_type n) {
        if (n > _capacity)
            resize_alloc(n);
    }

    void shrink_to_fit() {
        if (_capacity > _size)
            resize_alloc(_size);
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
        if (_size + 1 > _capacity) {
            pointer new_data = expand();
            _alloc_traits::construct(_alloc, new_data + _size,
                                     ala::forward<Args>(args)...);
            move_to(_data, _data + _size, new_data);
            _data = new_data;
        } else {
            _alloc_traits::construct(_alloc, _data + _size,
                                     ala::forward<Args>(args)...);
        }
        ++_size;
        return *end();
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
    iterator emplace(const_iterator position, Args &&... args) {
        size_type offset = position - cbegin();
        if (_size + 1 > _capacity) {
            pointer new_data = expand();
            _alloc_traits::construct(_alloc, new_data + offset,
                                     ala::forward<Args>(args)...);
            move_to(_data, _data + offset, new_data);
            move_to(_data + offset + 1, _data + _size, new_data + offset + 1);
            _data = new_data;
        } else {
            T tmp(ala::forward<Args>(args)...);
            _alloc_traits::construct(_alloc, _data + _size,
                                     ala::move(*(_data + _size - 1)));
            ala::move_backward(_data + offset + 1, _data + _size,
                               _data + _size + 1);
            _alloc_traits::construct(_alloc, _data + offset, ala::move(tmp));
        }
        ++_size;
        return _data + offset;
    }

    iterator insert(const_iterator position, const value_type &v) {
        return this->emplace(position, v);
    }

    iterator insert(const_iterator position, value_type &&v) {
        return this->emplace(position, ala::move(v));
    }

    iterator insert(const_iterator position, size_type n, const value_type &v) {
        difference_type offset = position - cbegin();
        if (_size + n > _capacity) {
            pointer new_data = _alloc.allocate(_size + n);
            _capacity = _size + n;
            for (size_type i = 0; i < n; ++i)
                _alloc_traits::construct(_alloc, new_data + offset + i, v);
            move_to(_data, _data + offset, new_data);
            move_to(_data + offset + 1, _data + _size, new_data + offset + n);
            _data = new_data;
        } else {
            for (size_type i = 0; i < n; ++i)
                _alloc_traits::construct(_alloc, _data + _size + i,
                                         ala::move(*(_data + _size + i - n)));
            ala::move_backward(_data + offset + 1, _data + _size,
                               _data + _size + n);
            for (size_type i = 0; i < n; ++i)
                _alloc_traits::construct(_alloc, _data + offset + i, v);
        }
        _size += n;
        return _data + offset;
    }

    template<class ForwardIter>
    enable_if_t<
        is_base_of<input_iterator_tag,
                   typename iterator_traits<ForwardIter>::iterator_category>::value &&
            is_base_of<forward_iterator_tag,
                       typename iterator_traits<ForwardIter>::iterator_category>::value,
        iterator>
    insert(const_iterator position, ForwardIter first, ForwardIter last) {
        difference_type offset = position - cbegin();
        typedef typename iterator_traits<ForwardIter>::difference_type diff_t;
        diff_t n = distance(first, last);
        if (_size + n > _capacity) {
            pointer new_data = _alloc.allocate(_size + n);
            _capacity = _size + n;
            for (size_type i = 0; i < n; ++i)
                _alloc_traits::construct(_alloc, new_data + offset + i,
                                         *(first++));
            move_to(_data, _data + offset, new_data);
            move_to(_data + offset + 1, _data + _size, new_data + offset + n);
            _data = new_data;
        } else {
            for (size_type i = 0; i < n; ++i)
                _alloc_traits::construct(_alloc, _data + _size + i,
                                         ala::move(*(_data + _size + i - n)));
            ala::move_backward(_data + offset + 1, _data + _size,
                               _data + _size + n);
            for (size_type i = 0; i < n; ++i)
                _alloc_traits::construct(_alloc, _data + offset + i, *(first++));
        }
        _size += n;
        return _data + offset;
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

    iterator insert(const_iterator position, initializer_vector<value_type> il) {
        return this->insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        ALA_CONST_IF(!is_trivially_destructible<value_type>::value) {
            _alloc_traits::destroy(_alloc, position);
        }
        if (position + 1 < cend())
            ala::move(position + 1, cend(), position);
        --_size;
        return (iterator)position;
    }

    iterator erase(const_iterator first, const_iterator last) {
        ALA_CONST_IF(!is_trivially_destructible<value_type>::value) {
            for (const_iterator i = first; i < last; ++i, --_size)
                _alloc_traits::destroy(_alloc, first);
        }
        if (last < cend())
            ala::move(last, cend(), first);
        return (iterator)first;
    }

    void swap(vector &other) {
        swap(_data, other._data);
        swap(_capacity, other._capacity);
        swap(_size, other._size);
        ALA_CONST_IF(_alloc_traits::propagate_on_container_move_assignment::value) {
            swap(_alloc, other._alloc);
        }
    }

    void clear() noexcept {
        ALA_CONST_IF(!is_trivially_destructible<value_type>::value) {
            size_type sz = size();
            for (size_type i = 0; i < sz; ++i, --_size)
                _alloc_traits::destroy(_data + i);
        }
        _size = 0;
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