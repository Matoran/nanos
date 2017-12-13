EXECUTABLES=fs_create fs_add fs_del fs_list
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o, $(SRCS))
DEPS=$(OBJS:.o=.d)

all: $(EXECUTABLES)

fs_%: fs_%.o common.o
	gcc $^ -o $@

%.o: %.c
	gcc -c -MMD -o $@ $< -Wall -Wextra

clean:
	rm -f *.o *.d $(EXECUTABLES)

-include $(DEPS)