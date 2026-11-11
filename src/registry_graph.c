#include "registry_graph.h"
#include "registry.h"
#include "http.h"
#include "index.h"
#include "metadata.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Graph *graph;
    size_t from;
} DependencyTarget;

static int add_dependency(const char *name, void *context)
{
    DependencyTarget *target = context;
    size_t to = graph_node(target->graph, name);
    if (to == GRAPH_NONE) return -1;
    return graph_edge(target->graph, target->from, to);
}

int registry_expand(Graph *graph, size_t node, const void *context)
{
    const char *repository = context;
    char url[2048];
    if (registry_url(repository, graph->nodes[node].name, url, sizeof(url)) != 0) {
        fprintf(stderr, "Cannot construct URL for %s\n", graph->nodes[node].name);
        return -1;
    }
    fprintf(stderr, "Fetching %s...\n", graph->nodes[node].name);
    if (download_index(url) != 0) return -1;
    char *text = index_read(INDEX_FILE);
    if (!text) return -1;
    DependencyTarget target = {graph, node};
    int result = metadata_each(text, 1, add_dependency, &target);
    free(text);
    if (result != 0) {
        fprintf(stderr, "Cannot read dependencies of %s\n", graph->nodes[node].name);
    }
    return result;
}
