#ifndef FS_CREATE_STRUCTS_H
#define FS_CREATE_STRUCTS_H


#include <stdint.h>

#define MAX_FILENAME_LENGTH 30
#define NUMBER_OF_BLOCKS 10
#define DIRECT_BLOCKS (NUMBER_OF_BLOCKS-2)
#define INDIRECT_BLOCK (NUMBER_OF_BLOCKS-2)
#define DOUBLE_INDIRECT_BLOCK (NUMBER_OF_BLOCKS-1)
#define SIGNATURE 0x5E72
#define MAX_BLOCK_SIZE 4096
#define MIN_BLOCK_SIZE 512

typedef struct superblock_st {
    uint16_t signature;
    uint8_t version;
    char name[256];
    uint16_t block_size;
    uint32_t number_of_groups;
    uint32_t direct;
    uint32_t single_indirect;
    uint32_t double_indirect;
    uint32_t free_inodes;
    uint32_t free_blocks;
    char padding[227];
} __attribute__((packed)) superblock_t;

typedef struct inode_st {
    char name[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t blocks[NUMBER_OF_BLOCKS]; // 8 simple indirect, 9 double indirect
} __attribute__((packed)) inode_t;

#endif //FS_CREATE_STRUCTS_H
