/* SPDX-License-Identifier: GPL-2.0-or-later */

OUTPUT_ARCH(mips)
ENTRY(_start)

MEMORY {
	fw (rwx) : ORIGIN = 0x1fc00000, LENGTH = 0x800
}

SECTIONS {
	. = ORIGIN(fw);

	.text : {
		KEEP(*(.text.start))
		*(.text*)
		*(.rodata*)
	} > fw

	. = ALIGN(4);
	.data : {
		*(.data*)
	} > fw

	. = ALIGN(4);
	__bss_start = .;
	.bss (NOLOAD) : {
		*(.bss*)
		*(COMMON)
	} > fw
	__bss_end = .;

	.msp_fw_config ORIGIN(fw) + 0x700 (NOLOAD) : {
		KEEP(*(.msp_fw_config))
	} > fw
	__msp_fw_config = ADDR(.msp_fw_config);
	__msp_fw_config_end = .;
	__stack_top = ORIGIN(fw) + 0x700;

	/DISCARD/ : {
		*(.MIPS.abiflags)
		*(.reginfo)
		*(.pdr)
		*(.gnu.attributes)
		*(.comment)
		*(.note*)
	}
}

ASSERT(__bss_end <= __stack_top, "MSP firmware text/data/bss overlaps stack")
ASSERT(__msp_fw_config_end <= ORIGIN(fw) + LENGTH(fw), "MSP firmware config exceeds reserved memory")
