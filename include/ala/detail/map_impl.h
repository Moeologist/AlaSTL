#ifndef _ALA_DETAIL_MAP_H
#define _ALA_DETAIL_MAP_H


template<class K, class T, class C, class A>
struct map<K, T, C, A>::value_compare {
    friend class map<key_type, mapped_type, key_compare, allocator_type>;
    friend class rb_tree<value_type, value_compare, allocator_type>;
    bool operator()(const value_type &x, const value_type &y) const {
        return comp(x.first, y.first);
    }

protected:
    key_compare comp;
    value_compare(key_compare c): comp(c) {}
    typedef typename map::key_type key_type;
};

template<class K, class T, class C, class A>
map<K, T, C, A>::map() noexcept(
    is_nothrow_default_constructible<allocator_type>::value
        &&is_nothrow_default_constructible<key_compare>::value)
    : tree(value_compare(key_compare()), allocator_type()) {}

template<class K, class T, class C, class A>
explicit map<K, T, C, A>::map(const key_compare &comp,
                              const allocator_type &a = allocator_type())
    : tree(value_compare(comp), a) {}

template<class K, class T, class C, class A>
explicit map<K, T, C, A>::map(const allocator_type &a)
    : tree(value_compare(key_compare()), a) {}

template<class K, class T, class C, class A>
map<K, T, C, A>::map(const map &m): tree(m.tree) {}

template<class K, class T, class C, class A>
map<K, T, C, A>::map(map &&m) noexcept(
    is_nothrow_move_constructible<allocator_type>::value
        &&is_nothrow_move_constructible<key_compare>::value)
    : tree(ala::move(tree)) {}

template<class K, class T, class C, class A>
map<K, T, C, A>::map(const map &m, const allocator_type &a): tree(m.tree, a) {}

template<class K, class T, class C, class A>
map<K, T, C, A>::map(map &&m, const allocator_type &a)
    : tree(ala::move(m.tree), a) {}

template<class K, class T, class C, class A>
template<class It>
map<K, T, C, A>::map(It first, It last, const key_compare &comp = key_compare(),
                     const allocator_type &a = allocator_type())
    : tree(value_compare(comp), a) {
    this->insert(first, last);
}

template<class K, class T, class C, class A>
template<class It>
map<K, T, C, A>::map(It first, It last, const allocator_type &a)
    : map(first, last, key_compare(), a) {}

template<class K, class T, class C, class A>
map<K, T, C, A>::map(initializer_list<value_type> il, const key_compare &comp,
                     const allocator_type &a)
    : tree(value_compare(comp), a) {
    this->insert(il);
}

template<class K, class T, class C, class A>
map<K, T, C, A>::map(initializer_list<value_type> il, const allocator_type &a)
    : map(il, key_compare(), a) {}

template<class K, class T, class C, class A>
map<K, T, C, A>::~map() {}

template<class K, class T, class C, class A>
map<K, T, C, A>::iterator map<K, T, C, A>::begin() noexcept {
    return tree.begin();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_iterator map<K, T, C, A>::begin() const noexcept {
    return tree.cbegin();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::iterator map<K, T, C, A>::end() noexcept {
    return tree.end();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_iterator map<K, T, C, A>::end() const noexcept {
    return tree.cend();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::reverse_iterator map<K, T, C, A>::rbegin() noexcept {
    return tree.rbegin();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_reverse_iterator map<K, T, C, A>::rbegin() const noexcept {
    return tree.crbegin();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::reverse_iterator map<K, T, C, A>::rend() noexcept {
    return tree.rend();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_reverse_iterator map<K, T, C, A>::rend() const noexcept {
    return tree.crend();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_iterator map<K, T, C, A>::cbegin() const noexcept {
    return tree.cbegin();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_iterator map<K, T, C, A>::cend() const noexcept {
    return tree.cend();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_reverse_iterator map<K, T, C, A>::crbegin() const
    noexcept {
    return tree.crbegin();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::const_reverse_iterator map<K, T, C, A>::crend() const noexcept {
    return tree.crend();
}

template<class K, class T, class C, class A>
bool map<K, T, C, A>::empty() const noexcept {
    return size() == 0;
}

template<class K, class T, class C, class A>
map<K, T, C, A>::size_type map<K, T, C, A>::size() const noexcept {
    return tree._size;
}

template<class K, class T, class C, class A>
map<K, T, C, A>::size_type map<K, T, C, A>::max_size() const noexcept {
    return numeric_limits<difference_type>::max();
}

template<class K, class T, class C, class A>
map<K, T, C, A>::mapped_type &map<K, T, C, A>::operator[](const key_type &k) {
    auto pr = make_pair(k, mapped_type{});
    if (tree.search(pr)) {
        auto ptr = tree.insert(ala::move(pr));
        if (ptr)
            return ptr->_data;
    }
}

template<class K, class T, class C, class A>
map<K, T, C, A>::mapped_type &map<K, T, C, A>::operator[](key_type &&k);

template<class K, class T, class C, class A>
map<K, T, C, A>::mapped_type &map<K, T, C, A>::at(const key_type &k) {
    if (auto p = tree.search_key(k))
        return p->_data;
    else
        throw out_of_range{};
}

template<class K, class T, class C, class A>
const map<K, T, C, A>::mapped_type &map<K, T, C, A>::at(const key_type &k) const {
    if (auto p = tree.search_key(k))
        return const_cast<const mapped_type &>(p->_data);
    else
        throw out_of_range{};
}

template<class K, class T, class C, class A>
void map<K, T, C, A>::clear() noexcept {
    tree.clear();
}