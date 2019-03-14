#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

template<class T>
struct graph {
    typedef T value_type;
    typedef size_t index_type;
    typedef int weight_type;
    typedef std::forward_list<index_type> edges_type;
    typedef std::forward_list<weight_type> edges_weight_type;
    typedef std::vector<value_type> nodes_type;
    typedef std::vector<edges_type> nodes_edges_type;
    typedef std::vector<edges_weight_type> nodes_edges_weight_type;

    index_type N;
    nodes_type V;
    nodes_edges_type E;
    nodes_edges_weight_type EW;

    std::vector<bool> visited;
    std::vector<index_type> path;
    std::vector<std::vector<index_type>> paths;

    graph(index_type order)
        : N(order), V(order), E(order), EW(order), visited(order, false),
          path(order), paths() {}

    graph(index_type order, T data[]): graph(order) {
        for (value_type &i : V)
            i = data[i];
    }

    void init() {
        std::fill(visited.begin(), visited.end(), false);
        path.clear();
        paths.clear();
    }

    index_type search(T t) {
        for (int i = 0; i < N; ++i)
            if (t == V[i])
                return i;
        return -1;
    }

    void print_paths() {
        for (auto &&path : paths) {
            for (index_type i : path)
                std::cout << V[i] << ',';
            std::cout << '\n';
        }
    }

    void dfs(index_type u, index_type v) {
        visited[u] = true;
        path.push_back(u);
        if (u == v) {
            paths.push_back(path);
            return;
        }
        for (index_type i : E[u])
            if (!visited[i]) {
                dfs(i, v);
                visited[i] = false;
                path.pop_back();
            }
    }

    void dfs_path(index_type u, index_type v) {
        init();
        dfs(u, v);
        print_paths();
    }

    void b(index_type u, index_type v) {
        std::list<index_type> Q;
        Q.push_back(u);
        int cur_count = 1, next_count = 0, path_length = 0;
        while (!Q.empty()) {
            u = Q.front();
            Q.pop_front();
            visited[u] = true;
            path.push_back(u);
            for (index_type i : E[u])
                if (!visited[i]) {
                    if (i == v) {
                        paths.emplace_back(path);
                        return;
                    } else {
                        Q.push_back(i);
                        ++next_count;
                    }
                }
        }
    }

    void bfsn(index_type u, index_type v) {
        std::list<index_type> cur, next;
        visited[u] = true;
        cur.push_back(u);
        path.emplace_back();
        while (!cur.empty()) {
            u = cur.front();
            cur.pop_front();
            *path.rbegin() = u;
            if (u == v) {
                paths.push_back(path);
                return;
            }
            for (index_type i : E[u])
                if (!visited[i]) {
                    visited[i] = true;
                    next.push_back(i);
                }
            if (cur.empty()) {
                cur.swap(next);
                path.emplace_back();
            }
        }
    }

    void bfs_path(index_type u, index_type v) {
        init();
        bfsn(u, v);
        print_paths();
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