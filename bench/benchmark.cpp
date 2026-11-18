#include "../app/cabling.hpp"
#include "../src/structB/prim.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdexcept>

using namespace campus;
using Clock = std::chrono::steady_clock;

Graph sample(int n, std::size_t target, unsigned seed) {
    Graph graph{n, {}};
    graph.edges.reserve(target);
    std::mt19937 random(seed);
    std::set<std::pair<int, int>> links;
    auto add = [&](int u, int v) {
        links.emplace(u, v);
        graph.edges.push_back({u, v, static_cast<std::int64_t>(1 + random() % 1000000)});
    };
    for (int v = 1; v < n; ++v) {
        add(random() % v, v);
    }
    std::vector<std::pair<int, int>> remaining;
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (!links.count({u, v})) {
                remaining.emplace_back(u, v);
            }
        }
    }
    std::shuffle(remaining.begin(), remaining.end(), random);
    for (std::size_t i = 0; graph.edges.size() < target; ++i) {
        const auto [u, v] = remaining[i];
        graph.edges.push_back({u, v, static_cast<std::int64_t>(1 + random() % 1000000)});
    }
    std::shuffle(graph.edges.begin(), graph.edges.end(), random);
    return graph;
}

template<class Operation>
double timed(Operation operation) {
    const auto start = Clock::now();
    operation();
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

void measure(const Graph& graph, const char* family, unsigned seed) {
    Tree sorted = graph.edges;
    heapSort(sorted);
    constexpr int repeats = 5;
    for (int trial = 1; trial <= repeats; ++trial) {
        Tree a, b, compressed, ranked;
        std::size_t peak = 0;
        auto runA = [&] { a = kruskal(graph); };
        auto runB = [&] { b = prim(graph, adjacency(graph), &peak); };
        double aMs, bMs;
        if (trial % 2) {
            aMs = timed(runA);
            bMs = timed(runB);
        } else {
            bMs = timed(runB);
            aMs = timed(runA);
        }

        auto withCompression = [&] { compressed = scanSortedEdges(graph.vertices, sorted, true); };
        auto rankOnly = [&] { ranked = scanSortedEdges(graph.vertices, sorted, false); };
        double compressedMs, rankOnlyMs;
        if (trial % 2) {
            compressedMs = timed(withCompression);
            rankOnlyMs = timed(rankOnly);
        } else {
            rankOnlyMs = timed(rankOnly);
            compressedMs = timed(withCompression);
        }
        const auto weight = totalWeight(a);
        if (a.size() != static_cast<std::size_t>(graph.vertices - 1) ||
            a.size() != b.size() || b.size() != compressed.size() || b.size() != ranked.size() ||
            weight != totalWeight(b) || weight != totalWeight(compressed) ||
            weight != totalWeight(ranked)) {
            throw std::runtime_error("benchmark MST mismatch");
        }
        if (peak > graph.edges.size()) {
            throw std::runtime_error("heap exceeded the edge count");
        }
        std::cout << family << ',' << graph.vertices << ',' << graph.edges.size() << ',' << seed
                  << ',' << trial << ',' << aMs << ',' << bMs << ',' << compressedMs << ','
                  << rankOnlyMs << ',' << peak << ',' << decimal(weight) << '\n';
    }
}

int main() {
    std::cout << "family,n,m,seed,trial,kruskal_ms,prim_ms,compressed_ms,rank_only_ms,peak_heap,mst_weight\n"
              << std::fixed << std::setprecision(6);
    constexpr int n = 700;
    constexpr unsigned seed = 25304;
    for (int perVertex : {3, 6, 12, 25, 50, 100, 200}) {
        measure(sample(n, n * perVertex, seed), "density", seed);
    }
    measure(sample(n, n * (n - 1) / 2, seed), "density", seed);
    measure(geometricGraph(20000, 10000, 230, 253), "geometric", 253);
}
