#include <stdlib.h>
#include <stdio.h>
#include "common.h"

void *calloc_error (size_t __nmemb, size_t __size){
    int *result = calloc(__nmemb, __size);
    if(result == NULL){
        printf("error calloc");
        exit(4);
    }else{
        return result;
    }
}
