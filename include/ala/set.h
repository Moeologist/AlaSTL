#ifndef _ALA_SET_H
#define _ALA_SET_H

#include <ala/detail/allocator.h>
#include <ala/detail/functional_base.h>
#include <ala/detail/rb_tree.h>
#include <ala/numeric_limits.h>

namespace ala {

template<class Key, class Comp = less<Key>, class Alloc = allocator<const Key>>
class set {
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Comp key_compare;
    typedef key_compare value_compare;
    typedef Alloc allocator_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef typename allocator_type::pointer point;
    typedef typename allocator_type::const_pointer const_point;

protected:
    typedef rb_tree<value_type, value_compare, allocator_type> tree_type;
    tree_type tree;

    template<class NodePtr,
             class NAlloc = typename set::allocator_type::template rebind<
                 typename pointer_traits<NodePtr>::element_type>::other>
    struct _node_adaptor {
        typedef set::value_type setped_type;
        typedef set::allocator_type allocator_type;

        constexpr _node_adaptor() noexcept: _ptr(nullptr) {}

        _node_adaptor(_node_adaptor &&nh) noexcept {
            if (nh._ptr != nullptr) {
                _ptr = ala::move(nh._ptr);
                _a = ala::move(nh._a);
            }
            nh._ptr = nullptr;
        }

        _node_adaptor &operator=(_node_adaptor &&nh) {
            if (_ptr != nullptr) {
                if (_ptr->_is_construct)
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
                if (_ptr->_is_construct)
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

        value_type &value() const {
            return _ptr->_data;
        }

        void swap(_node_adaptor &nh) noexcept(
            allocator_type::propagate_on_container_swap::value ||
            allocator_type::is_always_equal::value) {
            ala::swap(_ptr, nh._ptr);
            ALA_CONST_IF(allocator_type::propagate_on_container_swap::value)
            ala::swap(_a, nh._a);
        }

    private:
        friend class set<key_type, key_compare, allocator_type>;
        typedef NodePtr node_pointer;
        node_pointer _ptr;
        allocator_type _a;
        _node_adaptor(node_pointer p) noexcept: _ptr(p), _a() {
            if (p->_is_nil)
                p = nullptr;
        }
    };
    template<class It, class NodeType>
    struct _insert_return_adaptor {
        It position;
        bool inserted;
        NodeType node;
    };

public:
    typedef rb_iterator<typename tree_type::node_pointer> iterator;
    typedef ala::const_iterator<iterator> const_iterator;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef _node_adaptor<typename tree_type::node_pointer> node_type;
    typedef _insert_return_adaptor<iterator, node_type> insert_return_type;

    // construct/copy/destroy:
    set() noexcept(is_nothrow_default_constructible<allocator_type>::value
                       &&is_nothrow_default_constructible<key_compare>::value)
        : tree(value_compare(key_compare()), allocator_type()) {}

    explicit set(const key_compare &comp,
                 const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {}

    explicit set(const allocator_type &a)
        : tree(value_compare(key_compare()), a) {}

    set(const set &m): tree(m.tree) {}

    set(set &&m) noexcept(is_nothrow_move_constructible<allocator_type>::value &&
                              is_nothrow_move_constructible<key_compare>::value)
        : tree(ala::move(m.tree)) {}

    set(const set &m, const allocator_type &a): tree(m.tree, a) {}

    set(set &&m, const allocator_type &a): tree(ala::move(m.tree), a) {}

    template<class It>
    set(It first, It last, const key_compare &comp = key_compare(),
        const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(first, last);
    }

    template<class It>
    set(It first, It last, const allocator_type &a)
        : set(first, last, key_compare(), a) {}

    set(initializer_list<value_type> il, const key_compare &comp = key_compare(),
        const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(il);
    }

    set(initializer_list<value_type> il, const allocator_type &a)
        : set(il, key_compare(), a) {}

    ~set() {}

    // assignment
    set &operator=(const set &m) {
        tree = m.tree;
        return *this;
    }

    set &operator=(set &&m) noexcept(
        allocator_type::propagate_on_container_move_assignment::value
            &&is_nothrow_move_assignable<allocator_type>::value
                &&is_nothrow_move_assignable<key_compare>::value) {
        tree = move(m.tree);
        return *this;
    }

    set &operator=(initializer_list<value_type> il) {
        tree.clear();
        this->insert(il);
        return *this;
    }

    // iterators:
    iterator begin() noexcept {
        return iterator(tree.begin());
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    iterator end() noexcept {
        return iterator(tree.end());
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
        return const_iterator(const_cast<set *>(this)->begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(const_cast<set *>(this)->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_iterator(const_cast<set *>(this)->rbegin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_iterator(const_cast<set *>(this)->rend());
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
    pair<iterator, bool> emplace(Args &&... args) {
        return tree.emplace(nullptr, ala::forward<Args>(args)...);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator position, Args &&... args) {
        return tree.emplace(position._ptr, ala::forward<Args>(args)...);
    }

    pair<iterator, bool> insert(const value_type &v) {
        return tree.emplace(nullptr, v);
    }

    pair<iterator, bool> insert(value_type &&v) {
        return tree.emplace(nullptr, ala::move(v));
    }

    iterator insert(const_iterator position, const value_type &v) {
        return tree.emplace(position._ptr, v).first;
    }

    iterator insert(const_iterator position, value_type &&v) {
        return tree.emplace(position._ptr, ala::move(v));
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

    insert_return_type insert(node_type &&nh) {
        auto pr = tree.insert(nullptr, nh._ptr);
        if (!pr.second)
            return {iterator(pr.first), pr.second, ala::move(nh)};
        nh._empty = true;
        return {iterator(pr.first), pr.second, node_type()};
    }

    iterator insert(const_iterator hint, node_type &&nh) {
        auto pr = tree.insert(hint._ptr, nh._ptr);
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
    void merge(set<key_type, key_compare, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(set<key_type, key_compare, allocator_type> &&source) {
        this->merge(source);
    }

    iterator erase(iterator position) {
        iterator ret = position;
        ++ret;
        tree.remove(position._ptr);
        return ret;
    }

    size_type erase(const key_type &k) {
        return tree.template erase<key_type>(k);
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator ret;
        for (const_iterator i = first; i != last; ++i)
            ret = tree.erase(i->_ptr);
        return ret;
    }

    void clear() noexcept {
        tree.clear();
    }

    void swap(set &m) noexcept(allocator_traits<allocator_type>::is_always_equal::value
                                   &&is_nothrow_swappable<key_compare>::value) {
        tree.swap(m.tree);
    }

    // observers:
    allocator_type get_allocator() const noexcept {
        return tree.get_allocator();
    }

    key_compare key_comp() const {
        return this->value_comp();
    }

    value_compare value_comp() const {
        return tree.value_comp();
    }

    // set operations:
    iterator find(const key_type &k) {
        return tree.template find<key_type>(k);
    }

    const_iterator find(const key_type &k) const {
        return const_cast<set *>(this)->find(k);
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
        return tree.template contains<key_type>(k);
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
        return const_cast<set *>(this)->lower_bound(k);
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
        return const_cast<set *>(this)->lower_bound(k);
    }

    iterator upper_bound(const key_type &k) {
        key_compare comp = key_comp();
        iterator i = begin();
        for (; i != end() && !comp(k, i->first); ++i)
            ;
        return i;
    }

    const_iterator upper_bound(const key_type &k) const {
        return const_cast<set *>(this)->upper_bound(k);
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
        return const_cast<set *>(this)->upper_bound(k);
    }

    pair<iterator, iterator> equal_range(const key_type &k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type &k) const {
        return const_cast<set *>(this)->equal_range(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<iterator, iterator> equal_range(const K &k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<const_iterator, const_iterator> equal_range(const K &k) const {
        return const_cast<set *>(this)->equal_range(k);
    }
};

} // namespace ala

#endif // HEAD