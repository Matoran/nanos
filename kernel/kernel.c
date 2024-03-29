/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Kernel
 * Language:  C
 * Date : October/November 2017
 */

#define BUFFER_SIZE 1024

#include "kernel.h"
#include "gdt.h"
#include "../common/types.h"
#include "console.h"
#include "idt.h"
#include "x86.h"
#include "pic.h"
#include "keyboard.h"
#include "timer.h"
#include "sext2.h"



/**
 * Initialise the kernel
 * @param informations of multiboot
 */
void kernel_init(multiboot_info_t *informations) {
    gdt_init();
    console_init();
    pic_init();
    idt_init();
    timer_init(100);
    keyboard_init();
    printf("RAM %dKB\n", informations->mem_upper);
    sti();
#ifdef TEST
    printf("Test mode on\n");
    printf("Tests will be launched soon\n");
    sleep(5000);
    //console_tests();
    printf("sleep 5 seconds\n");
    sleep(5000);
#else
    printf("Test mode off\n");
#endif
    sleep(1000);
    sext2_init();
    console_clear();
    int ss = file_open("nanos.ss");
    if (ss != -1) {
        uint8_t buffer[BUFFER_SIZE];
        int bytes;
        while ((bytes = file_read(ss, buffer, BUFFER_SIZE))) {
            for (int i = 0; i < bytes; ++i) {
                printf("%c", buffer[i]);
                if (buffer[i] == '\n') {
                    sleep(100);
                }
            }
        };
    } else {
        printf("file not found\n");
    }



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
#ifdef TEST
    int crash = 8 / 0;
#endif
    halt();
}

