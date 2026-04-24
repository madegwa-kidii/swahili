#ifndef SWAHILI_UTILS_H
#define SWAHILI_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Kosa: kumbukumbu haitoshi.\n");
        exit(1);
    }
    return ptr;
}

static inline char *xstrdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *copy = (char *)xmalloc(n);
    memcpy(copy, s, n);
    return copy;
}

#endif
