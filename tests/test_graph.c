#include "graph.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static void test_initialization(void)
{
    Graph graph;
    int i, j;
    graph_init(&graph);
    assert(graph_location_count(&graph) == 0);
    assert(!graph_is_valid_vertex(&graph, 0));
    for (i = 0; i < MAX_LOCATIONS; ++i) {
        char name[16];
        snprintf(name, sizeof name, "Location %d", i);
        assert(graph_add_location(&graph, name) == i);
        for (j = 0; j <= i; ++j) {
            assert(graph_get_weight(&graph, i, j) == GRAPH_NO_EDGE);
            assert(graph_get_weight(&graph, j, i) == GRAPH_NO_EDGE);
        }
    }
    assert(graph_add_location(&graph, "Overflow") == -1);
    graph_init(&graph);
    assert(graph_location_count(&graph) == 0);
    assert(graph_add_location(&graph, "New") == 0);
    assert(graph_get_weight(&graph, 0, 0) == GRAPH_NO_EDGE);
}

static void test_names(void)
{
    Graph graph;
    char name[] = "Library";
    char longest[MAX_LOCATION_NAME];
    char too_long[MAX_LOCATION_NAME + 1];
    graph_init(&graph);
    assert(graph_add_location(&graph, name) == 0);
    name[0] = 'X';
    assert(strcmp(graph_get_location_name(&graph, 0), "Library") == 0);
    assert(graph_find_location(&graph, "Library") == 0);
    assert(graph_find_location(&graph, "library") == -1);
    assert(graph_find_location(&graph, NULL) == -1);
    assert(graph_add_location(&graph, "Library") == -1);
    assert(graph_add_location(&graph, "") == -1);
    assert(graph_add_location(&graph, NULL) == -1);
    memset(longest, 'a', sizeof longest - 1);
    longest[sizeof longest - 1] = '\0';
    assert(graph_add_location(&graph, longest) == 1);
    memset(too_long, 'b', sizeof too_long - 1);
    too_long[sizeof too_long - 1] = '\0';
    assert(graph_add_location(&graph, too_long) == -1);
    assert(graph_location_count(&graph) == 2);
}

static void test_edges(void)
{
    Graph graph;
    graph_init(&graph);
    assert(graph_add_location(&graph, "Gate") == 0);
    assert(graph_add_location(&graph, "Library") == 1);
    assert(graph_add_location(&graph, "Hostel") == 2);
    assert(graph_add_edge(&graph, 0, 1, 120));
    assert(graph_get_weight(&graph, 0, 1) == 120);
    assert(graph_get_weight(&graph, 1, 0) == 120);
    assert(graph_get_weight(&graph, 0, 2) == GRAPH_NO_EDGE);
    assert(graph_get_weight(&graph, 1, 1) == GRAPH_NO_EDGE);
    assert(!graph_add_edge(&graph, 0, 1, 0));
    assert(!graph_add_edge(&graph, 0, 1, -10));
    assert(!graph_add_edge(&graph, 1, 1, 10));
    assert(graph_get_weight(&graph, 0, 1) == 120);
    assert(graph_add_edge(&graph, 1, 0, INT_MAX));
    assert(graph_get_weight(&graph, 0, 1) == INT_MAX);
    assert(graph_get_weight(&graph, 1, 0) == INT_MAX);
    graph_init(&graph);
    assert(graph_add_location(&graph, "A") == 0);
    assert(graph_add_location(&graph, "B") == 1);
    assert(graph_get_weight(&graph, 0, 1) == GRAPH_NO_EDGE);
}

static void test_invalid_vertices(void)
{
    Graph graph;
    int invalid[] = {-1, 1, MAX_LOCATIONS, INT_MAX};
    size_t i;
    graph_init(&graph);
    assert(graph_add_location(&graph, "Gate") == 0);
    assert(graph_is_valid_vertex(&graph, 0));
    for (i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        assert(!graph_is_valid_vertex(&graph, invalid[i]));
        assert(!graph_add_edge(&graph, invalid[i], 0, 10));
        assert(!graph_add_edge(&graph, 0, invalid[i], 10));
        assert(graph_get_weight(&graph, invalid[i], 0) == GRAPH_INVALID_WEIGHT);
        assert(graph_get_weight(&graph, 0, invalid[i]) == GRAPH_INVALID_WEIGHT);
        assert(graph_get_location_name(&graph, invalid[i]) == NULL);
    }
    graph_init(NULL);
    assert(graph_location_count(NULL) == 0);
    assert(!graph_is_valid_vertex(NULL, 0));
    assert(graph_add_location(NULL, "Gate") == -1);
    assert(!graph_add_edge(NULL, 0, 1, 10));
    assert(graph_get_weight(NULL, 0, 1) == GRAPH_INVALID_WEIGHT);
    assert(graph_get_location_name(NULL, 0) == NULL);
    assert(graph_find_location(NULL, "Gate") == -1);
    graph_display_locations(NULL);
    graph_display_matrix(NULL);
}

static void test_dataset(void)
{
    Graph graph;
    int i, j, edges = 0;
    graph_init(&graph);
    assert(graph_load_campus(&graph, "data/campus.txt"));
    assert(graph_location_count(&graph) == 10);
    assert(strcmp(graph_get_location_name(&graph, 0), "Main Gate") == 0);
    assert(strcmp(graph_get_location_name(&graph, 9), "Parking Area") == 0);
    assert(graph_get_weight(&graph, 0, 2) == 400);
    assert(graph_get_weight(&graph, 0, 1) == 100);
    assert(graph_get_weight(&graph, 1, 2) == 120);
    assert(graph_get_weight(&graph, 0, 6) == GRAPH_NO_EDGE);
    for (i = 0; i < graph_location_count(&graph); ++i) {
        assert(graph_get_weight(&graph, i, i) == GRAPH_NO_EDGE);
        for (j = i + 1; j < graph_location_count(&graph); ++j) {
            int weight = graph_get_weight(&graph, i, j);
            assert(weight >= 0);
            assert(weight == graph_get_weight(&graph, j, i));
            if (weight > 0) {
                ++edges;
            }
        }
    }
    assert(edges == 17);
}

static void write_fixture(const char *contents)
{
    FILE *file = fopen("build/test_campus.txt", "w");
    assert(file != NULL);
    assert(fputs(contents, file) >= 0);
    assert(fclose(file) == 0);
}

static void test_loader_validation(void)
{
    const char *bad[] = {
        "", "11\n", "0\n", "2\nA\n", "2\nA\nA\n0\n",
        "1\n\n0\n", "2\nA\nB\n1\n0 1 0\n",
        "2\nA\nB\n1\n0 1 -5\n", "2\nA\nB\n1\n0 2 5\n",
        "2\nA\nB\n1\n0 0 5\n", "2\nA\nB\n1\n0 1 5 extra\n",
        "2\nA\nB\n1\n0 1 9999999999999999999999999999\n",
        "3\nA\nB\nC\n2\n0 1 5\n1 0 6\n",
        "2\nA\nB\n1\n", "1\nA\n0\nextra\n",
        "2\nA\nB\n1\n0+1 5\n", "1x\nA\n0\n",
        "1\nA\n-1\n", "1\nA\n2\n"
    };
    Graph graph;
    size_t i;
    graph_init(&graph);
    assert(graph_add_location(&graph, "Preserved") == 0);
    for (i = 0; i < sizeof bad / sizeof bad[0]; ++i) {
        write_fixture(bad[i]);
        assert(!graph_load_campus(&graph, "build/test_campus.txt"));
        assert(graph_location_count(&graph) == 1);
        assert(strcmp(graph_get_location_name(&graph, 0), "Preserved") == 0);
    }
    assert(remove("build/test_campus.txt") == 0);
    assert(!graph_load_campus(&graph, "build/test_campus.txt"));
    assert(!graph_load_campus(&graph, NULL));
    assert(!graph_load_campus(NULL, "data/campus.txt"));
    write_fixture("2\r\nA\r\nB\r\n1\r\n0 1 12");
    assert(graph_load_campus(&graph, "build/test_campus.txt"));
    assert(graph_location_count(&graph) == 2);
    assert(graph_get_weight(&graph, 0, 1) == 12);
    assert(remove("build/test_campus.txt") == 0);
}

int main(void)
{
    test_initialization();
    test_names();
    test_edges();
    test_invalid_vertices();
    test_dataset();
    test_loader_validation();
    puts("PASS: all 6 graph test groups passed.");
    return 0;
}
