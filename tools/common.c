/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: common functions for filesystem tools
 * Language:  C
 * Date : December 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"

void *calloc_error(size_t __nmemb, size_t __size) {
    int *result = calloc(__nmemb, __size);
    if (result == NULL) {
        printf("error calloc");
        exit(4);
    } else {
        return result;
    }
}

uint32_t inode_id_to_offset(superblock_t sb, uint32_t inode_id) {
    uint16_t group = inode_id / (sb.block_size * 8);
    return sb.block_size + //superblock
           sb.block_size * 2 * (group + 1) + //inode bitmap and block bitmap of all groups
           sb.block_size * sizeof(inode_t) * 8 * group + //all previous inode groups
           inode_id * sizeof(inode_t) + //jump to current inode
           sb.block_size * sb.block_size * 8 * group; //all previous block groups;
}

void do_action_to_allocated_inode(char *filename, char *img_name, Action action) {
    superblock_t sb = {0};
    FILE *img = fopen(img_name, "r+b");
    if (img != NULL) {
        fread(&sb, sizeof(sb), 1, img);
        //jump superblock
        fseek(img, sb.block_size, SEEK_SET);
        uint32_t position = 0;
        uint32_t inode_bitmap;
        long offset;
        uint16_t group = 0;
        do {
            offset = ftell(img);
            fread(&inode_bitmap, sizeof(inode_bitmap), 1, img);
            for (uint8_t i = sizeof(inode_bitmap) * 8; i > 0; --i) {
                if (inode_bitmap & 1 << (i - 1)) {
                    switch (action) {
                        case LIST:
                            printf("%s\n", get_inode(position, img, sb).name);
                            fseek(img, offset + sizeof(inode_bitmap), SEEK_SET);
                            break;
                        case DELETE:
                            if (strcmp(filename, get_inode(position, img, sb).name) == 0) {
                                fseek(img, offset, SEEK_SET);
                                inode_bitmap = inode_bitmap & ~(1 << (i - 1));
                                fwrite(&inode_bitmap, sizeof(inode_bitmap), 1, img);
                                printf("found and deleted\n");
                            }
                            break;
                    }
                }
                position++;
            }
            if (position % sb.block_size * 8 == 0) {
                if (group < sb.number_of_groups - 1) {
                    group++;
                    //block bitmap + inode blocks + blocks
                    fseek(img, sb.block_size * (1 + sizeof(inode_t) * 8 + sb.block_size * 8), SEEK_CUR);
                } else {
                    printf("end action to allocated inodes\n");
                    break;
                }
            }
        } while (true);
    }
}

inode_t get_inode(uint32_t position, FILE *img, superblock_t sb) {
    fseek(img, inode_id_to_offset(sb, position), SEEK_SET);
    inode_t inode;
    fread(&inode, sizeof(inode_t), 1, img);
    return inode;
}
