// Introduction to Algorithms
#ifndef _ALA_DETAIL_RB_TREE_H
#define _ALA_DETAIL_RB_TREE_H

#include <ala/type_traits.h>
#include <ala/detail/allocator.h>
#include <ala/detail/pair.h>
#include <ala/detail/macro.h>
#include <ala/iterator.h>

#define ALA_RED true
#define ALA_BLACK false

namespace ala {

template<class Data>
struct rb_node {
    Data _data;
    rb_node *_left, *_right, *_parent;
    bool _color, _is_nil, _nil_type;
};

template<class Data>
constexpr bool is_black(rb_node<Data> *node) {
    return node == nullptr || !node->_color;
}

template<class Data>
constexpr bool is_red(rb_node<Data> *node) {
    return node != nullptr && node->_color;
}

template<class Data>
constexpr bool is_nil(rb_node<Data> *node) {
    return node == nullptr || node->_is_nil;
}

template<class Data>
constexpr rb_node<Data> *left_leaf(rb_node<Data> *node) {
    if (is_nil(node))
        return node;
    while (!is_nil(node->_left))
        node = node->_left;
    return node;
}

template<class Data>
constexpr rb_node<Data> *right_leaf(rb_node<Data> *node) {
    if (is_nil(node))
        return node;
    while (!is_nil(node->_right))
        node = node->_right;
    return node;
}

template<class Data>
constexpr void next_node(rb_node<Data> *&_ptr) {
    if (_ptr == nullptr || (_ptr->_is_nil && _ptr->_nil_type))
        return;
    else if (_ptr->_right != nullptr)
        _ptr = left_leaf(_ptr->_right);
    else
        while (true) {
            if (_ptr->_parent->_left == _ptr) {
                _ptr = _ptr->_parent;
                break;
            }
            _ptr = _ptr->_parent;
        }
}

template<class Data>
constexpr void prev_node(rb_node<Data> *&_ptr) {
    if (_ptr == nullptr || (_ptr->_is_nil && !_ptr->_nil_type))
        return;
    else if (_ptr->_left != nullptr)
        _ptr = right_leaf(_ptr->_left);
    else {
        while (true) {
            if (_ptr->_parent->_right == _ptr) {
                _ptr = _ptr->_parent;
                break;
            }
            _ptr = _ptr->_parent;
        }
    }
}

template<class Ptr>
struct rb_const_iterator;

template<class Ptr>
struct rb_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef decltype(declval<Ptr>()->_data) value_type;
    typedef typename pointer_traits<Ptr>::difference_type difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;

    constexpr rb_iterator() {}
    constexpr rb_iterator(const rb_iterator &other): _ptr(other._ptr) {}
    constexpr rb_iterator(const Ptr &ptr): _ptr(ptr) {}

    constexpr reference operator*() const {
        return _ptr->_data;
    }

    constexpr pointer operator->() const {
        return ala::addressof(_ptr->_data);
    }

    constexpr bool operator==(const rb_iterator &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_iterator &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr bool operator==(const rb_const_iterator<Ptr> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_const_iterator<Ptr> &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr rb_iterator &operator++() {
        next_node(_ptr);
        return *this;
    }

    constexpr rb_iterator operator++(int) {
        rb_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr rb_iterator &operator--() {
        prev_node(_ptr);
        return *this;
    }

    constexpr rb_iterator operator--(int) {
        rb_iterator tmp(*this);
        --*this;
        return tmp;
    }

protected:
    template<class, class, class, class>
    friend class map;
    template<class, class, class, class>
    friend class multimap;
    template<class, class, class>
    friend class set;
    template<class, class, class>
    friend class multiset;
    friend class rb_const_iterator<Ptr>;
    Ptr _ptr = nullptr;
};

template<class Ptr>
struct rb_const_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef decltype(declval<Ptr>()->_data) value_type;
    typedef typename pointer_traits<Ptr>::difference_type difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;

    constexpr rb_const_iterator() {}
    constexpr rb_const_iterator(const rb_const_iterator &other)
        : _ptr(other._ptr) {}
    constexpr rb_const_iterator(const Ptr &ptr): _ptr(ptr) {}
    constexpr rb_const_iterator(const rb_iterator<Ptr> &other)
        : _ptr(other._ptr) {}

    constexpr reference operator*() const {
        return _ptr->_data;
    }

    constexpr pointer operator->() const {
        return ala::addressof(_ptr->_data);
    }

    constexpr bool operator==(const rb_const_iterator &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_const_iterator &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr bool operator==(const rb_iterator<Ptr> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_iterator<Ptr> &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr rb_const_iterator &operator++() {
        next_node(_ptr);
        return *this;
    }

    constexpr rb_const_iterator operator++(int) {
        rb_const_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr rb_const_iterator &operator--() {
        prev_node(_ptr);
        return *this;
    }

    constexpr rb_const_iterator operator--(int) {
        rb_const_iterator tmp(*this);
        --*this;
        return tmp;
    }

protected:
    template<class, class, class, class>
    friend class map;
    template<class, class, class, class>
    friend class multimap;
    template<class, class, class>
    friend class set;
    template<class, class, class>
    friend class multiset;
    Ptr _ptr = nullptr;
    friend class rb_iterator<Ptr>;
};

template<class Data, class Comp, class Alloc, bool Uniq>
class rb_tree {
public:
    typedef Data value_type;
    typedef Comp value_compare;
    typedef Alloc allocator_type;
    typedef rb_node<Data> node_type;
    typedef allocator_traits<allocator_type> _alloc_traits;
    typedef typename _alloc_traits::template rebind_alloc<node_type> _node_alloc;
    typedef typename _alloc_traits::template rebind_traits<node_type> _nalloc_traits;
    typedef typename _nalloc_traits::pointer node_pointer;
    static constexpr bool is_uniq = Uniq;

    static_assert(is_same<node_pointer, node_type *>::value,
                  "ala node-based container use raw pointer");

    rb_tree(const value_compare &cmp, const allocator_type &a) noexcept(
        is_nothrow_default_constructible<allocator_type>::value
            &&is_nothrow_default_constructible<value_compare>::value)
        : _comp(cmp), _alloc(a), _nalloc(a), _size(0), _root(nullptr) {
        initializer_nil();
    }

    rb_tree(const rb_tree &other)
        : _comp(other._comp),
          _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)),
          _nalloc(_nalloc_traits::select_on_container_copy_construction(
              other._alloc)),
          _size(other._size) {
        initializer_nil();
        _root = copy_tree(other._root);
        fix_nil();
    }

    rb_tree(rb_tree &&other)
        : _comp(ala::move(other._comp)), _alloc(ala::move(other._alloc)),
          _nalloc(ala::move(other._nalloc)), _root(other._root),
          _size(other._size) {
        initializer_nil();
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    rb_tree(const rb_tree &other, const allocator_type &a)
        : _comp(other._comp), _alloc(a), _nalloc(a), _size(other._size) {
        initializer_nil();
        _root = copy_tree(other._root);
        fix_nil();
    }

    rb_tree(rb_tree &&other, const allocator_type &a)
        : _comp(other._comp), _alloc(a), _nalloc(a), _root(other._root),
          _size(other._size) {
        initializer_nil();
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    rb_tree &operator=(const rb_tree &other) {
        ALA_CONST_IF(_alloc_traits::propagate_on_container_copy_assignment::value) {
            _alloc = other._alloc;
        }
        ALA_CONST_IF(
            _nalloc_traits::propagate_on_container_copy_assignment::value) {
            _nalloc = other._nalloc;
        }
        destruct_tree(_root);
        _root = copy_tree(other._root);
        fix_nil();
        _size = other._size;
        return *this;
    }

    rb_tree &operator=(rb_tree &&other) {
        ALA_CONST_IF(_alloc_traits::propagate_on_container_move_assignment::value) {
            _alloc = ala::move(other._alloc);
        }
        ALA_CONST_IF(
            _nalloc_traits::propagate_on_container_move_assignment::value) {
            _nalloc = ala::move(other._nalloc);
        }
        destruct_tree(_root);
        _root = other._root;
        fix_nil();
        _size = other._size;
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
        return *this;
    }

    ~rb_tree() {
        destruct_tree(_root);
        // destruct_node(_left_nil);
        // destruct_node(_right_nil);
    }

    node_pointer begin() const noexcept {
        return _left_nil->_parent;
    }

    node_pointer end() const noexcept {
        return _right_nil;
    }

    void clear() {
        destruct_tree(_root);
        _size = 0;
        fix_nil();
    }

    size_t size() const noexcept {
        return _size;
    }

    void remove(node_pointer position) {
        if (!is_nil(position)) {
            detach(position);
            destruct_node(position);
        }
    }

    void extract(node_pointer position) noexcept {
        if (!is_nil(position)) {
            detach(position);
            position->_left = position->_right = position->_parent = nullptr;
        }
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    value_compare value_comp() const noexcept {
        return _comp;
    }

    void
    swap(rb_tree &other) noexcept(_alloc_traits::is_always_equal::value &&
                                      is_nothrow_swappable<value_compare>::value) {
        ala::swap(_comp, other._comp);
        ala::swap(_root, other._root);
        ala::swap(_left_nil, other._left_nil);
        ala::swap(_right_nil, other._right_nil);
        ala::swap(_size, other._size);
        ALA_CONST_IF(_alloc_traits::propagate_on_container_swap::value) {
            ala::swap(_alloc, other._alloc);
        }
        ALA_CONST_IF(_nalloc_traits::propagate_on_container_swap::value) {
            ala::swap(_nalloc, other._nalloc);
        }
    }

    template<class RBTree>
    void transfer(RBTree &other, node_pointer p) {
        pair<node_pointer, bool> pr = search(nullptr, p);
        if (!pr.second) {
            other.detach(p);
            attach_to(pr.first, p);
        }
    }

    template<class K>
    node_pointer find(const K &key) const {
        decltype(auto) comp = _key_comp();
        node_pointer current = _root;
        while (!is_nil(current))
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_right;
            else
                return current;
        return end();
    }

    template<class K>
    size_t count(const K &key, node_pointer current) const {
        decltype(auto) comp = _key_comp();
        size_t eql = 0;
        while (!is_nil(current)) {
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_right;
            else {
                return 1 + count(key, current->_left) +
                       count(key, current->_right);
            }
        }
        return eql;
    }

    template<class K>
    size_t count(const K &key) const {
        return count(key, _root);
    }

    template<class K>
    bool contains(const K &key) const {
        decltype(auto) comp = _key_comp();
        node_pointer current = _root;
        while (!is_nil(current))
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_right;
            else
                return true;
        return false;
    }

    template<typename...>
    struct Fuck;

    template<class K>
    bool erase(const K &key) {
        decltype(auto) comp = _key_comp();
        node_pointer current = _root;
        while (!is_nil(current)) {
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_right;
            else {
                remove(current);
                return true;
            }
        }
        return false;
    }

    pair<node_pointer, bool> insert(node_pointer hint, node_pointer p) {
        if (is_nil(p))
            return pair<node_pointer, bool>(end(), false);
        pair<node_pointer, bool> pr = search(hint, p);
        if (!pr.second)
            attach_to(pr.first, p);
        return pair<node_pointer, bool>(p, !pr.second);
    }

    template<bool Dummy = is_uniq, class... Args>
    enable_if_t<Dummy, pair<node_pointer, bool>> emplace(node_pointer hint,
                                                         Args &&... args) {
        node_pointer new_node = construct_node(ala::forward<Args>(args)...);
        pair<node_pointer, bool> pr = search(hint, new_node);
        if (pr.second) {
            destruct_node(new_node);
            return pair<node_pointer, bool>(pr.first, false);
        } else {
            attach_to(pr.first, new_node);
            return pair<node_pointer, bool>(new_node, true);
        }
    }

    template<bool Dummy = is_uniq, class... Args>
    enable_if_t<!Dummy, pair<node_pointer, bool>> emplace(node_pointer hint,
                                                          Args &&... args) {
        node_pointer new_node = construct_node(ala::forward<Args>(args)...);
        pair<node_pointer, bool> pr = search(hint, new_node);
        attach_to(pr.first, new_node);
        return pair<node_pointer, bool>(new_node, true);
    }

    template<bool Dummy = is_uniq>
    enable_if_t<Dummy, pair<node_pointer, bool>> search(node_pointer hint,
                                                        node_pointer p) {
        node_pointer guard, current = nullptr;
        bool found = false;
        if (!is_nil(hint) && !_comp(hint->_data, p->_data))
            guard = hint;
        else
            guard = _root;
        while (!is_nil(guard)) {
            current = guard;
            if (_comp(p->_data, guard->_data))
                guard = guard->_left;
            else if (_comp(guard->_data, p->_data))
                guard = guard->_right;
            else {
                found = true;
                break;
            }
        }
        return pair<node_pointer, bool>(current, found);
    }

    template<bool Dummy = is_uniq>
    enable_if_t<!Dummy, pair<node_pointer, bool>> search(node_pointer hint,
                                                         node_pointer p) {
        node_pointer guard, current = nullptr;
        if (!is_nil(hint) && !_comp(hint->_data, p->_data))
            guard = hint;
        else
            guard = _root;
        while (!is_nil(guard)) {
            current = guard;
            if (_comp(p->_data, guard->_data))
                guard = guard->_left;
            else if (_comp(guard->_data, p->_data))
                guard = guard->_right;
            else {
                guard = guard->_left;
            }
        }
        return pair<node_pointer, bool>(current, false);
    }

protected:
    node_pointer _root;
    aligned_storage_t<sizeof(node_type), alignof(node_type)> _left_nil_mem;
    aligned_storage_t<sizeof(node_type), alignof(node_type)> _right_nil_mem;
    const node_pointer _left_nil = (node_pointer)&_left_nil_mem;
    const node_pointer _right_nil = (node_pointer)&_right_nil_mem;

    size_t _size;
    allocator_type _alloc;
    _node_alloc _nalloc;
    value_compare _comp;

    ALA_HAS_MEM(first)
    ALA_HAS_MEM(comp)

    static_assert(_has_first<value_type>::value == _has_comp<value_compare>::value,
                  "key compare check failed");

    template<typename Dummy = value_type,
             typename = enable_if_t<_has_first<Dummy>::value>>
    const auto &_key(const value_type &v) const {
        return v.first;
    }

    template<typename Dummy = value_type,
             typename = enable_if_t<!_has_first<Dummy>::value>>
    const value_type &_key(const value_type &v) const {
        return v;
    }

    template<typename Dummy = value_compare,
             typename = enable_if_t<_has_comp<Dummy>::value>>
    const auto &_key_comp() const {
        return _comp.comp;
    }

    template<typename Dummy = value_compare,
             typename = enable_if_t<!_has_comp<Dummy>::value>>
    const value_compare &_key_comp() const {
        return _comp;
    }

    template<class... Args>
    node_pointer construct_node(Args &&... args) {
        node_pointer node = _nalloc.allocate(1);
        _alloc_traits::construct(_alloc, ala::addressof(node->_data),
                                 ala::forward<Args>(args)...);
        node->_is_nil = false;
        return node;
    }

    void destruct_node(node_pointer &node) {
        if (!node->_is_nil)
            _alloc_traits::destroy(_alloc, ala::addressof(node->_data));
        _nalloc.deallocate(node, 1);
        node = nullptr;
    }

    void destruct_tree(node_pointer &node) {
        if (is_nil(node))
            return;
        destruct_tree(node->_left);
        destruct_tree(node->_right);
        destruct_node(node);
    }

    node_pointer copy_tree(node_pointer other, node_pointer parent = nullptr) {
        if (is_nil(other))
            return nullptr;
        node_pointer node = construct_node(other->_data);
        node->_color = other->_color;
        node->_parent = parent;
        node->_left = copy_tree(other->_left, node);
        node->_right = copy_tree(other->_right, node);
        return node;
    }

    // rotate
    /*-------------------------------------
    |      x        left        y         |
    |     / \       ====>      / \        |
    |    a   y                x   c       |
    |       / \     <====    / \          |
    |      b   c    right   a   b         |
    --------------------------------------*/

    node_pointer rotate_left(node_pointer x) noexcept {
        node_pointer y = x->_right;
        if ((x->_right = y->_left) != nullptr)
            // b can't be equal to NIL,so using is_nil is correct,too
            y->_left->_parent = x;
        y->_left = x;
        if ((y->_parent = x->_parent) == nullptr)
            _root = y;
        else if (x == x->_parent->_left)
            x->_parent->_left = y;
        else
            x->_parent->_right = y;
        x->_parent = y;
        return y;
    }

    node_pointer rotate_right(node_pointer y) noexcept {
        node_pointer x = y->_left;
        if ((y->_left = x->_right) != nullptr)
            x->_right->_parent = y;
        x->_right = y;
        if ((x->_parent = y->_parent) == nullptr)
            _root = x;
        else if (y == y->_parent->_left)
            y->_parent->_left = x;
        else
            y->_parent->_right = x;
        y->_parent = x;
        return x;
    }

    /*---------------------------------
    |      up     trans     up        |
    |       \     ====>      \        |
    |        u                v       |
    ---------------------------------*/

    void transplant(node_pointer u, node_pointer v) noexcept {
        node_pointer uparent = u->_parent;
        if (uparent == nullptr)
            _root = v;
        else if (u == uparent->_left)
            uparent->_left = v;
        else
            uparent->_right = v;
        if (v != nullptr)
            v->_parent = uparent;
    }

    void initializer_nil() {
        // _left_nil = allocate_node();
        // _right_nil = allocate_node();
        // _left_nil =  reinterpret_cast<node_pointer>(_nil_mem);
        // _right_nil =  reinterpret_cast<node_pointer>(_nil_mem + sizeof(rb_node<Data>));

        _left_nil->_is_nil = true;
        _left_nil->_parent = _right_nil;
        _left_nil->_color = ALA_BLACK;
        _left_nil->_nil_type = false;
        _left_nil->_left = nullptr;
        _left_nil->_right = nullptr;

        _right_nil->_is_nil = true;
        _right_nil->_parent = _left_nil;
        _right_nil->_color = ALA_BLACK;
        _right_nil->_nil_type = true;
        _right_nil->_left = nullptr;
        _right_nil->_right = nullptr;
    }

    void fix_nil() noexcept {
        if (_root == nullptr) {
            _left_nil->_parent = _right_nil;
            _right_nil->_parent = _left_nil;
        } else {
            node_pointer lleaf = left_leaf(_root);
            node_pointer rleaf = right_leaf(_root);
            lleaf->_left = _left_nil;
            rleaf->_right = _right_nil;
            _left_nil->_parent = lleaf;
            _right_nil->_parent = rleaf;
        }
    }

    void attach_to(node_pointer pos, node_pointer p) noexcept {
        p->_color = ALA_RED;
        p->_left = p->_right = nullptr;
        p->_parent = pos;
        if (pos == nullptr) { // empty tree
            _root = p;
            _left_nil->_parent = _right_nil->_parent = p;
            p->_left = _left_nil;
            p->_right = _right_nil;
        } else if (!_comp(pos->_data, p->_data)) {
            if (pos->_left == _left_nil) { // fix nil
                p->_left = _left_nil;
                _left_nil->_parent = p;
            }
            pos->_left = p;
        } else {
            if (pos->_right == _right_nil) { // fix nil
                p->_right = _right_nil;
                _right_nil->_parent = p;
            }
            pos->_right = p;
        }
        ++_size;
        rebalance_for_attach(p);
    }

    /*---------------------------------------------------------------------------------
    |     G(B)                  G(B)                  G(B)                  P(B)      |
    |     / \       U.c=B       / \       RotL(P)     / \       RotR(G)     / \       |
    |  P(R) U(R)    ====>?   P(R) U(B)    ====>    P(R) U(B)    ====>    X(R) G(R)    |
    |  / \          loop(X)  / \          P<->X    / \          P.c=B         / \     |
    |    X(R)                  X(R)              X(R)           G.c=R           U(B)  |
    ---------------------------------------------------------------------------------*/
    // remove continuous red

    void rebalance_for_attach(node_pointer current) noexcept {
        node_pointer parent, grandp, uncle;
        while (is_red(parent = current->_parent)) {
            grandp = parent->_parent;
            if (parent == grandp->_left) { // see graph
                uncle = grandp->_right;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_right == current)
                        parent = rotate_left(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_right(grandp);
                }
            } else { // symmetrical
                uncle = grandp->_left;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_left == current)
                        parent = rotate_right(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_left(grandp);
                }
            }
        }
        _root->_color = ALA_BLACK;
    }

    void detach(node_pointer current) noexcept {
        node_pointer child, parent, subs, fix = nullptr;
        bool color;
        if (is_nil(current->_left)) {
            color = current->_color;
            child = current->_right;
            parent = current->_parent;
            transplant(current, child);
            fix = child == nullptr ? parent : child;
        } else if (is_nil(current->_right)) {
            color = current->_color;
            child = current->_left;
            parent = current->_parent;
            transplant(current, child);
            fix = child == nullptr ? parent : child;
        } else {
            subs = left_leaf(current->_right);
            color = subs->_color;
            child = subs->_right;
            parent = subs->_parent;
            if (parent == current) {
                parent = subs;
            } else {
                transplant(subs, child);
                subs->_right = current->_right;
                subs->_right->_parent = subs;
            }
            transplant(current, subs);
            subs->_left = current->_left;
            subs->_left->_parent = subs;
            subs->_color = current->_color;
            fix = subs;
        }
        --_size;
        if (_root != nullptr && color == ALA_BLACK)
            rebalance_for_detach(child, parent);
        if (fix)
            fix_nil_detach(current, fix);
    }

    /*----------------------------------------------------------------------------------------------------
    |     P(B)      P.c<->B.c   P(?)                  P(?)      B.l.c=B     P(?)      B.c=P.c     B(?)   |
    |     / \       RotL(P)     / \       B.c=R       / \       B.c=R       / \       P.c=B       / \    |
    |  X(B) B(R)    ====>^   X(B) B(B)    ====>?   X(B) B(B)    ====>    X(B) B(B)    ====>    P(B) (B)  |
    |       / \     B=P.r         / \     loop(X)       / \     RotR(B)       / \     B.r.c=B  /         |
    |      $B       fork        (B) (B)               (R) (B)   B=P.r          ~B(R)  RotL(B) X          |
    ----------------------------------------------------------------------------------------------------*/
    // make brother black-height -1

    void rebalance_for_detach(node_pointer current, node_pointer parent) noexcept {
        node_pointer brother;
        while (current != _root && is_black(current)) {
            if (parent->_left == current) {
                brother = parent->_right;
                if (brother->_color == ALA_RED) {
                    brother->_color = ALA_BLACK;
                    parent->_color = ALA_RED;
                    rotate_left(parent);
                    brother = parent->_right;
                }
                if (is_black(brother->_left) && is_black(brother->_right)) {
                    brother->_color = ALA_RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_right)) {
                        if (!is_nil(brother->_left))
                            brother->_left->_color = ALA_BLACK;
                        brother->_color = ALA_RED;
                        rotate_right(brother);
                        brother = parent->_right;
                    }
                    brother->_color = parent->_color;
                    parent->_color = ALA_BLACK;
                    if (!is_nil(brother->_right))
                        brother->_right->_color = ALA_BLACK;
                    rotate_left(parent);
                    current = _root; // make root black
                    break;
                }
            } else {
                brother = parent->_left;
                if (brother->_color == ALA_RED) {
                    brother->_color = ALA_BLACK;
                    parent->_color = ALA_RED;
                    rotate_right(parent);
                    brother = parent->_left;
                }
                if (is_black(brother->_left) && is_black(brother->_right)) {
                    brother->_color = ALA_RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_left)) {
                        if (!is_nil(brother->_right))
                            brother->_right->_color = ALA_BLACK;
                        brother->_color = ALA_RED;
                        rotate_left(brother);
                        brother = parent->_left;
                    }
                    brother->_color = parent->_color;
                    parent->_color = ALA_BLACK;
                    if (!is_nil(brother->_left))
                        brother->_left->_color = ALA_BLACK;
                    rotate_right(parent);
                    current = _root;
                    break;
                }
            }
        }
        if (!is_nil(current))
            current->_color = ALA_BLACK;
    }

    void fix_nil_detach(node_pointer current, node_pointer subs) noexcept {
        if (current->_left == _left_nil) {
            if (current->_right == _right_nil) {
                _left_nil->_parent = _right_nil;
                _right_nil->_parent = _left_nil;
                _root = nullptr;
            } else {
                subs->_left = _left_nil;
                _left_nil->_parent = subs;
            }
        } else if (current->_right == _right_nil) {
            subs->_right = _right_nil;
            _right_nil->_parent = subs;
        }
    }
}; // namespace ala

} // namespace ala

#undef ALA_RED
#undef ALA_BLACK

#endif // HEAD