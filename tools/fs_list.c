/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: list all files present in filesystem image
 * Language:  C
 * Date : December 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: fs_list <img>\n");
        printf("example: fs_list fs.img\n");
        exit(1);
    }
    do_action_to_allocated_inode(NULL, argv[1], LIST);
    return 0;
}