/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: add a file to a filesystem
 * Language:  C
 * Date : December 2017
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <libgen.h>
#include "structs.h"
#include "common.h"


void find_block_and_write(uint8_t *block, FILE *img, superblock_t *sb, inode_t *inode, uint32_t block_number);
uint32_t find_free_inode(FILE *img, superblock_t *sb);
void read_block(uint32_t block_number, superblock_t *sb, FILE *img, void *dst);
void goto_block_id(FILE *img, uint32_t block_id, superblock_t *sb);
void write_block(uint32_t block_number, uint8_t *block, superblock_t *sb, FILE *img);
void check_space_and_crash_if_not_enough(superblock_t *sb, char *filename);

/**
 *
 * @param filename
 * @param img_name
 */
void fs_add(char filename[], char img_name[]) {
    superblock_t sb = {0};
    inode_t inode = {0};
    FILE *img = fopen(img_name, "r+b");
    FILE *file = fopen(filename, "rb");
    if (img != NULL && file != NULL) {
        fread(&sb, sizeof(sb), 1, img);
        check_signature_and_crash_if_not_correct(&sb);
        check_space_and_crash_if_not_enough(&sb, filename);
        uint32_t inode_id = find_free_inode(img, &sb);
        int offset = inode_id_to_offset(&sb, inode_id);
        strcpy(inode.name, basename(filename));
        uint8_t block[sb.block_size];
        memset(block, 0, sb.block_size);
        int result;
        int i = 0;
        while ((result = fread(block, 1, sb.block_size, file))) {
            inode.size += result;
            find_block_and_write(block, img, &sb, &inode, i++);
        }

        fseek(img, offset, SEEK_SET);
        fwrite(&inode, sizeof(inode), 1, img);
        fseek(img, 0, SEEK_SET);
        fwrite(&sb, sizeof(sb), 1, img);
        fclose(img);
        fclose(file);
    } else {
        printf("Error %s \n", strerror(errno));
    }
}

/**
 *
 * @param sb
 * @param filename
 */
void check_space_and_crash_if_not_enough(superblock_t *sb, char *filename) {
    if (sb->free_inodes == 0) {
        printf("all inodes are taken, please delete some files\n");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    stat(filename, &st);

    uint32_t blocks = st.st_size / sb->block_size;
    if (st.st_size % sb->block_size != 0) {
        blocks += 1;
    }
    if (blocks > DIRECT_BLOCKS) {
        blocks += 1;
    }
    if (blocks > sb->single_indirect + DIRECT_BLOCKS + 1) {
        blocks += 1 + sb->single_indirect;
    }
    if (st.st_size > (sb->direct + sb->single_indirect + sb->double_indirect) * sb->block_size) {
        printf("this file is too big for this filesystem\n");
        uint32_t min_block_size = st.st_size / (sb->direct + sb->single_indirect + sb->double_indirect);
        if (sb->block_size < MAX_BLOCK_SIZE && min_block_size <= MAX_BLOCK_SIZE) {
            printf("you can use a bigger block size\n");
        }
        exit(EXIT_FAILURE);
    }

    if (blocks > sb->block_size * 8 * sb->number_of_groups) {
        printf("this file is too big for this number of groups please add more\n");
        exit(EXIT_FAILURE);
    }

    if (blocks > sb->free_blocks) {
        printf("space left is not enough to write this file, please delete some files\n");
        exit(EXIT_FAILURE);
    }
}

/**
 *
 * @param img
 * @param sb
 * @return
 */
uint32_t find_free_inode(FILE *img, superblock_t *sb) {
    uint8_t inode_bitmap;
    uint32_t inode_id = 0;
    uint16_t group = 0;
    bool found = false;
    //jump superblock
    fseek(img, sb->block_size, SEEK_SET);
    do {
        fread(&inode_bitmap, sizeof(inode_bitmap), 1, img);
        for (int i = sizeof(inode_bitmap) * 8 - 1; i >= 0; --i) {
            if (inode_bitmap & 1 << i) {
                inode_id++;
            } else {
                fseek(img, -sizeof(inode_bitmap), SEEK_CUR);
                inode_bitmap = inode_bitmap | 1 << i;
                fwrite(&inode_bitmap, sizeof(inode_bitmap), 1, img);
                sb->free_inodes--;
                found = true;
                break;
            }
        }
        if (inode_id % (sb->block_size * 8) == 0) {
            if (group < sb->number_of_groups) {
                group++;
                //block bitmap + inode blocks + blocks
                fseek(img, sb->block_size * (1 + sizeof(inode_t) * 8 + sb->block_size * 8), SEEK_CUR);
            }
        }
    } while (!found);
    return inode_id;
}

/**
 *
 * @param img
 * @param sb
 * @return
 */
uint32_t find_free_block(FILE *img, superblock_t *sb) {
    uint8_t block_bitmap;
    uint32_t block_id = 0;
    uint16_t group = 0;
    bool found = false;
    //jump superblock and inode bitmap
    fseek(img, sb->block_size * 2, SEEK_SET);
    do {
        fread(&block_bitmap, sizeof(block_bitmap), 1, img);
        for (int i = sizeof(block_bitmap) * 8 - 1; i >= 0; --i) {
            if (block_bitmap & 1 << i) {
                block_id++;
            } else {
                fseek(img, -sizeof(block_bitmap), SEEK_CUR);
                block_bitmap = block_bitmap | 1 << i;
                fwrite(&block_bitmap, sizeof(block_bitmap), 1, img);
                sb->free_blocks--;
                found = true;
                break;
            }
        }
        if (block_id == sb->block_size * 8) {
            if (group < sb->number_of_groups) {
                group++;
                //block bitmap + inode blocks + blocks
                fseek(img, sb->block_size * (sizeof(inode_t) * 8 + sb->block_size * 8), SEEK_CUR);
            }
        }
    } while (!found);
    return (sb->block_size + //superblock
            sb->block_size * 2 * (group + 1) + //inode bitmap and block bitmap of all groups
            sb->block_size * sizeof(inode_t) * 8 * (group + 1) +  //inodes of all groups
            //sb->block_size*sb->block_size*8*group + //all blocks of previous groups
            sb->block_size * block_id) / sb->block_size; //jump to current block
}

/**
 *
 * @param block
 * @param img
 * @param sb
 * @param inode
 * @param block_number
 */
void find_block_and_write(uint8_t *block, FILE *img, superblock_t *sb, inode_t *inode, uint32_t block_number) {
    //direct
    if (block_number < sb->direct) {
        uint32_t block_id = find_free_block(img, sb);
        write_block(block_id, block, sb, img);
        inode->blocks[block_number] = block_id;
        return;
    }

    //single indirect
    block_number -= sb->direct;
    if (block_number < sb->single_indirect) {
        if (inode->blocks[INDIRECT_BLOCK] == 0) {
            inode->blocks[INDIRECT_BLOCK] = find_free_block(img, sb);
        }
        uint32_t indirect_block[sb->block_size / sizeof(inode->blocks[0])];

        read_block(inode->blocks[INDIRECT_BLOCK], sb, img, indirect_block);
        indirect_block[block_number] = find_free_block(img, sb);
        write_block(inode->blocks[INDIRECT_BLOCK], (uint8_t *) indirect_block, sb, img);

        write_block(indirect_block[block_number], block, sb, img);
        return;
    }

    //double indirect
    block_number -= sb->single_indirect;
    if (block_number < sb->double_indirect) {
        if (inode->blocks[DOUBLE_INDIRECT_BLOCK] == 0) {
            inode->blocks[DOUBLE_INDIRECT_BLOCK] = find_free_block(img, sb);
        }

        uint32_t indirect_block[sb->block_size / sizeof(inode->blocks[0])];

        read_block(inode->blocks[DOUBLE_INDIRECT_BLOCK], sb, img, indirect_block);
        if (indirect_block[block_number / sb->single_indirect] == 0) {
            indirect_block[block_number / sb->single_indirect] = find_free_block(img, sb);
            write_block(inode->blocks[DOUBLE_INDIRECT_BLOCK], (uint8_t *) indirect_block, sb, img);
        }
        uint32_t double_indirect_block[sb->block_size / sizeof(inode->blocks[0])];

        read_block(indirect_block[block_number / sb->single_indirect], sb, img, double_indirect_block);
        double_indirect_block[block_number % sb->single_indirect] = find_free_block(img, sb);
        write_block(indirect_block[block_number / sb->single_indirect], (uint8_t *) double_indirect_block, sb, img);

        write_block(double_indirect_block[block_number % sb->single_indirect], block, sb, img);
        return;
    }
}

/**
 *
 * @param block_number
 * @param block
 * @param sb
 * @param img
 */
void write_block(uint32_t block_number, uint8_t *block, superblock_t *sb, FILE *img) {
    goto_block_id(img, block_number, sb);
    fwrite(block, sb->block_size, 1, img);
}


/**
 *
 * @param block_number
 * @param sb
 * @param img
 * @param block
 */
void read_block(uint32_t block_number, superblock_t *sb, FILE *img, void *block) {
    goto_block_id(img, block_number, sb);
    fread(block, sb->block_size, 1, img);
}

/**
 *
 * @param img
 * @param block_id
 * @param sb
 */
void goto_block_id(FILE *img, uint32_t block_id, superblock_t *sb) {
    if (fseek(img, block_id * sb->block_size, SEEK_SET) == -1) {
        printf("Error %s \n", strerror(errno));
    }
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: fs_add <file> <img>\n");
        printf("example: fs_add tetris fs.img\n");
        exit(EXIT_FAILURE);
    }
    if (do_action_to_allocated_inode(basename(argv[1]), argv[2], EXISTS)) {
        printf("file already exist\n");
        exit(EXIT_FAILURE);
    }
    fs_add(argv[1], argv[2]);
    return EXIT_SUCCESS;
}