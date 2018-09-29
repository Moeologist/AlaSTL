#include <ala/graph.h>
#include <iostream>

int main() {

	using namespace std;

	graph<int> Gi(10);

	Gi.V[0] = 0;
	Gi.V[1] = 1;
	Gi.V[2] = 2;
	Gi.V[3] = 3;
	Gi.V[4] = 4;
	Gi.V[5] = 5;

	Gi.E[0].push_front(1);
	Gi.E[0].push_front(2);
	Gi.E[0].push_front(3);

	Gi.E[1].push_front(0);
	Gi.E[1].push_front(5);

	Gi.E[2].push_front(0);
	Gi.E[2].push_front(4);

	Gi.E[3].push_front(0);
	Gi.E[3].push_front(4);

	Gi.E[4].push_front(2);
	Gi.E[4].push_front(3);
	Gi.E[4].push_front(5);

	Gi.E[5].push_front(1);
	Gi.E[5].push_front(1);

	Gi.bfs_path(0, 5);
	std::cout << std::endl;
	Gi.dfs_path(0, 4);
	return 0;
}