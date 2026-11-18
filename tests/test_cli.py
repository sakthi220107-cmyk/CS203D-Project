import subprocess


def run(structure, data="", *options):
    return subprocess.run(
        ["./run", "--structure", structure, *options],
        input=data,
        text=True,
        capture_output=True,
        check=True,
    ).stdout


cases = [
    ("4 5 3\n1 2 1\n2 3 2\n3 4 4\n1 4 8\n2 4 5\n", "7\n2\n"),
    ("3 1 2\n1 2 1\n", "DISCONNECTED\n"),
    ("2 1 2\n1 2 -2\n", "-2\n-2\n"),
    (
        "3 2 3\n1 2 -9223372036854775808\n2 3 -9223372036854775808\n",
        "-18446744073709551616\n-9223372036854775808\n",
    ),
]
for algorithm in ("A", "B"):
    for data, expected in cases:
        assert run(algorithm, data) == expected
    assert run(algorithm, "", "--generate", "4", "3", "1", "1", "253") == "0\n0\n"

large = ("--generate", "20000", "8", "10000", "230", "253")
reference = run("A", "", *large)
assert reference == run("B", "", *large)
assert reference != "DISCONNECTED\n"
print("CLI checks passed, including a connected 20,000-building graph")
