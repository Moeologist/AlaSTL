#include <list>
#include <vector>
#include <iostream>

template <class T, class MatrixType = short>
struct amgraph {
	int N;
	T *V;
	MatrixType **E;

	bool *visited;
	int *path;
	int path_n;
	int g_temp;

	amgraph(int order) : N(order) {
		V = new T[order];
		E = new MatrixType *[order];
		for (int i = 0; i < order; ++i)
			E[i] = new MatrixType[order];
		for (int i = 0; i < order; ++i)
			for (int j = 0; j < order; ++j)
				E[i][j] = 0;
		visited = new bool[order];
		path = new int[order];
	}

	amgraph(int order, T data[]) : amgraph(order) {
		for (int i = 0; i < order; ++i)
			V[i] = data[i];
	}

	amgraph(int order, T data[],int edges,MatrixType ) : amgraph(order) {
		for (int i = 0; i < order; ++i)
			V[i] = data[i];
	}

	void init() {
		for (int i = 0; i < N; ++i)
			visited[i] = false;
		for (int i = 0; i < N; ++i)
			path[i] = -1;
		path_n = g_temp = 0;
	}

	int search(T t) {
		for (int i = 0; i < N; ++i)
			if (t == V[i])
				return i;
		return -1;
	}

	void print_path() {
		for (int i = 0; path[i] != -1 && i < N; ++i)
			std::cout << V[path[i]] << ',';
		std::cout << '\n';
	}

	void dfs(int u, int v) {
		visited[u] = true;
		path[g_temp++] = u;
		if (u == v) {
			print_path();
			++path_n;
			return;
		}
		for (int i = 0; i < N; ++i)
			if (!visited[i] && E[u][i]) {
				dfs(i, v);
				visited[i] = false;
				path[--g_temp] = -1;
			}
	}

	void dfs_path(int u, int v) {
		init();
		dfs(u, v);
	}

	void bfs(int u, int v) {
		std::list<int> Q;
		Q.push_back(u);
		int cur_count = 1, next_count = 0;
		while (!Q.empty()) {
			u = Q.front();
			Q.pop_front();
			path[g_temp] = u;
			visited[u] = true;
			for (int i = 0; i < N; ++i)
				if (!visited[i] && E[u][i]) {
					Q.push_back(i);
					++next_count;
					if (i == v)
						return;
				}
			--cur_count;
			Q.push_back(-1);
			if (cur_count == 0) {
				++g_temp;
				cur_count = next_count;
				next_count = 0;
			}
		}
	}

	void bfsn(int u, int v) {
		std::list<int> Q;
		Q.push_back(u);
		int cur_count = 1, next_count = 0;
		while (!Q.empty()) {
			u = Q.front();
			Q.pop_front();
			path[g_temp] = u;
			visited[u] = true;
			for (int i = 0; i < N; ++i)
				if (!visited[i] && E[u][i]) {
					if (i == v) {
						path[++g_temp] = v;
						return;
					}
					else {
						Q.push_back(i);
						++next_count;
					}
				}
			if (--cur_count == 0) {
				++g_temp;
				cur_count = next_count;
				next_count = 0;
			}
		}
	}

	void bfs_path(int u, int v) {
		init();
		bfsn(u, v);
		print_path();
	}

	// int min(int costs[]) {
	// 	int min_V = -1;
	// 	for (int i = 0; i < N;++i)
	// 		if (E[v][i] != 0 && visited[i] == 0 && (E[v][i] < E[v][min_V] || min_V == -1))
	// 			min_V = i;
	// 	return min_V;
	// }

	// void dijkstra(int v) {
	// 	int distance[N];
	// 	for (int i = 0; i < N;++i)
	// 		distance[i] = E[v][i];

	// 	while (int node = min(u); node != -1; node = min(u)) {
	// 		for (int i = 0;++i; i < N)
	// 			if (E[node][i] != 0) {
	// 				new_cost = costs[node] + E[node][i];
	// 				if (costs[i] > new_cost) {
	// 					costs[i] = new_cost;
	// 					path[node] = i;
	// 				}
	// 			}
	// 		visited[node] = true;
	// 	}
	// }
};