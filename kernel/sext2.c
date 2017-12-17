//
// Created by matoran on 12/13/17.
//


#include "sext2.h"
#include "ide.h"
#include "../common/memory.h"
#include "console.h"

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))


#define MAX_FDS 256

typedef struct file_descriptor_st {
    bool open;
    uint32_t inode_id;
    uint32_t offset;
} file_descriptor_t;

typedef struct file_descriptors_st {
    file_descriptor_t fds[MAX_FDS];
    uint8_t free_fds[MAX_FDS];
    uint16_t last_fds_open;
    uint16_t number_free_fds;
} file_descriptors_t;


static superblock_t sb;
static file_descriptors_t fds = {0};


inode_t read_inode(uint32_t inode_id);

uint32_t bmap(inode_t *inode, uint32_t block_id);

void sext2_init() {
    read_sector(0, &sb);
}

void read_block(int block, uint8_t *dst) {
    for (int j = 0; j < sb.block_size / SECTOR_SIZE; ++j) {
        int block_id = block * sb.block_size / SECTOR_SIZE + j;
        int index = SECTOR_SIZE * j;
        read_sector(block_id, dst + index);
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

int file_stat(char *filename, stat_t *stat) {
    uint32_t inode_id = file_inode_id(filename);
    if (!inode_id) {
        return -1;
    }
    inode_t inode = read_inode(inode_id);
    stat->size = inode.size;
    return 0;
}

/**
 * read inode from an id (1-indexed)
 * @param inode_id inode number
 * @return inode
 */
inode_t read_inode(uint32_t inode_id) {
    inode_id--;
    uint8_t inode_block[sb.block_size * 2]; //some inodes are into two blocks

    uint16_t group = inode_id / (sb.block_size * 8);

    uint32_t block_id = 1 +
                        2 * (group + 1) + //inode bitmap and block bitmap of groups
                        sb.block_size * 8 * group + //blocks of previous groups
                        inode_id * sizeof(inode_t) / sb.block_size;
    read_block(block_id, inode_block);
    read_block(block_id + 1, inode_block + sb.block_size);
    inode_id %= (sb.block_size * 8);
    inode_t result = {0};
    memcpy(&result, inode_block + sizeof(inode_t) * inode_id, sizeof(inode_t));
    return result;
}


bool file_exists(char *filename) {
    return file_inode_id(filename) == 0;
}

int file_open(char *filename) {
    uint32_t inode_id = file_inode_id(filename);
    if (!inode_id) {
        return -1;
    }
    if (fds.number_free_fds > 0) {
        fds.number_free_fds--;
        fds.fds[fds.number_free_fds].offset = 0;
        fds.fds[fds.number_free_fds].inode_id = inode_id;
        fds.fds[fds.number_free_fds].open = true;
        return fds.free_fds[fds.number_free_fds];
    } else if (fds.last_fds_open < MAX_FDS) {
        fds.fds[fds.last_fds_open].offset = 0;
        fds.fds[fds.last_fds_open].inode_id = inode_id;
        fds.fds[fds.last_fds_open].open = true;
        return fds.last_fds_open++;
    } else {
        printf("You have already open %d files, it's the maximum for this system.\n", MAX_FDS);
    }
    return -1;
}

int file_read(int fd, void *buf, uint count) {
    file_descriptor_t file_descriptor = fds.fds[fd];
    if (!file_descriptor.open) {
        return -1;
    }
    inode_t inode = read_inode(file_descriptor.inode_id);
    uint32_t total_left = MIN(inode.size - file_descriptor.offset, count);
    uint32_t read = total_left;
    uint32_t buffer_offset = 0;
    uint8_t block[sb.block_size];
    while (total_left > 0) {
        uint32_t block_id = bmap(&inode, file_descriptor.offset / sb.block_size);
        uint32_t offset_in_block = file_descriptor.offset % sb.block_size;
        uint32_t to_read = MIN(sb.block_size - offset_in_block, total_left);
        if (block_id != 0) {
            read_block(block_id, block);
            memcpy(buf, block, to_read);
            buf += to_read;
        }
        file_descriptor.offset += to_read;
        buffer_offset += to_read;
        total_left -= to_read;
    }
    return read;
}

uint32_t bmap(inode_t *inode, uint32_t block_id) {
    if (block_id < sb.direct) {
        return inode->blocks[block_id];
    }
    if (inode->blocks[INDIRECT_BLOCK] == 0) {
        return 0;
    }
    block_id -= sb.direct;
    if (block_id < sb.single_indirect) {
        uint8_t block[sb.block_size];
        read_block(inode->blocks[INDIRECT_BLOCK], block);
        memcpy(&block_id, &block[block_id], sizeof(block_id));
        return block_id;
    }
    if (inode->blocks[DOUBLE_INDIRECT_BLOCK] == 0) {
        return 0;
    }
    block_id -= sb.single_indirect;
    if (block_id < sb.double_indirect) {
        uint8_t block[sb.block_size];
        read_block(inode->blocks[DOUBLE_INDIRECT_BLOCK], block);
        uint32_t double_indirect;
        memcpy(&double_indirect, &block[block_id / sb.single_indirect], sizeof(double_indirect));
        if (double_indirect) {
            return 0;
        }
        read_block(double_indirect, block);
        memcpy(&block_id, &block[block_id % sb.single_indirect], sizeof(block_id));
        return block_id;
    }
    return 0;
}

int file_seek(int fd, uint offset) {
    if (!fds.fds[fd].open) {
        return -1;
    }
    inode_t inode = read_inode(fds.fds[fd].inode_id);
    if (offset > inode.size) {
        return -1;
    }
    fds.fds[fd].offset = offset;
    return offset;
}

void file_close(int fd) {
    memset(&fds.fds[fd], 0, sizeof(file_descriptor_t));
    fds.free_fds[fds.number_free_fds++] = fd;
}

file_iterator_t file_iterator() {
    file_iterator_t file = {0};
    return file;
}

uint32_t block_id_inode_bitmap_from_group(uint16_t group) {
    return 1 +
           group * 2 + //inode bitmap and block bitmap previous groups
           sizeof(inode_t) * 8 * group + //inodes of previous groups
           sb.block_size * 8 * group; // blocks of previous groups
}

bool file_has_next(file_iterator_t *it) {
    uint8_t inode_bitmap_block[sb.block_size];
    uint8_t inode_bitmap;
    do {
        uint32_t block_id = block_id_inode_bitmap_from_group(it->group);
        read_block(block_id, inode_bitmap_block);
        for (uint16_t j = it->inode_id / 8; j < sizeof(inode_bitmap_block); ++j) {
            inode_bitmap = inode_bitmap_block[j];
            for (uint8_t i = sizeof(inode_bitmap) * 8 - it->inode_id % 8; i > 0; --i) {
                if (inode_bitmap & 1 << (i - 1)) {
                    return true;
                }
                it->inode_id++;
            }
        }
        it->group++;
    } while (it->group < sb.number_of_groups);
    return false;
}

void file_next(char *filename, file_iterator_t *it) {
    uint8_t inode_bitmap_block[sb.block_size];
    uint8_t inode_bitmap;
    do {
        uint32_t block_id = block_id_inode_bitmap_from_group(it->group);
        read_block(block_id, inode_bitmap_block);
        for (uint16_t j = it->inode_id / 8; j < sizeof(inode_bitmap_block); ++j) {
            inode_bitmap = inode_bitmap_block[j];
            for (uint8_t i = sizeof(inode_bitmap) * 8 - it->inode_id % 8; i > 0; --i) {
                if (inode_bitmap & 1 << (i - 1)) {
                    inode_t inode = read_inode(it->inode_id + 1);
                    memcpy(filename, inode.name, MAX_FILENAME_LENGTH);
                    it->inode_id++;
                    return;
                }
                it->inode_id++;
            }
        }
        it->group++;
    } while (it->group < sb.number_of_groups);
}
