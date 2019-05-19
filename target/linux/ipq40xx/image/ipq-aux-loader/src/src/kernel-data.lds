OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SECTIONS {
	.data.kernel : {
		_kernel_data_start = .;
		*(.data)
		_kernel_data_end = .;
	}
}
