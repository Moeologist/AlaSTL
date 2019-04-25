#include <cassert>
#include <cmath>
#include <cstdio>
#include <ala/La/Vector.h>
typedef ala::La::Vector<float, 3> Vec;
typedef ala::La::Vector<float, 2> Vec2;

int main() {
    ala::La::Vector ax{1., 2, 3};
    Vec a{1, 2, 3};
    const Vec b = Vec{3, 4, 5};

    assert(b + a == Vec({4, 6, 8}));
    assert(b - a == Vec({2, 2, 2}));
    assert(b * a == Vec({3, 8, 15}));
    assert(b / a == Vec({3., 2., 5. / 3}));

    assert(2 + a == Vec({3, 4, 5}));
    assert(a + 2 == Vec({3, 4, 5}));
    assert(2 - a == Vec({1, 0, -1}));
    assert(a - 2 == Vec({-1, 0, 1}));
    assert(2 * a == Vec({2, 4, 6}));
    assert(a * 2 == Vec({2, 4, 6}));
    assert(2 / a == Vec({2., 1., 2. / 3}));
    assert(a / 2 == Vec({1. / 2, 1., 3. / 2}));

    assert(a + b == b + a);
    assert(a - b == 0 - (b - a));
    assert(a * b == b * a);
    // assert(a/b == 1/(b/a));

    auto c = b;

    auto k = Vec2{3, 4}.normlized();
    assert((k == Vec2{0.6, 0.8}));

    c = ala::La::cross(Vec{1, 0, 0}, Vec{0, 1, 0});
    auto xx = ala::La::dot(Vec{1, 2, 3}, Vec{4, 5, 6});

    ala::La::Vector<double, 3> xd = c.to_type<double>();
    // auto xd = ala::La::vector<double ,3>{1,2,3};
    // Vec xf = xd;
    auto kl = ala::La::make_vector<double>(1, 2, 3);
    return 0;
}