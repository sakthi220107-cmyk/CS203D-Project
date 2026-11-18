#include "../app/cabling.hpp"
#include "../src/structB/prim.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <tuple>

using namespace campus;

void certify(const Graph& graph, const Tree& tree, int k) {
    const int n = graph.vertices;
    assert(tree.size() == static_cast<std::size_t>(n - 1));
    using Key = std::tuple<int, int, std::int64_t>;
    auto key = [](const Edge& e) { return Key{std::min(e.u, e.v), std::max(e.u, e.v), e.weight}; };
    std::multiset<Key> available;
    for (const auto& e : graph.edges) {
        available.insert(key(e));
    }
    for (const auto& e : tree) {
        auto found = available.find(key(e));
        assert(found != available.end());
        available.erase(found);
    }
    const auto treeLinks = adjacency({n, tree});
    assert(connected(treeLinks));

    // Compute the maximum weight on each tree path, independently of either MST routine.
    std::vector<std::vector<std::int64_t>> maximum(n, std::vector<std::int64_t>(n));
    for (int start = 0; start < n; ++start) {
        std::vector<bool> visited(n);
        std::queue<int> queue;
        maximum[start][start] = std::numeric_limits<std::int64_t>::min();
        visited[start] = true;
        queue.push(start);
        while (!queue.empty()) {
            int u = queue.front();
            queue.pop();
            for (const auto& arc : treeLinks[u]) {
                if (!visited[arc.to]) {
                    visited[arc.to] = true;
                    maximum[start][arc.to] = std::max(maximum[start][u], tree[arc.edge].weight);
                    queue.push(arc.to);
                }
            }
        }
    }
    for (const auto& [u, v, weight] : available) {
        assert(weight >= maximum[u][v]);
    }

    const auto zones = split(n, tree, k);
    assert(static_cast<int>(std::set<int>(zones.labels.begin(), zones.labels.end()).size()) == k);
    auto spacing = std::numeric_limits<std::int64_t>::max();
    for (const auto& e : graph.edges) {
        if (zones.labels[e.u] != zones.labels[e.v]) {
            spacing = std::min(spacing, e.weight);
        }
    }
    assert(spacing == zones.spacing);

    // At any strictly larger spacing, all edges at or below this spacing must
    // be internal to zones. Count the resulting forced groups independently.
    Graph forced{n, {}};
    for (const auto& e : graph.edges) {
        if (e.weight <= spacing) {
            forced.edges.push_back(e);
        }
    }
    const auto links = adjacency(forced);
    std::vector<bool> seen(n);
    int groups = 0;
    for (int vertex = 0; vertex < n; ++vertex) {
        if (seen[vertex]) {
            continue;
        }
        ++groups;
        std::vector<int> queue{vertex};
        seen[vertex] = true;
        for (std::size_t i = 0; i < queue.size(); ++i) {
            for (const auto& arc : links[queue[i]]) {
                if (!seen[arc.to]) {
                    seen[arc.to] = true;
                    queue.push_back(arc.to);
                }
            }
        }
    }
    assert(groups < k);
}

void check(const Graph& graph, int k) {
    const auto links = adjacency(graph);
    const auto a = kruskal(graph);
    const auto b = prim(graph, links);
    const auto size = static_cast<std::size_t>(graph.vertices - 1);
    const bool isConnected = connected(links);
    assert((a.size() == size) == isConnected);
    assert((b.size() == size) == isConnected);
    if (isConnected) {
        assert(totalWeight(a) == totalWeight(b));
        assert(split(graph.vertices, a, k).spacing == split(graph.vertices, b, k).spacing);
        certify(graph, a, k);
        certify(graph, b, k);
    }
}

int main() {
    Tree edges{{0, 1, 4}, {0, 1, -3}, {0, 1, 8}, {0, 1, 4}, {0, 1, 0}};
    heapSort(edges);
    assert(std::is_sorted(edges.begin(), edges.end(),
                          [](const Edge& a, const Edge& b) { return a.weight < b.weight; }));
    MinHeap heap;
    for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
        heap.push({it->weight, 0, 0});
    }
    for (const auto& e : edges) {
        assert(heap.pop().weight == e.weight);
    }
    assert(heap.empty());

    for (bool compress : {false, true}) {
        UnionFind components(64, compress);
        for (int stride = 1; stride < 64; stride *= 2) {
            for (int i = 0; i < 64; i += 2 * stride) {
                assert(components.join(i, i + stride));
            }
        }
        for (int i = 1; i < 64; ++i) {
            assert(components.find(0) == components.find(i));
            assert(!components.join(0, i));
        }
    }

    const auto low = std::numeric_limits<std::int64_t>::min();
    check({3, {{0, 0, low}, {0, 1, low}, {0, 1, -1}, {1, 2, low}, {0, 2, 0}}}, 2);
    check({2, {}}, 2);
    Graph dense{200, {}};
    for (int u = 0; u < dense.vertices; ++u) {
        for (int v = u + 1; v < dense.vertices; ++v) {
            dense.edges.push_back({u, v, 1});
        }
    }
    check(dense, 100);

    constexpr unsigned seed = 25304;
    std::mt19937 random(seed);
    int connectedCases = 0;
    for (int trial = 0; trial < 1200; ++trial) {
        const int n = 2 + random() % 49;
        const int k = 2 + random() % (n - 1);
        const unsigned density = random() % 75;
        auto weight = [&] { return static_cast<int>(random() % 19) - 9; };
        Graph graph{n, {}};
        const bool plantTree = trial % 3 != 0;
        if (plantTree) {
            for (int i = 1; i < n; ++i) {
                graph.edges.push_back({i, static_cast<int>(random() % i), weight()});
            }
        }
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (random() % 100 < density && (plantTree || i != 0)) {
                    graph.edges.push_back({i, j, weight()});
                }
            }
        }
        std::shuffle(graph.edges.begin(), graph.edges.end(), random);
        check(graph, k);
        connectedCases += connected(adjacency(graph));
    }
    std::cout << "1200 differential cases (" << connectedCases
              << " connected), seed " << seed << "\n";
}
