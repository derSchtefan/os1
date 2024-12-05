#!/bin/bash

export PATH="/mnt/c/Program Files/qemu:$PATH" 
qemu-system-i386.exe -s -S -m 16 -boot order=a -fda floppy.img &
gdb -x gdb.kernel -s kernel.elf
kill %1
wait

