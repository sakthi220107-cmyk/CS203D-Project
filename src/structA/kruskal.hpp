#pragma once

#include "../graph.hpp"

#include <algorithm>
#include <numeric>
#include <utility>

namespace campus {
class UnionFind {
    std::vector<int> parent_;
    std::vector<int> rank_;
    bool compress_;

public:
    explicit UnionFind(int size, bool compress = true)
        : parent_(size), rank_(size), compress_(compress) {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    int find(int vertex) {
        int root = vertex;
        while (root != parent_[root]) {
            root = parent_[root];
        }
        if (compress_) {
            while (vertex != root) {
                int next = parent_[vertex];
                parent_[vertex] = root;
                vertex = next;
            }
        }
        return root;
    }

    bool join(int u, int v) {
        u = find(u);
        v = find(v);
        if (u == v) {
            return false;
        }
        if (rank_[u] < rank_[v]) {
            std::swap(u, v);
        }
        parent_[v] = u;
        if (rank_[u] == rank_[v]) {
            ++rank_[u];
        }
        return true;
    }
};

inline void siftMax(Tree& edges, std::size_t root, std::size_t count) {
    while (root < count / 2) {
        auto child = 2 * root + 1;
        if (child + 1 < count && edges[child].weight < edges[child + 1].weight) {
            ++child;
        }
        if (edges[root].weight >= edges[child].weight) {
            break;
        }
        std::swap(edges[root], edges[child]);
        root = child;
    }
}

inline void heapSort(Tree& edges) {
    for (auto i = edges.size() / 2; i > 0; --i) {
        siftMax(edges, i - 1, edges.size());
    }
    for (auto count = edges.size(); count > 1; --count) {
        std::swap(edges.front(), edges[count - 1]);
        siftMax(edges, 0, count - 1);
    }
}

inline Tree scanSortedEdges(int vertices, const Tree& edges, bool compress = true) {
    UnionFind components(vertices, compress);
    Tree tree;
    tree.reserve(vertices - 1);
    for (const auto& edge : edges) {
        if (components.join(edge.u, edge.v)) {
            tree.push_back(edge);
            if (tree.size() == static_cast<std::size_t>(vertices - 1)) {
                break;
            }
        }
    }
    return tree;
}

inline Tree kruskal(const Graph& graph) {
    auto edges = graph.edges;
    heapSort(edges);
    return scanSortedEdges(graph.vertices, edges);
}
}
