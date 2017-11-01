//
// Created by matoran on 10/31/17.
//

#include "memory.h"


void *memset(void *dst, int value, uint count){
    char *d = dst;
    for (uint i = 0; i < count; ++i) {
        *d++ = value;
    }
    return dst;
}

/**
 * Copy src to dst of
 * @param dst
 * @param src
 * @param count
 * @return
 */
void *memcpy(void *dst, void *src, uint count){
    char *d = dst;
    char *s = src;
    for (uint i = 0; i < count; ++i) {
        *d++ = *s++;
    }
    return dst;
}

/**
 * Compare n characters of two string
 * @param p first string
 * @param q second string
 * @param n number of characters to compare
 * @return -1 if p < q, 1 if p > q, 0 if p == q
 */
int strncmp(const char *p, const char *q, uint n){
    for (uint i = 0; i < n; ++i) {
        if(*p > *q){
            return 1;
        }else if(*p < *q){
            return -1;
        }
    }
    return 0;
}