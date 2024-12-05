#!/bin/sh

set -e

./build-bootsector.sh $1
./build-kernel.sh $1

cat bootsector kernel.bin > floppyroot
dd if=floppyroot of=floppy.img conv=notrunc 
