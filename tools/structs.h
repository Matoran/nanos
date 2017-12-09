//
// Created by matoran on 12/9/17.
//

#ifndef FS_CREATE_STRUCTS_H
#define FS_CREATE_STRUCTS_H

typedef struct superblock_st{
    __uint16_t signature;
    __uint8_t version;
    char name[252];
    __uint16_t block_size;
    __uint32_t number_of_groups;
} __attribute__((packed))superblock_t;

typedef struct inode_st{
    char name[30];
    __uint8_t size;
    __uint32_t blocks[10]; // 8 simple indirect, 9 double indirect
} __attribute__((packed))inode_t;

#endif //FS_CREATE_STRUCTS_H
