OUTPUT_ARCH(mips)
SECTIONS {
	.rodata : {
		. = ALIGN(16);
		_lzma_data_start = .;
		*(.data)
		_lzma_data_end = .;
	}
}
