/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: "driver" for filesystem simple ext2
 * Language:  C
 * Date : December 2017
 */


#include "sext2.h"
#include "ide.h"
#include "../common/memory.h"
#include "console.h"
#include "sext2_private.h"

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))

static superblock_t sb;
static file_descriptors_t fds = {0};

/**
 * read superblock and init private section
 */
void sext2_init() {
    uint8_t superblock[MIN_BLOCK_SIZE];
    read_sector(0, superblock);
    memcpy(&sb, superblock, sizeof(sb));
    sext2_private_init(&sb);
}

/**
 * read some informations of a file
 * @param filename to stat
 * @param stat stat_t pointer previously declared
 * @return -1 if file doesn't exist 0 otherwise
 */
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
 * check if file exists
 * @param filename
 * @return bool
 */
bool file_exists(char *filename) {
    return file_inode_id(filename) != 0;
}

/**
 * open file if exist return -1 otherwise
 * @param filename to open
 * @return file descriptor or -1 if error
 */
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

/**
 * read from a file count bytes and write it on buffer
 * @param fd file descriptor
 * @param buf buffer to write
 * @param count number of byte to write
 * @return -1 if error, number of bytes really read otherwise
 */
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

/**
 * change offset of file descriptor to offset
 * @param fd file descriptor
 * @param offset new offset of the file
 * @return -1 if error, new offset otherwise
 */
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

/**
 * close file and free file descriptor
 * @param fd file descriptor
 */
void file_close(int fd) {
    memset(&fds.fds[fd], 0, sizeof(file_descriptor_t));
    fds.free_fds[fds.number_free_fds++] = fd;
}

/**
 * create a new file iterator
 * @return file_iterator_t
 */
file_iterator_t file_iterator() {
    file_iterator_t file = {0};
    return file;
}

/**
 * check if file iterator has a next file
 * @param it file_iterator_t
 * @return bool
 */
bool file_has_next(file_iterator_t *it) {
    uint8_t inode_bitmap_block[sb.block_size];
    uint8_t inode_bitmap;
    do {
        uint32_t block_id = block_id_inode_bitmap_from_group(it->group);
        read_block(block_id, inode_bitmap_block);
        uint16_t index_in_block = it->inode_id % (sb.block_size * 8) / 8;
        for (; index_in_block < sizeof(inode_bitmap_block); ++index_in_block) {
            inode_bitmap = inode_bitmap_block[index_in_block];
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

/**
 * copy inode name to filename
 * @param filename buffer previously declared
 * @param it file_iterator_t
 */
void file_next(char *filename, file_iterator_t *it) {
    uint8_t inode_bitmap_block[sb.block_size];
    uint8_t inode_bitmap;
    do {
        uint32_t block_id = block_id_inode_bitmap_from_group(it->group);
        read_block(block_id, inode_bitmap_block);
        uint16_t index_in_block = it->inode_id % (sb.block_size * 8) / 8;
        for (; index_in_block < sizeof(inode_bitmap_block); ++index_in_block) {
            inode_bitmap = inode_bitmap_block[index_in_block];
            for (uint8_t i = sizeof(inode_bitmap) * 8 - it->inode_id % 8; i > 0; --i) {
                if (inode_bitmap & 1 << (i - 1)) {
                    inode_t inode = read_inode(it->inode_id + 1);
                    memcpy(filename, inode.name, MAX_FILENAME_LENGTH);
                    it->inode_id++;
                    it->group = it->inode_id / (sb.block_size * 8);
                    return;
                }
                it->inode_id++;
            }
        }
        it->group++;
    } while (it->group < sb.number_of_groups);
}
