/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Kernel
 * Language:  C
 * Date : October/November 2017
 */

#include "kernel.h"
#include "gdt.h"
#include "../common/types.h"
#include "../common/console.h"
#include "../common/memory.h"
#include "console_tests.h"
#include "idt.h"
#include "x86.h"
#include "pic.h"

/**
 * Initialise the kernel
 * @param informations of multiboot
 */
void kernel_init(multiboot_info_t *informations) {
    gdt_init();
    pic_init();
    idt_init();
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
    sti();
}

