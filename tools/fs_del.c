#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "structs.h"
#include "common.h"

void delete_inode(char *filename, __uint32_t position, FILE *img, superblock_t sb, long offset, __uint32_t inode_bitmap,
                  __uint8_t current_bit);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: fs_del <file> <img>\n");
        printf("example: fs_del tetris fs.img\n");
        exit(1);
    }
    do_action_to_allocated_inode(argv[1], argv[2], delete_inode);
    return 0;
}

void delete_inode(char *filename, __uint32_t position, FILE *img, superblock_t sb, long offset, __uint32_t inode_bitmap,
                  __uint8_t current_bit) {
    if (strcmp(filename, get_inode(position, img, sb).name) == 0) {
        fseek(img, offset, SEEK_SET);
        inode_bitmap = inode_bitmap & ~(1 << current_bit);
        fwrite(&inode_bitmap, sizeof(inode_bitmap), 1, img);
        printf("found and deleted\n");
    }
}

