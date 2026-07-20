/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * A single PT_LOAD segment holding code, compressed kernel and DTB.
 *
 * Accesses to a separately loaded payload segment fail regardless of address
 * or MMU state, while the loader's own segment is always readable. MikroTik's
 * own loader has exactly one PT_LOAD too.
 */
ENTRY(_start)
PHDRS {
	all PT_LOAD FLAGS(7);	/* rwx, as in MikroTik's loader */
}
SECTIONS {
	. = 0x01100000;

	.text : { *(.text.start) *(.text*) } :all
	.rodata : { *(.rodata*) } :all
	.data : { *(.data*) } :all

	. = ALIGN(16);
	.payload.kernel : {
		_payload_start = .;
		*(.payload.kernel)
		_payload_end = .;
	} :all

	. = ALIGN(16);
	.payload.dtb : {
		_dtb_start = .;
		*(.payload.dtb)
		_dtb_end = .;
	} :all

	.bss : {
		. = ALIGN(8);
		__bss_start = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(8);
		__bss_end = .;
	} :all

	/DISCARD/ : { *(.comment) *(.note*) *(.ARM.attributes) *(.eh_frame*) }
}
