//
// Created by matoran on 12/13/17.
//

#include "sext2.h"

int file_stat(char *filename, stat_t *stat) {
    return -1;
}

bool file_exists(char *filename) {
    return -1;
}

int file_open(char *filename) {
    return -1;
}

int file_read(int fd, void *buf, uint count) {
    return -1;
}

int file_seek(int fd, uint offset) {
    return -1;
}

void file_close(int fd) {
}

file_iterator_t file_iterator() {
}

bool file_has_next(file_iterator_t *it) {
    return -1;
}

void file_next(char *filename, file_iterator_t *it) {
}
