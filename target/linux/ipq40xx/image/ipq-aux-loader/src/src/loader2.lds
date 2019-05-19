OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SECTIONS {
	.text : {
		_start = .;
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
	}

	. = ALIGN(32);
	.data : {
		_payload_data_begin = .;
		*(.data)
		_payload_data_end = .;
		*(.data.*)
		. = . + 524288;		/* workaround for buggy bootloaders */
	}
}
