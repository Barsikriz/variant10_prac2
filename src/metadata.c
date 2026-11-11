#include "metadata.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *spaces(const char *p)
{
    while (isspace((unsigned char)*p)) {
        ++p;
    }
    return p;
}

static const char *string_end(const char *p)
{
    if (*p != '"') return NULL;
    ++p;
    while (*p && *p != '"') {
        if ((unsigned char)*p < 32) return NULL;
        if (*p == '\\') {
            ++p;
            if (!*p) return NULL;
        }
        ++p;
    }
    return *p == '"' ? p + 1 : NULL;
}

static const char *skip_value(const char *p, unsigned depth)
{
    p = spaces(p);
    if (depth > 64 || !*p) return NULL;
    if (*p == '"') return string_end(p);

    if (*p == '{' || *p == '[') {
        int object = *p == '{';
        char close = object ? '}' : ']';
        p = spaces(p + 1);
        if (*p == close) return p + 1;

        for (;;) {
            if (object) {
                p = string_end(p);
                if (!p) return NULL;
                p = spaces(p);
                if (*p != ':') return NULL;
                ++p;
            }
            p = skip_value(p, depth + 1);
            if (!p) return NULL;
            p = spaces(p);
            if (*p == close) return p + 1;
            if (*p != ',') return NULL;
            p = spaces(p + 1);
        }
    }

    const char *start = p;
    while (*p && !strchr(" \t\r\n,]}", *p)) ++p;
    return p != start ? p : NULL;
}

static const char *field(const char *object, const char *key)
{
    if (*object != '{') return NULL;
    const char *p = spaces(object + 1);
    while (*p && *p != '}') {
        const char *end = string_end(p);
        if (!end) return NULL;
        int matches = (size_t)(end - p - 2) == strlen(key)
            && memcmp(p + 1, key, strlen(key)) == 0;
        p = spaces(end);
        if (*p != ':') return NULL;
        p = spaces(p + 1);
        if (matches) return p;
        p = skip_value(p, 0);
        if (!p) return NULL;
        p = spaces(p);
        if (*p == ',') p = spaces(p + 1);
        else if (*p != '}') return NULL;
    }
    return NULL;
}

static int stable(const char *version)
{
    const char *end = version ? string_end(version) : NULL;
    if (!end) return 0;
    for (const char *p = version + 1; p < end - 1; ++p) {
        if (*p == '+') break;
        if (*p == '-') return 0;
    }
    return 1;
}

static int newer(const char *a, const char *b)
{
    ++a;
    ++b;
    for (int part = 0; part < 3; ++part) {
        size_t na = strspn(a, "0123456789");
        size_t nb = strspn(b, "0123456789");
        if (na != nb) return na > nb;
        int difference = memcmp(a, b, na);
        if (difference) return difference > 0;
        a += na;
        b += nb;
        if (part < 2) {
            if (*a != '.' || *b != '.') return 0;
            ++a;
            ++b;
        }
    }
    return 0;
}

int metadata_each(char *text, int normal_only, MetadataVisitor visit, void *context)
{
    const char *selected = NULL;
    for (char *line = text; line; ) {
        char *next = strchr(line, '\n');
        if (next) *next++ = '\0';
        const char *record = spaces(line);
        if (*record) {
            const char *end = skip_value(record, 0);
            const char *yanked = field(record, "yanked");
            if (*record != '{' || !end || *spaces(end) || !yanked) {
                fprintf(stderr, "Invalid index record\n");
                return -1;
            }
            const char *version = field(record, "vers");
            if (strncmp(yanked, "false", 5) == 0 && stable(version)
                && (!selected || newer(version, field(selected, "vers")))) {
                selected = record;
            }
        }
        line = next;
    }

    if (!selected) {
        fprintf(stderr, "No stable non-yanked release found\n");
        return -1;
    }

    const char *deps = field(selected, "deps");
    if (!deps || *deps != '[') {
        fprintf(stderr, "Missing dependencies array\n");
        return -1;
    }

    const char *p = spaces(deps + 1);
    while (*p != ']') {
        const char *end = skip_value(p, 0);
        const char *name = field(p, "package");
        if (!name || *name != '"') name = field(p, "name");
        const char *name_end = name ? string_end(name) : NULL;
        if (*p != '{' || !end || !name_end) {
            fprintf(stderr, "Invalid dependency entry\n");
            return -1;
        }
        const char *kind = field(p, "kind");
        const char *optional = field(p, "optional");
        int normal = !kind || strncmp(kind, "null", 4) == 0
            || strncmp(kind, "\"normal\"", 8) == 0;
        int enabled = !optional || strncmp(optional, "false", 5) == 0;

        if (!normal_only || (normal && enabled)) {
            size_t length = (size_t)(name_end - name - 2);
            char *dependency = malloc(length + 1);
            if (!dependency) return -1;
            memcpy(dependency, name + 1, length);
            dependency[length] = '\0';
            int result = visit(dependency, context);
            free(dependency);
            if (result != 0) return -1;
        }
        p = spaces(end);
        if (*p == ',') p = spaces(p + 1);
        else if (*p != ']') return -1;
    }
    return 0;
}

static int print_dependency(const char *name, void *context)
{
    (void)context;
    puts(name);
    return 0;
}

int metadata_print(char *text)
{
    return metadata_each(text, 0, print_dependency, NULL);
}
