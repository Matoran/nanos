BOOT=build/boot/grub
KERNEL=build/boot/mykernel.elf
all: iso qemu
iso: nanos.iso
nanos.iso: $(KERNEL) $(BOOT)/stage2_eltorito $(BOOT)/menu.lst
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o nanos.iso build	
$(BOOT):
	mkdir -p $(BOOT)
$(BOOT)/stage2_eltorito: $(BOOT) grub/stage2_eltorito
	cp grub/stage2_eltorito $(BOOT)/stage2_eltorito
$(BOOT)/menu.lst: $(BOOT) grub/menu.lst
	cp grub/menu.lst $(BOOT)/menu.lst
$(KERNEL): $(BOOT)
	make -C kernel
	mv kernel/kernel.elf $(KERNEL)
qemu: nanos.iso
	qemu-system-i386 -cdrom nanos.iso
clean:
	rm -f -R build

