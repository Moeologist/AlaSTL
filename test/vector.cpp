#include <cstdio>
#include <cassert>
#include <cmath>
#include <amath/vector.hpp>
typedef amath::vector<float, 3> Vec;
typedef amath::vector<float, 2> Vec2;
int main() {
	float ax[1024*3];
	sqrt(8);
	float ar[] = {1, 2, 3};
	Vec a(ar);
	const Vec b=Vec({3, 4, 5});
	auto c = b;
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
	auto k = Vec2({3, 4}).norm();
	assert(k == Vec2({0.6, 0.8}));
	float x[] = {1, 0, 0}, y[] = {0, 1, 0};
	c = amath::cross(Vec(x), Vec(y));
	auto xx = amath::dot(Vec({1,2,3}), Vec({4,5,6}));
	return 0;
}