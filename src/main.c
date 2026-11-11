#include "http.h"
#include "index.h"
#include "metadata.h"
#include "registry.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if (strcmp(option, "--package") != 0 && strcmp(option, "--repo") != 0 &&
        strcmp(option, "--out") != 0) {
      fprintf(stderr, "Unknown option: %s\n", option);
      return 1;
    }

    if (i + 1 >= argc || argv[i + 1][0] == '\0' ||
        strncmp(argv[i + 1], "--", 2) == 0) {
      fprintf(stderr, "Missing value for %s\n", option);
      return 1;
    }

    const char *value = argv[++i];

    const char **target = NULL;

    if (strcmp(option, "--package") == 0) {
      target = &config.package;
    } else if (strcmp(option, "--repo") == 0) {
      target = &config.repo;
    } else {
      target = &config.out;
    }

    if (*target != NULL) {
      fprintf(stderr, "Duplicate option: %s\n", option);
      return 1;
    }

    *target = value;
  }

  if (config.package == NULL) {
    fprintf(stderr, "Required option: --package NAME\n");
    return 1;
  }

  if (config.test && config.repo == NULL) {
    fprintf(stderr, "Test mode requires --repo FILE\n");
    return 1;
  }

  if (config.repo == NULL) {
    config.repo = "https://index.crates.io";
  }

  if (config.out == NULL) {
    config.out = "graph.svg";
  }

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

  printf("package=%s\n", config.package);
  printf("repo=%s\n", config.repo);
  printf("test=%s\n", config.test ? "true" : "false");
  printf("ascii=%s\n", config.ascii ? "true" : "false");
  printf("out=%s\n", config.out);

  if (!config.test) {
    char url[2048];

    if (registry_url(config.repo, config.package, url, sizeof(url)) != 0) {
      fprintf(stderr, "Cannot construct registry URL\n");
      return 1;
    }

    if (download_index(url) != 0) {
      return 1;
    }

    char *text = index_read(INDEX_FILE);

    if (text == NULL) {
      return 1;
    }

    int result = metadata_print(text);

    free(text);

    if (result != 0) {
      return 1;
    }
  }

  return 0;
}
