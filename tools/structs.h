//
// Created by matoran on 12/9/17.
//

#ifndef FS_CREATE_STRUCTS_H
#define FS_CREATE_STRUCTS_H

#define MAX_LENGTH_NAME 30
#define NUMBER_OF_BLOCKS 10
#define DIRECT_BLOCKS (NUMBER_OF_BLOCKS-2)

typedef struct superblock_st{
    __uint16_t signature;
    __uint8_t version;
    char name[252];
    __uint16_t block_size;
    __uint32_t number_of_groups;
    __uint32_t direct;
    __uint32_t single_indirect;
    __uint32_t double_indirect;
} __attribute__((packed))superblock_t;

typedef struct inode_st{
    char name[MAX_LENGTH_NAME];
    __uint32_t size;
    __uint32_t blocks[NUMBER_OF_BLOCKS]; // 8 simple indirect, 9 double indirect
} __attribute__((packed))inode_t;

#endif //FS_CREATE_STRUCTS_H
