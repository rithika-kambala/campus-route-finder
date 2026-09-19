#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

typedef struct {
    int vertices[MAX_LOCATIONS];
    int length;
    long long distance;
} Route;

/* Returns 1 for a route, 0 if unreachable, -1 for invalid arguments.
 * On failure a non-NULL route is reset to length 0 and distance -1.
 * Equal-distance alternatives retain the first route found. */
int dijkstra_shortest_route(const Graph *graph, int source, int destination,
                            Route *route);

#endif
