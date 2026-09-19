#include "graph.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void graph_init(Graph *graph)
{
    if (graph != NULL) {
        *graph = (Graph){0};
    }
}

int graph_location_count(const Graph *graph)
{
    return graph != NULL ? graph->count : 0;
}

int graph_is_valid_vertex(const Graph *graph, int vertex)
{
    return graph != NULL && vertex >= 0 && vertex < graph->count;
}

int graph_find_location(const Graph *graph, const char *name)
{
    int i;
    if (graph == NULL || name == NULL) {
        return -1;
    }
    for (i = 0; i < graph->count; ++i) {
        if (strcmp(graph->names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

int graph_add_location(Graph *graph, const char *name)
{
    int index;
    if (graph == NULL || name == NULL || name[0] == '\0' ||
        strlen(name) >= MAX_LOCATION_NAME ||
        graph->count >= MAX_LOCATIONS || graph_find_location(graph, name) >= 0) {
        return -1;
    }
    index = graph->count;
    strcpy(graph->names[index], name);
    ++graph->count;
    return index;
}

int graph_add_edge(Graph *graph, int from, int to, int weight)
{
    if (!graph_is_valid_vertex(graph, from) ||
        !graph_is_valid_vertex(graph, to) || from == to || weight <= 0) {
        return 0;
    }
    /* Each undirected edge occupies two symmetric matrix cells. */
    graph->weights[from][to] = weight;
    graph->weights[to][from] = weight;
    return 1;
}

int graph_get_weight(const Graph *graph, int from, int to)
{
    if (!graph_is_valid_vertex(graph, from) ||
        !graph_is_valid_vertex(graph, to)) {
        return GRAPH_INVALID_WEIGHT;
    }
    return graph->weights[from][to];
}

const char *graph_get_location_name(const Graph *graph, int vertex)
{
    return graph_is_valid_vertex(graph, vertex) ? graph->names[vertex] : NULL;
}

void graph_display_locations(const Graph *graph)
{
    int i;
    for (i = 0; i < graph_location_count(graph); ++i) {
        printf("%d - %s\n", i, graph_get_location_name(graph, i));
    }
}

void graph_display_matrix(const Graph *graph)
{
    int row, column;
    if (graph == NULL) {
        return;
    }
    printf("Adjacency matrix (metres; 0 = no edge)\n%12s", "");
    for (column = 0; column < graph->count; ++column) {
        printf("%12d", column);
    }
    putchar('\n');
    for (row = 0; row < graph->count; ++row) {
        printf("%12d", row);
        for (column = 0; column < graph->count; ++column) {
            printf("%12d", graph_get_weight(graph, row, column));
        }
        putchar('\n');
    }
}

/* Read one complete line, accepting Unix and Windows line endings. */
static int read_line(FILE *file, char *line, size_t size)
{
    size_t length;
    if (fgets(line, (int)size, file) == NULL) {
        return 0;
    }
    length = strlen(line);
    if (length > 0 && line[length - 1] == '\n') {
        line[--length] = '\0';
    } else if (!feof(file)) {
        return 0;
    }
    if (length > 0 && line[length - 1] == '\r') {
        line[length - 1] = '\0';
    }
    return 1;
}

/* strtol permits checking overflow before converting to an int. */
static int read_numbers(const char *line, int *values, int count)
{
    int i;
    for (i = 0; i < count; ++i) {
        char *end;
        long value;
        errno = 0;
        value = strtol(line, &end, 10);
        if (end == line || errno == ERANGE || value < 0 || value > INT_MAX) {
            return 0;
        }
        values[i] = (int)value;
        line = end;
        if (i < count - 1 && !isspace((unsigned char)*line)) {
            return 0;
        }
    }
    while (isspace((unsigned char)*line)) {
        ++line;
    }
    return *line == '\0';
}

static int read_campus(FILE *file, Graph *graph)
{
    char line[256];
    int locations, edges, i;
    if (!read_line(file, line, sizeof line) ||
        !read_numbers(line, &locations, 1) ||
        locations < 1 || locations > MAX_LOCATIONS) {
        return 0;
    }
    for (i = 0; i < locations; ++i) {
        if (!read_line(file, line, sizeof line) ||
            graph_add_location(graph, line) < 0) {
            return 0;
        }
    }
    if (!read_line(file, line, sizeof line) ||
        !read_numbers(line, &edges, 1) ||
        edges > locations * (locations - 1) / 2) {
        return 0;
    }
    for (i = 0; i < edges; ++i) {
        int edge[3];
        if (!read_line(file, line, sizeof line) ||
            !read_numbers(line, edge, 3) ||
            graph_get_weight(graph, edge[0], edge[1]) != GRAPH_NO_EDGE ||
            !graph_add_edge(graph, edge[0], edge[1], edge[2])) {
            return 0;
        }
    }
    /* After the declared records, permit only whitespace. */
    while ((i = fgetc(file)) != EOF) {
        if (!isspace((unsigned char)i)) {
            return 0;
        }
    }
    return !ferror(file);
}

int graph_load_campus(Graph *graph, const char *path)
{
    FILE *file;
    Graph loaded;
    int success;
    if (graph == NULL || path == NULL) {
        return 0;
    }
    file = fopen(path, "r");
    if (file == NULL) {
        return 0;
    }
    graph_init(&loaded);
    success = read_campus(file, &loaded);
    if (fclose(file) != 0) {
        success = 0;
    }
    if (success) {
        *graph = loaded;
    }
    return success;
}
