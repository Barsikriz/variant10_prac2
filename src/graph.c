#include "graph.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t graph_find(const Graph *graph, const char *name) {
  for (size_t i = 0; i < graph->count; ++i) {
    if (strcmp(graph->nodes[i].name, name) == 0)
      return i;
  }
  return GRAPH_NONE;
}

size_t graph_node(Graph *graph, const char *name) {
  size_t existing = graph_find(graph, name);
  if (existing != GRAPH_NONE)
    return existing;
  if (graph->count == graph->capacity) {
    if (graph->capacity > SIZE_MAX / 2)
      return GRAPH_NONE;
    size_t capacity = graph->capacity ? graph->capacity * 2 : 8;
    if (capacity > SIZE_MAX / sizeof(Node))
      return GRAPH_NONE;
    Node *nodes = realloc(graph->nodes, capacity * sizeof(Node));
    if (!nodes)
      return GRAPH_NONE;
    graph->nodes = nodes;
    graph->capacity = capacity;
  }
  char *name_copy = malloc(strlen(name) + 1);
  if (!name_copy)
    return GRAPH_NONE;
  strcpy(name_copy, name);
  size_t id = graph->count;
  graph->nodes[id] = (Node){0};
  graph->nodes[id].name = name_copy;
  ++graph->count;
  return id;
}

int graph_edge(Graph *graph, size_t from, size_t to) {
  if (from >= graph->count || to >= graph->count)
    return -1;
  Node *node = &graph->nodes[from];
  for (size_t i = 0; i < node->count; ++i) {
    if (node->dependencies[i] == to)
      return 0;
  }
  if (node->count == node->capacity) {
    if (node->capacity > SIZE_MAX / 2)
      return -1;
    size_t capacity = node->capacity ? node->capacity * 2 : 4;
    if (capacity > SIZE_MAX / sizeof(size_t))
      return -1;
    size_t *dependencies =
        realloc(node->dependencies, capacity * sizeof(size_t));
    if (!dependencies)
      return -1;
    node->dependencies = dependencies;
    node->capacity = capacity;
  }
  node->dependencies[node->count++] = to;
  return 0;
}

/* The growing queue supports vertices discovered while fetching a crate. */
static int enqueue(size_t **queue, size_t *tail, size_t *capacity, size_t id) {
  if (*tail == *capacity) {
    if (*capacity > SIZE_MAX / 2)
      return -1;
    size_t next = *capacity ? *capacity * 2 : 8;
    if (next > SIZE_MAX / sizeof(size_t))
      return -1;
    size_t *items = realloc(*queue, next * sizeof(size_t));
    if (!items)
      return -1;
    *queue = items;
    *capacity = next;
  }
  (*queue)[(*tail)++] = id;
  return 0;
}

int graph_bfs_expand(Graph *graph, size_t root, GraphLoader load,
                     const void *context, size_t **order, size_t *count) {
  *order = NULL;
  *count = 0;
  if (root >= graph->count)
    return -1;
  for (size_t i = 0; i < graph->count; ++i)
    graph->nodes[i].visited = false;
  size_t *queue = NULL;
  size_t head = 0, tail = 0, capacity = 0;
  if (enqueue(&queue, &tail, &capacity, root) != 0)
    return -1;
  graph->nodes[root].visited = true;

  while (head < tail) {
    size_t current = queue[head++];
    /* load may realloc graph->nodes, so obtain node afterwards. */
    if (load && load(graph, current, context) != 0) {
      free(queue);
      return -1;
    }
    Node *node = &graph->nodes[current];
    for (size_t i = 0; i < node->count; ++i) {
      size_t dependency = node->dependencies[i];
      if (graph->nodes[dependency].visited)
        continue;
      if (enqueue(&queue, &tail, &capacity, dependency) != 0) {
        free(queue);
        return -1;
      }
      graph->nodes[dependency].visited = true;
    }
  }
  *order = queue;
  *count = tail;
  return 0;
}

int graph_bfs(Graph *graph, size_t root, size_t **order, size_t *count) {
  return graph_bfs_expand(graph, root, NULL, NULL, order, count);
}

int graph_reverse(Graph *graph) {
  Graph reversed = {0};

  for (size_t i = 0; i < graph->count; ++i) {
    if (graph_node(&reversed, graph->nodes[i].name) == GRAPH_NONE) {
      graph_free(&reversed);
      return -1;
    }
  }

  for (size_t from = 0; from < graph->count; ++from) {
    const Node *node = &graph->nodes[from];

    for (size_t j = 0; j < node->count; ++j) {
      size_t to = node->dependencies[j];

      if (graph_edge(&reversed, to, from) != 0) {
        graph_free(&reversed);
        return -1;
      }
    }
  }

  graph_free(graph);
  *graph = reversed;

  return 0;
}

void graph_free(Graph *graph) {
  for (size_t i = 0; i < graph->count; ++i) {
    free(graph->nodes[i].name);
    free(graph->nodes[i].dependencies);
  }
  free(graph->nodes);
  *graph = (Graph){0};
}
