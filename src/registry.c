#include "registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int registry_url(const char *repository, const char *package, char *output,
                 size_t capacity) {
  size_t length = strlen(package);

  if (length == 0 || repository[0] == '\0') {
    return -1;
  }

  char *name = malloc(length + 1);

  if (name == NULL) {
    return -1;
  }

  for (size_t i = 0; i < length; ++i) {
    char c = package[i];

    if (c >= 'A' && c <= 'Z') {
      c = (char)(c - 'A' + 'a');
    }

    name[i] = c;
  }

  name[length] = '\0';

  char prefix[8];

  if (length == 1) {
    snprintf(prefix, sizeof(prefix), "1");
  } else if (length == 2) {
    snprintf(prefix, sizeof(prefix), "2");
  } else if (length == 3) {
    snprintf(prefix, sizeof(prefix), "3/%c", name[0]);
  } else {
    snprintf(prefix, sizeof(prefix), "%c%c/%c%c", name[0], name[1], name[2],
             name[3]);
  }

  size_t repository_length = strlen(repository);
  const char *separator = repository[repository_length - 1] == '/' ? "" : "/";

  int written = snprintf(output, capacity, "%s%s%s/%s", repository, separator,
                         prefix, name);

  free(name);

  if (written < 0 || (size_t)written >= capacity) {
    return -1;
  }

  return 0;
}
