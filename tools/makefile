EXECUTABLES=fs_create fs_add fs_del fs_list
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o, $(SRCS))
DEPS=$(OBJS:.o=.d)

all: $(EXECUTABLES)

fs_%: fs_%.o common.o
	gcc $^ -o $@

%.o: %.c
	gcc -c -MMD -o $@ $< -Wall -Wextra

filesystem: $(EXECUTABLES) data/nanos.ss
	./fs_create sext2_512 512 fs.img 1 !
	./fs_add data/nanos.ss fs.img

clean:
	rm -f *.o *.d $(EXECUTABLES) fs.img

-include $(DEPS)