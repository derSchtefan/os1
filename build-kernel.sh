#!/bin/sh

set -e

g++ $1 $2 -m32 -nostdlib -nolibc -nostartfiles  \
    -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fno-use-cxa-atexit \
    -r -o kernel_cpp.elf cpp.cpp

nasm -f elf32 -g -o isr_ll.elf isr_ll.asm

gcc -m32 \
    -nostdlib \
    -nolibc -nostartfiles  -fno-asynchronous-unwind-tables \
    -mgeneral-regs-only \
    -r \
    $1 $2 \
    -o interrupts.elf \
    interrupts.c

gcc -m32 \
    -nostdlib -Wl,-e_entry_point  \
    -T kernel.ld \
    -nolibc -nostartfiles  -fno-asynchronous-unwind-tables \
    $1 $2 \
    -o kernel.elf \
    mylibc/mylibc.elf \
    mylibcpp/mylibcpp.elf \
    kernel_cpp.elf \
    interrupts.elf \
    isr_ll.elf \
    init.c  kernel.c keyboard.c


# objdump -D kernel.elf

objcopy --set-start 0x00000700 -j .text -j .init_array -j .data -j .rodata \
        -S kernel.elf -O binary kernel.bin 
        
# xxd kernel.bin
