#pragma once

#include "../src/structA/kruskal.hpp"

#include <cmath>
#include <map>
#include <random>
#include <string>

namespace campus {
inline bool connected(const Adjacency& links) {
    std::vector<int> queue{0};
    std::vector<bool> seen(links.size());
    seen[0] = true;
    for (std::size_t i = 0; i < queue.size(); ++i) {
        for (const auto& arc : links[queue[i]]) {
            if (!seen[arc.to]) {
                seen[arc.to] = true;
                queue.push_back(arc.to);
            }
        }
    }
    return queue.size() == links.size();
}

// GCC and Clang provide this signed type. A sum of 19,999 int64 weights fits.
using Total = __int128;

inline Total totalWeight(const Tree& tree) {
    Total total = 0;
    for (const auto& edge : tree) {
        total += edge.weight;
    }
    return total;
}

inline std::string decimal(Total value) {
    const bool negative = value < 0;
    if (negative) {
        value = -value;
    }
    std::string digits;
    do {
        digits += static_cast<char>('0' + value % 10);
        value /= 10;
    } while (value > 0);
    if (negative) {
        digits += '-';
    }
    std::reverse(digits.begin(), digits.end());
    return digits;
}

struct Zones {
    std::vector<int> labels;
    std::int64_t spacing;
};

inline Zones split(int vertices, Tree tree, int zones) {
    heapSort(tree);
    const int kept = vertices - zones;
    UnionFind components(vertices);
    for (int i = 0; i < kept; ++i) {
        components.join(tree[i].u, tree[i].v);
    }
    std::vector<int> labels(vertices);
    for (int i = 0; i < vertices; ++i) {
        labels[i] = components.find(i);
    }
    return {std::move(labels), tree[kept].weight};
}

// A radius-wide grid limits candidate comparisons to neighboring cells.
inline Graph geometricGraph(int vertices, int side, int radius, std::uint64_t seed) {
    using Point = std::pair<int, int>;
    Graph graph{vertices, {}};
    std::vector<Point> points;
    std::map<Point, std::vector<int>> cells;
    std::mt19937_64 engine(seed);
    std::uniform_int_distribution<int> coordinate(0, side - 1);
    const auto limit = std::int64_t{radius} * radius;

    for (int i = 0; i < vertices; ++i) {
        const int x = coordinate(engine);
        const int y = coordinate(engine);
        points.emplace_back(x, y);
        const Point cell{x / radius, y / radius};
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                auto nearby = cells.find({cell.first + dx, cell.second + dy});
                if (nearby == cells.end()) {
                    continue;
                }
                for (int j : nearby->second) {
                    const auto deltaX = std::int64_t{x} - points[j].first;
                    const auto deltaY = std::int64_t{y} - points[j].second;
                    const auto distance = deltaX * deltaX + deltaY * deltaY;
                    if (distance <= limit) {
                        auto weight = static_cast<std::int64_t>(std::sqrt(
                            static_cast<long double>(distance)));
                        if (weight * weight < distance) {
                            ++weight;
                        }
                        graph.edges.push_back({j, i, weight});
                    }
                }
            }
        }
        cells[cell].push_back(i);
    }
    return graph;
}
}
