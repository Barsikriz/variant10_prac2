CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

.PHONY: all test

all: depgraph.exe

depgraph.exe: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o depgraph.exe

test: depgraph.exe
	powershell -ExecutionPolicy Bypass -File tests/stage1.ps1


.PHONY: test-graph

test-graph: tests/test_graph.c src/graph.c src/graph.h
	$(CC) $(CFLAGS) -Isrc tests/test_graph.c src/graph.c -o test_graph.exe
	./test_graph.exe

.PHONY: test-expand test-stage3

test-expand: tests/test_graph_expand.c src/graph.c src/metadata.c src/graph.h src/metadata.h
	$(CC) $(CFLAGS) -Isrc tests/test_graph_expand.c src/graph.c src/metadata.c -o test_expand.exe
	./test_expand.exe

test-stage3: depgraph.exe test-graph test-expand
	powershell -ExecutionPolicy Bypass -File tests/stage3.ps1
