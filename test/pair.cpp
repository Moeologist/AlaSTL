#include <iostream>
#include <ala/detail/pair.h>
using namespace std;

template<class Data>
struct rb_node {
    Data _data;
    rb_node *_left, *_right, *_parent;
    bool _nil_type;
    bool _is_nil;
    bool _is_construct;
    bool _color;
};

struct big {
    int p[1024];
};

int main() {
    cout << sizeof(big) << endl;
    cout << sizeof(pair<int,big>) << endl;
    cout << sizeof(rb_node<pair<int,big>>) << endl;
    return 0;
}
