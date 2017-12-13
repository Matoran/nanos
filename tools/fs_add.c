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
#include "structs.h"
#include "common.h"

void find_block_and_write(uint8_t *block, FILE *img, superblock_t sb, inode_t *inode, uint32_t block_number);
uint32_t find_free_inode(FILE *img, superblock_t superblock);
uint32_t *read_block(uint32_t block_number, superblock_t sb, FILE *img);
void goto_block_id(FILE *img, uint32_t block_id, superblock_t sb);
void write_block(uint32_t block_number, uint8_t *block, superblock_t sb, FILE *img);

void fs_add(char filename[], char img_name[]) {
    superblock_t sb = {0};
    inode_t inode = {0};
    FILE *img = fopen(img_name, "r+b");
    FILE *file = fopen(filename, "rb");
    if (img != NULL && file != NULL) {
        fread(&sb, sizeof(sb), 1, img);
        uint32_t inode_id = find_free_inode(img, sb);
        int offset = inode_id_to_offset(sb, inode_id);
        strcpy(inode.name, filename);
        uint8_t *block = calloc_error(sb.block_size, sizeof(uint8_t));
        int result;
        for (int i = 0; i < NUMBER_OF_BLOCKS; ++i) {
            inode.blocks[i] = 0;
        }
        int i = 0;
        while ((result = fread(block, 1, sb.block_size, file))) {
            printf("return fread = %d\n", result);
            inode.size += result;
            find_block_and_write(block, img, sb, &inode, i++);
        }

        fseek(img, offset, SEEK_SET);
        fwrite(&inode, sizeof(inode), 1, img);
        fclose(img);
    } else {
        printf("Error %s \n", strerror(errno));
    }
}

uint32_t find_free_inode(FILE *img, superblock_t superblock) {
    uint32_t inode_bitmap;
    uint32_t position = 0;
    uint16_t group = 0;
    bool found = false;
    //jump superblock
    fseek(img, superblock.block_size, SEEK_SET);
    do {
        fread(&inode_bitmap, sizeof(inode_bitmap), 1, img);
        for (int i = sizeof(inode_bitmap) * 8 - 1; i >= 0; --i) {
            if (inode_bitmap & 1 << i) {
                position++;
            } else {
                fseek(img, -sizeof(inode_bitmap), SEEK_CUR);
                inode_bitmap = inode_bitmap | 1 << i;
                fwrite(&inode_bitmap, sizeof(inode_bitmap), 1, img);
                found = true;
                break;
            }
        }
        if (position % superblock.block_size * 8 == 0) {
            if (group < superblock.number_of_groups) {
                group++;
                //block bitmap + inode blocks + blocks
                fseek(img, superblock.block_size * (1 + sizeof(inode_t) * 8 + superblock.block_size * 8), SEEK_CUR);
            }
        }
    } while (!found);
    return position;
}

uint32_t find_free_block(FILE *img, superblock_t sb) {
    uint32_t block_bitmap;
    uint32_t position = 0;
    uint16_t group = 0;
    bool found = false;
    //jump superblock and inode bitmap
    fseek(img, sb.block_size * 2, SEEK_SET);
    do {
        fread(&block_bitmap, sizeof(block_bitmap), 1, img);
        for (int i = sizeof(block_bitmap) * 8 - 1; i >= 0; --i) {
            if (block_bitmap & 1 << i) {
                position++;
            } else {
                fseek(img, -sizeof(block_bitmap), SEEK_CUR);
                block_bitmap = block_bitmap | 1 << i;
                fwrite(&block_bitmap, sizeof(block_bitmap), 1, img);
                found = true;
                break;
            }
        }
        if (position == sb.block_size * 8) {
            if (group < sb.number_of_groups) {
                group++;
                //block bitmap + inode blocks + blocks
                fseek(img, sb.block_size * (sizeof(inode_t) * 8 + sb.block_size * 8), SEEK_CUR);
            }
        }
    } while (!found);
    return (sb.block_size + //superblock
            sb.block_size * 2 * (group + 1) + //inode bitmap and block bitmap of all groups
            sb.block_size * sizeof(inode_t) * 8 * (group + 1) +  //inodes of all groups
            //sb.block_size*sb.block_size*8*group + //all blocks of previous groups
            sb.block_size * position) / sb.block_size; //jump to current block
}

void find_block_and_write(uint8_t *block, FILE *img, superblock_t sb, inode_t *inode, uint32_t block_number) {

    if (block_number < sb.direct) {
        uint32_t block_id = find_free_block(img, sb);
        write_block(block_id, block, sb, img);
        inode->blocks[block_number] = block_id;
        return;
    }

    block_number -= sb.direct;
    if (block_number < sb.single_indirect) {
        if (inode->blocks[NUMBER_OF_BLOCKS - 2] == 0) {
            inode->blocks[NUMBER_OF_BLOCKS - 2] = find_free_block(img, sb);
        }
        uint32_t *indirect_block = read_block(inode->blocks[NUMBER_OF_BLOCKS - 2], sb, img);
        if (indirect_block[block_number] == 0) {
            indirect_block[block_number] = find_free_block(img, sb);
        }
        write_block(indirect_block[block_number], block, sb, img);
        return;
    }

    block_number -= sb.single_indirect;
    if (block_number < sb.double_indirect) {
        if (inode->blocks[NUMBER_OF_BLOCKS - 1] == 0) {
            inode->blocks[NUMBER_OF_BLOCKS - 1] = find_free_block(img, sb);
        }
        uint32_t *indirect_block = read_block(inode->blocks[NUMBER_OF_BLOCKS - 1], sb, img);
        if (indirect_block[block_number / sb.single_indirect] == 0) {
            indirect_block[block_number / sb.single_indirect] = find_free_block(img, sb);
        }
        uint32_t *double_indirect_block = read_block(indirect_block[block_number / sb.single_indirect], sb, img);
        if (double_indirect_block[block_number % sb.single_indirect] == 0) {
            double_indirect_block[block_number % sb.single_indirect] = find_free_block(img, sb);
        }
        write_block(double_indirect_block[block_number % sb.single_indirect], block, sb, img);
        return;
    }
}

void write_block(uint32_t block_number, uint8_t *block, superblock_t sb, FILE *img) {
    goto_block_id(img, block_number, sb);
    fwrite(block, sb.block_size, 1, img);

}


uint32_t *read_block(uint32_t block_number, superblock_t sb, FILE *img) {
    goto_block_id(img, block_number, sb);
    uint32_t *block = calloc_error(sb.block_size, 1);
    fread(block, sb.block_size, 1, img);
    return block;
}

void goto_block_id(FILE *img, uint32_t block_id, superblock_t sb) {
    fseek(img, block_id * sb.block_size, SEEK_SET);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: fs_add <file> <img>\n");
        printf("example: fs_add tetris fs.img\n");
        exit(1);
    }
    fs_add(argv[1], argv[2]);
    return 0;
}