#!/bin/sh

set -e

gcc -m32 \
    -nostdlib -Wl,-e_entry_point  \
    -T kernel.ld \
    -nolibc -nostartfiles  -fno-asynchronous-unwind-tables \
    $1 \
    -o kernel.elf \
    init.c  kernel.c 


objdump -D kernel.elf

objcopy --set-start 0x00000700 -j .text -j .init_array -j .data -j .rodata \
        -S kernel.elf -O binary kernel.bin 
        
xxd kernel.bin