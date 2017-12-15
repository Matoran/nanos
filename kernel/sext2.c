//
// Created by matoran on 12/13/17.
//

#include "sext2.h"
#include "ide.h"
#include "../tools/structs.h"
#include "console.h"
#include "../common/memory.h"
#include "timer.h"


inode_t read_inode(uint32_t inode_id);


void read_block(int block, uint8_t *dst) {
    /*for (int j = 0; j < sb.block_size / SECTOR_SIZE; ++j) {
        int block_id = block * sb.block_size / SECTOR_SIZE + j;
        int index = SECTOR_SIZE * j;
        read_sector(block_id, dst + index);
    }*/
}

int file_stat(char *filename, stat_t *stat) {
    return -1;
}

void recursion(int a) {
    printf("%d\n", a);
    recursion(a + 1);
}

inode_t read_inode(uint32_t inode_id) {
    superblock_t sb;
    read_sector(0, &sb);
    uint8_t inode_block[2048]; //some inodes are into two blocks

    uint16_t group = inode_id / (sb.block_size * 8);

    uint32_t block_id = 1 +
                        2 * (group + 1) + //inode bitmap and block bitmap of groups
                        sb.block_size * 8 * group + //blocks of previous groups
                        inode_id * sizeof(inode_t) / sb.block_size;
    read_sector(block_id * 2, inode_block);
    read_sector(block_id * 2 + 1, inode_block + SECTOR_SIZE);
    read_sector(block_id * 2 + 2, inode_block + SECTOR_SIZE * 2);
    read_sector(block_id * 2 + 3, inode_block + SECTOR_SIZE * 3);
    recursion(0);
    inode_id %= (sb.block_size * 8);
    inode_t result = {0};
    memcpy(&result, inode_block + sizeof(inode_t) * inode_id, sizeof(inode_t));
    return result;
}



bool file_exists(char *filename) {
    superblock_t sb;
    read_sector(0, &sb);
    uint8_t inode_bitmap_block[1024];
    inode_bitmap_block[0] = 0;
    read_sector(2, &inode_bitmap_block);
    //read_sector(3, inode_bitmap_block+SECTOR_SIZE);
    /*uint32_t inode_id = 0;
    uint8_t inode_bitmap;
    uint16_t group = 0;
    do {
        for (int j = 0; j < sizeof(inode_bitmap_block); ++j) {
            inode_bitmap = inode_bitmap_block[j];
            for (uint8_t i = sizeof(inode_bitmap) * 8; i > 0; --i) {
                if (inode_bitmap & 1 << (i - 1)) {
                    inode_t inode = read_inode(inode_id);
                    if(strcmp(inode.name, filename) == 0){
                        return true;
                    }
                }
                inode_id++;
            }
        }
        if (group < sb.number_of_groups - 1) {
            group++;
            read_block(1 +
                       group * 2 + //inode bitmap and block bitmap previous groups
                       sizeof(inode_t) * 8 * group + //inodes of previous groups
                       sb.block_size * 8 * group // blocks of previous groups
                    , inode_bitmap_block);
        } else {
            printf("end action to allocated inodes\n");
            break;
        }
    } while (true);
    return false;*/
}

int file_open(char *filename) {
    return -1;
}

int file_read(int fd, void *buf, uint count) {
    return -1;
}

int file_seek(int fd, uint offset) {
    return -1;
}

void file_close(int fd) {
}

file_iterator_t file_iterator() {
}

bool file_has_next(file_iterator_t *it) {
    return -1;
}

void file_next(char *filename, file_iterator_t *it) {
}
