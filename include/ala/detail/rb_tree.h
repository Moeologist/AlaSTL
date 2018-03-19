// Algorithms come from Introduction to Algorithms

#include "ala/detail/functional_base.h"
#include "ala/detail/utility_base.h"
#include "ala/detail/pair.h"

namespace ala {

namespace detail {

enum COLOR {
	BLACK = 0,
	RED = 1,
	NIL = 2
};

template <class Data>
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

#define is_nil(node) (node == nullptr || node->_color == NIL)
#define is_black(node) (node == nullptr || node->_color == NIL || node->_color == BLACK)
#define is_red(node) (node != nullptr && node->_color == RED)
#undef is_nil
#undef is_black
#undef is_red

template <class Data>
_ALA_FORCEINLINE bool const is_nil(rb_node<Data> *node) {
	return (node == nullptr || node->_color == NIL);
}

template <class Data>
_ALA_FORCEINLINE bool const is_black(rb_node<Data> *node) {
	return (node == nullptr || node->_color == NIL || node->_color == BLACK);
}

template <class Data>
_ALA_FORCEINLINE bool const is_red(rb_node<Data> *node) {
	return (node != nullptr && node->_color == RED);
}

template <class Data>
_ALA_FORCEINLINE rb_node<Data> *const leftmost(rb_node<Data> *node) {
	if (is_nil(node))
		return nullptr;
	while (!is_nil(node->_left))
		node = node->_left;
	return node;
}

template <class Data>
_ALA_FORCEINLINE rb_node<Data> *const rightmost(rb_node<Data> *node) {
	if (is_nil(node))
		return nullptr;
	while (!is_nil(node->_right))
		node = node->_right;
	return node;
}

template <class Data, class Compare>
struct rb_tree {
	typedef rb_node<Data> node_type;
	typedef typename Data::first_type Key;
	typedef typename Data::second_type Val;

	struct iterator {
		iterator(node_type *handle, node_type *root) : _handle(handle), _root(root) {}

		node_type &operator*() {
			return *_handle;
		}

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
					if (_handle->_parent == nullptr || _handle->_parent->_left == _handle) {
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
					if (_handle->_parent == nullptr || _handle->_parent->_right == _handle) {
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

	~rb_tree() {
		delete _root;
	}

	rb_tree() : _root(nullptr), _cmp() {
		_left_nil = new node_type(Data(), NIL, nullptr, nullptr, nullptr);
		_right_nil = new node_type(Data(), NIL, nullptr, nullptr, nullptr);
		_left_nil->_parent = _left_nil->_right = _right_nil;
		_right_nil->_parent = _right_nil->_left = _left_nil;
	}

	iterator begin() {
		return iterator(leftmost(_root), _root);
	}

	iterator end() {
		return iterator(nullptr, _root);
	}

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

		if (!is_nil(x->_right = y->_left))
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

		if (!is_nil(y->_left = x->_right))
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
		node_type *node = _root;
		while (!is_nil(node))
			if (_cmp(pair<Key, Val>(key, 0), node->_data))
				node = node->_left;
			else if (_cmp(node->_data, pair<Key, Val>(key, 0)))
				node = node->_right;
			else
				return node;
		return nullptr;
	}

	void rebalance_for_insert(node_type *node) {
		node_type *parent, *grandp, *uncle;
		while (is_red(parent = node->_parent)) {
			grandp = parent->_parent;
			if (parent == grandp->_left) {
				uncle = grandp->_right;
				if (is_red(uncle)) {
					parent->_color = uncle->_color = BLACK;
					grandp->_color = RED;
					node = grandp;
				}
				else {
					if (parent->_right == node)
						rotate_left(node = parent);
					parent->_color = BLACK;
					grandp->_color = RED;
					rotate_right(grandp);
				}
			}
			else {
				uncle = grandp->_left;
				if (is_red(uncle)) {
					parent->_color = uncle->_color = BLACK;
					grandp->_color = RED;
					node = grandp;
				}
				else {
					if (parent->_left == node)
						rotate_right(node = parent);
					parent->_color = BLACK;
					grandp->_color = RED;
					rotate_left(grandp);
				}
			}
		}
		_root->_color = BLACK;
	}

	node_type *insert(Data data, bool overwrite = false) {
		node_type *node = _root, *parent = nullptr;
		while (!is_nil(node)) {
			parent = node;
			if (_cmp(data, node->_data))
				node = node->_left;
			else if (_cmp(node->_data, data))
				node = node->_right;
			else {
				if (overwrite)
					// node->_val = val;
					;
				return node;
			}
		}

		node = new node_type(data, RED, nullptr, nullptr, nullptr);
		node->_parent = parent;
		if (parent == nullptr)
			_root = node;
		else if (_cmp(data, node->_data))
			parent->_left = node;
		else
			parent->_right = node;

		rebalance_for_insert(node);
		return node;
	}

	void transplant(node_type *u, node_type *v) {
		node_type *uparent = u->_parent;
		if (uparent == nullptr)
			_root = v;
		else if (u == uparent->_left)
			uparent->_left = v;
		else
			uparent->_right = v;
		if (!is_nil(v))
			v->_parent = uparent;
	}

	void rebalance_for_erase(node_type *node, node_type *parent) {
		node_type *brother;
		while (node != _root && is_black(node)) {
			if (parent->_left == node) {
				brother = parent->_right;
				if (brother->_color == RED) {
					brother->_color = BLACK;
					parent->_color = RED;
					rotate_left(parent);
					brother = parent->_right;
				}
				if (is_black(brother->_left) && is_black(brother->_right)) {
					brother->_color = RED;
					node = parent;
					parent = node->_parent;
				}
				else {
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
					node = _root;
					break;
				}
			}
			else {
				brother = parent->_left;
				if (brother->_color == RED) {
					brother->_color = BLACK;
					parent->_color = RED;
					rotate_right(parent);
					brother = parent->_left;
				}
				if (is_black(brother->_left) && is_black(brother->_right)) {
					brother->_color = RED;
					node = parent;
					parent = node->_parent;
				}
				else {
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
					node = _root;
					break;
				}
			}
		}
		if (!is_nil(node))
			node->_color = BLACK;
	}

	void erase(Key key) {
		node_type *node, *child, *parent, *temp;
		COLOR color;
		if ((temp = node = search(key)) == nullptr)
			return;
		if (is_nil(node->_left)) {
			color = node->_color;
			child = node->_right;
			parent = node->_parent;
			transplant(node, child);
		}
		else if (is_nil(node->_right)) {
			color = node->_color;
			child = node->_left;
			parent = node->_parent;
			transplant(node, child);
		}
		else {
			node = leftmost(node->_right);
			color = node->_color;
			child = node->_right;
			parent = node->_parent;
			if (node->_parent == temp)
				parent = node;
			else {
				transplant(node, child);
				node->_right = temp->_right;
				node->_right->_parent = node;
			}
			transplant(temp, node);
			node->_left = temp->_left;
			node->_left->_parent = node;
			node->_color = temp->_color;
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