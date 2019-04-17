#ifndef _ALA_DETAIL_TREE_CONTAINER_INC
#define _ALA_DETAIL_TREE_CONTAINER_INC
#endif

#if !defined(IS_MAP) || !defined(IS_UNIQ)
#error "internal error, nerver use this head"
#endif

#include <ala/detail/allocator.h>
#include <ala/detail/functional_base.h>
#include <ala/detail/algorithm_base.h>
#include <ala/detail/pair.h>
#include <ala/detail/rb_tree.h>

#if IS_MAP

#if IS_UNIQ
#include <ala/tuple.h>
#define CONTAINER map
#define O_CONTAINER multimap
#else
#define CONTAINER multimap
#define O_CONTAINER map
#endif

#else

#if IS_UNIQ
#define CONTAINER set
#define O_CONTAINER multiset
#else
#define CONTAINER multiset
#define O_CONTAINER set
#endif

#endif

namespace ala {

template<class, class, class, class>
class map;

template<class, class, class, class>
class multimap;

template<class, class, class>
class set;

template<class, class, class>
class multiset;

#if IS_MAP
template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
#else
template<class Key, class Comp = less<Key>, class Alloc = allocator<Key>>
#endif
class CONTAINER {
public:
    typedef Key key_type;
#if IS_MAP
    typedef T mapped_type;
    typedef pair<const Key, T> value_type;
#else
    typedef Key value_type;
#endif
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Comp key_compare;
    typedef Alloc allocator_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;

#if IS_MAP
    struct value_compare {
        bool operator()(const value_type &lhs, const value_type &rhs) const {
            return comp(lhs.first, rhs.first);
        }

    private:
        friend class CONTAINER<key_type, mapped_type, key_compare, allocator_type>;
        friend class rb_tree<value_type, value_compare, allocator_type, (bool)IS_UNIQ>;
        key_compare comp;
        value_compare(key_compare c): comp(c) {}
    };
#else
    typedef key_compare value_compare;
#endif
protected:
    typedef rb_tree<value_type, value_compare, allocator_type, IS_UNIQ> tree_type;
    tree_type tree;

#if IS_UNIQ
    template<class Iter, class NodeType>
    struct _insert_return_adaptor {
        Iter position;
        bool inserted;
        NodeType node;
    };
#endif

public:
    typedef rb_iterator<typename tree_type::node_pointer> iterator;
    typedef ala::const_iterator<iterator> const_iterator;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;
#if IS_MAP
    typedef _map_node_adaptor<typename tree_type::node_pointer, allocator_type> node_type;
#else
    typedef _set_node_adaptor<typename tree_type::node_pointer, allocator_type> node_type;
#endif

#if IS_UNIQ
    typedef _insert_return_adaptor<iterator, node_type> insert_return_type;
#endif

    // construct/copy/destroy:
    CONTAINER()
    noexcept(is_nothrow_default_constructible<allocator_type>::value
                 &&is_nothrow_default_constructible<key_compare>::value)
        : tree(value_compare(key_compare()), allocator_type()) {}

    explicit CONTAINER(const key_compare &comp,
                       const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {}

    explicit CONTAINER(const allocator_type &a)
        : tree(value_compare(key_compare()), a) {}

    CONTAINER(const CONTAINER &m): tree(m.tree) {}

    CONTAINER(CONTAINER &&m)
    noexcept(is_nothrow_move_constructible<allocator_type>::value
                 &&is_nothrow_move_constructible<key_compare>::value)
        : tree(ala::move(m.tree)) {}

    CONTAINER(const CONTAINER &m, const allocator_type &a): tree(m.tree, a) {}

    CONTAINER(CONTAINER &&m, const allocator_type &a)
        : tree(ala::move(m.tree), a) {}

    template<class InputIter>
    CONTAINER(InputIter first, InputIter last,
              const key_compare &comp = key_compare(),
              const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(first, last);
    }

    template<class InputIter>
    CONTAINER(InputIter first, InputIter last, const allocator_type &a)
        : CONTAINER(first, last, key_compare(), a) {}

    CONTAINER(initializer_list<value_type> il,
              const key_compare &comp = key_compare(),
              const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(il);
    }

    CONTAINER(initializer_list<value_type> il, const allocator_type &a)
        : CONTAINER(il, key_compare(), a) {}

    ~CONTAINER() {}

    // assignment
    CONTAINER &operator=(const CONTAINER &m) {
        tree = m.tree;
        return *this;
    }

    CONTAINER &operator=(CONTAINER &&m) noexcept(
        allocator_type::propagate_on_container_move_assignment::value
            &&is_nothrow_move_assignable<allocator_type>::value
                &&is_nothrow_move_assignable<key_compare>::value) {
        tree = move(m.tree);
        return *this;
    }

    CONTAINER &operator=(initializer_list<value_type> il) {
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
        return const_iterator(const_cast<CONTAINER *>(this)->begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(const_cast<CONTAINER *>(this)->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_iterator(const_cast<CONTAINER *>(this)->rbegin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_iterator(const_cast<CONTAINER *>(this)->rend());
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

    // element access:
#if IS_MAP && IS_UNIQ
    mapped_type &operator[](const key_type &k) {
        return this->try_emplace(k).first->second;
    }

    mapped_type &operator[](key_type &&k) {
        return this->try_emplace(ala::move(k)).first->second;
    }

    mapped_type &at(const key_type &k) {
        iterator it = iterator(tree.find(k));
        if (!it.first)
            throw out_of_range("map element not found");
        return it->second;
    }

    const mapped_type &at(const key_type &k) const {
        const_iterator it = const_iterator(iterator(tree.find(k)));
        if (!it.first)
            throw out_of_range("map element not found");
        return it->second;
    }
#endif

    // modifiers:
    template<class... Args>
#if IS_UNIQ
    pair<iterator, bool> emplace(Args &&... args) {
        return tree.emplace(nullptr, ala::forward<Args>(args)...);
    }
#else
    iterator emplace(Args &&... args) {
        return tree.emplace(nullptr, ala::forward<Args>(args)...).first;
    }
#endif

    template<class... Args>
    iterator emplace_hint(const_iterator position, Args &&... args) {
        return tree.emplace(position._ptr, ala::forward<Args>(args)...).first;
    }

#if IS_UNIQ
    pair<iterator, bool> insert(const value_type &v) {
        return tree.emplace(nullptr, v);
    }

    pair<iterator, bool> insert(value_type &&v) {
        return tree.emplace(nullptr, ala::move(v));
    }

    template<class P, typename = enable_if_t<is_constructible<value_type, P &&>::value>>
    pair<iterator, bool> insert(P &&p) {
        return tree.emplace(nullptr, ala::forward<P>(p));
    }
#else
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
#endif

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

    template<class InputIter>
    void insert(InputIter first, InputIter last) {
        for (InputIter i = first; i != last; ++i)
            this->insert(*i);
    }

    void insert(initializer_list<value_type> il) {
        for (auto i = il.begin(); i != il.end(); ++i)
            this->insert(*i);
    }

#if IS_UNIQ
    insert_return_type insert(node_type &&nh) {
        auto pr = tree.insert(nullptr, nh._ptr);
        if (!pr.second)
            return {iterator(pr.first), pr.second, ala::move(nh)};
        nh._ptr = nullptr;
        return {iterator(pr.first), pr.second, node_type()};
    }

    iterator insert(const_iterator hint, node_type &&nh) {
        auto pr = tree.insert(hint._ptr, nh._ptr);
        if (pr.second)
            nh._ptr = nullptr;
        return pr.first;
    }
#else
    iterator insert(node_type &&nh) {
        auto pr = tree.insert(nullptr, nh._ptr);
        if (pr.second)
            nh._ptr = nullptr;
        return pr.first;
    }

    iterator insert(const_iterator hint, node_type &&nh) {
        auto pr = tree.insert(hint._ptr, nh._ptr);
        if (pr.second)
            nh._ptr = nullptr;
        return pr.first;
    }
#endif

#if IS_MAP && IS_UNIQ
    template<class M>
    pair<iterator, bool> insert_or_assign(const key_type &k, M &&m) {
        return tree.emplace(nullptr, k, ala::forward<M>(m));
    }

    template<class M>
    pair<iterator, bool> insert_or_assign(key_type &&k, M &&m) {
        return tree.emplace(nullptr, ala::move(k), ala::forward<M>(m));
    }

    template<class M>
    iterator insert_or_assign(const_iterator hint, const key_type &k, M &&m) {
        return tree.emplace(nullptr, k, ala::forward<M>(m)).first;
    }

    template<class M>
    iterator insert_or_assign(const_iterator hint, key_type &&k, M &&m) {
        return tree.emplace(nullptr, ala::move(k), ala::forward<M>(m)).first;
    }
#endif

    node_type extract(const_iterator position) {
        tree.extract(position._ptr);
        return node_type(position._ptr);
    }

    node_type extract(const key_type &k) {
        return extract(this->find(k));
    }

#if IS_MAP
    template<class Comp1>
    void merge(CONTAINER<key_type, mapped_type, Comp1, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(CONTAINER<key_type, mapped_type, Comp1, allocator_type> &&source) {
        this->merge(source);
    }

    template<class Comp1>
    void merge(O_CONTAINER<key_type, mapped_type, Comp1, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(O_CONTAINER<key_type, mapped_type, Comp1, allocator_type> &&source) {
        this->merge(source);
    }
#else
    template<class Comp1>
    void merge(CONTAINER<value_type, Comp1, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(CONTAINER<value_type, Comp1, allocator_type> &&source) {
        this->merge(source);
    }

    template<class Comp1>
    void merge(O_CONTAINER<value_type, Comp1, allocator_type> &source) {
        for (auto i = source.begin(); i != source.end();) {
            auto tmp = i++;
            tree.transfer(source.tree, tmp._ptr);
        }
    }

    template<class Comp1>
    void merge(O_CONTAINER<value_type, Comp1, allocator_type> &&source) {
        this->merge(source);
    }
#endif

#if IS_MAP && IS_UNIQ
    template<class... Args>
    pair<iterator, bool> try_emplace(const key_type &k, Args &&... args) {
        return tree.emplace(nullptr, ala::piecewise_construct,
                            ala::forward_as_tuple(k),
                            ala::forward_as_tuple(ala::forward<Args>(args)...));
    }

    template<class... Args>
    pair<iterator, bool> try_emplace(key_type &&k, Args &&... args) {
        return tree.emplace(nullptr, ala::piecewise_construct,
                            ala::forward_as_tuple(ala::move(k)),
                            ala::forward_as_tuple(ala::forward<Args>(args)...));
    }

    template<class... Args>
    iterator try_emplace(const_iterator hint, const key_type &k, Args &&... args) {
        return tree
            .emplace(hint, ala::piecewise_construct, ala::forward_as_tuple(k),
                     ala::forward_as_tuple(ala::forward<Args>(args)...))
            .first;
    }

    template<class... Args>
    iterator try_emplace(const_iterator hint, key_type &&k, Args &&... args) {
        return tree
            .emplace(hint, ala::piecewise_construct,
                     ala::forward_as_tuple(ala::move(k)),
                     ala::forward_as_tuple(ala::forward<Args>(args)...))
            .first;
    }
#endif

    iterator erase(iterator position) {
        iterator tmp = position++;
        tree.remove(tmp._ptr);
        return position;
    }

    iterator erase(const_iterator position) {
        const_iterator tmp = position++;
        tree.remove(tmp._ptr);
        return position;
    }

    size_type erase(const key_type &k) {
        return tree.erase(k);
    }

    iterator erase(const_iterator first, const_iterator last) {
        for (const_iterator i = first; first != last;)
            first = this->erase(i);
        return first;
    }

    void clear() noexcept {
        tree.clear();
    }

    void swap(CONTAINER &m) noexcept(
        allocator_traits<allocator_type>::is_always_equal::value
            &&is_nothrow_swappable<key_compare>::value) {
        tree.swap(m.tree);
    }

    // observers:
    allocator_type get_allocator() const noexcept {
        return tree.get_allocator();
    }

    key_compare key_comp() const {
#if IS_MAP
        return this->value_comp().comp;
#else
        return this->value_comp();
#endif
    }

    value_compare value_comp() const {
        return tree.value_comp();
    }

    // find operations:
    iterator find(const key_type &k) {
        return tree.template find<key_type>(k);
    }

    const_iterator find(const key_type &k) const {
        return const_cast<CONTAINER *>(this)->find(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator find(const K &k) {
        return tree.find(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator find(const K &k) const {
        return const_cast<CONTAINER *>(this)->find(k);
    }

    size_type count(const key_type &k) const {
#if IS_UNIQ
        return static_cast<size_type>(this->contains(k));
#else
        return tree.template count<key_type>(k);
#endif
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    size_type count(const K &k) const {
#if IS_UNIQ
        return static_cast<size_type>(this->contains(k));
#else
        return tree.count(k);
#endif
    }

    bool contains(const key_type &k) const {
        return tree.template contains<key_type>(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    bool contains(const K &k) const {
        return tree.contains(k);
    }

    iterator lower_bound(const key_type &k) {
#if IS_MAP
        constexpr static auto _bin = [](const value_type &v, const key_type &k) {
            return key_compare()(v.first, k);
        };
        return ala::lower_bound(begin(), end(), k, _bin);
#else
        return ala::lower_bound(begin(), end(), k, tree._comp);
#endif
    }

    const_iterator lower_bound(const key_type &k) const {
        return const_cast<CONTAINER *>(this)->lower_bound(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator lower_bound(const K &k) {
#if IS_MAP
        constexpr static auto _bin = [](const value_type &v, const key_type &k) {
            return key_compare()(v.first, k);
        };
        return ala::lower_bound(begin(), end(), k, _bin);
#else
        return ala::lower_bound(begin(), end(), k, tree._comp);
#endif
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator lower_bound(const K &k) const {
        return const_cast<CONTAINER *>(this)->lower_bound(k);
    }

    iterator upper_bound(const key_type &k) {
#if IS_MAP
        constexpr static auto _bin = [](const key_type &k, const value_type &v) {
            return key_compare()(k, v.first);
        };
        return ala::upper_bound(begin(), end(), k, _bin);
#else
        return ala::upper_bound(begin(), end(), k, tree._comp);
#endif
    }

    const_iterator upper_bound(const key_type &k) const {
        return const_cast<CONTAINER *>(this)->upper_bound(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator upper_bound(const K &k) {
#if IS_MAP
        constexpr static auto _bin = [](const key_type &k, const value_type &v) {
            return key_compare()(k, v.first);
        };
        return ala::upper_bound(begin(), end(), k, _bin);
#else
        return ala::upper_bound(begin(), end(), k, tree._comp);
#endif
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator upper_bound(const K &k) const {
        return const_cast<CONTAINER *>(this)->upper_bound(k);
    }

    pair<iterator, iterator> equal_range(const key_type &k) {
        return pair<iterator, iterator>(this->lower_bound(k),
                                        this->upper_bound(k));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type &k) const {
        return const_cast<CONTAINER *>(this)->equal_range(k);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<iterator, iterator> equal_range(const K &k) {
        return pair<iterator, iterator>(this->lower_bound(k),
                                        this->upper_bound(k));
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<const_iterator, const_iterator> equal_range(const K &k) const {
        return const_cast<CONTAINER *>(this)->equal_range(k);
    }
};

#if IS_MAP
template<class Key, class T, class Comp, class Alloc>
bool operator==(const CONTAINER<Key, T, Comp, Alloc> &lhs,
                const CONTAINER<Key, T, Comp, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class Key, class T, class Comp, class Alloc>
bool operator!=(const CONTAINER<Key, T, Comp, Alloc> &lhs,
                const CONTAINER<Key, T, Comp, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class Key, class T, class Comp, class Alloc>
bool operator<(const CONTAINER<Key, T, Comp, Alloc> &lhs,
               const CONTAINER<Key, T, Comp, Alloc> &rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                   rhs.end());
}

template<class Key, class T, class Comp, class Alloc>
bool operator<=(const CONTAINER<Key, T, Comp, Alloc> &lhs,
                const CONTAINER<Key, T, Comp, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class Key, class T, class Comp, class Alloc>
bool operator>(const CONTAINER<Key, T, Comp, Alloc> &lhs,
               const CONTAINER<Key, T, Comp, Alloc> &rhs) {
    return rhs < lhs;
}

template<class Key, class T, class Comp, class Alloc>
bool operator>=(const CONTAINER<Key, T, Comp, Alloc> &lhs,
                const CONTAINER<Key, T, Comp, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class Key, class T, class Comp, class Alloc>
void swap(CONTAINER<Key, T, Comp, Alloc> &lhs,
          CONTAINER<Key, T, Comp, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

#else // IS_MAP

template<class Key, class Comp, class Alloc>
bool operator==(const CONTAINER<Key, Comp, Alloc> &lhs,
                const CONTAINER<Key, Comp, Alloc> &rhs) {
    if (lhs.size() == rhs.size())
        return equal(lhs.begin(), lhs.end(), rhs.begin());
    return false;
}

template<class Key, class Comp, class Alloc>
bool operator!=(const CONTAINER<Key, Comp, Alloc> &lhs,
                const CONTAINER<Key, Comp, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class Key, class Comp, class Alloc>
bool operator<(const CONTAINER<Key, Comp, Alloc> &lhs,
               const CONTAINER<Key, Comp, Alloc> &rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                   rhs.end());
}

template<class Key, class Comp, class Alloc>
bool operator<=(const CONTAINER<Key, Comp, Alloc> &lhs,
                const CONTAINER<Key, Comp, Alloc> &rhs) {
    return !(rhs < lhs);
}

template<class Key, class Comp, class Alloc>
bool operator>(const CONTAINER<Key, Comp, Alloc> &lhs,
               const CONTAINER<Key, Comp, Alloc> &rhs) {
    return rhs < lhs;
}

template<class Key, class Comp, class Alloc>
bool operator>=(const CONTAINER<Key, Comp, Alloc> &lhs,
                const CONTAINER<Key, Comp, Alloc> &rhs) {
    return !(lhs < rhs);
}

template<class Key, class Comp, class Alloc>
void swap(CONTAINER<Key, Comp, Alloc> &lhs,
          CONTAINER<Key, Comp, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
#endif

#if _ALA_ENABLE_DEDUCTION_GUIDES

#if IS_MAP
template<class InputIter,
         class Comp = less<typename iterator_traits<InputIter>::value_type::first_type>,
         class Alloc = allocator<
             pair<add_const_t<typename iterator_traits<InputIter>::value_type::first_type>,
                  typename iterator_traits<InputIter>::value_type::second_type>>>
CONTAINER(InputIter, InputIter, Comp = Comp(), Alloc = Alloc())
    ->CONTAINER<typename iterator_traits<InputIter>::value_type::first_type,
                typename iterator_traits<InputIter>::value_type::second_type,
                Comp, Alloc>;

template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
CONTAINER(initializer_list<pair<const Key, T>>, Comp = Comp(), Alloc = Alloc())
    ->CONTAINER<Key, T, Comp, Alloc>;

template<class InputIter, class Alloc>
CONTAINER(InputIter, InputIter, Alloc)
    ->CONTAINER<typename iterator_traits<InputIter>::value_type::first_type,
                typename iterator_traits<InputIter>::value_type::second_type,
                less<typename iterator_traits<InputIter>::value_type::first_type>,
                Alloc>;

template<class Key, class T, class Allocator>
CONTAINER(initializer_list<pair<const Key, T>>, Allocator)
    ->CONTAINER<Key, T, less<Key>, Allocator>;
#else // IS_MAP
template<class InputIter,
         class Comp = less<typename iterator_traits<InputIter>::value_type>,
         class Alloc = allocator<typename iterator_traits<InputIter>::value_type>>
CONTAINER(InputIter, InputIter, Comp = Comp(), Alloc = Alloc())
    ->CONTAINER<typename iterator_traits<InputIter>::value_type, Comp, Alloc>;

template<class Key, class Comp = less<Key>, class Alloc = allocator<Key>>
CONTAINER(initializer_list<Key>, Comp = Comp(), Alloc = Alloc())
    ->CONTAINER<Key, Comp, Alloc>;

template<class InputIter, class Alloc>
CONTAINER(InputIter, InputIter, Alloc)
    ->CONTAINER<typename iterator_traits<InputIter>::value_type,
                less<typename iterator_traits<InputIter>::value_type>, Alloc>;

template<class Key, class Alloc>
CONTAINER(initializer_list<Key>, Alloc)->CONTAINER<Key, less<Key>, Alloc>;
#endif

#endif // _ALA_ENABLE_DEDUCTION_GUIDES
} // namespace ala

#undef CONTAINER
#undef O_CONTAINER