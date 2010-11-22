/* ld script to make m68k Coldfire Linux kernel
 *
 * Derived from arch/m68k/kernel/vmlinux-std.lds
 *
 * Updated 11/26/2007 for new CodeSourcery toolset
 *     by Kurt Mahan <kmahan@freescale.com>
 * 
 * Copyright Freescale Semiconductor, Inc. 2008-2009
 *   Jason Jin Jason.Jin@freescale.com
 *   Shrek Wu B16972@freescale.com
 */

#define	LOAD_OFFSET	0x00000000

#include <asm-generic/vmlinux.lds.h>
#include <asm/page_offset.h>

#define	START_OFFSET	0x00020000
#define	IMAGE_START	PAGE_OFFSET_RAW + START_OFFSET

OUTPUT_FORMAT("elf32-m68k", "elf32-m68k", "elf32-m68k")
OUTPUT_ARCH(m68k)
ENTRY(_stext)
jiffies = jiffies_64 + 4;
SECTIONS
{
  . = IMAGE_START;
  .text.head : AT(ADDR(.text.head) - LOAD_OFFSET) {
	_text = .;			/* Text and read-only data */
	HEAD_TEXT
	} :text = 0x4e75

  .text : AT(ADDR(.text) - LOAD_OFFSET) {
	TEXT_TEXT
	SCHED_TEXT
	LOCK_TEXT
	*(.fixup)
	*(.gnu.warning)
	} :text = 0x4e75

  _etext = .;			/* End of text section */

  . = ALIGN(16);
  __ex_table : AT(ADDR(__ex_table) - LOAD_OFFSET) {
	__start___ex_table = .;
	*(__ex_table)
	__stop___ex_table = .;
  }

  RODATA

  . = ALIGN(8192);
  .data : AT(ADDR(.data) - LOAD_OFFSET) {	/* Data */
	DATA_DATA
	CONSTRUCTORS
  } :data


  . = ALIGN(16);
  .data.cacheline_aligned : AT(ADDR(.data.cacheline_aligned) - LOAD_OFFSET ) {
	*(.data.cacheline_aligned)
  } :data

  _edata = .;			/* End of data section */

  . = ALIGN(8192);		/* Initrd */
  .init.text : AT(ADDR(.init.text) - LOAD_OFFSET) {
  	__init_begin = .;
	_sinittext = .;
	INIT_TEXT
	_einittext = .;
  }

  .init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {
	INIT_DATA
  }

  . = ALIGN(16);
  .init.setup : AT(ADDR(.init.setup) - LOAD_OFFSET) {
	__setup_start = .;
	*(.init.setup)
  	__setup_end = .;
  }

  .initcall.init : AT(ADDR(.initcall.init) - LOAD_OFFSET) {
	__initcall_start = .;
	INITCALLS
	__initcall_end = .;
  }

  .con_initcall.init : AT(ADDR(.con_initcall.init) - LOAD_OFFSET) {
	__con_initcall_start = .;
	*(.con_initcall.init)
	__con_initcall_end = .;
  }

  SECURITY_INIT

#ifdef CONFIG_BLK_DEV_INITRD
  . = ALIGN(8192);
  .init.ramfs : AT(ADDR(.init.ramfs) - LOAD_OFFSET) {
	__initramfs_start = .;
	*(.init.ramfs)
	__initramfs_end = .;
  }
#endif
  NOTES
  . = ALIGN(8192);
  __init_end = .;

  .data.init_task : AT(ADDR(.data.init_task) - LOAD_OFFSET) {
	*(.data.init_task)	/* The initial task and kernel stack */
  }

  _sbss = .;
  .bss : AT(ADDR(.bss) - LOAD_OFFSET) {		/* BSS */
	*(.bss)
  }
  _ebss = .;

  _end = . ;

  /DISCARD/ : {
	EXIT_TEXT
	EXIT_DATA
	*(.exitcall.exit)
	*(.discard)
	}

  STABS_DEBUG
  .comment 0 : { *(.comment) }

  /* Sections to be discarded */
  /*DISCARDS*/

}
