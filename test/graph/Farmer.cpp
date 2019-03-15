#include <ala/graph.h>
#include <iostream>

class State {
public:
    int farmer, wolf, sheep, vege;

    int operator==(const State R) {
        if (R.farmer == farmer && R.wolf == wolf && R.sheep == sheep &&
            R.vege == vege)
            return 1;
        return 0;
    }

    friend std::ostream &operator<<(std::ostream &o, const State &s) {
        return (o << '(' << s.farmer << s.wolf << s.sheep << s.vege << ')');
    }
};

int Is_Safe(State S) {
    if (S.farmer != S.sheep && (S.wolf == S.sheep || S.sheep == S.vege))
        return 0;
    return 1;
}

int Is_Connect(graph<State> G, int u, int v) {
    //	if (abs(G.V[u].wolf - G.V[v].wolf) + abs(G.V[u].sheep - G.V[v].sheep) + abs(G.V[u].vege - G.V[v].vege) < 2)
    //		return 1;
    //	return 0;
    int i = 0;
    if (G.V[u].wolf != G.V[v].wolf)
        i++;
    if (G.V[u].sheep != G.V[v].sheep)
        i++;
    if (G.V[u].vege != G.V[v].vege)
        i++;
    if (G.V[u].farmer != G.V[v].farmer && i < 2)
        return 1;
    return 0;
}

graph<State> Creat() {
    graph<State> G(16);
    int F, W, S, V;
    int i = 0, j = 0;
    for (F = 0; F < 2; F++)
        for (W = 0; W < 2; W++)
            for (S = 0; S < 2; S++)
                for (V = 0; V < 2; V++) {
                    State st = {F, W, S, V};
                    if (Is_Safe(st)) {
                        G.V[i] = st;
                        i++;
                    }
                }
    for (i = 0; i < 12; i++)
        for (j = 0; j < 12; j++)
            if (Is_Connect(G, i, j)) {
                G.E[i].push_front(j);
            } else
                ;
    return G;
}

int main() {
    graph<State> G = Creat();
    auto u = G.search({0, 0, 0, 0});
    auto v = G.search({1, 1, 1, 1});
    G.bfs_path(u, v);
    std::cout << std::endl;
    G.dfs_path(u, v);
    std::cout << std::endl;

    std::cin >> u;
    return 0;
}