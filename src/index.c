#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *index_read(const char *filename) {
  FILE *file = fopen(filename, "rb");

  if (file == NULL) {
    perror("Cannot open index");
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Cannot seek index");
    fclose(file);
    return NULL;
  }

  long length = ftell(file);

  if (length < 0 || fseek(file, 0, SEEK_SET) != 0) {
    fprintf(stderr, "Cannot determine index size\n");
    fclose(file);
    return NULL;
  }

  size_t size = (size_t)length;
  char *text = malloc(size + 1);

  if (text == NULL) {
    fprintf(stderr, "Cannot allocate index buffer\n");
    fclose(file);
    return NULL;
  }

  size_t received = fread(text, 1, size, file);
  int read_error = ferror(file);
  fclose(file);

  if (received != size || read_error) {
    fprintf(stderr, "Cannot read complete index\n");
    free(text);
    return NULL;
  }

  if (memchr(text, '\0', size) != NULL) {
    fprintf(stderr, "Unexpected NUL byte in index\n");
    free(text);
    return NULL;
  }

  text[size] = '\0';

  return text;
}

char *index_last_record(char *text) {
  size_t length = strlen(text);

  while (length > 0 && (text[length - 1] == '\n' || text[length - 1] == '\r')) {
    text[--length] = '\0';
  }

  if (length == 0) {
    return NULL;
  }

  char *newline = strrchr(text, '\n');

  return newline != NULL ? newline + 1 : text;
}
