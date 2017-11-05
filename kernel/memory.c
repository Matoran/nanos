/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Base function
 * Language:  C
 * Date : October/November 2017
 */

#include "../common/memory.h"

/**
 * Set the value in the destination
 * @param dst address destination
 * @param value
 * @param count number of bytes
 * @return destination
 */
void *memset(void *dst, int value, uint count){
    char *d = dst;
    for (uint i = 0; i < count; ++i) {
        *d++ = value;
    }
    return dst;
}

/**
 * Copy source to destination
 * @param dst pointer to the destination
 * @param src pointer to the source
 * @param count number bytes to copy
 * @return destination
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