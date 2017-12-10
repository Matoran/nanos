#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "structs.h"
#include "common.h"

void
show_filenames(char *filename, __uint32_t position, FILE *img, superblock_t sb, long offset, __uint32_t inode_bitmap,
               __uint8_t current_bit);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: fs_list <img>\n");
        printf("example: fs_list fs.img\n");
        exit(1);
    }
    do_action_to_allocated_inode(NULL, argv[1], show_filenames);
    return 0;
}

void
show_filenames(char *filename, __uint32_t position, FILE *img, superblock_t sb, long offset, __uint32_t inode_bitmap,
               __uint8_t current_bit) {
    printf("%s\n", get_inode(position, img, sb).name);
    fseek(img, offset + sizeof(inode_bitmap), SEEK_SET);
}