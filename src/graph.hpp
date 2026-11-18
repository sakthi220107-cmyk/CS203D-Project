#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace campus {
struct Edge {
    int u;
    int v;
    std::int64_t weight;
};

using Tree = std::vector<Edge>;

struct Graph {
    int vertices;
    std::vector<Edge> edges;
};

struct Arc {
    int to;
    std::size_t edge;
};

using Adjacency = std::vector<std::vector<Arc>>;

inline Adjacency adjacency(const Graph& graph) {
    Adjacency result(graph.vertices);
    for (std::size_t i = 0; i < graph.edges.size(); ++i) {
        const auto& [u, v, weight] = graph.edges[i];
        result[u].push_back({v, i});
        result[v].push_back({u, i});
    }
    return result;
}
}
