//
// Created by matoran on 11/1/17.
//

#include "kernel.h"
#include "gdt.h"
#include "../common/types.h"
#include "console.h"
#include "memory.h"
#include "console_tests.h"

void kernel_init(multiboot_info_t *informations) {
    gdt_init();
    console_init();
    printf("RAM %dKB\n", informations->mem_upper);
    printf("GDT initialized\n");
    printf("Display initialized\n");
#ifdef TEST
    printf("Test mode on\n");
    printf("Tests will be launched soon\n");
    sleep(5);
    console_tests();

#else
    printf("Test mode off\n");
#endif
}

