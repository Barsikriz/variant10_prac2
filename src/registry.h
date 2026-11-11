#ifndef REGISTRY_H
#define REGISTRY_H

#include <stddef.h>

int registry_url(const char *repository, const char *package, char *output,
                 size_t capacity);

#endif
