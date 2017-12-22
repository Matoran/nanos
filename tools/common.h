

#ifndef FS_CREATE_COMMON_H
#define FS_CREATE_COMMON_H

#include <stdbool.h>
#include "structs.h"

typedef enum Action Action;
enum Action {
    DELETE,
    LIST,
    EXISTS
};

extern bool check_signature(superblock_t *sb);
extern void check_signature_and_crash_if_not_correct(superblock_t *sb);
extern uint32_t inode_id_to_offset(superblock_t *sb, uint32_t inode_id);
extern bool do_action_to_allocated_inode(char *filename, char *img_name, Action action);
extern inode_t get_inode(uint32_t inode_id, FILE *img, superblock_t *sb);


#endif //FS_CREATE_COMMON_H
