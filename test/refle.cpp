#include <iostream>
#include <ala/reflection.h>

using namespace std;

struct C {
	C() { cout << "C con\n"; }
};

REFLECTOR(C);

int main() {
	C *ptr = (C *)creat_instance("C");
	return 0;
}