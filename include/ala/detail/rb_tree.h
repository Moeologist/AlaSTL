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

#define ALA_RGHT true
#define ALA_LEFT false

namespace ala {

template<class Data>
struct rb_node {
    Data _data;
    rb_node *_left, *_rght, *_parent;
    bool _color, _is_nil, _nflag;
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
constexpr rb_node<Data> *rght_leaf(rb_node<Data> *node) {
    if (is_nil(node))
        return node;
    while (!is_nil(node->_rght))
        node = node->_rght;
    return node;
}

template<class Data>
constexpr void next_node(rb_node<Data> *&_ptr) {
    if (_ptr == nullptr || (_ptr->_is_nil && _ptr->_nflag))
        return;
    else if (_ptr->_rght != nullptr)
        _ptr = left_leaf(_ptr->_rght);
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
    if (_ptr == nullptr || (_ptr->_is_nil && !_ptr->_nflag))
        return;
    else if (_ptr->_left != nullptr)
        _ptr = rght_leaf(_ptr->_left);
    else {
        while (true) {
            if (_ptr->_parent->_rght == _ptr) {
                _ptr = _ptr->_parent;
                break;
            }
            _ptr = _ptr->_parent;
        }
    }
}

template<class Ptr, bool IsConst = false>
struct rb_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef decltype(declval<Ptr>()->_data) value_type;
    typedef typename pointer_traits<Ptr>::difference_type difference_type;
    typedef conditional_t<IsConst, const value_type, value_type> *pointer;
    typedef conditional_t<IsConst, const value_type, value_type> &reference;

    constexpr rb_iterator() {}
    constexpr rb_iterator(const rb_iterator &other): _ptr(other._ptr) {}
    constexpr rb_iterator(const rb_iterator<Ptr, !IsConst> &other)
        : _ptr(other._ptr) {}
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

    constexpr bool operator==(const rb_iterator<Ptr, !IsConst> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_iterator<Ptr, !IsConst> &rhs) const {
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
    friend class rb_iterator<Ptr, !IsConst>;
    template<class, class, class, class>
    friend class map;
    template<class, class, class, class>
    friend class multimap;
    template<class, class, class>
    friend class set;
    template<class, class, class>
    friend class multiset;
    Ptr _ptr = nullptr;
};

template<class Data, class Comp, class Alloc, bool Uniq>
class rb_tree {
public:
    typedef Data value_type;
    typedef Comp value_compare;
    typedef Alloc allocator_type;
    typedef rb_node<Data> _node_t;
    typedef allocator_traits<allocator_type> _alloc_traits;
    typedef _node_t *_hdle_t;
    static constexpr bool is_uniq = Uniq;

    rb_tree(const value_compare &cmp, const allocator_type &a) noexcept(
        is_nothrow_default_constructible<allocator_type>::value
            &&is_nothrow_default_constructible<value_compare>::value)
        : _comp(cmp), _alloc(a), _size(0), _root(nullptr) {
        initialize();
    }

    rb_tree(const rb_tree &other)
        : _comp(other._comp),
          _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)),
          _size(other._size) {
        initialize();
        _root = copy_tree(other._root);
        fix_nil();
    }

    rb_tree(rb_tree &&other)
        : _comp(ala::move(other._comp)), _alloc(ala::move(other._alloc)),
          _root(other._root), _size(other._size) {
        initialize();
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    rb_tree(const rb_tree &other, const allocator_type &a)
        : _comp(other._comp), _alloc(a), _size(other._size) {
        initialize();
        _root = copy_tree(other._root);
        fix_nil();
    }

    rb_tree(rb_tree &&other, const allocator_type &a)
        : _comp(other._comp), _alloc(a), _root(other._root), _size(other._size) {
        initialize();
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> _copy_helper(const rb_tree &other) {
        if (_alloc == other._alloc) {
            destruct_tree(_root);
            _alloc = other._alloc;
        } else {
            _alloc = other._alloc;
            destruct_tree(_root);
        }
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> _copy_helper(const rb_tree &other) {
        destruct_tree(_root);
    }

    rb_tree &operator=(const rb_tree &other) {
        return _copy_helper(other);
        _root = copy_tree(other._root);
        fix_nil();
        _size = other._size;
        return *this;
    }

    void _transfer_root() {
        destruct_tree(_root);
        _root = other._root;
        _size = other._size;
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy, rb_tree &> _move_helper(rb_tree &&other) {
        alloc = ala::move(other._alloc);
        _transfer_root();
        return *this;
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy, rb_tree &> _move_helper(rb_tree &&other) {
        if (_alloc == other._alloc) {
            _transfer_root();
            return *this;
        } else {
            return _copy_helper<false>(other);
        }
    }

    rb_tree &operator=(rb_tree &&other) {
        return _move_helper(ala::move(other));
    }

    ~rb_tree() {
        destruct_tree(_root);
        _alloc_traits::template deallocate_object<_node_t>(_alloc, _left_nil(),
                                                           2);
    }

    _hdle_t begin() const noexcept {
        return _left_nil()->_parent;
    }

    _hdle_t end() const noexcept {
        return _rght_nil();
    }

    void clear() {
        destruct_tree(_root);
        _size = 0;
        fix_nil();
    }

    size_t size() const noexcept {
        return _size;
    }

    void remove(_hdle_t position) {
        if (!is_nil(position)) {
            detach(position);
            destruct_node(position);
        }
    }

    void extract(_hdle_t position) noexcept {
        if (!is_nil(position)) {
            detach(position);
            position->_left = position->_rght = position->_parent = nullptr;
        }
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    value_compare value_comp() const noexcept {
        return _comp;
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> _swap_helper(rb_tree other) {
        ala::swap(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy> _swap_helper() {
        assert(get_allocator() == other.get_allocator());
    }

    void
    swap(rb_tree &other) noexcept(_alloc_traits::is_always_equal::value &&
                                      is_nothrow_swappable<value_compare>::value) {
        ala::swap(_comp, other._comp);
        ala::swap(_root, other._root);
        ala::swap(_size, other._size);
        ala::swap(_left_nil(), other._left_nil());
        ala::swap(_rght_nil(), other._rght_nil());
        _swap_helper();
    }

    template<class RBTree>
    void transfer(RBTree &other, _hdle_t p) {
        pair<_hdle_t, bool> pr = search(nullptr, p);
        if (!pr.second) {
            other.detach(p);
            attach_to(pr.first, p);
        }
    }

    template<class K>
    _hdle_t find(const K &key) const {
        const auto &comp = _key_comp();
        _hdle_t current = _root;
        while (!is_nil(current))
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_rght;
            else
                return current;
        return end();
    }

    template<class K>
    size_t count(const K &key, _hdle_t current) const {
        const auto &comp = _key_comp();
        size_t eql = 0;
        while (!is_nil(current)) {
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_rght;
            else {
                return 1 + count(key, current->_left) +
                       count(key, current->_rght);
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
        const auto &comp = _key_comp();
        _hdle_t current = _root;
        while (!is_nil(current))
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_rght;
            else
                return true;
        return false;
    }

    template<class K>
    bool erase(const K &key) {
        const auto &comp = _key_comp();
        _hdle_t current = _root;
        while (!is_nil(current)) {
            if (comp(key, _key(current->_data)))
                current = current->_left;
            else if (comp(_key(current->_data), key))
                current = current->_rght;
            else {
                remove(current);
                return true;
            }
        }
        return false;
    }

    pair<_hdle_t, bool> insert(_hdle_t hint, _hdle_t p) {
        if (is_nil(p))
            return pair<_hdle_t, bool>(end(), false);
        pair<_hdle_t, bool> pr = search(hint, p);
        if (!pr.second)
            attach_to(pr.first, p);
        return pair<_hdle_t, bool>(p, !pr.second);
    }
    template<bool Dummy = is_uniq, class... Args>
    enable_if_t<Dummy, pair<_hdle_t, bool>> emplace(_hdle_t hint,
                                                         Args &&... args) {
        _hdle_t new_node = construct_node(ala::forward<Args>(args)...);
        pair<_hdle_t, bool> pr = search(hint, new_node);
        if (pr.second) {
            destruct_node(new_node);
            return pair<_hdle_t, bool>(pr.first, false);
        } else {
            attach_to(pr.first, new_node);
            return pair<_hdle_t, bool>(new_node, true);
        }
    }

    template<bool Dummy = is_uniq, class... Args>
    enable_if_t<!Dummy, pair<_hdle_t, bool>> emplace(_hdle_t hint,
                                                          Args &&... args) {
        _hdle_t new_node = construct_node(ala::forward<Args>(args)...);
        pair<_hdle_t, bool> pr = search(hint, new_node);
        attach_to(pr.first, new_node);
        return pair<_hdle_t, bool>(new_node, true);
    }

    template<bool Dummy = is_uniq>
    enable_if_t<Dummy, pair<_hdle_t, bool>> search(_hdle_t hint,
                                                        _hdle_t p) {
        _hdle_t guard, current = nullptr;
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
                guard = guard->_rght;
            else {
                found = true;
                break;
            }
        }
        return pair<_hdle_t, bool>(current, found);
    }

    template<bool Dummy = is_uniq>
    enable_if_t<!Dummy, pair<_hdle_t, bool>> search(_hdle_t hint,
                                                         _hdle_t p) {
        _hdle_t guard, current = nullptr;
        if (!is_nil(hint) && !_comp(hint->_data, p->_data))
            guard = hint;
        else
            guard = _root;
        while (!is_nil(guard)) {
            current = guard;
            if (_comp(p->_data, guard->_data))
                guard = guard->_left;
            else if (_comp(guard->_data, p->_data))
                guard = guard->_rght;
            else {
                guard = guard->_left;
            }
        }
        return pair<_hdle_t, bool>(current, false);
    }
protected:
    _hdle_t _root;
    _hdle_t _guard;

    size_t _size;
    allocator_type _alloc;
    value_compare _comp;

    _hdle_t _left_nil() const noexcept {
        return _guard;
    }

    _hdle_t _rght_nil() const noexcept {
        return _guard + 1;
    }

    ALA_HAS_MEM(first)
    ALA_HAS_MEM(comp)

    static_assert(_has_first<value_type>::value == _has_comp<value_compare>::value,
                  "Key compare check failed");

    template<typename Dummy = value_type,
             typename = enable_if_t<_has_first<Dummy>::value>>
    auto _key(const value_type &v) const noexcept -> decltype(v.first) {
        return v.first;
    }

    template<typename Dummy = value_type,
             typename = enable_if_t<!_has_first<Dummy>::value>>
    const value_type &_key(const value_type &v) const noexcept {
        return v;
    }

    template<typename Dummy = value_compare,
             typename = enable_if_t<_has_comp<Dummy>::value>>
    auto _key_comp() const noexcept -> decltype(_comp.comp) {
        return _comp.comp;
    }

    template<typename Dummy = value_compare,
             typename = enable_if_t<!_has_comp<Dummy>::value>>
    const value_compare &_key_comp() const noexcept {
        return _comp;
    }

    template<class... Args>
    _hdle_t construct_node(Args &&... args) {
        _hdle_t node = _alloc_traits::template allocate_object<_node_t>(_alloc,
                                                                        1);
        _alloc_traits::construct(_alloc, ala::addressof(node->_data),
                                 ala::forward<Args>(args)...);
        node->_is_nil = false;
        return node;
    }

    void destruct_node(_hdle_t &node) {
        if (!node->_is_nil)
            _alloc_traits::destroy(_alloc, ala::addressof(node->_data));
        _alloc_traits::template deallocate_object<_node_t>(_alloc, node, 1);
        node = nullptr;
    }

    void destruct_tree(_hdle_t &node) {
        if (is_nil(node))
            return;
        destruct_tree(node->_left);
        destruct_tree(node->_rght);
        destruct_node(node);
    }

    _hdle_t copy_tree(_hdle_t other, _hdle_t parent = nullptr) {
        if (is_nil(other))
            return nullptr;
        _hdle_t node = construct_node(other->_data);
        node->_color = other->_color;
        node->_parent = parent;
        node->_left = copy_tree(other->_left, node);
        node->_rght = copy_tree(other->_rght, node);
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

    _hdle_t rotate_left(_hdle_t x) noexcept {
        _hdle_t y = x->_rght;
        if ((x->_rght = y->_left) != nullptr)
            // b can't be equal to NIL,so using is_nil is correct,too
            y->_left->_parent = x;
        y->_left = x;
        if ((y->_parent = x->_parent) == nullptr)
            _root = y;
        else if (x == x->_parent->_left)
            x->_parent->_left = y;
        else
            x->_parent->_rght = y;
        x->_parent = y;
        return y;
    }

    _hdle_t rotate_rght(_hdle_t y) noexcept {
        _hdle_t x = y->_left;
        if ((y->_left = x->_rght) != nullptr)
            x->_rght->_parent = y;
        x->_rght = y;
        if ((x->_parent = y->_parent) == nullptr)
            _root = x;
        else if (y == y->_parent->_left)
            y->_parent->_left = x;
        else
            y->_parent->_rght = x;
        y->_parent = x;
        return x;
    }

    /*---------------------------------
    |      up     trans     up        |
    |       \     ====>      \        |
    |        u                v       |
    ---------------------------------*/

    void transplant(_hdle_t u, _hdle_t v) noexcept {
        _hdle_t uparent = u->_parent;
        if (uparent == nullptr)
            _root = v;
        else if (u == uparent->_left)
            uparent->_left = v;
        else
            uparent->_rght = v;
        if (v != nullptr)
            v->_parent = uparent;
    }

    void initialize() {
        _guard = _alloc_traits::template allocate_object<_node_t>(_alloc, 2);
        _left_nil()->_is_nil = true;
        _left_nil()->_parent = _rght_nil();
        _left_nil()->_color = ALA_BLACK;
        _left_nil()->_nflag = ALA_LEFT;
        _left_nil()->_left = nullptr;
        _left_nil()->_rght = nullptr;

        _rght_nil()->_is_nil = true;
        _rght_nil()->_parent = _left_nil();
        _rght_nil()->_color = ALA_BLACK;
        _rght_nil()->_nflag = ALA_RGHT;
        _rght_nil()->_left = nullptr;
        _rght_nil()->_rght = nullptr;
    }

    void fix_nil() noexcept {
        if (_root == nullptr) {
            _left_nil()->_parent = _rght_nil();
            _rght_nil()->_parent = _left_nil();
        } else {
            _hdle_t lleaf = left_leaf(_root);
            _hdle_t rleaf = rght_leaf(_root);
            lleaf->_left = _left_nil();
            rleaf->_rght = _rght_nil();
            _left_nil()->_parent = lleaf;
            _rght_nil()->_parent = rleaf;
        }
    }

    void attach_to(_hdle_t pos, _hdle_t p) noexcept {
        p->_color = ALA_RED;
        p->_left = p->_rght = nullptr;
        p->_parent = pos;
        if (pos == nullptr) { // empty tree
            _root = p;
            _left_nil()->_parent = _rght_nil()->_parent = p;
            p->_left = _left_nil();
            p->_rght = _rght_nil();
        } else if (!_comp(pos->_data, p->_data)) {
            if (pos->_left == _left_nil()) { // fix nil
                p->_left = _left_nil();
                _left_nil()->_parent = p;
            }
            pos->_left = p;
        } else {
            if (pos->_rght == _rght_nil()) { // fix nil
                p->_rght = _rght_nil();
                _rght_nil()->_parent = p;
            }
            pos->_rght = p;
        }
        ++_size;
        rebalance_for_attach(p);
    }

    /*---------------------------------------------------------------------------------
    |      Gb       P.c=b        Gb                    Gb                    Pb       |
    |     / \       U.c=b       / \       RotL(P)     / \       RotR(G)     / \       |
    |   Pr   Ur     ====>*    Pr   Ub     ====>     Pr   Ub     ====>     Xr   Gr     |
    |  / \          G.c=r    / \          P<->X    / \          P.c=b         / \     |
    |     Xr        X<-G        Xr                Xr            G.c=r            Ub   |
    ---------------------------------------------------------------------------------*/
    // remove continuous red
    // X: current, P: parent, U:uncle, G:grandp
    // b/r: black/red
    // c: color, L: left, R: right
    // <-: assign, <->: swap
    // =>: jump to next case, *: jump to a possiable case
    void rebalance_for_attach(_hdle_t current) noexcept {
        _hdle_t parent, grandp, uncle;
        while (is_red(parent = current->_parent)) {
            grandp = parent->_parent;
            if (parent == grandp->_left) { // see graph
                uncle = grandp->_rght;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_rght == current)
                        parent = rotate_left(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_rght(grandp);
                }
            } else { // symmetrical
                uncle = grandp->_left;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_left == current)
                        parent = rotate_rght(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_left(grandp);
                }
            }
        }
        _root->_color = ALA_BLACK;
    }

    void detach(_hdle_t current) noexcept {
        _hdle_t child, parent, subs, fix = nullptr;
        bool color;
        if (is_nil(current->_left)) {
            color = current->_color;
            child = current->_rght;
            parent = current->_parent;
            transplant(current, child);
            fix = child == nullptr ? parent : child;
        } else if (is_nil(current->_rght)) {
            color = current->_color;
            child = current->_left;
            parent = current->_parent;
            transplant(current, child);
            fix = child == nullptr ? parent : child;
        } else {
            subs = left_leaf(current->_rght);
            color = subs->_color;
            child = subs->_rght;
            parent = subs->_parent;
            if (parent == current) {
                parent = subs;
            } else {
                transplant(subs, child);
                subs->_rght = current->_rght;
                subs->_rght->_parent = subs;
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
    |      Pb       P.c=r        P?                    P?       Y.L.c=b      P?       Y.c=P.c      Y?    |
    |     / \       Y.c=b       / \       Y.c=R       / \       Y.c=r       / \       P.c=b       / \    |
    |   Xb   Yr     ====>*    Xb   Yb     ====>*    Xb   Yb     ====>     Xb   Yb     ====>     Pb   b   |
    |       / \     RotL(P)       / \     X<-P          / \     RotR(Y)       / \     Y.R.c=b  / \       |
    |     P.R       Y=P.R        b   b                 r   b    Y=P.R            r    RotL(Y) X          |
    ----------------------------------------------------------------------------------------------------*/
    // make current black-height + 1
    // Y: brother

    void rebalance_for_detach(_hdle_t current, _hdle_t parent) noexcept {
        _hdle_t brother;
        while (current != _root && is_black(current)) {
            if (parent->_left == current) {
                brother = parent->_rght;
                assert(!is_nil(brother));
                if (brother->_color == ALA_RED) {
                    brother->_color = ALA_BLACK;
                    parent->_color = ALA_RED;
                    rotate_left(parent);
                    brother = parent->_rght;
                }
                if (is_black(brother->_left) && is_black(brother->_rght)) {
                    brother->_color = ALA_RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_rght)) {
                        if (!is_nil(brother->_left))
                            brother->_left->_color = ALA_BLACK;
                        brother->_color = ALA_RED;
                        rotate_rght(brother);
                        brother = parent->_rght;
                    }
                    brother->_color = parent->_color;
                    parent->_color = ALA_BLACK;
                    if (!is_nil(brother->_rght))
                        brother->_rght->_color = ALA_BLACK;
                    rotate_left(parent);
                    current = _root; // make root black
                    break;
                }
            } else {
                brother = parent->_left;
                if (brother->_color == ALA_RED) {
                    brother->_color = ALA_BLACK;
                    parent->_color = ALA_RED;
                    rotate_rght(parent);
                    brother = parent->_left;
                }
                if (is_black(brother->_left) && is_black(brother->_rght)) {
                    brother->_color = ALA_RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_left)) {
                        if (!is_nil(brother->_rght))
                            brother->_rght->_color = ALA_BLACK;
                        brother->_color = ALA_RED;
                        rotate_left(brother);
                        brother = parent->_left;
                    }
                    brother->_color = parent->_color;
                    parent->_color = ALA_BLACK;
                    if (!is_nil(brother->_left))
                        brother->_left->_color = ALA_BLACK;
                    rotate_rght(parent);
                    current = _root;
                    break;
                }
            }
        }
        if (!is_nil(current))
            current->_color = ALA_BLACK;
    }

    void fix_nil_detach(_hdle_t current, _hdle_t subs) noexcept {
        if (current->_left == _left_nil()) {
            if (current->_rght == _rght_nil()) {
                _left_nil()->_parent = _rght_nil();
                _rght_nil()->_parent = _left_nil();
                _root = nullptr;
            } else {
                subs->_left = _left_nil();
                _left_nil()->_parent = subs;
            }
        } else if (current->_rght == _rght_nil()) {
            subs->_rght = _rght_nil();
            _rght_nil()->_parent = subs;
        }
    }
}; // namespace ala

} // namespace ala

#undef ALA_RED
#undef ALA_BLACK
#undef ALA_RGHT
#undef ALA_LEFT

#endif // HEAD