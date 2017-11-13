#include "idt.h"
#include "../common/memory.h"
#include "x86.h"
#include "../common/console.h"
#include "pic.h"
#include "keyboard.h"

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

// Exception handler
void exception_handler(regs_t *regs) {
	printf("exception %d", regs->number);
    halt();
}

void irq_handler(regs_t *regs) {
    printf("irq");
    switch (regs->number){
        case 1:
            keyboard_handler();
            break;
        default:
            printf("irq handler %d don't exist", regs->number);
    }
    pic_eoi(regs->number);
}

void idt_init() {
	idt_entry_t idt_table[256];
	memset(idt_table, 0, 256*sizeof(idt_entry_t));
	idt_table[0] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_0, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[1] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_1, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[2] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_2, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[3] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_3, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[4] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_4, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[5] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_5, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[6] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_6, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[7] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_7, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[8] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_8, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[9] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_9, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[10] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_10, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[11] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_11, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[12] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_12, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[13] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_13, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[14] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_14, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[15] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_15, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[16] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_16, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[17] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_17, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[18] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_18, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[19] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_19, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[20] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_exception_20, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[32] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_0, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[33] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_1, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[34] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_2, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[35] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_3, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[36] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_4, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[37] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_5, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[38] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_6, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[39] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_7, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[40] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_8, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[41] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_9, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[42] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_10, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[43] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_11, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[44] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_12, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[45] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_13, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[46] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_14, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_table[47] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)_irq_15, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt_ptr_t ptr;
	ptr.base = (uint32_t)&idt_table;
	ptr.limit = sizeof(idt_table)-1;
	idt_load(&ptr);

}

