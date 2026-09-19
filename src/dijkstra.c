#include "dijkstra.h"

#include <limits.h>
#include <stddef.h>

int dijkstra_shortest_route(const Graph *graph, int source, int destination,
                            Route *route)
{
    long long distance[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS] = {0};
    int previous[MAX_LOCATIONS];
    int count = graph_location_count(graph);
    int i, step;

    if (route == NULL) {
        return -1;
    }
    *route = (Route){ .length = 0, .distance = -1 };
    if (!graph_is_valid_vertex(graph, source) ||
        !graph_is_valid_vertex(graph, destination)) {
        return -1;
    }
    for (i = 0; i < count; ++i) {
        distance[i] = LLONG_MAX;
        previous[i] = -1;
    }
    distance[source] = 0;

    for (step = 0; step < count; ++step) {
        int current = -1;
        /* Select the nearest unsettled vertex using a linear scan. */
        for (i = 0; i < count; ++i) {
            if (!visited[i] && distance[i] != LLONG_MAX &&
                (current == -1 || distance[i] < distance[current])) {
                current = i;
            }
        }
        if (current == -1) {
            break; /* Remaining vertices cannot be reached from the source. */
        }
        visited[current] = 1;
        if (current == destination) {
            break;
        }
        for (i = 0; i < count; ++i) {
            int weight = graph_get_weight(graph, current, i);
            if (!visited[i] && weight > 0 &&
                distance[current] <= LLONG_MAX - weight) {
                long long candidate = distance[current] + weight;
                /* Relax the edge and remember how this shorter route arrived. */
                if (candidate < distance[i]) {
                    distance[i] = candidate;
                    previous[i] = current;
                }
            }
        }
    }
    if (distance[destination] == LLONG_MAX) {
        return 0;
    }
    /* Predecessors lead backwards from destination; reverse them for display. */
    for (i = destination; i != -1; i = previous[i]) {
        route->vertices[route->length++] = i;
    }
    for (i = 0; i < route->length / 2; ++i) {
        int last = route->length - 1 - i;
        int temporary = route->vertices[i];
        route->vertices[i] = route->vertices[last];
        route->vertices[last] = temporary;
    }
    route->distance = distance[destination];
    return 1;
}
