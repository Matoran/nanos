//
// Created by matoran on 11/1/17.
//

#include "kernel.h"
#include "gdt.h"
#include "../common/types.h"

void kernel_init(multiboot_info_t *informations){
    gdt_init();
    /*uint *screen = 0xB8000;
    *screen = 1 | 1 << 8 | 2 << 12;*/
}

