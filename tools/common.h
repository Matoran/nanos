//
// Created by matoran on 12/9/17.
//

#include "structs.h"

#ifndef FS_CREATE_COMMON_H
#define FS_CREATE_COMMON_H

extern void *calloc_error (size_t __nmemb, size_t __size);

extern __uint32_t inode_id_to_offset(superblock_t sb, __uint32_t inode_id);

extern void do_action_to_allocated_inode(char *filename, char *img_name,
                                         void(*function)(char *, __uint32_t, FILE *, superblock_t, long, __uint32_t,
                                                         __uint8_t));

extern inode_t get_inode(__uint32_t position, FILE *img, superblock_t sb);

#endif //FS_CREATE_COMMON_H
