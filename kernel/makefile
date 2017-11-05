SRCS=$(wildcard *.c *.s)
OBJS=$(patsubst %.c,%.o,$(patsubst %.s,%.o,$(SRCS)))
DEPS=$(OBJS:.o=.d)
kernel.elf: kernel.ld $(OBJS)
	gcc -static -T$< -m32 -ffreestanding -nostdlib $(OBJS) -o $@ -lgcc
%.o: %.s
	nasm -f elf32 $<
%.o: %.c
	gcc $(TEST_MODE) -m32 -ffreestanding -nostdlib -c -MMD -o $@ $< -Wall -Wextra
clean:
	rm -f *.o *.d kernel.elf

-include $(DEPS)
