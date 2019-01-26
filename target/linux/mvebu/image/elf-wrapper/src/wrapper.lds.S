ENTRY(_start);

ZIMAGE_START = WRAPPER_LOADADDR;
MAX_KERNEL_SIZE = 128M;

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
