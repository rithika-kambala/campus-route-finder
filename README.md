# Campus Route Finder

## Problem Statement
Students and visitors need the shortest walking route between campus locations.
This terminal application calculates a route and its total distance from a small
campus graph instead of relying on hardcoded routes.

## Objectives
- Model campus locations and walking connections using a graph.
- Find minimum-distance routes and display each intermediate location.
- Handle invalid input, disconnected locations and same-location requests.

## Data Structure Used
A weighted, undirected graph represents the campus. Vertices are locations,
edges are walking connections, and positive integer weights are metres.
The graph stores names, a location count and an adjacency matrix. Each undirected
edge occupies two symmetric matrix cells. Zero means no edge; the diagonal is
zero and self-loops are rejected. The fixed capacity is ten locations, with names
up to 63 bytes. A graph naturally represents alternative and intermediate routes.

## Algorithm Used
Dijkstra uses three arrays: `distance[]` for the best known distance,
`visited[]` for settled vertices, and `previous[]` for predecessors.
It starts the source at zero and other distances at infinity. Each iteration
selects the nearest unvisited vertex by scanning the array, then relaxes its
edges: a shorter candidate updates both distance and predecessor. It stops when
the destination is settled or no reachable unvisited vertex remains.
Reconstruction follows predecessors backwards and reverses the resulting list.
Equal-distance alternatives retain the first route found.

## Why Dijkstra?
The task requires shortest paths in a weighted graph with non-negative weights.
Our dataset uses strictly positive weights, which meet Dijkstra's requirement.
Fewest edges does not necessarily mean shortest walking distance.

## Workflow
User → Select Source & Destination → Campus Graph → Dijkstra Algorithm →
Distance + Previous Arrays → Path Reconstruction → Shortest Route + Distance

## Project Structure
```text
campus-route-finder/
├── src/
│   ├── graph.c
│   ├── dijkstra.c
│   └── main.c
├── include/
│   ├── graph.h
│   └── dijkstra.h
├── tests/
│   ├── test_graph.c
│   └── test_routes.c
├── data/
│   └── campus.txt
├── Makefile
├── README.md
└── .gitignore
```

## Compilation
Requires a C11 compiler and Make, with no third-party libraries.
Run from the project directory:
```sh
make
```
Warnings are enabled with `-Wall -Wextra -pedantic`. `make clean` removes
all generated objects and executables. Binaries are ignored by Git.

## Execution
```sh
./campus_route_finder
```
Menu: **1** lists locations, **2** finds a route, **3** displays the adjacency
matrix, **4** exits. Enter location IDs from 0 to 9. Invalid input returns to
the menu; end-of-input exits cleanly. Run from the project directory so the
application can load `data/campus.txt`; a missing or malformed dataset produces
an error and nonzero exit status.

Example: choose `2`, enter source `0`, destination `2`:
```text
Starting Location: Main Gate
Destination: Library

Shortest Route:
Main Gate -> Administration Block -> Library
Total Distance: 220 metres
```
Other examples:
- `0` to `9`: Main Gate → Parking Area, **80 metres**.
- `0` to `6`: Main Gate → Parking Area → Medical Centre → Hostel, **370 metres**.
- `2` to `2`: Library, **0 metres**.

## Testing
```sh
make test
```
Runs all six original graph test groups and the route tests. Route coverage
includes direct and multi-hop routes, an indirect route shorter than a direct
edge, reverse routes, identical endpoints, unreachable vertices, invalid inputs,
equal-distance alternatives, sums beyond `INT_MAX`, and the actual campus data.
Use assertions enabled (do not pass `-DNDEBUG`).

## Complexity
- Dijkstra: **O(V²)** time, using linear minimum selection and neighbour scans.
- Graph storage: **O(V²)** for the adjacency matrix.
- Algorithm arrays and output route: **O(V)** extra space.
- Path reconstruction: **O(V)** time.
The fixed limit is ten vertices; these describe scaling with vertex count.

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

## Module Contract
`graph.h` remains the Part 1 contract and its implementation is unchanged.
Initialize with `graph_init`, load with `graph_load_campus`, and inspect through
`graph_location_count`, `graph_is_valid_vertex`, `graph_get_weight` and
`graph_get_location_name`. Edge lookup returns positive metres, zero for no edge,
and -1 for invalid input. Name pointers belong to the graph.

`dijkstra_shortest_route(graph, source, destination, &route)` returns:
- `1`: success; `route.vertices`, `route.length` and `route.distance` hold the result.
- `0`: destination unreachable.
- `-1`: invalid graph/endpoints or NULL output pointer.

A non-NULL output is reset to length zero and distance -1 on failure.
Distances use `long long` and addition is checked for overflow.

## Team Contributions
- **Rithika / Part 1:** Graph structure and API, campus dataset and loader,
  original graph tests and build foundation (`rithika/graph-foundation`).
- **Keith / Part 2 scope:** Dijkstra, reconstruction, terminal CLI, route tests
  and integration (`keith/route-finder`). This scope was implemented with Codex
  at Rithika's request; the commits retain Rithika's configured author identity.

## Limitations
This is an illustrative ten-location campus, not a surveyed map. It models
symmetric walking distances only, with no travel times, accessibility constraints
or live updates. The CLI selects locations by numeric ID. Negative and zero-weight
edges are unsupported; disconnected destinations report that no route exists.
