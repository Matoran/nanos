cmake_minimum_required(VERSION 3.8)
project(nanos)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        common/types.h
        kernel/gdt.c
        kernel/gdt.h
        kernel/multiboot.h
        kernel/x86.h
        kernel/memory.c
        common/memory.h
        kernel/console.c
        kernel/console.h
        kernel/kernel.c
        kernel/kernel.h
        kernel/kernel.ld
        kernel/console_tests.c
        kernel/console_tests.h
        kernel/pio_asm.s
        kernel/gdt_asm.s
        kernel/bootstrap_asm.s
        makefile
        kernel/makefile
        kernel/pic.c
        kernel/pic.h
        kernel/keyboard.c
        kernel/idt_asm.s
        kernel/idt.c
        kernel/ide.c
        kernel/timer.c
        kernel/const.inc kernel/io.h kernel/sext2.c kernel/sext2.h)

add_executable(nanos ${SOURCE_FILES})

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES tools/fs_create.c tools/structs.h tools/common.c tools/common.h)
add_executable(fs_create ${SOURCE_FILES})

set(SOURCE_FILES tools/fs_add.c tools/structs.h tools/common.c tools/common.h)
add_executable(fs_add ${SOURCE_FILES})

set(SOURCE_FILES tools/fs_del.c tools/structs.h tools/common.c tools/common.h)
add_executable(fs_del ${SOURCE_FILES})

set(SOURCE_FILES tools/fs_list.c tools/structs.h tools/common.c tools/common.h)
add_executable(fs_list ${SOURCE_FILES})
