#!/bin/sh

set -e

gcc -m32 \
    -nostdlib \
    -r \
    -nolibc -nostartfiles  -fno-asynchronous-unwind-tables \
    -g \
    -o mylibc.elf \
    *.c

#gcc -m32 \
#    -nostdlib \
#    -r \
#    -nolibc -nostartfiles  -fno-asynchronous-unwind-tables \
#    -g \
#    -o mylibc.elf \
#    snprintf.c printf.c
