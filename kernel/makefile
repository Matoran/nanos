kernel.elf: bootstrap_asm.o gdt_asm.o pio_asm.o kernel.o gdt.o memory.o console.o console_tests.o kernel.ld
	gcc -static -Tkernel.ld -m32 -ffreestanding -nostdlib bootstrap_asm.o gdt_asm.o pio_asm.o kernel.o gdt.o memory.o console.o console_tests.o -o kernel.elf -lgcc
%.o: %.s
	nasm -f elf32 $<
%.o: %.c
	gcc -DTEST -m32 -ffreestanding -nostdlib -c -o $@ $< -Wall -Wextra
clean:
	rm -f *.o
