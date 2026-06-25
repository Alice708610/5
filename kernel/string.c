// string.c - String and memory operations for ZephyrOS

#include "types.h"
#include "defs.h"

void *
memset(void *dst, int c, uint n)
{
  char *d = dst;
  while (n-- > 0)
    *d++ = c;
  return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const unsigned char *s1 = v1, *s2 = v2;
  while (n-- > 0) {
    if (*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }
  return 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s = src;
  char *d = dst;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    while (n-- > 0)
      *--d = *--s;
  } else {
    while (n-- > 0)
      *d++ = *s++;
  }
  return dst;
}

int
strncmp(const char *p, const char *q, uint n)
{
  while (n > 0 && *p && *p == *q)
    n--, p++, q++;
  if (n == 0)
    return 0;
  return (unsigned char)*p - (unsigned char)*q;
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

uint64
strlen(const char *s)
{
  uint64 n;
  for (n = 0; s[n]; n++)
    ;
  return n;
}
