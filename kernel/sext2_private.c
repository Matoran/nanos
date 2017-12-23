/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: private functions for filesystem simple ext2 "low" level
 * Language:  C
 * Date : December 2017
 */

#include "sext2_private.h"
#include "ide.h"
#include "sext2.h"
#include "../common/memory.h"

static superblock_t *sb;

/**
 * init the superblock
 * @param superblock
 */
void sext2_private_init(superblock_t *superblock) {
    sb = superblock;
}

/**
 * read block in dst
 * @param block_id block to read 0-indexed
 * @param dst buffer with block size
 */
void read_block(uint32_t block_id, uint8_t *dst) {
    for (int j = 0; j < sb->block_size / SECTOR_SIZE; ++j) {
        uint32_t sector_id = block_id * sb->block_size / SECTOR_SIZE + j;
        int index = SECTOR_SIZE * j;
        read_sector(sector_id, dst + index);
    }
}

/**
 * search inode id of a file (inode id is 1-indexed)
 * @param filename filename to search
 * @return inode id or 0 if file doesn't exist
 */
uint32_t file_inode_id(char *filename) {
    char result[MAX_FILENAME_LENGTH];
    file_iterator_t it = file_iterator();

    while (file_has_next(&it)) {
        file_has_next(&it);
        file_next(result, &it);
        if (strcmp(filename, result) == 0) {
            return it.inode_id;
        }
    }
    return 0;
}

/**
 * read inode from an id (1-indexed)
 * @param inode_id inode number
 * @return inode
 */
inode_t read_inode(uint32_t inode_id) {
    inode_id--;
    uint8_t inode_block[sb->block_size * 2]; //some inodes are into two blocks

    uint16_t group = inode_id / (sb->block_size * 8);

    uint32_t offset = sb->block_size * (1 +
                                        2 * (group + 1) + //inode bitmap and block bitmap of groups
                                        sb->block_size * 8 * group) + //blocks of previous groups
                      inode_id * sizeof(inode_t);
    uint32_t block_id = offset / sb->block_size;
    read_block(block_id, inode_block);
    read_block(block_id + 1, inode_block + sb->block_size);
    inode_t result = {0};
    memcpy(&result, inode_block + offset % sb->block_size, sizeof(inode_t));
    return result;
}

/**
 * find global block id from block number
 * @param inode inode we want read
 * @param block_number block of the file that you want, 0, 1, 2, 3, .., 110
 * @return block id
 */
uint32_t bmap(inode_t *inode, uint32_t block_number) {
    if (block_number < sb->direct) {
        return inode->blocks[block_number];
    }
    if (inode->blocks[INDIRECT_BLOCK] == 0) {
        return 0;
    }
    block_number -= sb->direct;
    if (block_number < sb->single_indirect) {
        uint8_t block[sb->block_size];
        read_block(inode->blocks[INDIRECT_BLOCK], block);
        memcpy(&block_number, &block[block_number], sizeof(block_number));
        return block_number;
    }
    if (inode->blocks[DOUBLE_INDIRECT_BLOCK] == 0) {
        return 0;
    }
    block_number -= sb->single_indirect;
    if (block_number < sb->double_indirect) {
        uint8_t block[sb->block_size];
        read_block(inode->blocks[DOUBLE_INDIRECT_BLOCK], block);
        uint32_t double_indirect;
        memcpy(&double_indirect, &block[block_number / sb->single_indirect], sizeof(double_indirect));
        if (double_indirect) {
            return 0;
        }
        read_block(double_indirect, block);
        memcpy(&block_number, &block[block_number % sb->single_indirect], sizeof(block_number));
        return block_number;
    }
    return 0;
}

/**
 * calc block id of inode bitmap from a group
 * @param group
 * @return block id of inode bitmap
 */
uint32_t block_id_inode_bitmap_from_group(uint16_t group) {
    return 1 +
           group * 2 + //inode bitmap and block bitmap previous groups
           sizeof(inode_t) * 8 * group + //inodes of previous groups
           sb->block_size * 8 * group; // blocks of previous groups
}