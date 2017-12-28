EXECUTABLES=fs_create fs_add fs_del fs_list
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o, $(SRCS))
DEPS=$(OBJS:.o=.d)

all: $(EXECUTABLES)
filesystem: fs.img

fs_%: fs_%.o common.o
	gcc $^ -o $@

%.o: %.c
	gcc -c -MMD -o $@ $< -Wall -Wextra

fs.img: $(EXECUTABLES) data/nanos.ss
	./fs_create sext2_512 512 fs.img 1 !
	./fs_add data/nanos.ss fs.img

clean:
	rm -f *.o *.d $(EXECUTABLES) fs.img
.PRECIOUS: %.o

-include $(DEPS)