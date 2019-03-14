#include "ala/detail/xorshift.h"
#include "ala/timer.h"
#include <iostream>
#include <iomanip>
#include <type_traits>

template <class T>
class Test;

using namespace std;
int main() {
	int v[] = {1, 1008611, 789456123};
	auto ss = 0ull;
	auto x1 = [&](auto &&n) {
		for (int x : v) {
			ala::xoshiro<uint32_t> x128(x);
			for (int i = 0; i < n; ++i)
				x128.next();
			ss += x128.plus();
		}
	};

	static_assert(std::is_same_v<unsigned long long, std::uint64_t>);

	// auto x2 = [&](auto &&n) {
	// 	for (int x : v) {
	// 		ala::xoshiro x256(x);
	// 		for (int i = 0; i < n; ++i)
	// 			x256.next();
	// 		ss += x256.plus();
	// 	}
	// };
	auto s = ala::timer(x1, 1000000);
	// auto s1 = ala::timer(x2, 1000000000);
	cout << s << endl;
	// cout << s1 << endl;
	cout << ss <<endl;
	cin >> *(new int);
	return 0;
}