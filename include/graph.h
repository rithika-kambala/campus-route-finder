#ifndef GRAPH_H
#define GRAPH_H

#define MAX_LOCATIONS 10
#define MAX_LOCATION_NAME 64
#define GRAPH_NO_EDGE 0
#define GRAPH_INVALID_WEIGHT (-1)

/* Storage is visible to allow simple stack allocation. Use the functions below
 * to read or change it so that the undirected graph stays consistent. */
typedef struct {
    int weights[MAX_LOCATIONS][MAX_LOCATIONS];
    char names[MAX_LOCATIONS][MAX_LOCATION_NAME];
    int count;
} Graph;

/* Initialize before use. NULL is accepted and ignored. */
void graph_init(Graph *graph);
/* NULL graph has zero locations. */
int graph_location_count(const Graph *graph);
int graph_is_valid_vertex(const Graph *graph, int vertex);
/* Return the new index, or -1 for NULL/empty/duplicate/too-long names or a full
 * graph. Names are copied; matching is exact and case-sensitive. */
int graph_add_location(Graph *graph, const char *name);
/* Return 1 on success, 0 on invalid vertices, self-loops or nonpositive weight.
 * Adding an existing edge replaces its weight in both directions. */
int graph_add_edge(Graph *graph, int from, int to, int weight);
/* Positive metres = edge, 0 = no edge (including diagonal), -1 = invalid index
 * or NULL graph. Dijkstra must consider only weights greater than zero. */
int graph_get_weight(const Graph *graph, int from, int to);
/* Return borrowed name storage, or NULL if invalid. Do not modify/free it. */
const char *graph_get_location_name(const Graph *graph, int vertex);
/* Return index or -1 if absent/invalid. */
int graph_find_location(const Graph *graph, const char *name);
/* Print to stdout; NULL graphs are ignored. */
void graph_display_locations(const Graph *graph);
void graph_display_matrix(const Graph *graph);

/* Load data/campus.txt format. Return 1 on success, 0 on failure.
 * Success replaces the graph; failure leaves it unchanged. */
int graph_load_campus(Graph *graph, const char *path);

#endif
