/*
 * arch/ubicom32/kernel/vmlinux.lds.S
 *	vmlinux primary linker script
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <asm-generic/vmlinux.lds.h>
#include <asm/ocm_size.h>
#include <asm/memory_map.h>
#include <asm/thread_info.h>
#include <linux/threads.h>

/*
 * Sanity checks to prevent errors later on that are much harder to understand
 */
#if !defined APP_OCM_CODE_SIZE
#error APP_OCM_CODE_SIZE has not been defined in ocm_size.h
#endif

#if !defined APP_OCM_DATA_SIZE
#error APP_OCM_DATA_SIZE has not been defined in ocm_size.h
#endif

/*
 * The `free' ocm area that ultra does not use.
 */
#if APP_OCM_CODE_SIZE || APP_OCM_DATA_SIZE
#define OCM_FREE_START	(OCMSTART + APP_OCM_CODE_SIZE)
#define OCM_FREE_LENGTH	(OCMSIZE - APP_OCM_CODE_SIZE - APP_OCM_DATA_SIZE)
#else
#define OCM_FREE_START OCMEND
#define OCM_FREE_LENGTH 0
#endif

/*
 * If you want to limit OCM use for text/data or completely disable it
 * you can change these values.
 */
#define OCM_TEXT_LENGTH	OCM_FREE_LENGTH
#define OCM_DATA_LENGTH	OCM_FREE_LENGTH

#define	RAM_START	KERNELSTART
#define	RAM_LENGTH	((SDRAMSTART + CONFIG_MIN_RAMSIZE) - RAM_START)
#define	TEXT		ram
#define	DATA		ram
#define	INIT		ram
#define	BSS		ram

#ifndef DATA_ADDR
#define	DATA_ADDR
#endif

#include <asm-generic/vmlinux.lds.h>

OUTPUT_ARCH(ubicom32)
ENTRY(_start)

MEMORY {
	ram		: ORIGIN = RAM_START, LENGTH = RAM_LENGTH
	syscall		: ORIGIN = OS_SYSCALL_BEGIN, LENGTH = (OS_SYSCALL_END - OS_SYSCALL_BEGIN)
	ocm		: ORIGIN = OCM_FREE_START, LENGTH = OCM_FREE_LENGTH
}

jiffies = jiffies_64 + 4;

/*
 * Fixed locations required by gdb coredumps.
 *
 * Note that the names are what gdb is expecting so renaming will break
 * the toolchain.
 */
__ocm_begin		= OCMSTART;
__ocm_limit		= __ocm_begin + OCMSIZE;
__sdram_begin		= SDRAMSTART;
__sdram_limit		= __sdram_begin + CONFIG_MIN_RAMSIZE;
__filemedia_begin_addr	= FLASHSTART;
__filemedia_end_addr	= __filemedia_begin_addr + 0x00800000;

/*
 * For internal diagnostics
 */
__os_syscall_begin	= OS_SYSCALL_BEGIN;
__os_syscall_end	= OS_SYSCALL_END;

SECTIONS {

	.fixed_text : {
		_begin = .;
		*(.skip_syscall)
		*(.old_syscall_entry.text)
		__fixed_text_end = .;
	} > TEXT
	. = _begin + SIZEOF(.fixed_text) ;

	/*
	 * System call text in lower ocm (fixed location, can never change)
	 */
	__syscall_text_load_begin = .;
	__syscall_text_run_begin = OS_SYSCALL_BEGIN;

	.syscall_text __syscall_text_run_begin : AT(__syscall_text_load_begin) {
		*(.syscall_entry.text) /* Must be at OS_SYSCALL_BEGIN 0x3ffc0040 */
		*(.kernel_unprotected)
		. = ALIGN(4);
		__syscall_text_run_end = .;
	} > syscall /* .syscall_text */
	. = __syscall_text_load_begin + __syscall_text_run_end - __syscall_text_run_begin ;
	__ocm_text_load_begin = .;
	__ocm_text_run_begin = OCM_FREE_START ;
	.ocm_text __ocm_text_run_begin : AT(__ocm_text_load_begin) {
#if OCM_TEXT_LENGTH
		*(.ocm_text)
		*(.sched.text)
		*(.spinlock.text)
#include <asm/ocm_text.lds.inc>
		. = ALIGN(4);
#endif
		__ocm_text_run_end = .;
		__data_begin = ALIGN(OCM_SECTOR_SIZE);
	} > ocm /* .ocm_text */

	.ocm_module_text __ocm_text_run_end (NOLOAD) : AT(__ocm_text_run_end) {
		__ocm_inst_heap_begin = .;
		/* Reserve the min requested */
		. += (CONFIG_OCM_MODULES_RESERVATION) * 1024;
#ifdef CONFIG_OCM_MODULES_MAY_CONSUME_REMAINING_CODESPACE
		/* Round up to OCM sector size (we cannot use it for data) */
		. = ALIGN(OCM_SECTOR_SIZE);
#endif
		__ocm_inst_heap_end = .;
		/* update __data_begin */
		__data_begin = ALIGN(OCM_SECTOR_SIZE);
	} > ocm  /* .ocm_module_text */

	. = __ocm_text_load_begin + __ocm_text_run_end - __ocm_text_run_begin ;
	__ocm_text_load_end = .;

	__ocm_data_load_begin = .;
	__ocm_data_run_begin = __data_begin ;
#if OCM_DATA_LENGTH
	.ocm_data __ocm_data_run_begin : AT(__ocm_data_load_begin) {
#if defined(CONFIG_IRQSTACKS_USEOCM)
		percpu_irq_stacks = .;
		. += NR_CPUS * THREAD_SIZE;
#endif
		*(.ocm_data)
		. = ALIGN(4) ;
		__ocm_data_run_end = .;
	} > ocm
	. = __ocm_data_load_begin + __ocm_data_run_end - __ocm_data_run_begin ;
#else
	__ocm_data_run_end = __ocm_data_run_begin;
#endif
	__ocm_data_load_end = .;

	__ocm_free_begin = __ocm_data_run_end;
	__ocm_free_end = OCM_FREE_START + OCM_FREE_LENGTH;

	.text __ocm_data_load_end : AT(__ocm_data_load_end) {
		. = ALIGN(4);
		_stext = .;
		_text = .;
		TEXT_TEXT
		SCHED_TEXT
		LOCK_TEXT
		*(.text.lock)
		*(.text.__libgcc_udivmodsi)
		*(.text.__libgcc_divmodsi)
		*(.text.__libgcc_muldi3)
		*(.text.__libgcc_udivmoddi)
		*(.text.__libgcc_divmoddi)
		*(.text.*)
#if OCM_TEXT_LENGTH == 0
		*(.ocm_text)
		*(.sched.text)
		*(.spinlock.text)
#endif
		. = ALIGN(16);          /* Exception table              */
		__start___ex_table = .;
		*(__ex_table)
		__stop___ex_table = .;

		*(.rodata) *(.rodata.*)
		*(__vermagic)		/* Kernel version magic */
		*(__markers_strings)
		*(.rodata1)
		*(.rodata.str1.1)
		*(__tracepoints_strings)

		 /* PCI quirks */
		__start_pci_fixups_early = . ;
			*(.pci_fixup_early)
		__end_pci_fixups_early = . ;
		__start_pci_fixups_header = . ;
			*(.pci_fixup_header)
		__end_pci_fixups_header = . ;
		__start_pci_fixups_final = . ;
			*(.pci_fixup_final)
		__end_pci_fixups_final = . ;
		__start_pci_fixups_enable = . ;
			*(.pci_fixup_enable)
		__end_pci_fixups_enable = . ;
		__start_pci_fixups_resume = . ;
		       *(.pci_fixup_resume)
		__end_pci_fixups_resume = . ;
		__start_pci_fixups_resume_early = . ;
		       *(.pci_fixup_resume_early)
		__end_pci_fixups_resume_early = . ;
		__start_pci_fixups_suspend  = . ;
		       *(.pci_fixup_suspend)
		__end_pci_fixups_suspend = . ;

		__start_builtin_fw = . ;
			*(.builtin_fw)
		__end_builtin_fw = . ;


		/* Kernel symbol table: Normal symbols */
		. = ALIGN(4);
		__start___ksymtab = .;
		*(__ksymtab)
		__stop___ksymtab = .;

		/* Kernel symbol table: GPL-only symbols */
		__start___ksymtab_gpl = .;
		*(__ksymtab_gpl)
		__stop___ksymtab_gpl = .;

		/* Kernel symbol table: Normal unused symbols */
		__start___ksymtab_unused = .;
		*(__ksymtab_unused)
		__stop___ksymtab_unused = .;

		/* Kernel symbol table: GPL-only unused symbols */
		__start___ksymtab_unused_gpl = .;
		*(__ksymtab_unused_gpl)
		__stop___ksymtab_unused_gpl = .;

		/* Kernel symbol table: GPL-future symbols */
		__start___ksymtab_gpl_future = .;
		*(__ksymtab_gpl_future)
		__stop___ksymtab_gpl_future = .;

		/* Kernel symbol table: Normal symbols */
		__start___kcrctab = .;
		*(__kcrctab)
		__stop___kcrctab = .;

		/* Kernel symbol table: GPL-only symbols */
		__start___kcrctab_gpl = .;
		*(__kcrctab_gpl)
		__stop___kcrctab_gpl = .;

		/* Kernel symbol table: GPL-future symbols */
		__start___kcrctab_gpl_future = .;
		*(__kcrctab_gpl_future)
		__stop___kcrctab_gpl_future = .;

		/* Kernel symbol table: strings */
		*(__ksymtab_strings)

		/* Built-in module parameters */
		. = ALIGN(4) ;
		__start___param = .;
		*(__param)
		__stop___param = .;

		. = ALIGN(4) ;
		_etext = . ;
	} > TEXT

	.data DATA_ADDR : {
		. = ALIGN(4);
		_sdata = . ;
		DATA_DATA
#if OCM_DATA_LENGTH == 0
		*(.ocm_data)
#endif
		. = ALIGN(8192) ;
		_data_protection_end = .;
		*(.data.init_task)
		. = ALIGN(4);
		_edata = . ;
	} > DATA

	.init : {
		. = ALIGN(4096);
		__init_begin = .;
		_sinittext = .;
		INIT_TEXT
		_einittext = .;
		*(.init.rodata)
		INIT_DATA
		. = ALIGN(16);
		__setup_start = .;
		*(.init.setup)
		__setup_end = .;
		__initcall_start = .;
		INITCALLS
		__initcall_end = .;
		__con_initcall_start = .;
		*(.con_initcall.init)
		__con_initcall_end = .;
		___security_initcall_start = .;
		*(.security_initcall.init)
		___security_initcall_end = .;
#ifdef CONFIG_BLK_DEV_INITRD
		. = ALIGN(4);
		__initramfs_start = .;
		*(.init.ramfs)
		__initramfs_end = .;
#endif
		. = ALIGN(4096);
		__per_cpu_start = .;
			*(.data.percpu)
			*(.data.percpu.shared_aligned)
		__per_cpu_end = .;

		. = ALIGN(4096);
		__init_end = .;
	} > INIT

	  .eh_frame   :
	  {
	    PROVIDE (___eh_frame_begin = .);
	    *(.eh_frame)
	    LONG (0);
	    PROVIDE (___eh_frame_end = .);
	  } > INIT

	/DISCARD/ : {
		EXIT_TEXT
		EXIT_DATA
		*(.exitcall.exit)
	}

	.bss : {
		. = ALIGN(4);
		_sbss = . ;
		*(.bss)
		*(COMMON)
		. = ALIGN(4) ;
		_ebss = . ;
		_end = . ;
	} > BSS

	NOTES > BSS

}
