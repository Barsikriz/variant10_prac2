#include "graph.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  Graph graph = {0};

  size_t a = graph_node(&graph, "A");
  size_t b = graph_node(&graph, "B");
  size_t c = graph_node(&graph, "C");

  assert(a != GRAPH_NONE);
  assert(b != GRAPH_NONE);
  assert(c != GRAPH_NONE);

  assert(graph_node(&graph, "A") == a);
  assert(graph.count == 3);

  assert(graph_edge(&graph, a, b) == 0);
  assert(graph_edge(&graph, a, c) == 0);
  assert(graph_edge(&graph, b, c) == 0);

  assert(graph_edge(&graph, a, b) == 0);
  assert(graph.nodes[a].count == 2);

  /* Cycle and self-loop must be representable. */
  assert(graph_edge(&graph, c, a) == 0);
  assert(graph_edge(&graph, c, c) == 0);

  assert(graph_edge(&graph, a, GRAPH_NONE) == -1);

  for (size_t i = 0; i < graph.count; ++i) {
    Node *node = &graph.nodes[i];

    printf("%s:", node->name);

    for (size_t j = 0; j < node->count; ++j) {
      size_t dependency = node->dependencies[j];
      printf(" %s", graph.nodes[dependency].name);
    }

    putchar('\n');
  }

  size_t isolated = graph_node(&graph, "D");
  assert(isolated != GRAPH_NONE);

  size_t *order = NULL;
  size_t count = 0;

  assert(graph_bfs(&graph, a, &order, &count) == 0);

  assert(count == 3);
  assert(order[0] == a);
  assert(order[1] == b);
  assert(order[2] == c);
  assert(!graph.nodes[isolated].visited);

  printf("BFS:");

  for (size_t i = 0; i < count; ++i) {
    printf(" %s", graph.nodes[order[i]].name);
  }

  putchar('\n');
  free(order);

  /* A second traversal must reset the visited flags. */
  assert(graph_bfs(&graph, c, &order, &count) == 0);

  assert(count == 3);
  assert(order[0] == c);
  assert(order[1] == a);
  assert(order[2] == b);

  free(order);

  /* An isolated vertex reaches only itself. */
  assert(graph_bfs(&graph, isolated, &order, &count) == 0);
  assert(count == 1);
  assert(order[0] == isolated);

  free(order);

  assert(graph_bfs(&graph, GRAPH_NONE, &order, &count) == -1);
  assert(order == NULL);
  assert(count == 0);

  graph_free(&graph);

  assert(graph.nodes == NULL);
  assert(graph.count == 0);

  puts("Graph tests passed.");
  return 0;
}
