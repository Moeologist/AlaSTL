#include <cstring>
#include <memory>
#include "ala/detail/red_black_tree.h"
#include "ala/timer.h"
#include <functional>
#include <map>
#include <iostream>
// #include "EASTL/map.h"

ala::detail::red_black_tree<int, char> rb;
std::map<int, char> srb;

void test1() {
	for (int i = 0; i < 1000; ++i)
		srb.insert_or_assign(i, '0');
	for (int i = 0; i < 1000; i += 7)
		srb.erase(i);
	auto i=srb.end();
	srb.extract(srb.begin());
	++i;
	++i;
	++i;
	--------i;
}

void test() {
	for (int i = 0; i < 1000; ++i)
		rb.insert(i, '0');

	// for (int i = 0; i < 1000; i += 1)
	// 	rb.erase(i);

	// for (auto i = rb.begin(); i != rb.end(); ++i)
	// 	std::cout << (*i)._key << ",";

	for (auto i = --rb.end(); i != rb.begin(); --i)
		std::cout << (*i)._key << ",";
}

// void test_tree(auto x) {
// 	if (x->_left) {
// 		if (x->_left->_parent != x)
// 			return;
// 		test_tree(x->_left);
// 	}

// 	if (x->_right) {
// 		if (x->_right->_parent != x)
// 			return;
// 		test_tree(x->_right);
// 	}
// }

// void r(auto x) {
// 	if (x->_left)
// 		r(x->_left);
// 	std::cout << x->_key << ",";
// 	if (x->_right)
// 		r(x->_right);
// }

int main() {
	std::cout << ala::timer(test1);
	std::cout << ala::timer(test);
	auto x = rb._root;
	// r(x);
	return 0;
}