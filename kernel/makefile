kernel.elf: bootstrap_asm.o kernel.ld
	gcc $< -Tkernel.ld -m32 -ffreestanding -nostdlib -o $@ -static	
bootstrap_asm.o: bootstrap_asm.s
	nasm -f elf32 $<
clean:
	rm -f bootstrap_asm.o
