BOOT=build/boot/grub
KERNEL=build/boot/mykernel.elf
run: iso qemu
test:
	make run TEST_MODE=-DTEST
iso: nanos.iso
nanos.iso: $(KERNEL) $(BOOT)/stage2_eltorito $(BOOT)/menu.lst
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o nanos.iso build
$(BOOT):
	mkdir -p $(BOOT)
$(BOOT)/stage2_eltorito: grub/stage2_eltorito
	cp grub/stage2_eltorito $(BOOT)/stage2_eltorito
$(BOOT)/menu.lst: grub/menu.lst
	cp grub/menu.lst $(BOOT)/menu.lst
$(KERNEL): $(BOOT) kernel common
	make -C kernel
	cp kernel/kernel.elf $(KERNEL)
qemu: nanos.iso
	qemu-system-i386 -cdrom nanos.iso
clean:
	rm -f -R build nanos.iso
	make -C kernel clean

