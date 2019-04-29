#include <utility>
#include <ala/utility.h>
using namespace ala;

template<class ...T>
void dummy(T...a) {

}

template<size_t... Is>
void ok(int *i, index_sequence<Is...>) {
    (i[Is] = 0)...;
}

int main() {
    int i [4];
    ok(i, make_index_sequence<4>());
    return 0;
}
