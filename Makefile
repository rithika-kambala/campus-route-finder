CC = cc
CPPFLAGS = -Iinclude
CFLAGS = -std=c11 -Wall -Wextra -pedantic
LDFLAGS =
LDLIBS =

OBJECTS = build/graph.o build/dijkstra.o

.PHONY: all app test clean
all: campus_route_finder
app: campus_route_finder

build:
	mkdir -p build

build/graph.o: src/graph.c include/graph.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/dijkstra.o: src/dijkstra.c include/dijkstra.h include/graph.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

campus_route_finder: src/main.c $(OBJECTS) include/graph.h include/dijkstra.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) src/main.c $(OBJECTS) $(LDLIBS) -o $@

build/test_graph: tests/test_graph.c build/graph.o include/graph.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) tests/test_graph.c build/graph.o $(LDLIBS) -o $@

build/test_routes: tests/test_routes.c $(OBJECTS) include/graph.h include/dijkstra.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) tests/test_routes.c $(OBJECTS) $(LDLIBS) -o $@

test: build/test_graph build/test_routes
	./build/test_graph
	./build/test_routes

clean:
	rm -rf build campus_route_finder
