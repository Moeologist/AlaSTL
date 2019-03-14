// Algorithms come from Introduction to Algorithms

#include "ala/detail/functional_base.h"
#include "ala/detail/pair.h"
#include "ala/detail/utility_base.h"

namespace ala {

namespace detail {

enum COLOR { LEFT_NIL = -2, RIGHT_NIL = -1, BLACK = 0, RED = 1 };

template<class Data>
struct rb_node {
    rb_node(const Data &data, COLOR color, rb_node *l, rb_node *r, rb_node *p)
        : _data(data), _color(color), _left(l), _right(r), _parent(p) {}

    ~rb_node() {
        if (_left != nullptr)
            delete _left;
        if (_right != nullptr)
            delete _right;
    }

    Data _data;
    COLOR _color;
    rb_node *_left, *_right, *_parent;
};

#define is_nil(node) (node == nullptr || node->_color < 0)
#define is_black(node) \
    (node == nullptr || node->_color < 0 || node->_color == BLACK)
#define is_red(node) (node != nullptr && node->_color == RED)
#undef is_nil
#undef is_black
#undef is_red

template<class Data>
_ALA_FORCEINLINE bool const is_nil(rb_node<Data> *node) {
    return (node == nullptr || node->_color < 0);
}

template<class Data>
_ALA_FORCEINLINE bool const is_black(rb_node<Data> *node) {
    return (node == nullptr || node->_color < 0 || node->_color == BLACK);
}

template<class Data>
_ALA_FORCEINLINE bool const is_red(rb_node<Data> *node) {
    return (node != nullptr && node->_color == RED);
}

template<class Data>
_ALA_FORCEINLINE bool const is_left_nil(rb_node<Data> *node) {
    return (node != nullptr && node->_color == LEFT_NIL);
}

template<class Data>
_ALA_FORCEINLINE bool const is_right_nil(rb_node<Data> *node) {
    return (node != nullptr && node->_color == RIGHT_NIL);
}

template<class Data>
_ALA_FORCEINLINE rb_node<Data> *leftmost(rb_node<Data> *node) {
    if (is_nil(node))
        return nullptr;
    while (!is_nil(node->_left))
        node = node->_left;
    return node;
}

template<class Data>
_ALA_FORCEINLINE rb_node<Data> *rightmost(rb_node<Data> *node) {
    if (is_nil(node))
        return nullptr;
    while (!is_nil(node->_right))
        node = node->_right;
    return node;
}

template<class Data>
_ALA_FORCEINLINE void dealloc_tree(rb_node<Data> *root) {
    if (_root->left != nullptr)
}

template<class Data, class Compare>
struct rb_tree {
    typedef rb_node<Data> node_type;
    typedef typename Data::first_type Key;
    typedef typename Data::second_type Val;

    struct iterator {
        iterator(node_type *handle, node_type *root)
            : _handle(handle), _root(root) {}

        node_type &operator*() { return *_handle; }

        bool operator==(const iterator &rhs) const {
            return _handle == rhs._handle;
        }

        bool operator!=(const iterator &rhs) const {
            return _handle != rhs._handle;
        }

        iterator &operator++() {
            if (_handle == nullptr)
                return *this;
            else if (_handle->_right != nullptr)
                _handle = leftmost(_handle->_right);
            else
                while (true) {
                    if (_handle->_parent == nullptr ||
                        _handle->_parent->_left == _handle) {
                        _handle = _handle->_parent;
                        break;
                    }
                    _handle = _handle->_parent;
                }
            return *this;
        }

        iterator &operator--() {
            if (_handle == nullptr)
                _handle = rightmost(_root);
            else if (_handle->_left != nullptr)
                _handle = rightmost(_handle->_left);
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
            return *this;
        }

        node_type *_handle;
        node_type *_root;
    };

    // member type alias

    // member function

    ~rb_tree() { delete _root; }

    rb_tree(): _root(nullptr), _cmp() {
        _left_nil = new node_type(Data(), NIL, nullptr, nullptr, nullptr);
        _right_nil = new node_type(Data(), NIL, nullptr, nullptr, nullptr);
        _left_nil->_parent = _right_nil;
        _right_nil->_parent = _left_nil;
    }

    iterator begin() { return iterator(leftmost(_root), _root); }

    iterator end() { return iterator(nullptr, _root); }

    // rotate
    /*-------------------------------------
	|      x        left        y         |
	|     / \       ====>      / \        |
	|    a   y                x   c       |
	|       / \     <====    / \          |
	|      b   c    right   a   b         |
	--------------------------------------*/

    _ALA_FORCEINLINE node_type *rotate_left(node_type *x) {
        node_type *y = x->_right;

        if ((x->_right = y->_left) !=
            nullptr) // b can't be equal to NIL,so using is_nil is correct,too
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

    _ALA_FORCEINLINE node_type *rotate_right(node_type *y) {
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

    node_type *search(Key key) {
        node_type *current = _root;
        while (!is_nil(current))
            if (_cmp(pair<Key, Val>(key, 0), current->_data))
                current = current->_left;
            else if (_cmp(current->_data, pair<Key, Val>(key, 0)))
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
                    parent->_color = uncle->_color = BLACK;
                    grandp->_color = RED;
                    current = grandp;
                } else {
                    if (parent->_right == current)
                        rotate_left(current = parent);
                    parent->_color = BLACK;
                    grandp->_color = RED;
                    rotate_right(grandp);
                }
            } else {
                uncle = grandp->_left;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = BLACK;
                    grandp->_color = RED;
                    current = grandp;
                } else {
                    if (parent->_left == current)
                        rotate_right(current = parent);
                    parent->_color = BLACK;
                    grandp->_color = RED;
                    rotate_left(grandp);
                }
            }
        }
        _root->_color = BLACK;
    }

    node_type *insert(Data data, bool overwrite = false) {
        node_type *current = _root, *parent = nullptr, temp = nullptr;
        while (!is_nil(current)) {
            parent = current;
            if (_cmp(data, current->_data))
                current = current->_left;
            else if (_cmp(current->_data, data))
                current = current->_right;
            else {
                if (overwrite)
                    // current->_val = val;
                    ;
                return current;
            }
        }
        temp == current;
        current = new node_type(data, RED, nullptr, nullptr, nullptr);
        current->_parent = parent;
        if (is_left_nil(temp->_color)) {
            temp->_parent = current;
            current->_left = temp;
        } else if (is_right_nil(_temp->_color)) {
            temp->_parent = current;
            current->_right = temp;
        }
        if (parent == nullptr) { // empty tree
            _root = current;
            current->_left = _left_nil;
            current->_right = _right_nil;
            _left_nil->_parent = _right_nil->_parent = current;
        } else if (_cmp(data, current->_data))
            parent->_left = current;
        else
            parent->_right = current;

        rebalance_for_insert(current);
        return current;
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
                if (brother->_color == RED) {
                    brother->_color = BLACK;
                    parent->_color = RED;
                    rotate_left(parent);
                    brother = parent->_right;
                }
                if (is_black(brother->_left) && is_black(brother->_right)) {
                    brother->_color = RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_right)) {
                        if (!is_nil(brother->_left))
                            brother->_left->_color = BLACK;
                        brother->_color = RED;
                        rotate_right(brother);
                        brother = parent->_right;
                    }
                    brother->_color = parent->_color;
                    parent->_color = BLACK;
                    if (!is_nil(brother->_right))
                        brother->_right->_color = BLACK;
                    rotate_left(parent);
                    current = _root;
                    break;
                }
            } else {
                brother = parent->_left;
                if (brother->_color == RED) {
                    brother->_color = BLACK;
                    parent->_color = RED;
                    rotate_right(parent);
                    brother = parent->_left;
                }
                if (is_black(brother->_left) && is_black(brother->_right)) {
                    brother->_color = RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_left)) {
                        if (!is_nil(brother->_right))
                            brother->_right->_color = BLACK;
                        brother->_color = RED;
                        rotate_left(brother);
                        brother = parent->_left;
                    }
                    brother->_color = parent->_color;
                    parent->_color = BLACK;
                    if (!is_nil(brother->_left))
                        brother->_left->_color = BLACK;
                    rotate_right(parent);
                    current = _root;
                    break;
                }
            }
        }
        if (!is_nil(current))
            current->_color = BLACK;
    }

    void erase(Key key) {
        node_type *current, *child, *parent, *temp;
        COLOR color;
        if ((temp = current = search(key)) == nullptr)
            return;
        if (is_nil(current->_left)) {
            color = current->_color;
            child = current->_right;
            parent = current->_parent;
            transplant(current, child);
            if (is_left_nil(current->_left)) {
                if (is_right_nil(child))
                    child->_parent == _left_nil;
                else if (child == nullptr)
                    left_leaf = parent;
                else
                    left_leaf = left_most(child);
                left_leaf->_left = _left_nil;
                left_nil->_parent = left_leaf;
            }
        } else if (is_nil(current->_right)) {
            color = current->_color;
            child = current->_left;
            parent = current->_parent;
            transplant(current, child);
            if (is_right_nil(current->_right)) {
                if (is_nil(child))
                    right_leaf = parent;
                else
                    right_leaf = right_most(child);
                right_leaf = _right_nil;
                right_nil->_parent = right_leaf;
            }
        } else {
            current = leftmost(current->_right);
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
        delete temp;
        if (color == BLACK)
            rebalance_for_erase(child, parent);
    }

    // member variable
    node_type *_root, *_left_nil, *_right_nil;
    Compare _cmp;
};

} // namespace detail

} // namespace ala