#include <ala/functional.h>
#include <ala/vector.h>
#include <functional>
#include <vector>

ala::reference_wrapper<int> purr();
int main() {
    auto x = true ? purr() : 0; // error, ambiguous: ICS exists from int prvalue to
        // reference_wrapper<int> and from reference_wrapper<int> to int

    using t = std::common_type_t<ala::reference_wrapper<int>, int>;
    std::vector<int> v;
    ala::vector<int> v1;

    std::ref(v1);
    return 0;
}