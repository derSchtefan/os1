#!/bin/sh

set -e

nasm bootsector.asm
dd if=bootsector of=floppy.img conv=notrunc 

