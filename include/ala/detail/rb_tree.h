// Introduction to Algorithms
#ifndef _ALA_DETAIL_RB_TREE_H
#define _ALA_DETAIL_RB_TREE_H

#include <ala/detail/allocator.h>
#include <ala/iterator.h>
#include <ala/type_traits.h>

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

template<class NAlloc>
constexpr typename NAlloc::pointer allocate_node() {
    typename NAlloc::pointer node = NAlloc().allocate(1);
    return node;
}

template<class NAlloc, class Alloc, class... Args>
constexpr typename NAlloc::pointer construct_node(Args &&... args) {
    typename NAlloc::pointer node = NAlloc().allocate(1);
    Alloc().construct(ala::addressof(node->_data), ala::forward<Args>(args)...);
    return node;
}

template<class NAlloc, class Alloc>
constexpr typename NAlloc::pointer copy_node(typename NAlloc::pointer other) {
    typename NAlloc::pointer node;
    if (other._is_construct)
        node = construct_node<NAlloc, Alloc>(other._data);
    else
        node = allocate_node<NAlloc>(other._data);
    node->_color = other._color;
    node->_is_construct = other._is_construct;
    node->_is_nil = other._is_nil;
    node->_nil_type = other._nil_type;
    node->_left = other._left;
    node->_right = other._right;
    node->_parent = other._parent;
    return node;
}

template<class NAlloc, class Alloc>
constexpr void destruct_node(typename NAlloc::pointer node) {
    if (node->_is_construct)
        Alloc().destroy(ala::addressof(node->_data));
    NAlloc().deallocate(node, 1);
}

template<class NAlloc, class Alloc>
constexpr void destruct_tree(typename NAlloc::pointer node) {
    if (node->_left != nullptr)
        destruct_tree<NAlloc, Alloc>(node->_left);
    if (node->_right != nullptr)
        destruct_tree<NAlloc, Alloc>(node->_right);
    destruct_node<NAlloc, Alloc>(node);
}

template<class NAlloc, class Alloc>
constexpr typename NAlloc::pointer
copy_tree(typename NAlloc::pointer other,
          typename NAlloc::pointer p = nullptr) {
    typename NAlloc::pointer node = construct_node<NAlloc>(other._data);
    node->_color = other._color;
    node->_is_construct = other._is_construct;
    node->_is_nil = other._is_nil;
    node->_nil_type = other._nil_type;
    node->_parent = p;
    if (other._left == nullptr)
        node->_left = nullptr;
    else
        node->_left = copy_tree(other._left, node);
    if (other._right == nullptr)
        node->_right = nullptr;
    else
        node->_right = copy_tree(other._right, node);
    return node;
}

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
constexpr bool is_left_nil(rb_node<Data> *node) {
    return node != nullptr && node->_is_nil && !node->_nil_type;
}

template<class Data>
constexpr bool is_right_nil(rb_node<Data> *node) {
    return node != nullptr && node->_is_nil && node->_nil_type;
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
        return nullptr;
    while (!is_nil(node->_right))
        node = node->_right;
    return node;
}

template<class Data>
constexpr void move_suc(rb_node<Data> *&_handle) {
    if (_handle == nullptr || is_right_nil(_handle))
        return;
    else if (_handle->_left != nullptr)
        _handle = right_leaf(_handle->_left);
    else {
        while (true) {
            if (_handle->_parent == nullptr ||
                _handle->_parent->_right == _handle) {
                _handle = _handle->_parent;
                break;
            }
            _handle = _handle->_parent;
        }
    }
    return;
}

template<class Data>
constexpr void move_pre(rb_node<Data> *&_handle) {
    if (_handle == nullptr || is_left_nil(_handle))
        return;
    else if (_handle->_right != nullptr)
        _handle = left_leaf(_handle->_right);
    else
        while (true) {
            if (_handle->_parent == nullptr ||
                _handle->_parent->_left == _handle) {
                _handle = _handle->_parent;
                break;
            }
            _handle = _handle->_parent;
        }
    return;
}

template<typename>
struct rb_const_iterator;
template<typename>
struct rb_iterator;

template<typename Data>
struct rb_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef Data value_type;
    typedef ptrdiff_t difference_type;
    typedef Data *pointer;
    typedef Data &reference;
    typedef rb_iterator<Data> _self_type;
    typedef rb_node<Data> *_handle_type;

    constexpr rb_iterator(): _handle() {}
    constexpr rb_iterator(const _self_type &other): _handle(other._handle) {}
    constexpr explicit rb_iterator(const rb_const_iterator<Data> &other)
        : _handle(other._handle) {}

    constexpr reference operator*() const {
        return const_cast<reference>(_handle->_data);
    }

    constexpr decltype(auto) operator-> () const {
        return ala::pointer_traits<pointer>::pointer_to(_handle->_data);
    }

    constexpr bool operator==(const _self_type &rhs) const {
        return _handle == rhs._handle;
    }

    constexpr bool operator!=(const _self_type &rhs) const {
        return _handle != rhs._handle;
    }

    constexpr _self_type operator++() {
        move_suc(_handle);
        return *this;
    }

    constexpr _self_type operator++(int) {
        _handle_type tmp(_handle);
        move_suc(_handle);
        return _self_type(tmp);
    }

    constexpr _self_type operator--() {
        move_pre(_handle);
        return *this;
    }

    constexpr _self_type operator--(int) {
        _handle_type tmp(_handle);
        move_pre(_handle);
        return _self_type(tmp);
    }

    template<typename, typename, typename>
    friend struct rb_tree;
    template<typename>
    friend struct rb_const_iterator;

protected:
    rb_iterator(rb_node<Data> *handle): _handle(handle) {}
    _handle_type _handle;
};

template<typename Data>
struct rb_const_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef Data value_type;
    typedef ptrdiff_t difference_type;
    typedef Data *const pointer;
    typedef const Data &reference;
    typedef rb_const_iterator<Data> _self_type;
    typedef rb_node<Data> *_handle_type;

    constexpr rb_const_iterator(): _handle() {}
    constexpr rb_const_iterator(const _self_type &other)
        : _handle(other._handle) {}
    constexpr explicit rb_const_iterator(const rb_iterator<Data> &other)
        : _handle(other._handle) {}

    constexpr reference &operator*() const {
        return const_cast<reference>(_handle->_data);
    }

    constexpr pointer operator->() const {
        return ala::pointer_traits<pointer>::pointer_to(_handle->_data);
    }

    constexpr bool operator==(const _self_type &rhs) const {
        return _handle == rhs._handle;
    }

    constexpr bool operator!=(const _self_type &rhs) const {
        return _handle != rhs._handle;
    }

    constexpr _self_type operator++() {
        move_suc(_handle);
        return *this;
    }

    constexpr _self_type operator++(int) {
        _handle_type tmp(_handle);
        move_suc(_handle);
        return _self_type(tmp);
    }

    constexpr _self_type operator--() {
        move_pre(_handle);
        return *this;
    }

    constexpr _self_type operator--(int) {
        _handle_type tmp(_handle);
        move_pre(_handle);
        return _self_type(tmp);
    }

private:
    template<typename, typename, typename>
    friend struct rb_tree;
    template<typename>
    friend struct rb_iterator;
    rb_const_iterator(_handle_type *handle): _handle(handle) {}
    _handle_type _handle;
};

template<class Data, class Comp, class Alloc>
class rb_tree {
public:
    typedef Data value_type;
    typedef Comp value_compare;
    typedef Alloc allocator_type;
    typedef rb_node<Data> node_type;
    typedef
        typename Alloc::template rebind<node_type>::other node_allocator_type;
    typedef rb_iterator<Data> iterator;
    typedef rb_const_iterator<Data> const_iterator;
    typedef ala::reverse_iterator<iterator> reverse_iterator;
    typedef ala::reverse_iterator<const_iterator> const_reverse_iterator;

    ~rb_tree() {
        if (_root != nullptr) {
            destruct_tree<node_allocator_type, Alloc>(_root);
        } else {
            destruct_node<node_allocator_type, Alloc>(_left_nil);
            destruct_node<node_allocator_type, Alloc>(_right_nil);
        }
    }

    rb_tree(Comp cmp, Alloc alloc)
        : _cmp(cmp), _root(nullptr),
          _left_nil(allocate_node<node_allocator_type>()),
          _right_nil(allocate_node<node_allocator_type>()) {
        _left_nil->_is_construct = false;
        _left_nil->_parent = _right_nil;
        _left_nil->_color = ALA_BLACK;
        _left_nil->_is_nil = true;
        _left_nil->_nil_type = false;
        _left_nil->_left = nullptr;
        _left_nil->_right = nullptr;

        _right_nil->_is_construct = false;
        _right_nil->_parent = _left_nil;
        _right_nil->_color = ALA_BLACK;
        _right_nil->_is_nil = true;
        _right_nil->_nil_type = true;
        _right_nil->_left = nullptr;
        _right_nil->_right = nullptr;
    }

    template<class OtherAlloc>
    rb_tree(const rb_tree<Data, Comp, OtherAlloc> &other,
            Alloc a = OtherAlloc())
        : _cmp(other._cmp) {
        if (other._root != nullptr) {
            _root = copy_tree<node_allocator_type, Alloc>(other._root);
            _left_nil = left_leaf(_root);
        } else {
            destruct_node<node_allocator_type, Alloc>(_left_nil);
            destruct_node<node_allocator_type, Alloc>(_right_nil);
        }
    }

    template<class OtherAlloc>
    rb_tree(rb_tree<Data, Comp, OtherAlloc> &&other, Alloc a = OtherAlloc())
        : _cmp(other._cmp) {
        _root = other._root;
        _left_nil = other._left_nil;
        _right_nil = other._right_nil;
        other._root = nullptr;
        other._left_nil = nullptr;
        other._right_nil = nullptr;
    }

    iterator begin() { return iterator(_left_nil->_parent); }
    iterator end() { return iterator(_right_nil); }
    const_iterator cbegin() const { return const_iterator(_left_nil->_parent); }
    const_iterator cend() const { return const_iterator(_right_nil); }
    reverse_iterator rbegin() {
        return reverse_iterator(iterator(_right_nil->_parent));
    }
    reverse_iterator rend() { return reverse_iterator(iterator(_left_nil)); }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(const_iterator(_right_nil->_parent));
    }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(const_iterator(_left_nil));
    }

    // rotate
    /*-------------------------------------
    |      x        left        y         |
    |     / \       ====>      / \        |
    |    a   y                x   c       |
    |       / \     <====    / \          |
    |      b   c    right   a   b         |
    --------------------------------------*/

    node_type *rotate_left(node_type *x) noexcept {
        node_type *y = x->_right;

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

    node_type *rotate_right(node_type *y) noexcept {
        node_type *x = y->_left;

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

    void fix_nil() noexcept {
        if (_root == nullptr) {
            _left_nil->_parent = _right_nil;
            _right_nil->_parent = _left_nil;
        } else {
            node_type *lleaf = left_leaf(_root);
            node_type *rleaf = right_leaf(_root);
            lleaf->_left = _left_nil;
            rleaf->_right = _right_nil;
            _left_nil->_parent = lleaf;
            _right_nil->_parent = rleaf;
        }
    }

    template<typename Data1>
    node_type *search(Data1 &&data) {
        node_type *current = _root;
        while (!is_nil(current))
            if (_cmp(data, current->_data))
                current = current->_left;
            else if (_cmp(current->_data, data))
                current = current->_right;
            else
                return current;
        return nullptr;
    }

    void rebalance_for_insert(node_type *current) {
        node_type *parent, *grandp, *uncle;
        while (is_red(parent = current->_parent)) {
            grandp = parent->_parent;
            if (parent == grandp->_left) {
                uncle = grandp->_right;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_right == current)
                        rotate_left(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
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
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_left(grandp);
                }
            }
        }
        _root->_color = ALA_BLACK;
    }

    template<typename Data1>
    node_type *insert(Data1 &&data, bool overwrite = false) {
        node_type *current = nullptr, *guard = _root;
        node_type *new_node = construct_node<node_allocator_type, Alloc>(
            ala::forward<Data1>(data));
        new_node->_is_construct = true;
        new_node->_is_nil = false;
        new_node->_color = ALA_RED;
        new_node->_left = nullptr;
        new_node->_right = nullptr;
        while (!is_nil(guard)) {
            current = guard;
            if (_cmp(data, guard->_data))
                guard = guard->_left;
            else if (_cmp(guard->_data, data))
                guard = guard->_right;
            else {
                if (overwrite)
                    // current->_val = val;
                    ;
                return guard;
            }
        }
        new_node->_parent = current;
        if (current == nullptr) // empty tree
            _root = new_node;
        else if (_cmp(new_node->_data, current->_data))
            current->_left = new_node;
        else
            current->_right = new_node;
        fix_nil();
        rebalance_for_insert(new_node);
        return new_node;
    }

    void transplant(node_type *u, node_type *v) {
        node_type *uparent = u->_parent;
        if (uparent == nullptr)
            _root = v;
        else if (u == uparent->_left)
            uparent->_left = v;
        else
            uparent->_right = v;
        if (v != nullptr)
            v->_parent = uparent;
    }

    void rebalance_for_erase(node_type *current, node_type *parent) {
        node_type *brother;
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

    template<typename Data1>
    void erase(Data1 &&data) {
        node_type *current, *child, *parent, *temp;
        bool color;
        temp = current = search(ala::forward<Data1>(data));
        if (current == nullptr)
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

        temp->_left = temp->_right = nullptr;
        destruct_node<node_allocator_type, Alloc>(temp);
        fix_nil();
        if (color == ALA_BLACK)
            rebalance_for_erase(child, parent);
    }
    template<class Key, class T, class Comp,class Alloc>
    friend class map;
    // member variable
protected:
    node_type *_root, *_left_nil, *_right_nil;
    Comp _cmp;
};

} // namespace ala

#undef ALA_RED
#undef ALA_BLACK

#endif // HEAD