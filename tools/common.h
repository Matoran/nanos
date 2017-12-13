//
// Created by matoran on 12/9/17.
//

#include "structs.h"

#ifndef FS_CREATE_COMMON_H
#define FS_CREATE_COMMON_H

typedef enum Action Action;
enum Action {
    DELETE,
    LIST
};

extern void *calloc_error(size_t __nmemb, size_t __size);
extern uint32_t inode_id_to_offset(superblock_t sb, uint32_t inode_id);
extern void do_action_to_allocated_inode(char *filename, char *img_name, Action action);
extern inode_t get_inode(uint32_t position, FILE *img, superblock_t sb);


#endif //FS_CREATE_COMMON_H