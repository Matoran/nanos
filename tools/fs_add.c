#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>
#include <stdbool.h>
#include "structs.h"
#include "common.h"

void fs_add(char filename[], char img_name[]){
    superblock_t superblock = {0};
    inode_t inode = {0};
    FILE * file= fopen(img_name, "r+b");
    if (file != NULL) {
        fread(&superblock, sizeof(superblock), 1, file);
        fseek(file, superblock.block_size-sizeof(superblock),SEEK_CUR);
        __uint32_t inode_bitmap;
        __uint32_t position = 0;
        bool found = false;
        do{
            fread(&inode_bitmap, sizeof(inode_bitmap), 1, file);
            for (int i = sizeof(inode_bitmap)*8-1; i >= 0; --i) {
                if(inode_bitmap && 1 << i){
                    position++;
                }else{
                    found = true;
                    break;
                }
            }
            if(position == superblock.block_size*8){
                //TODO fseek to next inode bitmap
            }
        }while(!found);
        fseek(file, superblock.block_size*3 + position*sizeof(inode),SEEK_SET);
        printf("%s\n", superblock.name);
        printf("%d\n", position);
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        printf("usage: fs_add <file> <img>\n");
        printf("example: fs_add tetris fs.img\n");
        exit(1);
    }
    fs_add(argv[1], argv[2]);
    return 0;
}