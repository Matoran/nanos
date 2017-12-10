#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>
#include "structs.h"
#include "common.h"





void fs_create(char label[], __uint16_t block_size, char name[], __uint32_t number_of_groups){
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
    FILE * file= fopen(name, "wb");
    if (file != NULL) {
        fwrite(&superblock, sizeof(superblock), 1, file);
        int padding = block_size-sizeof(superblock);
        if(padding < 0){
            printf("error superblock is bigger than %d Bytes", block_size);
            exit(3);
        }else{
            __uint8_t *padding_superblock = calloc_error(padding, sizeof(__uint8_t));
            fwrite(padding_superblock, padding, 1, file); //inode bitmap
        }
        __uint8_t *block = calloc_error(block_size, sizeof(__uint8_t));
        for (int i = 0; i < number_of_groups; ++i) {
            fwrite(block, block_size, 1, file); //inode bitmap
            fwrite(block, block_size, 1, file); //block bitmap
            for (int j = 0; j < block_size * 8; ++j) {
                fwrite(&inode, sizeof(inode_t), 1, file); //create inode
            }
            for (int j = 0; j < block_size * 8; ++j) {
                fwrite(block, block_size, 1, file); //create block
            }
        }

        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    if(argc < 5){
        printf("arg0 block size needs to be multiple of 512\n");
        printf("usage: fs_create <label> <block size> <img name> <number of groups>\n");
        printf("example: fs_create next2 1024 fs.img 1, will create a 8MiB usable blocks\n");
        printf("you can calculate it: block_size^2*8*number_of_groups");
        exit(1);
    }
    char *label = argv[1];
    __uint16_t block_size = atoi(argv[2]);
    char *name = argv[3];
    __uint32_t number_of_groups = atoi(argv[4]);
    if(block_size % 512 != 0){
        printf("block size need to be multiple of 512\n");
        exit(2);
    }
    fs_create(label, block_size, name, number_of_groups);
    return 0;
}