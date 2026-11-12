#ifndef RENDER_H
#define RENDER_H

#include "graph.h"

int graph_write_dot(const Graph *graph, const size_t *order, size_t count,
                    const char *filename);
int graph_render_svg(const char *dot_filename, const char *svg_filename);
#endif
