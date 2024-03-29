#ifndef NANOS_MEMORY_H
#define NANOS_MEMORY_H

#include "types.h"

extern void *memset(void *dst, int value, uint count);
extern void *memcpy(void *dst, void *src, uint count);
extern int strncmp(const char *p, const char *q, uint n);
extern int strcmp(const char *p, const char *q);

#endif
