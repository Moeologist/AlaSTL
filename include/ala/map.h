#ifndef _ALA_MAP_H
#define _ALA_MAP_H

#include <ala/detail/allocator.h>
#include <ala/external/initializer_list.h>
#include <ala/detail/functional_base.h>
#include <ala/detail/pair.h>
#include <ala/detail/rb_tree.h>
#include <ala/numeric_limits.h>

namespace ala {

template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
class map {
    // clang-format off
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

    struct value_compare {
        friend class map<Key, T, Comp, Alloc>;
    protected:
        Comp comp;
        value_compare(Comp c): comp(c) {}
    public:
        bool operator()(const value_type &x, const value_type &y) const {
            return comp(x.first, y.first);
        }
    };
protected:
    typedef rb_tree<value_type, value_compare, allocator_type>  tree_type;
    tree_type tree;
public:
    typedef typename tree_type::iterator               iterator;
    typedef typename tree_type::const_iterator         const_iterator;
    typedef typename tree_type::reverse_iterator       reverse_iterator;
    typedef typename tree_type::const_reverse_iterator const_reverse_iterator;
    typedef          void                              node_type;
    typedef             void                           insert_return_type;

    // construct/copy/destroy:
    map() noexcept(
        is_nothrow_default_constructible<allocator_type>::value &&
        is_nothrow_default_constructible<key_compare>::value &&
        is_nothrow_copy_constructible<key_compare>::value)
        : tree(value_compare(key_compare()), allocator_type()) {}

    map(const key_compare& comp, const allocator_type& a) : tree(value_compare(comp), a) {}
    explicit map(const key_compare& comp) : tree(value_compare(comp), allocator_type()) {}
    explicit map(const allocator_type& a) : tree(value_compare(key_compare()), a) {}

    map(const map& m) : tree(m.tree) {}
    map(map&& m) noexcept(
        is_nothrow_move_constructible<allocator_type>::value &&
        is_nothrow_move_constructible<key_compare>::value)
        : tree(ala::move(tree)) {}
    map(const map& m, const allocator_type& a) : tree(m.tree, a) {}
    map(map&& m, const allocator_type& a) : tree(ala::move(m.tree), a) {}

    template <class It> map(It first, It last, const key_compare& comp, const allocator_type& a)
        : tree(value_compare(comp), a) { for (It i = first; i != last; ++i) tree.insert(*i); }
    template <class It> map(It first, It last, const key_compare& comp = key_compare()) : map(first, last, comp, allocator_type()) {}
    template <class It> map(It first, It last, const allocator_type& a) : map(first, last, key_compare(), a) {}

    map(initializer_list<value_type> il, const key_compare& comp, const allocator_type& a)
        : tree(value_compare(comp), a) { for (auto i = il.begin(); i != il.end(); ++i) tree.insert(*i); }
    map(initializer_list<value_type> il, const key_compare& comp = key_compare()) : map(il, comp, allocator_type()) {}
    map(initializer_list<value_type> il, const allocator_type& a) : map(il, key_compare(), a) {}

    ~map();

    map& operator=(const map& m) { tree(m.tree); return *this; }
    map& operator=(map&& m) noexcept(
        allocator_type::propagate_on_container_move_assignment::value &&
        is_nothrow_move_assignable<allocator_type>::value &&
        is_nothrow_move_assignable<key_compare>::value) { tree(m.tree); return *this; }
    map& operator=(initializer_list<value_type> il) { tree.clear(); insert(il); return *this; }

    // iterators:
    iterator       begin() noexcept       { return tree.begin();  }
    const_iterator begin() const noexcept { return tree.cbegin(); }
    iterator       end()   noexcept       { return tree.end();    }
    const_iterator end()   const noexcept { return tree.cend();   }

    reverse_iterator       rbegin() noexcept       { return tree.rbegin();  }
    const_reverse_iterator rbegin() const noexcept { return tree.crbegin(); }
    reverse_iterator       rend()   noexcept       { return tree.rend();    }
    const_reverse_iterator rend()   const noexcept { return tree.crend();   }

    const_iterator         cbegin()  const noexcept { return tree.cbegin();  }
    const_iterator         cend()    const noexcept { return tree.cend();    }
    const_reverse_iterator crbegin() const noexcept { return tree.crbegin(); }
    const_reverse_iterator crend()   const noexcept { return tree.crend();   }

    // capacity:
    bool      empty()    const noexcept { return size()==0; }
    size_type size()     const noexcept { return tree.size(); }
    size_type max_size() const noexcept { return numeric_limits<difference_type>::max(); }

    // element access:
    mapped_type& operator[](const key_type& k) {
        if (search())
    }
    mapped_type& operator[](key_type&& k);

    mapped_type&       at(const key_type& k);
    const mapped_type& at(const key_type& k) const;

    // modifiers:
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator             emplace_hint(const_iterator position, Args&&... args);
    pair<iterator, bool>                    insert(const value_type& v);
    pair<iterator, bool>                    insert(value_type&& v); // C++17
    template <class P> pair<iterator, bool> insert(P&& p);
    iterator                                insert(const_iterator position, const value_type& v);
    iterator                                insert(const_iterator position,       value_type&& v); // C++17
    template <class P> iterator             insert(const_iterator position, P&& p);
    template <class It> void                insert(It first, It last);
    void                                    insert(initializer_list<value_type> il);

    node_type extract(const_iterator position);                     // C++17
    node_type extract(const key_type& x);                           // C++17
    insert_return_type insert(node_type&& nh);                      // C++17
    iterator           insert(const_iterator hint, node_type&& nh); // C++17

    template <class... Args> pair<iterator, bool> try_emplace(const key_type& k, Args&&... args);                      // C++17
    template <class... Args> pair<iterator, bool> try_emplace(key_type&& k, Args&&... args);                           // C++17
    template <class... Args> iterator             try_emplace(const_iterator hint, const key_type& k, Args&&... args); // C++17
    template <class... Args> iterator             try_emplace(const_iterator hint, key_type&& k, Args&&... args);      // C++17
    template <class M> pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);                              // C++17
    template <class M> pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj);                                   // C++17
    template <class M> iterator             insert_or_assign(const_iterator hint, const key_type& k, M&& obj);         // C++17
    template <class M> iterator             insert_or_assign(const_iterator hint, key_type&& k, M&& obj);              // C++17

    iterator  erase(const_iterator position);
    iterator  erase(iterator position); // C++14
    size_type erase(const key_type& k);
    iterator  erase(const_iterator first, const_iterator last);
    void      clear() noexcept;

    void swap(map& m) noexcept(
        allocator_traits<allocator_type>::is_always_equal::value &&
        is_nothrow_swappable<key_compare>::value); // C++17

    // observers:
    allocator_type get_allocator() const noexcept { return allocator_type(); }
    key_compare    key_comp()      const { return tree._cmp.comp; }
    value_compare  value_comp()    const { return tree._cmp; }

    // map operations:
    iterator                         find(const key_type& k);
    const_iterator                   find(const key_type& k) const;
    template<class K> iterator       find(const K& x);                     // C++14
    template<class K> const_iterator find(const K& x) const;               // C++14
    template<class K> size_type      count(const K& x) const;                   // C++14
    size_type                        count(const key_type& k) const;
    iterator                         lower_bound(const key_type& k);
    const_iterator                   lower_bound(const key_type& k) const;
    template<class K> iterator       lower_bound(const K& x);              // C++14
    template<class K> const_iterator lower_bound(const K& x) const;        // C++14
    iterator                         upper_bound(const key_type& k);
    const_iterator                   upper_bound(const key_type& k) const;
    template<class K> iterator       upper_bound(const K& x);              // C++14
    template<class K> const_iterator upper_bound(const K& x) const;        // C++14
    pair<iterator,iterator>                                  equal_range(const key_type& k);
    pair<const_iterator,const_iterator>                      equal_range(const key_type& k) const;
    template<class K> pair<iterator,iterator>             equal_range(const K& x);        // C++14
    template<class K> pair<const_iterator,const_iterator> equal_range(const K& x) const;  // C++14
    // clang-format on
};

} // namespace ala

#endif // HEAD