OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc)
ENTRY(_start)

SECTIONS
{
    . = 0x00020000;
    .text : {
        KEEP(*(.text.start))
        *(.text*)
        *(.rodata*)
    }

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
