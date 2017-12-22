/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: common functions for filesystem tools
 * Language:  C
 * Date : December 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include "common.h"

/**
 * check signature of filesystem
 * @param sb superblock_t
 * @return true if signature is sext2, false otherwise
 */
bool check_signature(superblock_t *sb) {
    return sb->signature == SIGNATURE;
}

/**
 * check signature of filesystem and exit if not sext2 format
 * @param sb superblock_t
 */
void check_signature_and_crash_if_not_correct(superblock_t *sb) {
    if (!check_signature(sb)) {
        printf("signature of img doesn't correspond with sext2\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * calculate offset of an inode id in the file
 * @param sb superblock_t
 * @param inode_id
 * @return absolute offset
 */
uint32_t inode_id_to_offset(superblock_t *sb, uint32_t inode_id) {
    uint16_t group = inode_id / (sb->block_size * 8);
    return sb->block_size + //superblock
           sb->block_size * 2 * (group + 1) + //inode bitmap and block bitmap of all groups
           inode_id * sizeof(inode_t) + //jump to current inode
           sb->block_size * sb->block_size * 8 * group; //all previous block groups;
}

/**
 * do action on allocated inodes
 * @param filename optional param, send NULL if LIST, filename if it's for delete file
 * @param img_name filesystem image name
 * @param action LIST or DELETE
 */
bool do_action_to_allocated_inode(char *filename, char *img_name, Action action) {
    superblock_t sb = {0};
    FILE *img = fopen(img_name, "r+b");
    if (img != NULL) {
        fread(&sb, sizeof(sb), 1, img);
        check_signature_and_crash_if_not_correct(&sb);
        //jump superblock
        fseek(img, sb.block_size, SEEK_SET);
        uint32_t inode_id = 0;
        uint8_t inode_bitmap;
        long offset;
        uint16_t group = 0;
        do {
            offset = ftell(img);
            fread(&inode_bitmap, sizeof(inode_bitmap), 1, img);
            for (uint8_t i = sizeof(inode_bitmap) * 8; i > 0; --i) {
                if (inode_bitmap & 1 << (i - 1)) {
                    inode_t inode = get_inode(inode_id, img, &sb);
                    switch (action) {
                        case LIST:
                            printf("%s %d\n", inode.name, inode.size);
                            fseek(img, offset + sizeof(inode_bitmap), SEEK_SET);
                            break;
                        case DELETE:
                            if (strcmp(filename, inode.name) == 0) {
                                fseek(img, offset, SEEK_SET);
                                inode_bitmap = inode_bitmap & ~(1 << (i - 1));
                                fwrite(&inode_bitmap, sizeof(inode_bitmap), 1, img);
                                printf("found and deleted\n");
                                return true;
                            }
                            break;
                        case EXISTS:
                            if (strcmp(filename, inode.name) == 0) {
                                return true;
                            }
                            fseek(img, offset + sizeof(inode_bitmap), SEEK_SET);
                            break;
                    }

                }
                inode_id++;
            }
            if (inode_id % (sb.block_size * 8) == 0) {
                if (group < sb.number_of_groups - 1) {
                    group++;
                    //block bitmap + inode blocks + blocks
                    fseek(img, sb.block_size * (1 + sizeof(inode_t) * 8 + sb.block_size * 8), SEEK_CUR);
                } else {
                    break;
                }
            }
        } while (true);
    }
    return false;
}

/**
 *
 * @param inode_id
 * @param img
 * @param sb
 * @return
 */
inode_t get_inode(uint32_t inode_id, FILE *img, superblock_t *sb) {
    fseek(img, inode_id_to_offset(sb, inode_id), SEEK_SET);
    inode_t inode;
    fread(&inode, sizeof(inode_t), 1, img);
    return inode;
}
