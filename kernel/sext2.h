//
// Created by matoran on 12/13/17.
//

#ifndef NANOS_SEXT2_H
#define NANOS_SEXT2_H

#include <stdbool.h>
#include "../common/types.h"
#include "../tools/structs.h"

typedef struct stat_st {
    uint32_t size;
} stat_t;

typedef struct file_iterator_st {
    uint32_t inode_id;
    uint16_t group;
} file_iterator_t;

extern void sext2_init();
extern int file_stat(char *filename, stat_t *stat);
extern bool file_exists(char *filename);
extern int file_open(char *filename);
extern int file_read(int fd, void *buf, uint count);
extern int file_seek(int fd, uint offset);
extern void file_close(int fd);
extern file_iterator_t file_iterator();
extern bool file_has_next(file_iterator_t *it);
extern void file_next(char *filename, file_iterator_t *it);

#endif //NANOS_SEXT2_H
