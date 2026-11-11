#include "graph.h"
#include "metadata.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { size_t calls; size_t nodes; } Source;

static int expand(Graph *graph, size_t id, const void *context)
{
    Source *source = (Source *)context;
    ++source->calls;
    if (id + 1 < source->nodes) {
        char name[32];
        snprintf(name, sizeof(name), "NODE%zu", id + 1);
        size_t next = graph_node(graph, name);
        if (next == GRAPH_NONE || graph_edge(graph, id, next) != 0) return -1;
    }
    return graph_edge(graph, id, 0);
}

static int fail_load(Graph *graph, size_t id, const void *context)
{
    (void)graph;
    (void)id;
    (void)context;
    return -1;
}

static int collect(const char *name, void *context)
{
    size_t *count = context;
    assert(strcmp(name, "runtime") == 0);
    ++*count;
    return 0;
}

int main(void)
{
    Graph graph = {0};
    assert(graph_node(&graph, "NODE0") == 0);
    Source source = {0, 40};
    size_t *order = NULL, count = 0;
    assert(graph_bfs_expand(&graph, 0, expand, &source, &order, &count) == 0);
    assert(count == 40 && source.calls == 40);
    for (size_t i = 0; i < count; ++i) assert(order[i] == i);
    free(order);
    source.calls = 0;
    assert(graph_bfs_expand(&graph, 0, expand, &source, &order, &count) == 0);
    assert(count == 40 && source.calls == 40);
    free(order);
    assert(graph_bfs(&graph, 0, &order, &count) == 0 && count == 40);
    free(order);
    assert(graph_bfs_expand(&graph, 0, fail_load, NULL, &order, &count) != 0);
    assert(order == NULL && count == 0);
    graph_free(&graph);

    char text[] = "{\"vers\":\"1.0.0\",\"yanked\":false,\"deps\":["
        "{\"name\":\"runtime\",\"kind\":\"normal\",\"optional\":false},"
        "{\"name\":\"test\",\"kind\":\"dev\"},"
        "{\"name\":\"build\",\"kind\":\"build\"},"
        "{\"name\":\"extra\",\"kind\":\"normal\",\"optional\":true}]}";
    size_t found = 0;
    assert(metadata_each(text, 1, collect, &found) == 0);
    assert(found == 1);
    puts("Growing BFS and metadata tests passed.");
    return 0;
}
