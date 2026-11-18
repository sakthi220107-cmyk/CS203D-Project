CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wshadow
HEADERS := $(wildcard src/*.hpp src/*/*.hpp app/*.hpp)

.PHONY: all test bench
all: build/cabling

build:
	mkdir -p build

build/cabling: app/main.cpp $(HEADERS) | build
	$(CXX) $(CXXFLAGS) $< -o $@

build/test_mst: tests/test_mst.cpp $(HEADERS) | build
	$(CXX) $(CXXFLAGS) $< -o $@

build/benchmark: bench/benchmark.cpp $(HEADERS) | build
	$(CXX) $(CXXFLAGS) $< -o $@

test: all build/test_mst
	./build/test_mst
	python3 tests/test_cli.py

bench: build/benchmark
	./build/benchmark > results/timings.csv
	python3 bench/plot.py
