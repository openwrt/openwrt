OUTPUT_ARCH(mips)
ENTRY(startup)
SECTIONS {
	. = 0x80001000;
	.text : {
		*(.text)
		*(.rodata)
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(.bss)
	}
}
