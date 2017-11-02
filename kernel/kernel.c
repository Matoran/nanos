//
// Created by matoran on 11/1/17.
//

#include "kernel.h"
#include "gdt.h"
#include "../common/types.h"
#include "console.h"

void kernel_init(multiboot_info_t *informations){
    gdt_init();
    console_init();
}

