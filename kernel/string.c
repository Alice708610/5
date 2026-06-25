// string.c - Kernel string/mem functions

#include "types.h"
#include "defs.h"

int
strlen(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    return p - s;
}

int
strncmp(const char *p, const char *q, uint n)
{
    while (n > 0 && *p == *q && *p) {
        n--;
        p++;
        q++;
    }
    if (n == 0)
        return 0;
    return (uchar)*p - (uchar)*q;
}

int
strcmp(const char *p, const char *q)
{
    while (*p && *p == *q)
        p++, q++;
    return (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, int n)
{
    char *os = s;
    while (n-- > 0 && (*s++ = *t++) != 0)
        ;
    while (n-- > 0)
        *s++ = 0;
    return os;
}

char *
safestrcpy(char *s, const char *t, int n)
{
    char *os = s;
    if (n <= 0)
        return os;
    while (--n > 0 && (*s++ = *t++) != 0)
        ;
    *s = 0;
    return os;
}

void *
memset(void *dst, int c, uint n)
{
    char *cdst = (char *)dst;
    for (int i = 0; i < n; i++)
        cdst[i] = c;
    return dst;
}

void *
memmove(void *dst, const void *src, uint n)
{
    char *cdst = (char *)dst;
    const char *csrc = (const char *)src;
    if (csrc < cdst && cdst < csrc + n) {
        // Copy backwards to handle overlap
        for (int i = n - 1; i >= 0; i--)
            cdst[i] = csrc[i];
    } else {
        for (int i = 0; i < n; i++)
            cdst[i] = csrc[i];
    }
    return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
    const uchar *s1 = (const uchar *)v1;
    const uchar *s2 = (const uchar *)v2;
    while (n-- > 0) {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++, s2++;
    }
    return 0;
}
