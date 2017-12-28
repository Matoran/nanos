#ifndef NANOS_SEXT2_PRIVATE_H
#define NANOS_SEXT2_PRIVATE_H

#include <stdbool.h>
#include "../tools/structs.h"

#define MAX_FDS 256

typedef struct file_descriptor_st {
    bool open;
    uint32_t inode_id;
    uint32_t offset;
} file_descriptor_t;

void sext2_private_init(superblock_t *superblock);
void read_block(uint32_t block_id, uint8_t *dst);
inode_t read_inode(uint32_t inode_id);
uint32_t bmap(inode_t *inode, uint32_t block_number);
uint32_t file_inode_id(char *filename);
uint32_t block_id_inode_bitmap_from_group(uint16_t group);


#endif //NANOS_SEXT2_PRIVATE_H
