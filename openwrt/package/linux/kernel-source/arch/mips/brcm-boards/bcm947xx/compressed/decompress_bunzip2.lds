OUTPUT_ARCH(mips)
ENTRY(load_and_run)
SECTIONS {
	. = 0x81000000-0x4000;
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
