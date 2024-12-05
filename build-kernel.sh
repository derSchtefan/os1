#!/bin/sh

set -e

#g++ -g -m32 -nostdlib -nolibc -nostartfiles  \
#    -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fno-use-cxa-atexit \
#    -r -o kernel_cpp.o cpp.cpp 

gcc -m32 \
    -nostdlib -Wl,-e_entry_point  \
    -T kernel.ld \
    -nolibc -nostartfiles  -fno-asynchronous-unwind-tables \
    $1 $2 \
    mylibc/mylibc.elf \
    -o kernel.elf \
    init.c  kernel.c 

    #mylibcpp/mylibcpp.elf \
    #kernel_cpp.o \


# objdump -D kernel.elf

objcopy --set-start 0x00000700 -j .text -j .init_array -j .data -j .rodata \
        -S kernel.elf -O binary kernel.bin 
        
# xxd kernel.bin