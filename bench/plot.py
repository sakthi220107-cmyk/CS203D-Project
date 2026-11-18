import csv
import statistics
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


output = Path("results")
with (output / "timings.csv").open() as source:
    rows = list(csv.DictReader(source))

density = defaultdict(list)
for row in rows:
    if row["family"] == "density":
        density[int(row["m"])].append(row)

n = int(next(iter(density.values()))[0]["n"])
edge_counts = sorted(density)
x = [m / n for m in edge_counts]


def means(field):
    return [statistics.mean(float(row[field]) for row in density[m]) for m in edge_counts]


plots = [
    ("mst-time", "MST algorithm time (ms)", [
        ("kruskal_ms", "Kruskal"), ("prim_ms", "Prim"),
    ]),
    ("union-find", "Sorted edge scan time (ms)", [
        ("compressed_ms", "Rank + compression"), ("rank_only_ms", "Rank only"),
    ]),
    ("prim-heap", "Maximum heap entries (count)", [("peak_heap", "Prim heap")]),
]

for name, ylabel, fields in plots:
    fig, ax = plt.subplots(figsize=(7.2, 3.6), layout="constrained")
    for field, label in fields:
        ax.plot(x, means(field), marker="o", label=label)
    if name == "prim-heap":
        ax.plot(x, edge_counts, linestyle="--", label="Edge-count bound")
    ax.set(xscale="log", yscale="log", xlabel="Edges per vertex (m/n)", ylabel=ylabel)
    ax.set_title(f"Campus cabling · {n} vertices · five trials per density")
    ax.grid(alpha=0.25, which="both")
    ax.legend()
    fig.savefig(output / f"{name}.png", dpi=170)
    plt.close(fig)
