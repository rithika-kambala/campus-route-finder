#include "dijkstra.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

static void check_route(const Graph *graph, int from, int to,
                        const int *expected, int length, long long distance)
{
    Route route;
    int i;
    long long sum = 0;
    assert(dijkstra_shortest_route(graph, from, to, &route) == 1);
    assert(route.length == length);
    assert(route.distance == distance);
    for (i = 0; i < length; ++i) {
        assert(route.vertices[i] == expected[i]);
        if (i > 0) {
            int weight = graph_get_weight(graph, route.vertices[i - 1], route.vertices[i]);
            assert(weight > 0);
            sum += weight;
        }
    }
    assert(sum == route.distance);
}

int main(void)
{
    Graph graph;
    Route route;
    const int direct[] = {0, 1};
    const int multi[] = {0, 1, 2};
    const int same[] = {0};
    const int reverse[] = {2, 1, 0};
    const int campus[] = {0, 9, 8, 6};
    graph_init(&graph);
    assert(graph_add_location(&graph, "A") == 0);
    assert(graph_add_location(&graph, "B") == 1);
    assert(graph_add_location(&graph, "C") == 2);
    assert(graph_add_location(&graph, "Isolated") == 3);
    assert(graph_add_edge(&graph, 0, 1, 10));
    assert(graph_add_edge(&graph, 1, 2, 20));
    check_route(&graph, 0, 1, direct, 2, 10);
    check_route(&graph, 0, 2, multi, 3, 30);
    assert(graph_add_edge(&graph, 0, 2, 80));
    check_route(&graph, 0, 2, multi, 3, 30);
    check_route(&graph, 2, 0, reverse, 3, 30);
    check_route(&graph, 0, 0, same, 1, 0);
    assert(dijkstra_shortest_route(&graph, 0, 3, &route) == 0);
    assert(route.length == 0 && route.distance == -1);
    assert(dijkstra_shortest_route(&graph, -1, 0, &route) == -1);
    assert(dijkstra_shortest_route(&graph, 0, 4, &route) == -1);
    assert(dijkstra_shortest_route(&graph, 4, 0, &route) == -1);
    assert(dijkstra_shortest_route(&graph, 0, -1, &route) == -1);
    assert(dijkstra_shortest_route(NULL, 0, 0, &route) == -1);
    assert(dijkstra_shortest_route(&graph, 0, 1, NULL) == -1);
    assert(route.length == 0 && route.distance == -1);
    /* A tied direct route remains valid; no special tie-breaking is required. */
    assert(graph_add_edge(&graph, 0, 2, 30));
    assert(dijkstra_shortest_route(&graph, 0, 2, &route) == 1);
    assert(route.distance == 30);
    graph_init(&graph);
    assert(dijkstra_shortest_route(&graph, 0, 0, &route) == -1);
    assert(graph_add_location(&graph, "A") == 0);
    assert(graph_add_location(&graph, "B") == 1);
    assert(graph_add_location(&graph, "C") == 2);
    assert(graph_add_edge(&graph, 0, 1, INT_MAX));
    assert(graph_add_edge(&graph, 1, 2, INT_MAX));
    check_route(&graph, 0, 2, multi, 3, 2LL * INT_MAX);
    assert(graph_load_campus(&graph, "data/campus.txt"));
    check_route(&graph, 0, 2, multi, 3, 220);
    check_route(&graph, 0, 6, campus, 4, 370);
    puts("PASS: route tests (direct, multi-hop, alternatives, same vertex, "
         "unreachable, invalid inputs, ties, large distances, campus data).");
    return 0;
}
