#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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

__uint32_t inode_id_to_offset(superblock_t sb, __uint32_t inode_id) {
    __uint16_t group = inode_id / (sb.block_size * 8);
    return sb.block_size + //superblock
           sb.block_size * 2 * (group + 1) + //inode bitmap and block bitmap of all groups
           sb.block_size * sizeof(inode_t) * 8 * group + //all previous inode groups
           inode_id * sizeof(inode_t) + //jump to current inode
           sb.block_size * sb.block_size * 8 * group; //all previous block groups;
}

void do_action_to_allocated_inode(char *filename, char *img_name,
                                  void(*function)(char *, __uint32_t, FILE *, superblock_t, long, __uint32_t,
                                                  __uint8_t)) {
    superblock_t sb = {0};
    inode_t inode = {0};
    FILE *img = fopen(img_name, "r+b");
    if (img != NULL) {
        fread(&sb, sizeof(sb), 1, img);
        //jump superblock
        fseek(img, sb.block_size, SEEK_SET);
        __uint32_t position = 0;
        __uint32_t inode_bitmap;
        long offset;
        __uint16_t group = 0;
        do {
            offset = ftell(img);
            fread(&inode_bitmap, sizeof(inode_bitmap), 1, img);
            for (__uint8_t i = sizeof(inode_bitmap) * 8; i > 0; --i) {
                if (inode_bitmap & 1 << i - 1) {
                    function(filename, position, img, sb, offset, inode_bitmap, i - 1);
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

inode_t get_inode(__uint32_t position, FILE *img, superblock_t sb) {
    fseek(img, inode_id_to_offset(sb, position), SEEK_SET);
    inode_t inode;
    fread(&inode, sizeof(inode_t), 1, img);
    return inode;
}
