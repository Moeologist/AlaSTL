// Introduction to Algorithms
#ifndef _ALA_DETAIL_RB_TREE_H
#define _ALA_DETAIL_RB_TREE_H

#include <ala/type_traits.h>
#include <ala/detail/allocator.h>
#include <ala/detail/macro.h>
#include <ala/iterator.h>
#include <ala/external/assert.h>

#define ALA_RED true
#define ALA_BLACK false

namespace ala {

template<class Data>
struct rb_node {
    Data _data;
    rb_node *_left, *_right, *_parent;
    bool _nil_type;
    bool _is_nil;
    bool _is_construct;
    bool _color;
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

template<class Data, class Ptr>
struct rb_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef Data value_type;
    typedef ptrdiff_t difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;

    constexpr rb_iterator(): _ptr(nullptr) {}
    constexpr rb_iterator(const rb_iterator &other): _ptr(other._ptr) {}
    constexpr rb_iterator(const Ptr &ptr): _ptr(ptr) {}

    constexpr reference operator*() const {
        return const_cast<reference>(_ptr->_data);
    }

    constexpr pointer operator->() const {
        return ala::pointer_traits<pointer>::pointer_to(_ptr->_data);
    }

    constexpr bool operator==(const rb_iterator &rhs) const {
        return _ptr == rhs._ptr;
    }

    constexpr bool operator!=(const rb_iterator &rhs) const {
        return _ptr != rhs._ptr;
    }

    constexpr rb_iterator &operator++() {
        if (_ptr == nullptr || (_ptr->_is_nil && _ptr->_nil_type))
            return *this;
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
        return *this;
    }

    constexpr rb_iterator operator++(int) {
        rb_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr rb_iterator &operator--() {
        if (_ptr == nullptr || (_ptr->_is_nil && !_ptr->_nil_type))
            return *this;
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
        return *this;
    }

    constexpr rb_iterator operator--(int) {
        rb_iterator tmp(*this);
        --*this;
        return tmp;
    }

protected:
    template<typename, typename, typename, typename>
    friend struct map;
    Ptr _ptr;
};

template<class Data, class Comp, class Alloc,
         class NAlloc = typename Alloc::template rebind<rb_node<Data>>::other>
class rb_tree {
public:
    typedef Data value_type;
    typedef Comp value_compare;
    typedef Alloc allocator_type;
    typedef rb_node<Data> node_type;
    typedef NAlloc node_allocator_type;
    typedef typename node_allocator_type::pointer node_pointer;

    static_assert(is_same<node_pointer, node_type *>::value,
                  "ala node-based container use raw pointer");

    rb_tree(const value_compare &cmp, const allocator_type &a)
        : _comp(cmp), _alloc(a), _nalloc(), _size(0), _root(nullptr) {
        initializer_nil();
    }

    rb_tree(const rb_tree &other)
        : _comp(other._comp), _alloc(other._alloc), _nalloc(other._nalloc),
          _size(other._size) {
        initializer_nil();
        _root = copy_tree(other._root);
        fix_nil();
    }

    rb_tree(rb_tree &&other)
        : _comp(ala::move(other._comp)), _alloc(ala::move(other._alloc)),
          _nalloc(ala::move(other._nalloc)), _size(other._size) {
        initializer_nil();
        _root = other._root;
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    rb_tree(const rb_tree &other, const allocator_type &a)
        : _comp(other._comp), _alloc(a), _nalloc(), _size(other._size) {
        initializer_nil();
        _root = copy_tree(other._root);
        fix_nil();
    }

    rb_tree(rb_tree &&other, const allocator_type &a)
        : _comp(other._comp), _alloc(a), _nalloc(), _size(other._size) {
        initializer_nil();
        _root = other._root;
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

    rb_tree &operator=(const rb_tree &other) {
        destruct_tree(_root);
        _root = copy_tree(other._root);
        _size = other._size;
        fix_nil();
        return *this;
    }

    rb_tree &operator=(rb_tree &&other) {
        destruct_tree(_root);
        _root = other._root;
        _size = other._size;
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
        return *this;
    }

    ~rb_tree() {
        destruct_tree(_root);
        destruct_node(_left_nil);
        destruct_node(_right_nil);
    }

    node_pointer begin() const {
        return _left_nil->_parent;
    }

    node_pointer end() const {
        return _right_nil;
    }

    // node_pointer rbegin() const {
    //     return _right_nil->_parent;
    // }

    // node_pointer rend() const {
    //     return _left_nil;
    // }

    void clear() {
        destruct_tree(_root);
        _size = 0;
        fix_nil();
    }

    size_t size() const {
        return _size;
    }

    void remove(node_pointer position) {
        if (!is_nil(position)) {
            detach(position);
            destruct_node(position);
        }
    }

    void extract(node_pointer position) {
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

    void swap(rb_tree &other) noexcept(
        allocator_traits<allocator_type>::is_always_equal::value
            &&is_nothrow_swappable<value_compare>::value) {
        ala::swap(_comp, other._comp);
        ala::swap(_root, other._root);
        ala::swap(_left_nil, other._left_nil);
        ala::swap(_right_nil, other._right_nil);
        ala::swap(_size, other._size);
    }

    template<class RBTree, bool ReBalance = true>
    void merge_tree(RBTree &tree, node_pointer other) {
        if (!is_nil(other->_left))
            merge_tree_rv(tree, other->_left);
        if (!is_nil(other->_right))
            merge_tree_rv(tree, other->_right);
        pair<node_pointer, bool> pr(search(other));
        if (!pr.second) {
            tree.detach<ReBalance>(other);
            attach_to(pr.first, other);
        }
    }

    template<class Comp1>
    void merge(rb_tree<value_type, Comp1, allocator_type> &source) {
        merge_tree<true>(source, source._root);
    }

    template<class Comp1>
    void merge(rb_tree<value_type, Comp1, allocator_type> &&source) {
        merge_tree<false>(source, source._root);
        source.clear();
    }

    template<class K>
    node_pointer find(const K &key) const {
        decltype(auto) comp = get_key_comp();
        node_pointer current = _root;
        while (!is_nil(current))
            if (comp(key, get_key(current->_data)))
                current = current->_left;
            else if (comp(get_key(current->_data), key))
                current = current->_right;
            else
                return current;
        return end();
    }

    template<class K>
    size_t count(const K &key) const {
        decltype(auto) comp = get_key_comp();
        node_pointer current = _root;
        size_t eql = 0;
        while (!is_nil(current)) {
            if (comp(key, get_key(current->_data)))
                current = current->_left;
            else if (comp(get_key(current->_data), key))
                current = current->_right;
            else
                ++eql;
        }
        return eql;
    }

    template<class K>
    bool contains(const K &key) const {
        decltype(auto) comp = get_key_comp();
        node_pointer current = _root;
        while (!is_nil(current))
            if (comp(key, get_key(current->_data)))
                current = current->_left;
            else if (comp(get_key(current->_data), key))
                current = current->_right;
            else
                return true;
        return false;
    }

    template<class K>
    size_t erase(const K &key) {
        decltype(auto) comp = get_key_comp();
        node_pointer current = _root;
        size_t eql = 0;
        while (!is_nil(current)) {
            if (comp(key, get_key(current->_data)))
                current = current->_left;
            else if (comp(get_key(current->_data), key))
                current = current->_right;
            else {
                remove(current);
                ++eql;
                current = current->_left;
            }
        }
        return eql;
    }

    template<class... Args>
    pair<node_pointer, bool> emplace(Args &&... args) {
        using ret = pair<node_pointer, bool>;
        node_pointer new_node = construct_node(ala::forward<Args>(args)...);
        pair<node_pointer, bool> pr(search(new_node));
        if (pr.second) {
            destruct_node(new_node);
            return ret(pr.first, false);
        } else {
            attach_to(pr.first, new_node);
            return ret(new_node, true);
        }
    }

    template<class... Args>
    node_pointer emplace_hint(node_pointer hint, Args &&... args) {
        node_pointer new_node = construct_node(ala::forward<Args>(args)...);
        pair<node_pointer, bool> pr(search_hint(hint, new_node));
        if (pr.second) {
            destruct_node(new_node);
            return pr.first;
        } else {
            attach_to(pr.first, new_node);
            return new_node;
        }
    }

    pair<node_pointer, bool> insert(node_pointer p) {
        if (is_nil(p))
            return pair<node_pointer, bool>(end(), false);
        pair<node_pointer, bool> pr(search(p));
        if (pr.second) {
            return pair<node_pointer, bool>(pr.first, false);
        } else {
            attach_to(pr.first, p);
            return pair<node_pointer, bool>(p, true);
        }
    }

    pair<node_pointer, bool> insert_hint(node_pointer hint, node_pointer p) {
        if (is_nil(p))
            return pair<node_pointer, bool>(end(), false);
        pair<node_pointer, bool> pr(search_hint(hint, p));
        if (pr.second) {
            return pair<node_pointer, bool>(pr.first, false);
        } else {
            attach_to(pr.first, p);
            return pair<node_pointer, bool>(p, true);
        }
    }

protected:
    node_pointer _root, _left_nil, _right_nil;
    size_t _size;
    allocator_type _alloc;
    node_allocator_type _nalloc;
    value_compare _comp;

    ALA_HAS_MEM_VAL(first)
    ALA_HAS_MEM_VAL(comp)

    static_assert(_has_first<value_type>::value ==
                      _has_comp<value_compare>::value,
                  "key compare check failed");

    template<typename Dummy = value_type,
             typename Ret = decltype(declval<value_type>().first)>
    const Ret &get_key(const value_type &v) const {
        return v.first;
    }

    template<typename Dummy = value_type, typename = void,
             typename = enable_if_t<!_has_first<Dummy>::value>>
    const value_type &get_key(const value_type &v) const {
        return v;
    }

    template<typename Dummy = value_compare,
             typename Ret = decltype(declval<value_compare>().comp)>
    const Ret &get_key_comp() const {
        return _comp.comp;
    }

    template<typename Dummy = value_compare, typename = void,
             typename = enable_if_t<!_has_comp<Dummy>::value>>
    const value_compare &get_key_comp() const {
        return _comp;
    }

    node_pointer allocate_node() {
        node_pointer node = _nalloc.allocate(1);
        node->_is_construct = false;
        return node;
    }

    template<class... Args>
    node_pointer construct_node(Args &&... args) {
        node_pointer node = _nalloc.allocate(1);
        _alloc.construct(ala::addressof(node->_data),
                         ala::forward<Args>(args)...);
        node->_is_construct = true;
        return node;
    }

    void destruct_node(node_pointer node) {
        if (node->_is_construct)
            _alloc.destroy(ala::addressof(node->_data));
        _nalloc.deallocate(node, 1);
    }

    void destruct_tree(node_pointer node) {
        if (is_nil(node))
            return;
        destruct_tree(node->_left);
        destruct_tree(node->_right);
        destruct_node(node);
    }

    node_pointer copy_node(node_pointer other) {
        node_pointer node;
        if (other->_is_construct)
            node = construct_node(other->_data);
        else
            node = allocate_node();
        node->_color = other->_color;
        node->_is_nil = other->_is_nil;
        node->_nil_type = other->_nil_type;
        return node;
    }

    node_pointer copy_tree(node_pointer other, node_pointer parent = nullptr) {
        if (is_nil(other))
            return nullptr;
        node_pointer node = copy_node(other);
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
        _left_nil = allocate_node();
        _right_nil = allocate_node();

        _left_nil->_parent = _right_nil;
        _left_nil->_color = ALA_BLACK;
        _left_nil->_is_nil = true;
        _left_nil->_nil_type = false;
        _left_nil->_left = nullptr;
        _left_nil->_right = nullptr;

        _right_nil->_parent = _left_nil;
        _right_nil->_color = ALA_BLACK;
        _right_nil->_is_nil = true;
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

    pair<node_pointer, bool> search(node_pointer p) {
        return search_at(_root, p);
    }

    pair<node_pointer, bool> search_hint(node_pointer hint, node_pointer p) {
        if (_comp(p->_data, hint->_data))
            return search_at(hint, p);
        else
            return search_at(_root, p);
    }

    pair<node_pointer, bool> search_at(node_pointer guard, node_pointer p) {
        using ret = pair<node_pointer, bool>;
        node_pointer current = nullptr;
        bool found = false;
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
        if (found)
            return ret(current, true);
        return ret(current, false);
    }

    void attach_to(node_pointer pos, node_pointer p) noexcept {
        p->_color = ALA_RED;
        p->_left = p->_right = nullptr;
        p->_is_nil = false;
        p->_parent = pos;
        if (pos == nullptr) // empty tree
            _root = p;
        else if (_comp(p->_data, pos->_data))
            pos->_left = p;
        else
            pos->_right = p;
        ++_size;
        fix_nil();
        rebalance_for_attach(p);
    }

    void rebalance_for_attach(node_pointer current) noexcept {
        node_pointer parent, grandp, uncle;
        while (is_red(parent = current->_parent)) {
            grandp = parent->_parent;
            /*if (grandp == nullptr)
                break;
            else */
            if (parent == grandp->_left) {
                uncle = grandp->_right;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_right == current)
                        rotate_left(current = parent);
                    current->_parent->_color = ALA_BLACK;
                    current->_parent->_parent->_color = ALA_RED;
                    rotate_right(grandp);
                }
            } else {
                uncle = grandp->_left;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_left == current)
                        rotate_right(current = parent);
                    current->_parent->_color = ALA_BLACK;
                    current->_parent->_parent->_color = ALA_RED;
                    rotate_left(grandp);
                }
            }
        }
        _root->_color = ALA_BLACK;
    }

    template<bool Rebalance = true>
    void detach(node_pointer current) noexcept {
        node_pointer child, parent, temp = current;
        bool color;
        if (is_nil(current))
            return;
        if (is_nil(current->_left)) {
            color = current->_color;
            child = current->_right;
            parent = current->_parent;
            transplant(current, child);
        } else if (is_nil(current->_right)) {
            color = current->_color;
            child = current->_left;
            parent = current->_parent;
            transplant(current, child);
        } else {
            current = left_leaf(current->_right);
            color = current->_color;
            child = current->_right;
            parent = current->_parent;
            if (current->_parent == temp)
                parent = current;
            else {
                transplant(current, child);
                current->_right = temp->_right;
                current->_right->_parent = current;
            }
            transplant(temp, current);
            current->_left = temp->_left;
            current->_left->_parent = current;
            current->_color = temp->_color;
        }
        if (Rebalance && color == ALA_BLACK)
            rebalance_for_detach(child, parent);
        fix_nil();
        --_size;
    }

    void rebalance_for_detach(node_pointer current,
                              node_pointer parent) noexcept {
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
                    current = _root;
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
};

} // namespace ala

#undef ALA_RED
#undef ALA_BLACK

#endif // HEAD