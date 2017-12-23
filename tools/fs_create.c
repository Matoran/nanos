/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: create a new filesystem image
 * Language:  C
 * Date : December 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include "structs.h"

/**
 * create filesystem img
 * @param label label ot the disk
 * @param block_size block size
 * @param name name of the filesystem img
 * @param number_of_groups number of groups in the filesystem
 */
void fs_create(char label[], uint16_t block_size, char name[], uint16_t number_of_groups) {
    superblock_t superblock = {0};
    inode_t inode = {0};
    strcpy(superblock.name, label);
    superblock.signature = SIGNATURE;
    superblock.version = 0;
    superblock.block_size = block_size;
    superblock.number_of_groups = number_of_groups;
    superblock.direct = DIRECT_BLOCKS;
    superblock.single_indirect = block_size / sizeof(inode.blocks[0]);
    superblock.double_indirect = superblock.single_indirect * superblock.single_indirect;
    superblock.free_blocks = block_size * 8 * number_of_groups;
    superblock.free_inodes = block_size * 8 * number_of_groups;
    FILE *file = fopen(name, "wb");
    if (file != NULL) {
        fwrite(&superblock, sizeof(superblock), 1, file);
        int padding = block_size - sizeof(superblock);
        if (padding < 0) {
            printf("error superblock is bigger than %d Bytes", block_size);
            exit(EXIT_FAILURE);
        }
        uint8_t padding_superblock[padding];
        memset(padding_superblock, 0, padding);
        fwrite(padding_superblock, padding, 1, file); //inode bitmap
        uint8_t block[block_size];
        memset(block, 0, block_size);
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

/**
 * create filesystem img if arguments are correct
 * @param argc arguments count
 * @param argv arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("block size needs to be multiple of 512 and maximum 4096\n");
        printf("usage: fs_create <label> <block size> <img name> <number of groups>\n");
        printf("example: fs_create next2 1024 fs.img 1, will create a 8MiB usable blocks\n");
        printf("you can calculate it: block_size^2*8*number_of_groups\n");
        exit(EXIT_FAILURE);
    }
    char *label = argv[1];
    uint16_t block_size = atoi(argv[2]);
    char *name = argv[3];
    uint16_t number_of_groups = atoi(argv[4]);
    if (block_size < MIN_BLOCK_SIZE || block_size > MAX_BLOCK_SIZE) {
        printf("block size need to be >= 512 and <= 4096\n");
        exit(EXIT_FAILURE);
    }
    if (block_size % MIN_BLOCK_SIZE != 0) {
        printf("block size need to be multiple of 512\n");
        exit(EXIT_FAILURE);
    }
    if (!access(name, F_OK) && !(argc == 6 && strcmp(argv[5], "!") == 0)) {
        printf("file already exist, if you want overwrite it please add ! at the end of your command\n");
        printf("example: fs_create next2 1024 fs.img 1 !\n");
        exit(EXIT_FAILURE);
    }
    fs_create(label, block_size, name, number_of_groups);
    return EXIT_SUCCESS;
}