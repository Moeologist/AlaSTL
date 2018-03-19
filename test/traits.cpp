#include <cstring>
#include <memory>
#include "ala/detail/red_black_tree.h"
#include "ala/timer.h"
#include <type_traits>
#include <functional>
#include <map>
#include <iostream>


ala::detail::red_black_tree<int, char> rb;
std::map<int, char> srb;
void test1() {
	for (int i = 0; i < 1000; ++i)
		srb.insert_or_assign(i, '0');
}
void test() {
	for (int i = 0; i < 1000; ++i)
		rb.insert(i, '0');
	auto x = rb.search(8);
	for (int i = 0; i < 1000; i += 7)
		rb.erase(i);
}

//void test_tree(auto x) {
//	if (x->_left) {
//		if (x->_left->_parent != x)
//			return;
//		test_tree(x->_left);
//	}
//
//	if (x->_right) {
//		if (x->_right->_parent != x)
//			return;
//		test_tree(x->_right);
//	}
//}

int main() {
	std::cout << ala::timer(test1);
	std::cout << ala::timer(test);
	auto x = rb._root;
	//test_tree(x);
	return 0;
}