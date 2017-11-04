//
// Created by matoran on 10/31/17.
//

#ifndef NANOS_MEMORY_H
#define NANOS_MEMORY_H

#include "../common/types.h"

extern void *memset(void *dst, int value, uint count);

extern void *memcpy(void *dst, void *src, uint count);

extern int strncmp(const char *p, const char *q, uint n);

#endif //NANOS_MEMORY_H
