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
#include "keyboard.h"
#include "timer.h"

/**
 * Initialise the kernel
 * @param informations of multiboot
 */
void kernel_init(multiboot_info_t *informations) {
    gdt_init();
    idt_init();
    pic_init();
    console_init();
    timer_init(100000);

    printf("RAM %dKB\n", informations->mem_upper);
    printf("GDT initialized\n");
    printf("Display initialized\n");
    sti();
#ifdef TEST
    printf("Test mode on\n");
    printf("Tests will be launched soon\n");
    sleep(5000);
    console_tests();
#else
    printf("Test mode off\n");
#endif
    printf("sleep 5 seconds\n");
    sleep(5000);
    while (1) {
        if (keypressed()) {
            uchar c = getc();
            printf("%c", c);
            if (c == 'Q') {
                printf("bye bye see you soon\n");
                break;
            }
        }
    }
    int crash = 8 / 0;
    halt();
}

