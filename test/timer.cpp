#include <mach/mach_time.h>
#include <unistd.h>
#include <iostream>
#include <ala/timer.h>

int main() {
	// std::cout<<"shit";
	std::cout<<ala::timer(usleep,1000)<<std::endl;
	// while(true)std::cout<<"fuc";
	return 0;
}