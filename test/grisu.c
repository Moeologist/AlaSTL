#include <stdio.h>
#include <stdlib.h>
#include <grisu2.h>
int main(void)
{
	char s[32];
	emyg_dtoa(1.2,s);
	return 0;
}