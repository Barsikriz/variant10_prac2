#ifndef GRAPH_H
#define GRAPH_H
#include <stdbool.h>
#include <stddef.h>
#define GRAPH_NONE ((size_t)-1)

typedef struct {
  char *name;
  size_t *dependencies;
  size_t count;
  size_t capacity;
  bool visited;
} Node;

typedef struct {
  Node *nodes;
  size_t count;
  size_t capacity;
} Graph;

typedef int (*GraphLoader)(Graph *graph, size_t node, const void *context);

size_t graph_find(const Graph *graph, const char *name);
size_t graph_node(Graph *graph, const char *name);
int graph_edge(Graph *graph, size_t from, size_t to);
int graph_load(Graph *graph, const char *filename);
int graph_bfs(Graph *graph, size_t root, size_t **order, size_t *count);
int graph_bfs_expand(Graph *graph, size_t root, GraphLoader load,
                     const void *context, size_t **order, size_t *count);
int graph_reverse(Graph *graph);
void graph_free(Graph *graph);
#endif
