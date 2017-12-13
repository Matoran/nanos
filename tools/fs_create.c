/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: create a new filesystem image
 * Language:  C
 * Date : December 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "common.h"

void fs_create(char label[], uint16_t block_size, char name[], uint16_t number_of_groups) {
    superblock_t superblock = {0};
    inode_t inode = {0};
    strcpy(superblock.name, label);
    superblock.signature = 0x2ACE;
    superblock.version = 0;
    superblock.block_size = block_size;
    superblock.number_of_groups = number_of_groups;
    superblock.direct = DIRECT_BLOCKS;
    superblock.single_indirect = block_size / sizeof(inode.blocks[0]);
    superblock.double_indirect = superblock.single_indirect * superblock.single_indirect;
    FILE *file = fopen(name, "wb");
    if (file != NULL) {
        fwrite(&superblock, sizeof(superblock), 1, file);
        int padding = block_size - sizeof(superblock);
        if (padding < 0) {
            printf("error superblock is bigger than %d Bytes", block_size);
            exit(3);
        } else {
            uint8_t *padding_superblock = calloc_error(padding, sizeof(uint8_t));
            fwrite(padding_superblock, padding, 1, file); //inode bitmap
        }
        uint8_t *block = calloc_error(block_size, sizeof(uint8_t));
        for (uint32_t i = 0; i < number_of_groups; ++i) {
            fwrite(block, block_size, 1, file); //inode bitmap
            fwrite(block, block_size, 1, file); //block bitmap
            for (uint32_t j = 0; j < block_size * 8; ++j) {
                fwrite(&inode, sizeof(inode_t), 1, file); //create inode
            }
            for (uint32_t j = 0; j < block_size * 8; ++j) {
                fwrite(block, block_size, 1, file); //create block
            }
        }

        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("arg0 block size needs to be multiple of 512\n");
        printf("usage: fs_create <label> <block size> <img name> <number of groups>\n");
        printf("example: fs_create next2 1024 fs.img 1, will create a 8MiB usable blocks\n");
        printf("you can calculate it: block_size^2*8*number_of_groups\n");
        exit(1);
    }
    char *label = argv[1];
    uint16_t block_size = atoi(argv[2]);
    char *name = argv[3];
    uint32_t number_of_groups = atoi(argv[4]);
    if (block_size % 512 != 0) {
        printf("block size need to be multiple of 512\n");
        exit(2);
    }
    fs_create(label, block_size, name, number_of_groups);
    return 0;
}