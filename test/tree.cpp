#include "include/BinTree.h"
#include <cstring>
#include <memory>
using namespace std;

struct A{
	int buildkey;
	char c;
	A(int I,char C):buildkey(I),c(C){}
};

int main()
{
	A ar[]={{2,3},{2,3},{1,1}};
	TreeNode<A> *tree=Build(ar,3);
	return 0;
}