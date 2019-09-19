OUTPUT_ARCH(arm)
SECTIONS {
	.text : {
		_start = .;
		*(.text)
		*(.text.*)
		*(.data)
		*(.data.*)
	}
	. = ALIGN(32);
	_bss_start = .;
	.bss : {
		*(.bss)
		*(.bss.*)
	}
	. = ALIGN(32);
	_bss_end = .;
	. = ALIGN(32);
	workspace = .;
}
