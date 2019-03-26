#ifndef _ALA_MAP_H
#define _ALA_MAP_H
/*
template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
class map {
public:
    typedef Key                                             key_type;
    typedef T                                               mapped_type;
    typedef pair<const Key, T>                              value_type;
    typedef size_t                                          size_type;
    typedef ptrdiff_t                                       difference_type;
    typedef Comp                                            key_compare;
    typedef Alloc                                           allocator_type;
    typedef value_type&                                     reference;
    typedef const value_type&                               const_reference;
    typedef typename allocator_traits<Alloc>::pointer       point;
    typedef typename allocator_traits<Alloc>::const_pointer const_point;
    typedef ? node_type;
    typedef ? insert_return_type;
    struct value_compare;
protected:
    typedef rb_tree<value_type, value_compare, allocator_type>  tree_type;
    tree_type tree;
public:
    typedef typename tree_type::iterator               iterator;
    typedef typename tree_type::const_iterator         const_iterator;
    typedef typename tree_type::reverse_iterator       reverse_iterator;
    typedef typename tree_type::const_reverse_iterator const_reverse_iterator;

    // construct/copy/destroy:
    map() noexcept(
        is_nothrow_default_constructible<allocator_type>::value &&
        is_nothrow_default_constructible<key_compare>::value);

    explicit map(const key_compare& comp, const allocator_type& a = allocator_type());
    explicit map(const allocator_type& a);

    map(const map& m);
    map(map&& m) noexcept(
        is_nothrow_move_constructible<allocator_type>::value &&
        is_nothrow_move_constructible<key_compare>::value);
    map(const map& m, const allocator_type& a);
    map(map&& m, const allocator_type& a);

    template <class It>
    map(It first, It last, const key_compare& comp = key_compare(), const allocator_type& a = allocator_type());
    template <class It>
    map(It first, It last, const allocator_type& a);

    map(initializer_list<value_type> il, const key_compare& comp, const allocator_type& a);
    map(initializer_list<value_type> il, const allocator_type& a);

    ~map();

    // assignment
    map& operator=(const map& m);
    map& operator=(map&& m) noexcept(
        allocator_type::propagate_on_container_move_assignment::value &&
        is_nothrow_move_assignable<allocator_type>::value &&
        is_nothrow_move_assignable<key_compare>::value);
    map& operator=(initializer_list<value_type> il);

    // iterators:
    iterator       begin() noexcept;
    const_iterator begin() const noexcept;
    iterator       end()   noexcept;
    const_iterator end()   const noexcept;

    reverse_iterator       rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator       rend()   noexcept;
    const_reverse_iterator rend()   const noexcept;

    const_iterator         cbegin()  const noexcept;
    const_iterator         cend()    const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend()   const noexcept;

    // capacity:
    bool      empty()    const noexcept;
    size_type size()     const noexcept;
    size_type max_size() const noexcept;

    // element access:
    mapped_type& operator[](const key_type& k);
    mapped_type& operator[](key_type&& k);

    mapped_type&       at(const key_type& k);
    const mapped_type& at(const key_type& k) const;

    // modifiers:
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator             emplace_hint(const_iterator position, Args&&... args);
    pair<iterator, bool>                    insert(const value_type& v);
    pair<iterator, bool>                    insert(value_type&& v);
    template <class P> pair<iterator, bool> insert(P&& p);
    iterator                                insert(const_iterator position, const value_type& v);
    iterator                                insert(const_iterator position,       value_type&& v);
    template <class P> iterator             insert(const_iterator position, P&& p);
    template <class It> void                insert(It first, It last);
    void                                    insert(initializer_list<value_type> il);
    insert_return_type                      insert(node_type&& nh);
    iterator                                insert(const_iterator hint, node_type&& nh);

    node_type extract(const_iterator position);
    node_type extract(const key_type& x);

    template <class... Args> pair<iterator, bool> try_emplace(const key_type& k, Args&&... args);
    template <class... Args> pair<iterator, bool> try_emplace(key_type&& k, Args&&... args);
    template <class... Args> iterator             try_emplace(const_iterator hint, const key_type& k, Args&&... args);
    template <class... Args> iterator             try_emplace(const_iterator hint, key_type&& k, Args&&... args);
    template <class M> pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);
    template <class M> pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj);
    template <class M> iterator             insert_or_assign(const_iterator hint, const key_type& k, M&& obj);
    template <class M> iterator             insert_or_assign(const_iterator hint, key_type&& k, M&& obj);

    iterator  erase(const_iterator position);
    iterator  erase(iterator position);
    size_type erase(const key_type& k);
    iterator  erase(const_iterator first, const_iterator last);
    void      clear() noexcept;

    void swap(map& m) noexcept(
        allocator_traits<allocator_type>::is_always_equal::value &&
        is_nothrow_swappable<key_compare>::value);

    // observers:
    allocator_type get_allocator() const noexcept;
    key_compare    key_comp()      const;
    value_compare  value_comp()    const;

    // map operations:
    iterator                         find(const key_type& k);
    const_iterator                   find(const key_type& k) const;
    template<class K> iterator       find(const K& x);
    template<class K> const_iterator find(const K& x) const;
    template<class K> size_type      count(const K& x) const;
    size_type                        count(const key_type& k) const;
    iterator                         lower_bound(const key_type& k);
    const_iterator                   lower_bound(const key_type& k) const;
    template<class K> iterator       lower_bound(const K& x);
    template<class K> const_iterator lower_bound(const K& x) const;
    iterator                         upper_bound(const key_type& k);
    const_iterator                   upper_bound(const key_type& k) const;
    template<class K> iterator       upper_bound(const K& x);
    template<class K> const_iterator upper_bound(const K& x) const;
    pair<iterator,iterator>                               equal_range(const key_type& k);
    pair<const_iterator,const_iterator>                   equal_range(const key_type& k) const;
    template<class K> pair<iterator,iterator>             equal_range(const K& x);
    template<class K> pair<const_iterator,const_iterator> equal_range(const K& x) const;
};
*/

#include <ala/tuple.h>
#include <ala/detail/allocator.h>
#include <ala/detail/functional_base.h>
#include <ala/detail/pair.h>
#include <ala/detail/rb_tree.h>
#include <ala/external/initializer_list.h>
#include <ala/numeric_limits.h>

namespace ala {

template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
class map {
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
    typedef typename allocator_type::pointer point;
    typedef typename allocator_type::const_pointer const_point;

    struct value_compare {
        bool operator()(const value_type &x, const value_type &y) const {
            return comp(x.first, y.first);
        }

    private:
        friend class map<key_type, mapped_type, key_compare, allocator_type>;
        friend class rb_tree<value_type, value_compare, allocator_type>;
        key_compare comp;
        value_compare(key_compare c): comp(c) {}
    };

protected:
    typedef rb_tree<value_type, value_compare, allocator_type> tree_type;
    tree_type tree;

    template<class NodePtr,
             class NAlloc = typename map::allocator_type::template rebind<
                 typename pointer_traits<NodePtr>::element_type>::other>
    struct _node_adaptor {
        typedef map::key_type key_type;
        typedef map::mapped_type mapped_type;
        typedef map::allocator_type allocator_type;

        constexpr _node_adaptor() noexcept: _empty(true) {}

        _node_adaptor(_node_adaptor &&nh) noexcept: _empty(nh._empty) {
            if (!nh._empty) {
                _ptr = ala::move(nh._ptr);
                _a = ala::move(nh._a)
            }
            nh._empty = true;
        }

        _node_adaptor &operator=(_node_adaptor &&nh) {
            if (!_empty) {
                using nalloc_type =
                    typename allocator_type::template rebind<NodePtr>::other;
                _a.destroy(ala::addressof(_ptr->_data));
                nalloc_type().deallocate(_ptr);
            }

            if (!nh._empty) {
                _ptr = ala::move(nh._ptr);
                if (_empty ||
                    allocator_type::propagate_on_container_move_assignment::value ||)
                    _a = ala::move(nh._ptr);
            }
            _empty = nh._empty;
            nh._empty = true;
        }

        ~_node_adaptor() {
            if (!_empty) {
                using nalloc_type =
                    typename allocator_type::template rebind<NodePtr>::other;
                _a.destroy(ala::addressof(_ptr->_data));
                nalloc_type().deallocate(_ptr);
            }
        }

        explicit operator bool() const noexcept { return !_empty; }
        ALA_NODISCARD bool empty() const noexcept { return _empty; }

        allocator_type get_allocator() const { return _a; }
        key_type &key() const { return _ptr->_data.first; }
        mapped_type &mapped() const { return _ptr->_data.second; }

        void swap(_node_adaptor &nh) noexcept(
            allocator_type::propagate_on_container_swap::value ||
            allocator_type::is_always_equal::value) {
            if (!_empty || !nh._empty) {
                ala::swap(_empty, nh._empty);
                ala::swap(_ptr, nh._ptr);
                ALA_CONST_IF(allocator_type::propagate_on_container_swap::value)
                ala::swap(_a, nh._a);
            }
        }

        friend void swap(_node_adaptor &, _node_adaptor &) noexcept(
            allocator_type::propagate_on_container_swap::value ||
            allocator_type::is_always_equal::value);

    private:
        friend class map<key_type, mapped_type, key_compare, allocator_type>;
        typedef NodePtr node_pointer;
        node_pointer _ptr;
        allocator_type _a;
        bool _empty;
        _node_adaptor(node_pointer p) noexcept: _empty(false), _ptr(p), _a() {}
    };

    template<class It, class NodeType>
    struct _insert_return_adaptor {
        It position;
        bool inserted;
        NodeType node;
    };

public:
    typedef typename tree_type::iterator iterator;
    typedef typename tree_type::const_iterator const_iterator;
    typedef typename tree_type::reverse_iterator reverse_iterator;
    typedef typename tree_type::const_reverse_iterator const_reverse_iterator;
    typedef _node_adaptor<typename tree_type::node_pointer> node_type;
    typedef _insert_return_adaptor<iterator, node_type> insert_return_type;

    // construct/copy/destroy:
    map() noexcept(is_nothrow_default_constructible<allocator_type>::value
                       &&is_nothrow_default_constructible<key_compare>::value)
        : tree(value_compare(key_compare()), allocator_type()) {}

    explicit map(const key_compare &comp,
                 const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {}

    explicit map(const allocator_type &a)
        : tree(value_compare(key_compare()), a) {}

    map(const map &m): tree(m.tree) {}
    map(map &&m) noexcept(is_nothrow_move_constructible<allocator_type>::value &&
                              is_nothrow_move_constructible<key_compare>::value)
        : tree(ala::move(m.tree)) {}
    map(const map &m, const allocator_type &a): tree(m.tree, a) {}
    map(map &&m, const allocator_type &a): tree(ala::move(m.tree), a) {}

    template<class It>
    map(It first, It last, const key_compare &comp = key_compare(),
        const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(first, last);
    }

    template<class It>
    map(It first, It last, const allocator_type &a)
        : map(first, last, key_compare(), a) {}

    map(initializer_list<value_type> il, const key_compare &comp = key_compare(),
        const allocator_type &a = allocator_type())
        : tree(value_compare(comp), a) {
        this->insert(il);
    }
    map(initializer_list<value_type> il, const allocator_type &a)
        : map(il, key_compare(), a) {}

    ~map() {}

    // assignment
    map &operator=(const map &m) {
        tree(m.tree);
        return *this;
    }

    map &operator=(map &&m) noexcept(
        allocator_type::propagate_on_container_move_assignment::value
            &&is_nothrow_move_assignable<allocator_type>::value
                &&is_nothrow_move_assignable<key_compare>::value) {
        tree(move(m.tree));
        return *this;
    }

    map &operator=(initializer_list<value_type> il) {
        tree.clear();
        insert(il);
        return *this;
    }

    // iterators:
    iterator begin() noexcept { return tree.begin(); }
    const_iterator begin() const noexcept { return tree.cbegin(); }
    iterator end() noexcept { return tree.end(); }
    const_iterator end() const noexcept { return tree.cend(); }

    reverse_iterator rbegin() noexcept { return tree.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return tree.crbegin(); }
    reverse_iterator rend() noexcept { return tree.rend(); }
    const_reverse_iterator rend() const noexcept { return tree.crend(); }

    const_iterator cbegin() const noexcept { return tree.cbegin(); }
    const_iterator cend() const noexcept { return tree.cend(); }
    const_reverse_iterator crbegin() const noexcept { return tree.crbegin(); }
    const_reverse_iterator crend() const noexcept { return tree.crend(); }

    // capacity:
    bool empty() const noexcept { return size() == 0; }
    size_type size() const noexcept { return tree.size(); }
    size_type max_size() const noexcept {
        return numeric_limits<difference_type>::max();
    }

    // element access:
    mapped_type &operator[](const key_type &k) {
        return tree
            .emplace(ala::piecewise_construct, ala::forward_as_tuple(k),
                     ala::forward_as_tuple())
            .first->second;
    }
    mapped_type &operator[](key_type &&k) {
        return tree
            .emplace(ala::piecewise_construct,
                     ala::forward_as_tuple(ala::move(k)), ala::forward_as_tuple())
            .first->second;
    }

    mapped_type &at(const key_type &k) { return tree.find(k)->second; }
    const mapped_type &at(const key_type &k) const {
        return tree.find(k)->second;
    }

    // modifiers:
    template<class... Args>
    pair<iterator, bool> emplace(Args &&... args) {
        return tree.emplace(ala::forward<Args>(args)...);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator position, Args &&... args) {
        return tree.emplace_hint(position, ala::forward<Args>(args)...);
    }

    pair<iterator, bool> insert(const value_type &v) { return tree.emplace(v); }

    pair<iterator, bool> insert(value_type &&v) {
        return tree.emplace(ala::move(v));
    }

    template<class P, typename = enable_if_t<is_constructible<value_type, P &&>::value>>
    pair<iterator, bool> insert(P &&p) {
        return tree.emplace(ala::forward<P>(p));
    }

    iterator insert(const_iterator position, const value_type &v) {
        return tree.emplace_hint(position, v);
    }

    iterator insert(const_iterator position, value_type &&v) {
        return tree.emplace_hint(position, ala::move(v));
    }

    template<class P>
    iterator insert(const_iterator position, P &&p) {
        return tree.emplace_hint(position, ala::forward<P>(p)...);
    }

    template<class It>
    void insert(It first, It last) {
        for (It i = first; i != last; ++i)
            insert(*i);
    }

    void insert(initializer_list<value_type> il) {
        for (auto i = il.begin(); i != il.end(); ++i)
            insert(*i);
    }

    insert_return_type insert(node_type &&nh) {
        if (nh._empty) {
            return {end(), false, node_type()};
        } else {
            auto pr(search(nh._ptr));
            if (pr.second)
                return {iterator(pr.first), false, ala::move(nh)};
            tree.attach(nh._ptr);
            nh._empty = true;
            return {iterator(nh._ptr), true, node_type()};
        }
    }

    iterator insert(const_iterator hint, node_type &&nh) {
        auto pr(tree.search_hint(hint._ptr, nh._ptr));
        if (pr.second)
            return pr.first;
        tree.attach_to(pr.first, nh.ptr);
        nh._empty = true;
        return nh._ptr;
    }

    template<class M>
    pair<iterator, bool> insert_or_assign(const key_type &k, M &&m) {
        return tree.emplace(k, ala::forward<M>(m));
    }

    template<class M>
    pair<iterator, bool> insert_or_assign(key_type &&k, M &&m) {
        return tree.emplace(ala::move(k), ala::forward<M>(m));
    }

    template<class M>
    iterator insert_or_assign(const_iterator hint, const key_type &k, M &&m) {
        return tree.emplace_hint(k, ala::forward<M>(m));
    }

    template<class M>
    iterator insert_or_assign(const_iterator hint, key_type &&k, M &&m) {
        return tree.emplace_hint(ala::move(k), ala::forward<M>(m));
    }

    node_type extract(const_iterator position) {
        if (position == end())
            return node_type();
        tree.detach(position->_ptr);
        return node_type(position->_ptr);
    }

    node_type extract(const key_type &k) { return extract(this->find(k)); }

    template<class... Args>
    pair<iterator, bool> try_emplace(const key_type &k, Args &&... args) {
        tree.emplace(ala::piecewise_construct, ala::forward_as_tuple(k),
                     ala::forward_as_tuple(ala::forward<Args>(args)...));
    }

    template<class... Args>
    pair<iterator, bool> try_emplace(key_type &&k, Args &&... args) {
        tree.emplace(ala::piecewise_construct,
                     ala::forward_as_tuple(ala::move(k)),
                     ala::forward_as_tuple(ala::forward<Args>(args)...));
    }

    template<class... Args>
    iterator try_emplace(const_iterator hint, const key_type &k, Args &&... args) {
        tree.emplace_hint(ala::piecewise_construct, ala::forward_as_tuple(k),
                          ala::forward_as_tuple(ala::forward<Args>(args)...));
    }

    template<class... Args>
    iterator try_emplace(const_iterator hint, key_type &&k, Args &&... args) {
        tree.emplace_hint(ala::piecewise_construct, ala::forward_as_tuple(k),
                          ala::forward_as_tuple(ala::forward<Args>(args)...));
    }

    iterator erase(iterator position) { return tree.erase(position); }
    size_type erase(const key_type &k) { return tree.erase<key_type>(k); }
    iterator erase(const_iterator first, const_iterator last) {
        iterator ret;
        for (const_iterator i = first; i != last; ++i)
            ret = tree.erase(position);
        return ret;
    }

    void clear() noexcept { tree.clear(); }

    void swap(map &m) noexcept(allocator_traits<allocator_type>::is_always_equal::value
                                   &&is_nothrow_swappable<key_compare>::value) {
        tree.swap(m.tree);
    }

    // observers:
    allocator_type get_allocator() const noexcept {
        return tree.get_allocator();
    }
    key_compare key_comp() const { return value_comp().comp; }
    value_compare value_comp() const { return tree.value_comp(); }

    // map operations:
    iterator find(const key_type &k) { return tree.find(k); }

    const_iterator find(const key_type &k) const { return tree.find(k); }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator find(const K &x) {
        return tree.find(x);
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator find(const K &x) const {
        return tree.find(x);
    }

    size_type count(const key_type &k) const { return tree.count<key_type>(k); }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    size_type count(const K &x) const {
        return tree.count(x);
    }

    iterator lower_bound(const key_type &k) {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (; I != end() && comp(key, (*i).first); ++i)
            ;
        return i;
    }

    const_iterator lower_bound(const key_type &k) const {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (; I != end() && comp(key, (*i).first); ++i)
            ;
        return i;
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator lower_bound(const K &x) {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (; I != end() && comp(key, (*i).first); ++i)
            ;
        return i;
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator lower_bound(const K &x) const {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (iterator i = begin(); I != end() && !comp((*i).first, key); ++i)
            ;
        return i;
    }

    iterator upper_bound(const key_type &k) {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (iterator i = begin(); I != end() && !comp((*i).first, key); ++i)
            ;
        return i;
    }

    const_iterator upper_bound(const key_type &k) const {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (iterator i = begin(); I != end() && !comp((*i).first, key); ++i)
            ;
        return i;
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    iterator upper_bound(const K &x) {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (iterator i = begin(); I != end() && !comp((*i).first, key); ++i)
            ;
        return i;
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    const_iterator upper_bound(const K &x) const {
        const key_compare &comp = key_comp();
        iterator i = begin();
        for (iterator i = begin(); I != end() && !comp((*i).first, key); ++i)
            ;
        return i;
    }

    pair<iterator, iterator> equal_range(const key_type &k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }
    pair<const_iterator, const_iterator> equal_range(const key_type &k) const {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<iterator, iterator> equal_range(const K &x) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    template<class K, typename Dummy = key_compare, typename = typename Dummy::is_transparent>
    pair<const_iterator, const_iterator> equal_range(const K &x) const {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }
};

} // namespace ala

#endif // HEAD