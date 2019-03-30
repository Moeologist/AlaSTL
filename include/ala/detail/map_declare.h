
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
    typedef Comp                                            value_compare;
    typedef Alloc                                           allocator_type;
    typedef value_type&                                     reference;
    typedef const value_type&                               const_reference;
    typedef typename allocator_traits<Alloc>::pointer       pointer;
    typedef typename allocator_traits<Alloc>::const_pointer const_pointer;
    typedef ? node_type;
    typedef ? insert_return_type;
    struct value_compare;
protected:
    typedef rb_tree<value_type, value_compare, allocator_type>  tree_type;
    tree_type tree;
public:
    typedef rb_iterator<value_type, typename tree_type::node_pointer> iterator;
    typedef ala::const_iterator<iterator> const_iterator;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;

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

    map(initializer_list<value_type> il, const key_compare& comp = key_compare(), const allocator_type& a = allocator_type());
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

    template<class Comp1>
    void merge(map<key_type, mapped_type, key_compare, allocator_type> &);
    template<class Comp1>
    void merge(map<key_type, mapped_type, key_compare, allocator_type> &&);

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
