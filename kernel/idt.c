/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: interrupts and exceptions
 * Language:  C
 * Date : November 2017
 */
#include "idt.h"
#include "../common/memory.h"
#include "x86.h"
#include "../common/console.h"
#include "pic.h"
#include "keyboard.h"
#include "timer.h"

// CPU context used when saving/restoring context from an interrupt
typedef struct regs_st {
    uint32_t gs, fs, es, ds;
    uint32_t ebp, edi, esi;
    uint32_t edx, ecx, ebx, eax;
    uint32_t number, error_code;
    uint32_t eip, cs, eflags, esp, ss;
} regs_t;

// Build and return an IDT entry.
// selector is the code segment selector to access the ISR
// offset is the address of the ISR (for task gates, offset must be 0)
// type indicates the IDT entry type
// dpl is the privilege level required to call the associated ISR
static idt_entry_t idt_build_entry(uint16_t selector, uint32_t offset, uint8_t type, uint8_t dpl) {
    idt_entry_t entry;
    entry.offset15_0 = offset & 0xffff;
    entry.selector = selector;
    entry.reserved = 0;
    entry.type = type;
    entry.dpl = dpl;
    entry.p = 1;
    entry.offset31_16 = (offset >> 16) & 0xffff;
    return entry;
}

/**
 * manage cpu exceptions, show exception description and halt system
 * @param regs exception
 */
void exception_handler(regs_t *regs) {
    printf("\n");
    console_set_foreground_color(COLOR_RED);
    switch (regs->number) {
        case 0:
            printf("Exception 0 - Divide Error");
            break;
        case 1:
            printf("Exception 1 - RESERVED");
            break;
        case 2:
            printf("Exception 2 - NMI Interrupt");
            break;
        case 3:
            printf("Exception 3 - Breakpoint");
            break;
        case 4:
            printf("Exception 4 - Overflow");
            break;
        case 5:
            printf("Exception 5 - BOUND Range Exceeded");
            break;
        case 6:
            printf("Exception 6 - Invalid Opcode (Undefined Opcode)");
            break;
        case 7:
            printf("Exception 7 - Device Not Available (No Math Coprocessor)");
            break;
        case 8:
            printf("Exception 8 - Double Fault");
            break;
        case 9:
            printf("Exception 9 - Coprocessor Segment Overrun (reserved)");
            break;
        case 10:
            printf("Exception 10 - Invalid TSS");
            break;
        case 11:
            printf("Exception 11 - Segment Not Present");
            break;
        case 12:
            printf("Exception 12 - Stack-Segment Fault");
            break;
        case 13:
            printf("Exception 13 - General Protection");
            break;
        case 14:
            printf("Exception 14 - Page Fault");
            break;
        case 15:
            printf("Exception 15 - (Intel reserved. Do not use.)");
            break;
        case 16:
            printf("Exception 16 - x87 FPU Floating-Point Error (Math Fault)");
            break;
        case 17:
            printf("Exception 17 - Alignment Check");
            break;
        case 18:
            printf("Exception 18 - Machine Check");
            break;
        case 19:
            printf("Exception 19 - SIMD Floating-Point Exception");
            break;
        case 20:
            printf("Exception 20 - Virtualization Exception");
            break;
        default:
            printf("Exception 21-31 - Intel reserved. Do not use.");
            break;
    }
    halt();
}

/**
 * manage irq, call subhandler and send end of interrupt to controller
 * @param regs
 */
void irq_handler(regs_t *regs) {
    switch (regs->number) {
        case 0:
            timer_handler();
        case 1:
            keyboard_handler();
            break;
        default:
            printf("irq handler %d don't exist", regs->number);
    }
    pic_eoi(regs->number);
}

/**
 * create idt table empty fill it with asm functions and finally load the idt table
 */
void idt_init() {
    idt_entry_t idt_table[256];
    memset(idt_table, 0, 256 * sizeof(idt_entry_t));
    //cpu exceptions
    idt_table[0] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_0, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[1] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_1, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[2] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_2, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[3] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_3, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[4] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_4, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[5] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_5, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[6] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_6, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[7] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_7, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[8] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_8, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[9] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_9, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[10] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_10, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[11] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_11, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[12] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_12, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[13] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_13, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[14] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_14, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[15] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_15, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[16] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_16, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[17] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_17, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[18] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_18, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[19] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_19, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    idt_table[20] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _exception_20, TYPE_INTERRUPT_GATE,
                                    DPL_KERNEL);
    //material interrupts
    idt_table[32] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_0, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[33] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_1, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[34] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_2, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[35] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_3, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[36] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_4, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[37] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_5, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[38] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_6, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[39] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_7, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[40] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_8, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[41] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_9, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[42] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_10, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[43] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_11, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[44] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_12, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[45] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_13, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[46] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_14, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_table[47] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) _irq_15, TYPE_INTERRUPT_GATE, DPL_KERNEL);
    idt_ptr_t ptr;
    ptr.base = (uint32_t) idt_table;
    ptr.limit = sizeof(idt_table) - 1;
    idt_load(&ptr);
    printf("IDT initialized\n");
}
