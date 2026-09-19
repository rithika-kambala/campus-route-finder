#include "dijkstra.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Return 1 for an integer, 0 for invalid input, -1 for end of input. */
static int read_integer(const char *prompt, int *value)
{
    char line[128], *end;
    long parsed;
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(line, sizeof line, stdin) == NULL) {
        return -1;
    }
    if (strchr(line, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        return 0;
    }
    errno = 0;
    parsed = strtol(line, &end, 10);
    if (end == line || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
        return 0;
    }
    while (isspace((unsigned char)*end)) {
        ++end;
    }
    if (*end != '\0') {
        return 0;
    }
    *value = (int)parsed;
    return 1;
}

/* Return zero when input ends, otherwise return to the menu. */
static int find_route(const Graph *graph)
{
    int source, destination, status, i;
    Route route;
    graph_display_locations(graph);
    status = read_integer("Enter starting location: ", &source);
    if (status == -1) {
        return 0;
    }
    if (status == 0 || !graph_is_valid_vertex(graph, source)) {
        puts("Invalid starting location.");
        return 1;
    }
    status = read_integer("Enter destination: ", &destination);
    if (status == -1) {
        return 0;
    }
    if (status == 0 || !graph_is_valid_vertex(graph, destination)) {
        puts("Invalid destination.");
        return 1;
    }
    status = dijkstra_shortest_route(graph, source, destination, &route);
    if (status == 0) {
        puts("No route connects these locations.");
        return 1;
    }
    if (status == -1) {
        puts("Invalid route request.");
        return 1;
    }
    printf("\nStarting Location: %s\nDestination: %s\n\nShortest Route:\n",
           graph_get_location_name(graph, source),
           graph_get_location_name(graph, destination));
    for (i = 0; i < route.length; ++i) {
        printf("%s%s", i == 0 ? "" : " -> ",
               graph_get_location_name(graph, route.vertices[i]));
    }
    printf("\nTotal Distance: %lld metres\n", route.distance);
    return 1;
}

int main(void)
{
    Graph graph;
    graph_init(&graph);
    if (!graph_load_campus(&graph, "data/campus.txt")) {
        fputs("Cannot load data/campus.txt. Run from the project directory and "
              "check the dataset format.\n", stderr);
        return EXIT_FAILURE;
    }
    for (;;) {
        int choice, status;
        puts("\n====================================\n"
             "       CAMPUS ROUTE FINDER\n"
             "====================================\n"
             "1. View Campus Locations\n"
             "2. Find Shortest Route\n"
             "3. View Campus Connections\n"
             "4. Exit");
        status = read_integer("Choice: ", &choice);
        if (status == -1) {
            break;
        }
        if (status == 0 || choice < 1 || choice > 4) {
            puts("Invalid menu option. Enter 1 to 4.");
            continue;
        }
        if (choice == 4) {
            break;
        }
        if (choice == 1) {
            graph_display_locations(&graph);
        } else if (choice == 3) {
            graph_display_locations(&graph);
            graph_display_matrix(&graph);
        } else if (!find_route(&graph)) {
            break;
        }
    }
    puts("Goodbye!");
    return EXIT_SUCCESS;
}
