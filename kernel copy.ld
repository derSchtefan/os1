MEMORY
{
    STACK (rw) : ORIGIN = 0x00000500, LENGTH = 0x7B00
    RAM (rw) : ORIGIN = 0x00008000, LENGTH = 0x7000
    PROG (rx) : ORIGIN = 0x0000F000, LENGTH = 0x71000
}

_eram = ORIGIN(RAM) + LENGTH(RAM);
_stack = ORIGIN(STACK) + LENGTH(STACK) - 1;

SECTIONS 
{
    .text : ALIGN(8)
    {
        *(.entry*)
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