#pragma once

#include "../graph.hpp"

#include <algorithm>
#include <utility>

namespace campus {
struct Candidate {
    std::int64_t weight;
    int to;
    std::size_t edge;
};

class MinHeap {
    std::vector<Candidate> entries_;

public:
    bool empty() const { return entries_.empty(); }
    std::size_t size() const { return entries_.size(); }

    void push(Candidate entry) {
        entries_.push_back(entry);
        auto child = entries_.size() - 1;
        while (child > 0) {
            const auto parent = (child - 1) / 2;
            if (entries_[parent].weight <= entries_[child].weight) {
                break;
            }
            std::swap(entries_[parent], entries_[child]);
            child = parent;
        }
    }

    Candidate pop() {
        Candidate result = entries_.front();
        entries_.front() = entries_.back();
        entries_.pop_back();
        std::size_t root = 0;
        while (root < entries_.size() / 2) {
            auto child = 2 * root + 1;
            if (child + 1 < entries_.size() &&
                entries_[child + 1].weight < entries_[child].weight) {
                ++child;
            }
            if (entries_[root].weight <= entries_[child].weight) {
                break;
            }
            std::swap(entries_[root], entries_[child]);
            root = child;
        }
        return result;
    }
};

inline Tree prim(const Graph& graph, const Adjacency& links, std::size_t* peak = nullptr) {
    MinHeap candidates;
    std::vector<bool> visited(graph.vertices);
    Tree tree;
    tree.reserve(graph.vertices - 1);
    std::size_t highWater = 0;

    auto visit = [&](int u) {
        visited[u] = true;
        for (const auto& arc : links[u]) {
            if (!visited[arc.to]) {
                candidates.push({graph.edges[arc.edge].weight, arc.to, arc.edge});
            }
        }
        highWater = std::max(highWater, candidates.size());
    };

    visit(0);
    while (!candidates.empty() && tree.size() < static_cast<std::size_t>(graph.vertices - 1)) {
        const auto next = candidates.pop();
        if (visited[next.to]) {
            continue;
        }
        tree.push_back(graph.edges[next.edge]);
        if (tree.size() < static_cast<std::size_t>(graph.vertices - 1)) {
            visit(next.to);
        }
    }
    if (peak) {
        *peak = highWater;
    }
    return tree;
}
}
