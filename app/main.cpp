#include "cabling.hpp"
#include "../src/structB/prim.hpp"

#include <charconv>
#include <iostream>
#include <stdexcept>
#include <string_view>

using namespace campus;

template<class Number>
Number parse(std::string_view text) {
    Number number{};
    auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), number);
    if (error != std::errc{} || end != text.data() + text.size()) {
        throw std::runtime_error("invalid numeric argument");
    }
    return number;
}

void validate(int vertices, int zones) {
    if (vertices < 2 || vertices > 20000 || zones < 2 || zones > vertices) {
        throw std::runtime_error("expected 2 <= k <= n <= 20000");
    }
}

Graph readGraph(int& zones) {
    int vertices;
    std::int64_t edges;
    if (!(std::cin >> vertices >> edges >> zones) || edges < 0) {
        throw std::runtime_error("expected n m k");
    }
    validate(vertices, zones);
    Graph graph{vertices, {}};
    for (std::int64_t i = 0; i < edges; ++i) {
        int u, v;
        std::int64_t weight;
        if (!(std::cin >> u >> v >> weight) || u < 1 || u > vertices || v < 1 || v > vertices) {
            throw std::runtime_error("invalid or missing edge");
        }
        graph.edges.push_back({u - 1, v - 1, weight});
    }
    return graph;
}

int main(int argc, char** argv) {
    try {
        if ((argc != 3 && argc != 9) || std::string_view(argv[1]) != "--structure") {
            throw std::runtime_error(
                "usage: ./run --structure A|B [--generate n k side radius seed]");
        }
        const std::string_view structure = argv[2];
        if (structure != "A" && structure != "B") {
            throw std::runtime_error("structure must be A or B");
        }
        int zones;
        Graph graph = [&] {
            if (argc == 3) {
                return readGraph(zones);
            }
            if (std::string_view(argv[3]) != "--generate") {
                throw std::runtime_error("expected --generate");
            }
            const int vertices = parse<int>(argv[4]);
            zones = parse<int>(argv[5]);
            validate(vertices, zones);
            const int side = parse<int>(argv[6]);
            const int radius = parse<int>(argv[7]);
            if (side < 1 || radius < 1) {
                throw std::runtime_error("side and radius must be positive");
            }
            return geometricGraph(vertices, side, radius, parse<std::uint64_t>(argv[8]));
        }();

        auto links = adjacency(graph);
        if (!connected(links)) {
            std::cout << "DISCONNECTED\n";
            return 0;
        }
        auto tree = structure == "A" ? kruskal(graph) : prim(graph, links);
        std::cout << decimal(totalWeight(tree)) << '\n'
                  << split(graph.vertices, std::move(tree), zones).spacing << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
