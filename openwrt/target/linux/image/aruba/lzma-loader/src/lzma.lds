OUTPUT_ARCH(mips)
ENTRY(_start)
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  . = 0x81000000;
  .text      :
  {
    _ftext = . ;
    *(.text)
    *(.rodata)
    lzma_start = .;
    kernel.o
    lzma_end = .;
  } =0

  .reginfo : { *(.reginfo) }

  .bss       :
  {
   *(.bss)
  }
}
