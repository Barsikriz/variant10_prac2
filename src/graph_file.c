#include "graph.h"
#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *text) {
  text += strspn(text, " \t\r");

  size_t length = strlen(text);

  while (length > 0 && strchr(" \t\r", text[length - 1]) != NULL) {
    text[--length] = '\0';
  }

  return text;
}

static int valid_name(const char *name) {
  if (*name == '\0') {
    return 0;
  }

  for (; *name != '\0'; ++name) {
    if (*name < 'A' || *name > 'Z') {
      return 0;
    }
  }

  return 1;
}

static int parse_line(Graph *graph, char *line) {
  char *comment = strchr(line, '#');

  if (comment != NULL) {
    *comment = '\0';
  }

  line = trim(line);

  if (*line == '\0') {
    return 0;
  }

  char *colon = strchr(line, ':');

  if (colon == NULL) {
    return -1;
  }

  *colon = '\0';

  char *name = trim(line);

  if (!valid_name(name)) {
    return -1;
  }

  size_t from = graph_node(graph, name);

  if (from == GRAPH_NONE) {
    return -1;
  }

  char *dependency = strtok(colon + 1, " \t\r");

  while (dependency != NULL) {
    if (!valid_name(dependency)) {
      return -1;
    }

    size_t to = graph_node(graph, dependency);

    if (to == GRAPH_NONE || graph_edge(graph, from, to) != 0) {
      return -1;
    }

    dependency = strtok(NULL, " \t\r");
  }

  return 0;
}

int graph_load(Graph *graph, const char *filename) {
  char *text = index_read(filename);

  if (text == NULL) {
    return -1;
  }

  Graph loaded = {0};
  char *line = text;
  size_t number = 0;

  while (line != NULL) {
    char *next = strchr(line, '\n');

    if (next != NULL) {
      *next++ = '\0';
    }

    ++number;

    if (parse_line(&loaded, line) != 0) {
      fprintf(stderr, "Cannot load graph at line %zu\n", number);

      graph_free(&loaded);
      free(text);
      return -1;
    }

    line = next;
  }

  free(text);

  graph_free(graph);
  *graph = loaded;

  return 0;
}
