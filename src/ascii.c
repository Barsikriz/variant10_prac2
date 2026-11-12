#include "ascii.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t id;
    size_t next_child;
    bool last;
} Frame;

int graph_print_tree(const Graph *graph, const size_t *order, size_t count)
{
    if (!count) return -1;
    size_t *parent = malloc(graph->count * sizeof(size_t));
    Frame *stack = malloc(graph->count * sizeof(Frame));
    if (!parent || !stack) {
        fprintf(stderr, "Cannot allocate ASCII tree buffers\n");
        free(parent);
        free(stack);
        return -1;
    }
    for (size_t i = 0; i < graph->count; ++i) parent[i] = GRAPH_NONE;
    size_t root = order[0];

    /* Reconstruct first-discovery parents in the same BFS order. */
    for (size_t i = 0; i < count; ++i) {
        size_t id = order[i];
        const Node *node = &graph->nodes[id];
        for (size_t j = 0; j < node->count; ++j) {
            size_t next = node->dependencies[j];
            if (next != root && parent[next] == GRAPH_NONE) parent[next] = id;
        }
    }

    size_t depth = 1;
    stack[0] = (Frame){root, 0, true};
    puts(graph->nodes[root].name);
    while (depth) {
        Frame *frame = &stack[depth - 1];
        const Node *node = &graph->nodes[frame->id];
        if (frame->next_child == node->count) {
            --depth;
            continue;
        }
        size_t child = node->dependencies[frame->next_child++];
        bool last = frame->next_child == node->count;
        for (size_t i = 1; i < depth; ++i) fputs(stack[i].last ? "    " : "|   ", stdout);
        fputs(last ? "`-- " : "|-- ", stdout);
        fputs(graph->nodes[child].name, stdout);
        if (parent[child] != frame->id) {
            puts(" [ref]");
        } else {
            putchar('\n');
            stack[depth++] = (Frame){child, 0, last};
        }
    }
    free(parent);
    free(stack);
    return 0;
}
