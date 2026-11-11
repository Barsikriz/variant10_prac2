#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "registry_graph.h"

typedef struct {
  const char *package;
  const char *repo;
  const char *out;
  bool test;
  bool ascii;
} Config;

int main(int argc, char *argv[]) {
  Config config = {0};

  for (int i = 1; i < argc; ++i) {
    const char *option = argv[i];
    bool *flag = NULL;

    if (strcmp(option, "--test") == 0) {
      flag = &config.test;
    } else if (strcmp(option, "--ascii") == 0) {
      flag = &config.ascii;
    }

    if (flag != NULL) {
      if (*flag) {
        fprintf(stderr, "Duplicate option: %s\n", option);
        return 1;
      }
      *flag = true;
      continue;
    }

    const char **target = NULL;

    if (strcmp(option, "--package") == 0) {
      target = &config.package;
    } else if (strcmp(option, "--repo") == 0) {
      target = &config.repo;
    } else if (strcmp(option, "--out") == 0) {
      target = &config.out;
    } else {
      fprintf(stderr, "Unknown option: %s\n", option);
      return 1;
    }

    if (i + 1 >= argc || argv[i + 1][0] == '\0' ||
        strncmp(argv[i + 1], "--", 2) == 0) {
      fprintf(stderr, "Missing value for %s\n", option);
      return 1;
    }

    if (*target != NULL) {
      fprintf(stderr, "Duplicate option: %s\n", option);
      return 1;
    }

    *target = argv[++i];
  }

  if (config.package == NULL) {
    fprintf(stderr, "Required option: --package NAME\n");
    return 1;
  }

  if (config.test && config.repo == NULL) {
    fprintf(stderr, "Test mode requires --repo FILE\n");
    return 1;
  }

  if (config.repo == NULL)
    config.repo = "https://index.crates.io";
  if (config.out == NULL)
    config.out = "graph.svg";

  for (const char *p = config.package; *p != '\0'; ++p) {
    bool upper = *p >= 'A' && *p <= 'Z';
    bool lower = *p >= 'a' && *p <= 'z';
    bool digit = *p >= '0' && *p <= '9';
    bool valid =
        config.test ? upper : upper || lower || digit || *p == '-' || *p == '_';

    if (!valid) {
      fprintf(stderr, "Invalid package name: %s\n", config.package);
      return 1;
    }
  }

  bool http = strncmp(config.repo, "http://", 7) == 0;
  bool https = strncmp(config.repo, "https://", 8) == 0;

  if (!config.test) {
    if (!http && !https) {
      fprintf(stderr, "Repository must be an HTTP(S) URL\n");
      return 1;
    }

    const char *host = config.repo + (https ? 8 : 7);

    if (*host == '\0' || *host == '/' ||
        strpbrk(config.repo, " \t\r\n") != NULL) {
      fprintf(stderr, "Invalid repository URL\n");
      return 1;
    }
  } else if (http || https) {
    fprintf(stderr, "Test mode expects a local file path\n");
    return 1;
  }

  size_t length = strlen(config.out);

  if (length <= 4 || strcmp(config.out + length - 4, ".svg") != 0) {
    fprintf(stderr, "Output filename must end with .svg\n");
    return 1;
  }

  Graph graph = {0};
  size_t root;

  if (config.test) {
    if (graph_load(&graph, config.repo) != 0)
      return 1;
    root = graph_find(&graph, config.package);
  } else {
    root = graph_node(&graph, config.package);
  }

  if (root == GRAPH_NONE) {
    fprintf(stderr, "Cannot select package: %s\n", config.package);
    graph_free(&graph);
    return 1;
  }

  size_t *order = NULL;
  size_t count = 0;
  int result;

  if (config.test) {
    result = graph_bfs(&graph, root, &order, &count);
  } else {
    result = graph_bfs_expand(&graph, root, registry_expand, config.repo,
                              &order, &count);
  }

  if (result != 0) {
    fprintf(stderr, "Cannot build dependency graph\n");
    graph_free(&graph);
    return 1;
  }

  for (size_t i = 0; i < count; ++i) {
    printf("%s\n", graph.nodes[order[i]].name);
  }

  free(order);
  graph_free(&graph);
  return 0;
}
