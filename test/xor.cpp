// #include "xorshift.hpp"
#include <iostream>
#include <iomanip>
#include <config/external.hpp>
using namespace std;
int main()
{
	xoroshiro128plus x128(7485674);
	xorshift1024star x1024(54545);
	while(true)
	cout<<setprecision(128)<<rand_double(x128())<<endl<<rand_double(x1024())<<endl;
	return 0;
}