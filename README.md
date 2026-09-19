# Campus Route Finder

## Problem Statement
Students and visitors need to find walking routes between campus locations.
Part 1 provides the campus graph and data. Part 2 will find shortest routes
and provide an interactive command-line interface.

## Data Structure
Each campus location is a vertex. Each walkable connection is an undirected
edge with a positive integer distance in metres.

## Why a Graph?
A graph models connections, alternative routes and journeys through intermediate
locations naturally. Walking distances can differ even when routes use the same
number of connections.

## Graph Representation
The graph uses a fixed 10-by-10 adjacency matrix, location names and a location
count. Active vertex indices are contiguous from 0 to count minus 1. An edge is
stored in both directions, so the matrix is symmetric. Zero means no edge,
including on the diagonal; self-loops are rejected. Disconnected graphs are valid.

Matrix space is O(V²), edge lookup/update is O(1), and scanning a vertex's
neighbours is O(V). This is simple to demonstrate for ten locations. Names use
fixed 64-byte buffers (at most 63 bytes plus the terminator).

## Planned Algorithm
Keith will implement Dijkstra's shortest-path algorithm in Part 2. There is no
shortest-path algorithm, route reconstruction or interactive CLI in Part 1.

## Project Structure
```text
campus-route-finder/
├── src/
│   └── graph.c
├── include/
│   └── graph.h
├── tests/
│   └── test_graph.c
├── data/
│   └── campus.txt
├── Makefile
├── README.md
└── .gitignore
```
Keith will add `src/dijkstra.c`, `include/dijkstra.h`, `src/main.c` and
`tests/test_routes.c` in Part 2.

## Building
Requires a C11 compiler and Make; no third-party libraries.
Run from the project directory:
```sh
make
make test
make clean
```
`make` compiles the graph object and graph test executable in `build/`.
Warnings use `-Wall -Wextra -pedantic`. Build outputs are ignored by Git.
There is no application to run yet. The optional `make app` target is prepared
for Keith's source/header files and will work once he supplies them. Keith will
also extend the test target to run his route tests.

## Running Tests
`make test` runs six assertion-based groups covering initialization, capacity,
name copying/lookup, edge insertion/replacement and symmetry, invalid indices,
no-edge values, NULL handling, dataset loading and malformed-file rejection.
Run tests without `-DNDEBUG`, which disables assertions. Temporary test data is
written inside `build/` and removed after a successful run.

## Campus Dataset
`data/campus.txt` is an illustrative campus, not a surveyed map. Format:
1. One integer location count (1–10).
2. That many nonempty, unique location names, one per line. Line order assigns IDs.
3. One integer undirected edge count.
4. That many lines of `from_id to_id distance_in_metres`.

Use positive integer distances, distinct valid endpoints and list each undirected
edge once. Comments and blank record lines are not supported. Trailing whitespace
after the records is allowed; Unix and Windows line endings are accepted. Names
are case-sensitive and whitespace in names is preserved. Malformed, duplicate,
truncated or out-of-range records fail without changing the destination graph.

The sample contains ten locations and seventeen edges. Main Gate–Library is
400 m directly, while Main Gate–Administration Block–Library totals 220 m.
Thus a route with more edges can be shorter. Main Gate and Hostel have no direct
edge, demonstrating the need for intermediate locations.

## Public Contract and Part 2 Handoff
Include `graph.h` and allocate `Graph graph;`, then call `graph_init(&graph)`.
Load the sample with `graph_load_campus(&graph, "data/campus.txt")` and check
its return value. That path is relative to the program's working directory.
The public structure enables stack allocation; clients should use the functions
instead of reading or writing its fields.

| Function | Result or purpose |
| --- | --- |
| `graph_init` | Reset all names, edges and the count |
| `graph_location_count` | Active vertex count |
| `graph_is_valid_vertex` | 1 for a valid index, otherwise 0 |
| `graph_add_location` | New vertex index, or -1 on failure |
| `graph_add_edge` | 1 on success, 0 on failure; replaces existing weight |
| `graph_get_weight` | Positive distance, 0 for no edge, -1 for invalid input |
| `graph_get_location_name` | Borrowed name pointer, or NULL |
| `graph_find_location` | Exact name match's index, or -1 |
| `graph_display_locations` | Print IDs and names to stdout |
| `graph_display_matrix` | Print the labelled matrix to stdout |
| `graph_load_campus` | 1 on success, 0 on failure; replaces graph on success |

Keith can scan candidate neighbours from zero to `graph_location_count(g) - 1`
and read weights using `graph_get_weight(g, u, v)`. Only positive values represent
edges. `MAX_LOCATIONS` is available for fixed-size algorithm arrays. This supplies
everything Dijkstra needs without changing `graph.c` or coupling it to the
algorithm. Validate endpoints first; handle unreachable vertices in Part 2.
Distances may reach `INT_MAX` per edge: Keith should check for overflow when
adding path distances and choose an appropriate distance type/sentinel.
Borrowed name pointers remain owned by the graph and may change after reloading
or reinitialization. Do not modify or free them.

## Team Responsibilities
- **Developer 1:** Graph data structure and operations, campus dataset, loading,
  build foundation and graph unit tests.
- **Developer 2 (Keith):** Dijkstra, route reconstruction, interactive CLI,
  route-specific tests and final integration.

Suggested Git commit message: `feat: implement campus weighted graph foundation`
