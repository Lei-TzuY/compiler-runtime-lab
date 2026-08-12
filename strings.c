#include "minicc.h"

// strings.c

// Duplicates a string of length n.
char *strndup(const char *s, size_t n) {
    const char *p = memchr(s, '\0', n);
    if (p)
        n = p - s;
    char *d = malloc(n + 1);
    if (!d)
        return NULL;
    memcpy(d, s, n);
    d[n] = '\0';
    return d;
}
