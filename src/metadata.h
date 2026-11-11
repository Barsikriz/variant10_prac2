#ifndef METADATA_H
#define METADATA_H

typedef int (*MetadataVisitor)(const char *name, void *context);

int metadata_each(char *text, int normal_only, MetadataVisitor visit, void *context);
int metadata_print(char *text);

#endif
