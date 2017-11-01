kernel.elf: bootstrap_asm.o gdt_asm.o kernel.o gdt.o memory.o kernel.ld
	gcc -static -Tkernel.ld -m32 -ffreestanding -nostdlib bootstrap_asm.o gdt_asm.o kernel.o gdt.o memory.o -o kernel.elf -lgcc
%.o: %.s
	nasm -f elf32 $<
%.o: %.c
	gcc -m32 -ffreestanding -nostdlib -c -o $@ $< -Wall -Wextra
clean:
	rm -f *.o
