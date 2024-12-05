MEMORY
{
    PROG (rwx) : ORIGIN = 0x00000700, LENGTH = 0x05F900     /* 391'424 bytes */
    IOBUF (rw) : ORIGIN = 0x00060000, LENGTH = 0x020000     /* 2 * 64 K */
    STACK (rw) : ORIGIN = 0x00100000, LENGTH = 0x100000     /* 1 meg */
    RAM (rw)   : ORIGIN = 0x00200000, LENGTH = 0x400000     /* 4 meg */ 
}

_eram = ORIGIN(RAM) + LENGTH(RAM);
_stack = ORIGIN(STACK) + LENGTH(STACK) - 1;
_iobuf1 = ORIGIN(IOBUF);
_iobuf2 = ORIGIN(IOBUF) + 0x010000;

SECTIONS 
{
    .text : ALIGN(8)
    {
        *(.entry)
        *(.text*)
        . = ALIGN(8);
        _etext = .; 
    } > PROG


  .init_array : ALIGN(8)
  {
    __init_array_start = .;
    KEEP(*(.init_array*))
    __init_array_end = .;
    . = ALIGN(8);
  } > PROG

  /* Initial (constant) values for data is placed in PRG after the program code */
  _idata = LOADADDR(.data);

  .data : ALIGN(8)
  {
    _data = .;
    *(.data*)
    *(.rodata*)
    . = ALIGN(8);
    _edata = .;
  } > RAM AT > PROG

  /* zero initialized */
  .bss : ALIGN(8)
  {
    _bss = .;
    *(.bss*)
    . = ALIGN(8);
    _ebss = .;
    PROVIDE(_end = .);
  } > RAM
}