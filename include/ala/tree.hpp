#include "ala/utility.hpp"

template <class T>
struct treenode {
	T data;
	treenode *L, *R;

	treenode()
	    : data(), L(nullptr), R(nullptr) {}

	treenode(T _data, treenode *_L = nullptr, treenode *_R = nullptr)
	    : data(_data), L(_L), R(_R) {}

	treenode(const treenode &tn) : data(tn.data) {
		if (tn.L)
			L = new treenode(tn.L);
		else
			L = nullptr;
		if (tn.R)
			R = new treenode(tn.R);
		else
			R = nullptr;
	}

	treenode(treenode &&tn) : data(tn.data), L(tn.L), R(tn.R) {
	}

	~treenode() {
		delete L, R, Pa;
	}

	treenode<T> &operator=(const treenode<T> &tn) {
		data = tn.data;
		if (tn.L)
			L = new treenode(tn.L);
		else
			L = nullptr;
		if (tn.R)
			R = new treenode(tn.R);
		else
			R = nullptr;
	}

	treenode<T> &operator=(treenode<T> &&tn) {
		data = move(tn.data);
		L = tn.L;
		R = tn.R;
	}
};

template <class T>
treenode<T> *build_tree(T *s, int length, const T &nullkey, int i = 0) {
	if (i >= length || s[i] == nullkey)
		return nullptr;
	treenode<T> *p = new treenode<T>(s[i]);
	p->L = build_tree(s, length, nullkey, 2 * i + 1);
	p->R = build_tree(s, length, nullkey, 2 * i + 2);
	return p;
}

template <class T>
void preorder(treenode<T> *p) {
	if (p) {
		std::cout << p->data << ',';
		preorder(p->L);
		preorder(p->R);
	}
}

template <class T>
void inorder(treenode<T> *p) {
	if (p) {
		inorder(p->L);
		std::cout << p->data << ',';
		inorder(p->R);
	}
}

template <class T>
void postorder(treenode<T> *p) {
	if (p) {
		postorder(p->L);
		postorder(p->R);
		std::cout << p->data << ',';
	}
}

template <class T>
void hierarchyorder(treenode<T> *p, int h) {
	if (!p || h < 0)
		return;
	if (h == 0) {
		std::cout << p->data << ',';
		return;
	}
	hierarchyorder(p->L, h - 1);
	hierarchyorder(p->R, h - 1);
}

template <class T>
int count(treenode<T> *p) {
	if (p) {
		return count(p->L) + count(p->R) + 1;
	}
	return 0;
}

template <class T>
int height(treenode<T> *p) {
	if (p) {
		return (height(p->L) > height(p->R) ? height(p->L) + 1 : height(p->R) + 1);
	}
	return 0;
}

template <class T>
void hiecount(treenode<T> *p, int H, int num[]) {
	if (p) {
		num[H]++;
		hiecount(p->L, H + 1, num);
		hiecount(p->R, H + 1, num);
	}
}