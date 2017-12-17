/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: delete file from filesystem image
 * Language:  C
 * Date : December 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: fs_del <file> <img>\n");
        printf("example: fs_del tetris fs.img\n");
        exit(EXIT_FAILURE);
    }
    do_action_to_allocated_inode(argv[1], argv[2], DELETE);
    return EXIT_SUCCESS;
}

