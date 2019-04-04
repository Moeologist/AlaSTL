#ifndef _ALA_MULTIMAP_H
#define _ALA_MULTIMAP_H

#include <ala/tuple.h>
#include <ala/detail/allocator.h>
#include <ala/detail/functional_base.h>
#include <ala/detail/pair.h>
#include <ala/detail/rb_tree.h>

namespace ala {

template<class, class, class, class>
class map;

template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
class multimap {
public:
    typedef Key key_type;
    typedef T mapped_type;
    typedef pair<const Key, T> value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Comp key_compare;
    typedef Alloc allocator_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;

    struct value_compare {
        bool operator()(const value_type &x, const value_type &y) const {
            return comp(x.first, y.first);
        }

    private:
        friend class multimap<key_type, mapped_type, key_compare, allocator_type>;
        friend class rb_tree<value_type, value_compare, allocator_type, false>;
        key_compare comp;
        value_compare(key_compare c): comp(c) {}
    };

protected:
    typedef rb_tree<value_type, value_compare, allocator_type, false> tree_type;
    tree_type tree;

    template<class NodePtr,
             class NAlloc = typename multimap::allocator_type::template rebind<
                 typename pointer_traits<NodePtr>::element_type>::other>
    struct _node_adaptor {
        typedef multimap::key_type key_type;
        typedef multimap::mapped_type mapped_type;
        typedef multimap::allocator_type allocator_type;

        constexpr _node_adaptor() noexcept: _ptr(nullptr) {}

        _node_adaptor(_node_adaptor &&nh) {
            if (nh._ptr != nullptr) {
                _ptr = ala::move(nh._ptr);
                _a = ala::move(nh._a);
            }
            nh._ptr = nullptr;
        }

        _node_adaptor &operator=(_node_adaptor &&nh) {
            if (_ptr != nullptr) {
                _a.destroy(ala::addressof(_ptr->_data));
                NAlloc().deallocate(_ptr, 1);
            }
            if (nh._ptr != nullptr) {
                _ptr = ala::move(nh._ptr);
                ALA_CONST_IF(
                    allocator_type::propagate_on_container_move_assignment::value)
                _a = ala::move(nh._ptr);
            }
            nh._ptr = nullptr;
        }

        ~_node_adaptor() {
            if (_ptr != nullptr) {
                _a.destroy(ala::addressof(_ptr->_data));
                NAlloc().deallocate(_ptr, 1);
            }
        }

        explicit operator bool() const noexcept {
            return !empty();
        }

        ALA_NODISCARD bool empty() const noexcept {
            return _ptr == nullptr;
        }

        allocator_type get_allocator() const {
            return _a;
        }

        key_type &key() const {
            return const_cast<key_type &>(_ptr->_data.first);
        }

        mapped_type &mapped() const {
            return _ptr->_data.second;
        }

        void swap(_node_adaptor &nh) noexcept(
            allocator_type::propagate_on_container_swap::value ||
            allocator_type::is_always_equal::value) {
            ala::swap(_ptr, nh._ptr);
            ALA_CONST_IF(allocator_type::propagate_on_container_swap::value)
            ala::swap(_a, nh._a);
        }

    private:
        friend class multimap<key_type, mapped_type, key_compare, allocator_type>;
        typedef NodePtr node_pointer;
        node_pointer _ptr;
        allocator_type _a;
        _node_adaptor(node_pointer p): _ptr(p), _a() {
            if (p->_is_nil)
                p = nullptr;
        }
    };

public:
    typedef rb_iterator<typename tree_type::node_pointer> iterator;
    typedef ala::const_iterator<iterator> const_iterator;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef _node_adaptor<typename tree_type::node_pointer> node_type;

    // construct/copy/destroy:
    multimap() noexcept(is_nothrow_default_constructible<allocator_type>::value &&
                            is_nothrow_default_constructible<key_compare>::value)
        : tree(value_compare(key_compare()), allocator_type()) {}

    explicit multimap(const key_compare &comp,
                      const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {}

    explicit multimap(const allocator_type &a)
        : tree(value_compare(key_compare()), a) {}

    multimap(const multimap &m): tree(m.tree) {}

    multimap(multimap &&m) noexcept(
        is_nothrow_move_constructible<allocator_type>::value
            &&is_nothrow_move_constructible<key_compare>::value)
        : tree(ala::move(m.tree)) {}

    multimap(const multimap &m, const allocator_type &a): tree(m.tree, a) {}

    multimap(multimap &&m, const allocator_type &a)
        : tree(ala::move(m.tree), a) {}

    template<class It>
    multimap(It first, It last, const key_compare &comp = key_compare(),
             const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(first, last);
    }

    template<class It>
    multimap(It first, It last, const allocator_type &a)
        : multimap(first, last, key_compare(), a) {}

    multimap(initializer_list<value_type> il,
             const key_compare &comp = key_compare(),
             const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(il);
    }

    multimap(initializer_list<value_type> il, const allocator_type &a)
        : multimap(il, key_compare(), a) {}

    ~multimap() {}

    // assignment
    multimap &operator=(const multimap &m) {
        tree = m.tree;
        return *this;
    }

    multimap &operator=(multimap &&m) noexcept(
        allocator_type::propagate_on_container_move_assignment::value
            &&is_nothrow_move_assignable<allocator_type>::value
                &&is_nothrow_move_assignable<key_compare>::value) {
        tree = move(m.tree);
        return *this;
    }

    multimap &operator=(initializer_list<value_type> il) {
        tree.clear();
        this->insert(il);
        return *this;
    }

    // iterators:
    iterator begin() noexcept {
        auto x = tree.begin();
        return iterator(x);
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    iterator end() noexcept {
        auto x = tree.end();
        return iterator(x);
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
        return const_iterator(const_cast<multimap *>(this)->begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(const_cast<multimap *>(this)->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_iterator(const_cast<multimap *>(this)->rbegin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_iterator(const_cast<multimap *>(this)->rend());
    }

    // capacity:
    bool empty() const noexcept {
        return size() == 0;
    }

    size_type size() const noexcept {
        return tree.size();
    }

    size_type max_size() const noexcept {
        return numeric_limits<difference_type>::max();
    }

    // modifiers:
    template<class... Args>
    iterator emplace(Args &&... args) {
        return tree.emplace(nullptr, ala::forward<Args>(args)...).first;
    }

    template<class... Args>
    iterator emplace_hint(const_iterator position, Args &&... args) {
        return tree.emplace(position._ptr, ala::forward<Args>(args)...).first;
    }

    iterator insert(const value_type &v) {
        return tree.emplace(nullptr, v).first;
    }

    iterator insert(value_type &&v) {
        return tree.emplace(nullptr, ala::move(v)).first;
    }

    template<class P, typename = enable_if_t<is_constructible<value_type, P &&>::value>>
    iterator insert(P &&p) {
        return tree.emplace(nullptr, ala::forward<P>(p)).first;
    }

    iterator insert(const_iterator position, const value_type &v) {
        return tree.emplace(position._ptr, v).first;
    }

    iterator insert(const_iterator position, value_type &&v) {
        return tree.emplace(position._ptr, ala::move(v)).first;
    }

    template<class P, typename = enable_if_t<is_constructible<value_type, P &&>::value>>
    iterator insert(const_iterator position, P &&p) {
        return tree.emplace(position._ptr, ala::forward<P>(p)).first;
    }

    template<class It>
    void insert(It first, It last) {
        for (It i = first; i != last; ++i)
            this->insert(*i);
    }

    void insert(initializer_list<value_type> il) {
        for (auto i = il.begin(); i != il.end(); ++i)
            this->insert(*i);
    }

    iterator insert(node_type &&nh) {
        auto pr(tree.insert(nullptr, nh._ptr));
        if (!pr.second)
            return {iterator(pr.first), pr.second, ala::move(nh)};
        nh._ptr = nullptr;
        return {iterator(pr.first), pr.second, node_type()};
    }

    iterator insert(const_iterator hint, node_type &&nh) {
        auto pr(tree.insert(hint._ptr, nh._ptr));
        if (pr.second)
            return nh._ptr = nullptr;
        return pr.first;
    }

    node_type extract(const_iterator position) {
        tree.extract(position._ptr);
        return node_type(position._ptr);
    }

    node_type extract(const key_type &k) {
        return extract(this->find(k));
    }

    template<class Comp1>
    void merge(multimap<key_type, mapped_type, Comp1, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(multimap<key_type, mapped_type, Comp1, allocator_type> &&source) {
        this->merge(source);
    }

    template<class Comp1>
    void merge(map<key_type, mapped_type, Comp1, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(map<key_type, mapped_type, Comp1, allocator_type> &&source) {
        this->merge(source);
    }

    iterator erase(iterator position) {
        iterator tmp = position++;
        tree.remove(tmp._ptr);
        return position;
    }

    size_type erase(const key_type &k) {
        auto pr = this->equal_range(k);
        const_iterator i = pr.first;
        size_type r = 0;
        for (; i != pr.second;) {
            i = this->erase(i);
            ++r;
        }
        return r;
    }

    iterator erase(const_iterator first, const_iterator last) {
        const_iterator i = first for (; i != last;) i = this->erase(i);
        return i;
    }

    void clear() noexcept {
        tree.clear();
    }

    void swap(multimap &m) noexcept(
        allocator_traits<allocator_type>::is_always_equal::value
            &&is_nothrow_swappable<key_compare>::value) {
        tree.swap(m.tree);
    }

    // observers:
    allocator_type get_allocator() const noexcept {
        return tree.get_allocator();
    }

    key_compare key_comp() const {
        return this->value_comp().comp;
    }

    value_compare value_comp() const {
        return tree.value_comp();
    }

    //  multimap operations:
    iterator find(const key_type &k) {
        return tree.template find<key_type>(k);
    }

    const_iterator find(const key_type &k) const {
        return const_cast<multimap *>(this)->find(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator find(const K &k) {
        return tree.find(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator find(const K &k) const {
        return tree.find(k);
    }

    size_type count(const key_type &k) const {
        return tree.template count<key_type>(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    size_type count(const K &k) const {
        return tree.count(k);
    }

    bool contains(const key_type &k) const {
        return tree.contains(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    bool contains(const K &k) const {
        return tree.contains(k);
    }

    iterator lower_bound(const key_type &k) {
        key_compare comp = key_comp();
        iterator i = begin();
        for (; i != end() && comp(i->first, k); ++i)
            ;
        return i;
    }

    const_iterator lower_bound(const key_type &k) const {
        return const_cast<multimap *>(this)->lower_bound(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator lower_bound(const K &k) {
        key_compare comp = key_comp();
        iterator i = begin();
        for (; i != end() && comp(i->first, k); ++i)
            ;
        return i;
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator lower_bound(const K &k) const {
        return const_cast<multimap *>(this)->lower_bound(k);
    }

    iterator upper_bound(const key_type &k) {
        key_compare comp = key_comp();
        iterator i = begin();
        for (; i != end() && !comp(k, i->first); ++i)
            ;
        return i;
    }

    const_iterator upper_bound(const key_type &k) const {
        return const_cast<multimap *>(this)->upper_bound(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator upper_bound(const K &k) {
        key_compare comp = key_comp();
        iterator i = begin();
        for (; i != end() && !comp(k, i->first); ++i)
            ;
        return i;
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator upper_bound(const K &k) const {
        return const_cast<multimap *>(this)->upper_bound(k);
    }

    pair<iterator, iterator> equal_range(const key_type &k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type &k) const {
        return const_cast<multimap *>(this)->equal_range(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<iterator, iterator> equal_range(const K &k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<const_iterator, const_iterator> equal_range(const K &k) const {
        return const_cast<multimap *>(this)->equal_range(k);
    }
};

template<class Key, class T, class Compare, class Alloc>
bool operator==(const multimap<Key, T, Compare, Alloc> &lhs,
                const multimap<Key, T, Compare, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class Key, class T, class Compare, class Alloc>
bool operator!=(const multimap<Key, T, Compare, Alloc> &lhs,
                const multimap<Key, T, Compare, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class Key, class T, class Compare, class Alloc>
bool operator<(const multimap<Key, T, Compare, Alloc> &lhs,
               const multimap<Key, T, Compare, Alloc> &rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                   rhs.end());
}

template<class Key, class T, class Compare, class Alloc>
bool operator<=(const multimap<Key, T, Compare, Alloc> &lhs,
                const multimap<Key, T, Compare, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class Key, class T, class Compare, class Alloc>
bool operator>(const multimap<Key, T, Compare, Alloc> &lhs,
               const multimap<Key, T, Compare, Alloc> &rhs) {
    return rhs < lhs;
}

template<class Key, class T, class Compare, class Alloc>
bool operator>=(const multimap<Key, T, Compare, Alloc> &lhs,
                const multimap<Key, T, Compare, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class Key, class T, class Compare, class Alloc>
void swap(multimap<Key, T, Compare, Alloc> &lhs,
          multimap<Key, T, Compare, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#if _ALA_ENABLE_DEDUCTION_GUIDES
template<class It,
         class Comp = less<typename iterator_traits<It>::value_type::first_type>,
         class Alloc = allocator<
             pair<add_const_t<typename iterator_traits<It>::value_type::first_type>,
                  typename iterator_traits<It>::value_type::second_type>>>
multimap(It, It, Comp = Comp(), Alloc = Alloc())
    ->multimap<typename iterator_traits<It>::value_type::first_type,
          typename iterator_traits<It>::value_type::second_type, Comp, Alloc>;

template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
multimap(initializer_list<pair<const Key, T>>, Comp = Comp(), Alloc = Alloc())
    ->multimap<Key, T, Comp, Alloc>;

template<class It, class Alloc>
multimap(It, It, Alloc)
    ->multimap<typename iterator_traits<It>::value_type::first_type,
          typename iterator_traits<It>::value_type::second_type,
          less<typename iterator_traits<It>::value_type::first_type>, Alloc>;

template<class Key, class T, class Allocator>
multimap(initializer_list<pair<const Key, T>>, Allocator)
    ->multimap<Key, T, less<Key>, Allocator>;

#endif

} // namespace ala

#endif // HEAD