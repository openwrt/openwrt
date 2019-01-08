ENTRY(_start);

ZIMAGE_START = 0x80010000;
MAX_KERNEL_SIZE = 16M;

MEMORY {
    zimage (rwx): ORIGIN = ZIMAGE_START, LENGTH = MAX_KERNEL_SIZE
}

SECTIONS {
    _start = ZIMAGE_START;
    . =      _start;
    .text (ALIGN(4096)) : {
        *(.data)
    } >zimage
}

/*    .dtb : {
        FILL(0x2342)
        . = 0x10000 - 1;
        BYTE(0x42)
    }
    . =      0x60020000;
*/
