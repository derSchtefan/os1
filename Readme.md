# Info
OS/1

Minimal, very copy pasty, haphazard demo of how to boot an i386 PC into 32 bit mode, setup the C and C++ environment, write to screen, read keyboard, reprogramm the PIC, handle timer interrupts, handle interrupts from C with an assembler trampoline, etc.

This is not meant to be a real operating system, just a demo of how to implement those basic things.

Especially if you want to get your C and C++ runtime up, want string printing, etc. 

Uses flat memory model, requires that QEMU is available, requires you to start this on Linux or on a WSL on Windows.

Demoes how to use GDB to debug QEMU.

# How this works

## Bootup

* bootsector.asm starts at 0x7C00, but copies itself to 0x500.
* Reads 124 additional sectors from sector 2 to 0x000700  (second sector, indexed at 1)  (change bootsector.asm, or patch the flopy image at location 0x508/509)
* Sets up 32 bit mode, flat memory model 
  * "Kernel" program memory resides at 0x000700, can go up to 0x0007FFFF
  * STACK starts at 0x1FFFFF down to 0x100000 
  * Heap starts at 0x200000 to 0x600000
* init.c is reading the variables from the linker script output, and sets up the C/C++ environemnt
  * copies the DATA (and RODATA) section from program memory to RW RAM
    * actually optional, we don't actually need to do this on a PC
  * zeroes the BSS section 
  * Initializes the heap pointer (BRK, so that SBRK can run, and malloc works)
  * Runs the libc init_array constructors for global ctors
    * Global defined static variables in C
    * Global defined static variables in C++ and their ctors
  * drops into the kernel_main function
* kernel.c does
  * Bunch of test stuff that is commented out (just to demo things)
  * Clears the screen
  * Inits the Interrupt vector table (interrupts.c)
  * Installs keyboard handler IRQ (IRQ1, which is mapped to processor interrupt 0x20)
  * Outputs, in a loop, some counters, and the value of the HEAP and STACK pointers (diagnoses memory leaks)
* interrupt.c does (called by kernel.c) (built seperately, because it requires -mgeneral-regs-only flag)
  * Initializes the interrupt table
  * loads interrupt vector
  * remaps the PIC so that IRQs from hardware are mapped to Intel Processor interrupts 0x20 and above
* isr_ll.asm does
  * Simply an assembler trampoline. Each interrupt handler is 16 bytes long, and simply 
    * Conditionally pushes a fake error code onto the stack (some intel exception interrupts have an error code, some don't. we are normalizing the stack)
    * Pushes the interrupt number onto the stack
    * Calls the C interrupt handler
    * Cleans up after itself, so that C code does not have to deal with it.

## Build

* A 1.44MB floppy image is built from bootsector.asm and the C libraries

# Warning

This is all very hacked together. There is no Makefile, etc. I know that no proper headers are used, etc.

# Requirements

Install via sudo apt install 
* NASM
* QEMU
* GDB
* GCC
* gcc-multilib
* g++-multilib
* ncurses-dev   

Requires that QEMU is in `/mnt/c/Program Files/qemu`. Adjust scripts accordingly.
Assumes there is at least 4 MB memory.

# Running / Debugging


Run:
/build-all.sh -O && ./run.sh

Debug:
./build-all.sh -g && ./run-debug.sh

