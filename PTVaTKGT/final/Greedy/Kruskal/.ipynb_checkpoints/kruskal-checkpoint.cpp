#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <functional>
using namespace std;

struct Edge {
    int u, v, w;
    bool removed;
    Edge(int u, int v, int w) : u(u), v(v), w(w), removed(false) {}
};

class DSU {
    vector<int> parent, rank;
public:
    DSU(int n) {
        parent.resize(n + 1);
        rank.resize(n + 1, 0);
        for (int i = 0; i <= n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }

    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;

        if (rank[px] < rank[py]) swap(px, py);
        parent[py] = px;
        if (rank[px] == rank[py]) rank[px]++;
        return true;
    }

    vector<int> findCycle(int u, int v, vector<Edge>& mst) {
        vector<vector<int>> adj(parent.size());
        for (auto& e : mst) {
            adj[e.u].push_back(e.v);
            adj[e.v].push_back(e.u);
        }

        vector<int> parentNode(adj.size(), -1);
        vector<bool> visited(adj.size(), false);

        function<bool(int, int, int)> dfs = [&](int node, int target, int par) -> bool {
            visited[node] = true;

            if (node == target) {
                return true;
            }

            for (int next : adj[node]) {
                if (next != par && !visited[next]) {
                    parentNode[next] = node;
                    if (dfs(next, target, node)) return true;
                }
            }

            return false;
        };

        dfs(u, v, -1);

        vector<int> path;
        int curr = v;
        while (curr != -1) {
            path.push_back(curr);
            if (curr == u) break;
            curr = parentNode[curr];
        }

        return path;
    }
};

void kruskal(vector<Edge>& edges, int n) {
    DSU dsu(n);
    vector<Edge> mst;
    int totalWeight = 0;

    cout << "Đồ thị ban đầu:\n";
    cout << "Số đỉnh: " << n << "\n";
    cout << "Số cạnh: " << edges.size() << "\n\n";

    cout << "Cạnh: E = {";
    for (int i = 0; i < edges.size(); i++) {
        cout << "(" << edges[i].u << ";" << edges[i].v << ":" << edges[i].w << ")";
        if (i < edges.size() - 1) cout << ", ";
    }
    cout << "}\n\n";

    sort(edges.begin(), edges.end(), [](Edge& a, Edge& b) {
        return a.w < b.w;
    });

    cout << "Sau khi sắp xếp: E = {";
    for (int i = 0; i < edges.size(); i++) {
        cout << "(" << edges[i].u << ";" << edges[i].v << ":" << edges[i].w << ")";
        if (i < edges.size() - 1) cout << ", ";
    }
    cout << "}\n\n";

    cout << "Trọng số: ";
    vector<int> weights;
    for (auto& e : edges) {
        if (find(weights.begin(), weights.end(), e.w) == weights.end())
            weights.push_back(e.w);
    }
    sort(weights.begin(), weights.end());
    for (int i = 0; i < weights.size(); i++) {
        cout << setw(4) << weights[i];
    }
    cout << "\n\n";

    cout << "Các bước:\n\n";

    int step = 0;
    for (int idx = 0; idx < edges.size(); idx++) {
        auto& e = edges[idx];
        if (e.removed) continue;

        step++;
        cout << "Bước " << step << ":\n\n";
        cout << "|T| = " << mst.size() << " <= " << (n-1) << " và E<> Ø\n\n";

        if (dsu.find(e.u) != dsu.find(e.v)) {
            dsu.unite(e.u, e.v);
            mst.push_back(e);
            e.removed = true;

            cout << "E = E\\(" << e.u << ";" << e.v << ") = {";
            bool first = true;
            for (auto& edge : edges) {
                if (edge.removed) continue;
                if (!first) cout << ", ";
                cout << "(" << edge.u << ";" << edge.v << ":" << edge.w << ")";
                first = false;
            }
            cout << "}\n\n";

            cout << "T = {";
            for (int i = 0; i < mst.size(); i++) {
                cout << "(" << mst[i].u << ";" << mst[i].v << ":" << mst[i].w << ")";
                if (i < mst.size() - 1) cout << ", ";
            }
            cout << "}\n\n";

            totalWeight += e.w;
        } else {
            e.removed = true;

            cout << "E = E\\(" << e.u << ";" << e.v << ") = {";
            bool first = true;
            for (auto& edge : edges) {
                if (edge.removed) continue;
                if (!first) cout << ", ";
                cout << "(" << edge.u << ";" << edge.v << ":" << edge.w << ")";
                first = false;
            }
            cout << "}\n\n";

            // Tìm chu trình
            vector<int> cycle = dsu.findCycle(e.u, e.v, mst);
            cout << "Vì cạnh (" << e.u << ";" << e.v << ") tạo thành chu trình ";

            int start = -1;
            for (int i = 1; i < cycle.size() - 1; i++) {
                if (cycle[i] != e.u && cycle[i] != e.v) {
                    start = i;
                    break;
                }
            }

            if (start == -1) start = 0;

            vector<int> orderedCycle;
            for (int i = start; i < cycle.size(); i++) {
                orderedCycle.push_back(cycle[i]);
            }
            for (int i = 0; i < start; i++) {
                orderedCycle.push_back(cycle[i]);
            }

            for (int i = 0; i < orderedCycle.size() - 1; i++) {
                int u1 = orderedCycle[i];
                int v1 = orderedCycle[i+1];
                int weight = 0;
                for (auto& edge : mst) {
                    if ((edge.u == u1 && edge.v == v1) || (edge.u == v1 && edge.v == u1)) {
                        weight = edge.w;
                        break;
                    }
                }
                cout << "(" << u1 << ";" << v1 << ":" << weight << ") ";
            }
            cout << "(" << orderedCycle[orderedCycle.size()-1] << ";" << orderedCycle[0] << ":" << e.w << ")";
            cout << " nên\n\n";

            cout << "T = {";
            for (int i = 0; i < mst.size(); i++) {
                cout << "(" << mst[i].u << ";" << mst[i].v << ":" << mst[i].w << ")";
                if (i < mst.size() - 1) cout << ", ";
            }
            cout << "}\n\n";
        }
    }

    step++;
    cout << "Bước " << step << ":\n\n";

    bool hasEdge = false;
    for (auto& edge : edges) {
        if (!edge.removed) {
            hasEdge = true;
            break;
        }
    }

    if (mst.size() == n - 1 && !hasEdge) {
        cout << "|T| = " << (n-1) << " <= " << (n-1) << " và E = Ø\n\n";
        cout << "Thuật toán dừng.\n\n";
    } else if (mst.size() == n - 1) {
        cout << "|T| = " << (n-1) << " = " << (n-1) << "\n\n";
        cout << "Thuật toán dừng.\n\n";
    } else {
        cout << "E = Ø\n\n";
        cout << "Thuật toán dừng.\n\n";
    }

    cout << "\nKết quả cây khung nhỏ nhất là:\n\n";
    cout << "T = {";
    for (int i = 0; i < mst.size(); i++) {
        cout << "(" << mst[i].u << ";" << mst[i].v << ":" << mst[i].w << ")";
        if (i < mst.size() - 1) cout << ", ";
    }
    cout << "}\n\n";
    cout << "Với tổng trọng lượng: ";
    for (int i = 0; i < mst.size(); i++) {
        cout << mst[i].w;
        if (i < mst.size() - 1) cout << " + ";
    }
    cout << " = " << totalWeight << "\n";
}

int main() {
    ifstream fin("input.txt");

    if (!fin) {
        cout << "Không thể mở file input.txt!\n";
        return 1;
    }

    int n, m;
    fin >> n >> m;

    vector<Edge> edges;
    for (int i = 0; i < m; i++) {
        int u, v, w;
        fin >> u >> v >> w;
        edges.push_back(Edge(u, v, w));
    }

    fin.close();

    kruskal(edges, n);

    return 0;
}