#include <cstring>
#include <memory>
#include "ala/detail/rb_tree.h"
#include "ala/utility.h"
#include "ala/timer.h"
#include <functional>
#include <map>
#include <iostream>

enum COLOR {
	BLACK = 0,
	RED = 1,
	NIL = 2
};

struct C {
	int x[100];
	C() = default;
	~C() {
		std::cout << "fuck";
	};
	C(const char &) {
		std::cout << "fuck";
	}
};

struct rb_node {
	COLOR _color;
	C c;
	rb_node *_left, *_right, *_parent;
};

template <class value_type>
struct value_compare {
  public:
	bool operator()(const value_type &x, const value_type &y) const {
		return x.first < y.first;
	}
};

ala::detail::rb_tree<ala::pair<const int, char>,value_compare<ala::pair<const int, char>> > rb;
std::map<int, C> srb;

void test1() {
	for (int i = 0; i < 1000; ++i)
		srb[i] = '0';
	for (int i = 0; i < 1000; i += 7)
		srb.erase(i);
	auto i = srb.end();
}

void test() {
	for (int i = 0; i < 1000; ++i)
		rb.insert(ala::pair<int const,char>(i, '0'));

	for (int i = 0; i < 1000; i += 7)
		rb.erase(i);

	for (int i = 0; i < 1000; i += 1)
		rb.search(i);

	// for (auto i = rb.begin(); i != rb.end(); ++i)
	// 	std::cout << (*i)._key << ",";

	// for (auto i = --rb.end(); i != rb.begin(); --i)
	// 	std::cout << (*i)._key << ",";
}

int main() {
	std::cout << sizeof(bool) << sizeof(rb_node) << sizeof(COLOR);
	std::cout << ala::timer(test1);
	std::cout << ala::timer(test);
	// auto x = rb._root;
	// r(x);
	return 0;
}