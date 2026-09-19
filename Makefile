CC = cc
CPPFLAGS = -Iinclude
CFLAGS = -std=c11 -Wall -Wextra -pedantic
LDFLAGS =
LDLIBS =

GRAPH_OBJECTS = build/graph.o

.PHONY: all test app clean
all: build/test_graph

build:
	mkdir -p build

build/graph.o: src/graph.c include/graph.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/test_graph: tests/test_graph.c $(GRAPH_OBJECTS) include/graph.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) tests/test_graph.c $(GRAPH_OBJECTS) $(LDLIBS) -o $@

test: build/test_graph
	./build/test_graph

# Part 2 supplies these files. This target is intentionally not part of all yet.
app: build/campus_route

build/campus_route: src/main.c src/dijkstra.c include/dijkstra.h include/graph.h $(GRAPH_OBJECTS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) src/main.c src/dijkstra.c $(GRAPH_OBJECTS) $(LDLIBS) -o $@

clean:
	rm -rf build
