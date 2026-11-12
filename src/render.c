#include "render.h"
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void write_label(FILE *file, const char *text) {
  fputc('"', file);

  for (; *text != '\0'; ++text) {
    switch (*text) {
    case '"':
    case '\\':
      fputc('\\', file);
      fputc(*text, file);
      break;
    case '\n':
      fputs("\\n", file);
      break;
    case '\r':
      fputs("\\r", file);
      break;
    default:
      fputc(*text, file);
      break;
    }
  }

  fputc('"', file);
}

int graph_write_dot(const Graph *graph, const size_t *order, size_t count,
                    const char *filename) {
  FILE *file = fopen(filename, "wb");

  if (file == NULL) {
    perror("Cannot create DOT file");
    return -1;
  }

  fputs("digraph dependencies {\n", file);
  fputs("    rankdir=LR;\n", file);

  for (size_t i = 0; i < count; ++i) {
    size_t id = order[i];
    const Node *node = &graph->nodes[id];

    fprintf(file, "    n%zu [label=", id);
    write_label(file, node->name);
    fputs("];\n", file);

    for (size_t j = 0; j < node->count; ++j) {
      fprintf(file, "    n%zu -> n%zu;\n", id, node->dependencies[j]);
    }
  }

  fputs("}\n", file);

  int failed = ferror(file);

  if (fclose(file) != 0) {
    failed = 1;
  }

  if (failed) {
    fprintf(stderr, "Cannot write complete DOT file\n");
    return -1;
  }

  return 0;
}
static char *quote_path(const char *path) {
  size_t length = strlen(path);

  if (length == 0 || strchr(path, '"') != NULL || path[length - 1] == '\\') {
    return NULL;
  }

  char *quoted = malloc(length + 5);

  if (quoted == NULL) {
    return NULL;
  }

  /*
   * Prefix a relative filename beginning with '-'
   * so Graphviz cannot interpret it as an option.
   */
  snprintf(quoted, length + 5, "\"%s%s\"", path[0] == '-' ? "./" : "", path);

  return quoted;
}

int graph_render_svg(const char *dot_filename, const char *svg_filename) {
  char *input = quote_path(dot_filename);
  char *output = quote_path(svg_filename);

  if (input == NULL || output == NULL) {
    fprintf(stderr, "Cannot prepare Graphviz file paths\n");
    free(input);
    free(output);
    return -1;
  }

  const char *arguments[] = {"dot.exe", "-Tsvg", "-o", output, input, NULL};

  fflush(stdout);

  intptr_t status = _spawnvp(_P_WAIT, arguments[0], arguments);

  free(input);
  free(output);

  if (status == -1) {
    perror("Cannot start Graphviz");
    return -1;
  }

  if (status != 0) {
    fprintf(stderr, "Graphviz returned %lld\n", (long long)status);
    return -1;
  }

  return 0;
}
