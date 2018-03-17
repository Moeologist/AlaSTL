#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>

int main(void)
{
	// generate 32M random numbers serially
	thrust::host_vector<int> h_vec(10000);
	std::generate(h_vec.begin(), h_vec.end(), rand);

	//std::sort(h_vec.begin(), h_vec.end());

	// transfer data to the device
	thrust::device_vector<int> d_vec = h_vec;

	// sort data on the device (846M keys per second on GeForce GTX 480)
	thrust::sort(d_vec.begin(), d_vec.end());

	// transfer data back to host
	thrust::copy(d_vec.begin(), d_vec.end(), h_vec.begin());

	std::cout << h_vec.size() << ", " << std::is_sorted(h_vec.begin(), h_vec.end());

	return 0;
}