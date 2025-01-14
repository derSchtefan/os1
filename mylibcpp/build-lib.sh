#!/bin/sh

set -e

g++ $1 $2 -m32 -nostdlib -nolibc -nostartfiles  \
    -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fno-use-cxa-atexit \
    -r \
    -o mylibcpp.elf \
    *.cpp
