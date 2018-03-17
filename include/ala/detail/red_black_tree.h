#include "ala/utility.h"

namespace detail {

template <class Key, class Val, class Compare = less<Key>>
struct red_black_tree {

	enum COLOR : bool {
		BLACK = false,
		RED = true
	};

	template <class Key1, class Val1>
	struct red_black_node {
		typedef Key1 key_type;
		typedef Val1 mapped_type;

		red_black_node(Key1 &key, Value1 &value, COLOR color = BLACK, red_black_node *l = nullptr, red_black_node *r = nullptr) : _pair(key, value), _left(l), _right(r), _color(color), _cmp() {}

		~red_black_node() { delete _left, _right; }

		key_type _key;
		mapped_type _val;
		COLOR _color;
		node *_parent, *_left, *_right;
	};

	typedef red_black_node<Key, Val> node_type;

	node_type *rotate_left(node_type *x) {
		node *y = x->_right;

		x->_right = y->_left;
		if (y->left != nullptr)
			y->left->_parent = x;

		y->_parent = x->_parent;
		if (x->_parent == nullptr)
			_root = y;
		else if (x == x->_parent->_left)
			x->_parent->_left = y;
		else
			x->_parent->_right = y;
		x->_parent = y;
		y->_left = x;
		return y;
	}

	node_type *rotate_right(node_type *y) {
		node *x = y->_left;

		y->_left = x->_right;
		if (x->_right != nullptr)
			x->_right->_parent = y;

		x->_parent = y->_parent;
		if (y->_parent == nullptr)
			_root = x;
		else if (y == y->_parent->_left)
			y->_parent->_left = x;
		else
			y->_parent->_right = x;
		y->_parent = x;
		x->_right = y;
		return x;
	}

	node_type *insert_or_assign(node_type *z) {
		node_type *x = _root, *y = nullptr, ret = z;
		while (x != nullptr) {
			y = x;
			if (_cmp(z->_key, x->_key))
				x = x->_left;
			else if (_cmp(x->_key, z->key))
				x = x->_right;
			else {
				x->_val = z->_val;
				return;
			}
		}
		z->_parent = y;
		if (y == nullptr)
			_root = z;
		else if (_cmp(z->_key, y->_key))
			y->_left = z;
		else
			y->_right = z;
		z->_color = RED;

		while (z->_parent->_color == RED) {
			if (z->_parent == z->_parent->_parent->_left) {
				y = z->_parent->_parent->_right;
				if (y->_color == RED) {
					z->_parent->_color = BLACK;
					y->_color = BLACK;
					Z->_parent->_parent->_color = RED;
					z = z->_parent->_parent;
				}
				else if (z == z->_parent->_right) {
					z = z->_parent;
					rotate_left(z)
				}
				z->_parent->_color=BLACK;
				z->_parent->_parent->_color=RED;
				rotate_right(z->_parent->_parent);
			}
			else {
				y = z->_parent->_parent->_left;
			}
		}
	}

	node<Key, Val> *_root;
	Compare _cmp;
} // red_black_tree

} // namespace detail