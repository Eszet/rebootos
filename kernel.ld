OUTPUT_FORMAT("binary")
ENTRY(entry)

phys = 0x10000;
align = 512;

SECTIONS
{
  . = phys;

  .text : {
    __code = .;
    *(.text)
  }

  .data BLOCK(align) :
  {
    __data = .;
    *(.data)

    __ctor_s = .;
    *(.ctor*)
    __ctor_e = .;

    __dtor_s = .;
    *(.dtor*)
    __dtor_e = .;
  }

/*
  .bss BLOCK(align) :
  // AT(ADDR(.data) + SIZEOF(.data))
  {
    __bss = .;
    *(COMMON)
  }
*/

  /DISCARD/ : {
    *(.comment) 
  }

  .common : { 
    *(*) *(COMMON)
    FILL(0);
    . = ALIGN(align);
  }

  __limit = .;
  _limit = 15;
  limit = .;
}
