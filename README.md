# Project 4 — Campus network cabling with minimum spanning trees

CS 203D Data Structures and Algorithms end-term project. Provisional team name: Campus. Replace it with the registered team name and fill in this roster before submission.

| Member | Roll number | GitHub | Role | Question |
| --- | --- | --- | --- | --- |
| Pending | Pending | Pending | Structure A owner | Q1 |
| Pending | Pending | Pending | Structure B owner | Q2 |
| Pending | Pending | Pending | Application owner | Q3 |
| Pending | Pending | Pending | Test owner | Q4 |
| Pending | Pending | Pending | Measurement owner | Q5 |
| If applicable | Pending | Pending | Sixth member | Q6 |

## Requirements

Linux, GNU Make, and GCC with C++17 and signed 128-bit integer support. Verified with GCC 14.2. Tests use Python 3. Benchmark figures additionally require Matplotlib (`python3 -m pip install matplotlib` in your Python environment).

```sh
make             # Build
make test        # Run unit, certificate, differential and CLI tests
make bench       # Regenerate results/timings.csv and all three PNG figures
```

## Run on an input file

```sh
./run --structure A < example.txt
./run --structure B < example.txt
```

Example `example.txt`:

```text
4 5 3
1 2 1
2 3 2
3 4 4
1 4 8
2 4 5
```

Expected output from both algorithms:

```text
7
2
```

The header is `n m k` with `2 <= k <= n <= 20000`. Each following line contains a candidate undirected edge `u v w`, with 1-indexed endpoints and a signed 64-bit integer weight. A connected graph produces two lines, MST cost and maximum zone spacing. A disconnected graph produces only `DISCONNECTED`. Zone membership is computed internally to preserve the specified output format.

## Generate a graph and run the 20,000-building demo

```sh
./run --structure A --generate 20000 8 10000 230 253
./run --structure B --generate 20000 8 10000 230 253
```

Arguments are `n k side radius seed`. The generator samples integer coordinates uniformly from `[0, side)` on both axes using the seed. It creates every edge at distance at most the radius and rounds Euclidean edge lengths up to the nearest integer. With the tested compiler and standard library, both demo commands print `931804` and `115`.

## Known limitations

- Time and memory grow with the number of candidate edges. At 20,000 vertices the verified example is a geometric graph with 325,806 edges. Density measurements through a complete graph use 700 vertices.
- Reproducing the exact generated graph from a seed can depend on the C++ standard library implementation. File inputs are independent of the generator.
- Team names, individual contributions, verified author emails and the instructor's GitHub username must come from the team. The assignment PDF supplies placeholders and contains no checkpoint dates.

See [REPORT.md](REPORT.md), [REPORT.pdf](REPORT.pdf), [presentation](docs/slides.pdf), and [tool declaration](TOOLS.md).
