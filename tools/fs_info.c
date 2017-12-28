/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: show informations of filesystem image
 * Language:  C
 * Date : December 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include "common.h"

/**
 * show informations of filesystem img
 * @param argc arguments count
 * @param argv arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: fs_info <img>\n");
        printf("example: fs_info fs.img\n");
        exit(EXIT_FAILURE);
    }
    superblock_t sb = {0};
    FILE *img = fopen(argv[1], "rb");
    if (img != NULL) {
        fread(&sb, sizeof(sb), 1, img);
        check_signature_and_crash_if_not_correct(&sb);
        printf("signature sext2: %x\n", sb.signature);
        printf("label: %s\n", sb.name);
        printf("version: %d\n", sb.version);
        printf("free space in bytes: %d\n", sb.free_blocks * sb.block_size);
        printf("free inodes: %d\n", sb.free_inodes);
        fclose(img);
    } else {
        printf("Error %s \n", strerror(errno));
    }
    return EXIT_SUCCESS;
}