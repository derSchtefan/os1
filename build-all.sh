#!/bin/sh

set -e

./build-bootsector.sh
./build-kernel.sh

cat bootsector kernel.bin > floppyroot
dd if=floppyroot of=floppy.img conv=notrunc 
