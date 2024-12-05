#!/bin/bash

export PATH="/mnt/c/Program Files/qemu:$PATH" 
qemu-system-i386.exe  -m 16 -monitor stdio -boot order=a -fda floppy.img

