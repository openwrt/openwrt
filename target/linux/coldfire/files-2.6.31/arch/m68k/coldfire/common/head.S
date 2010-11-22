/*
 *  head.S is the MMU enabled ColdFire specific initial boot code
 *
 *  Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *  Matt Waddel Matt.Waddel@freescale.com
 *  Kurt Mahan kmahan@freescale.com
 *  Jason Jin Jason.Jin@freescale.com
 *  Shrek Wu B16972@freescale.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Parts of this code came from arch/m68k/kernel/head.S
 */
#include <linux/linkage.h>
#include <linux/init.h>
#include <asm/bootinfo.h>
#include <asm/setup.h>
#include <asm/entry.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/coldfire.h>
#include <asm/cfcache.h>

#define DEBUG

.globl kernel_pg_dir
.globl availmem
.globl set_context
.globl set_fpga

#ifdef DEBUG
/* When debugging use readable names for labels */
#ifdef __STDC__
#define L(name) .head.S.##name
#else
#define L(name) .head.S./**/name
#endif
#else
#ifdef __STDC__
#define L(name) .L##name
#else
#define L(name) .L/**/name
#endif
#endif

/* The __INITDATA stuff is a no-op when ftrace or kgdb are turned on */
#ifndef __INITDATA
#define __INITDATA	.data
#define __FINIT		.previous
#endif

#if CONFIG_SDRAM_BASE != PAGE_OFFSET
/*
 * Kernel mapped to virtual ram address.
 *
 * M5445x:
 *    Data[0]: 0xF0000000 -> 0xFFFFFFFF	System regs
 *    Data[1]: 0xA0000000 -> 0xAFFFFFFF	PCI
 *    Code[0]: Not Mapped
 *    Code[1]: Not Mapped
 *
 * M547x/M548x
 *    Data[0]: 0xF0000000 -> 0xFFFFFFFF	System regs
 *    Data[1]: Not Mapped
 *    Code[0]: Not Mapped
 *    Code[1]: Not Mapped
 */
#if defined(CONFIG_M5445X)
#define ACR0_DEFAULT	#0xF00FA048   /* System regs */
#define ACR1_DEFAULT	#0xA00FA048   /* PCI */
#define ACR2_DEFAULT	#0x00000000   /* Not Mapped */
#define ACR3_DEFAULT	#0x00000000   /* Not Mapped */
#elif defined(CONFIG_M547X_8X)
#define ACR0_DEFAULT	#0xF00FA048   /* System Regs */
#define ACR1_DEFAULT	#0x00000000   /* Not Mapped */
#define ACR2_DEFAULT	#0x00000000   /* Not Mapped */
#define ACR3_DEFAULT	#0x00000000   /* Not Mapped */
#endif

#else /* CONFIG_SDRAM_BASE = PAGE_OFFSET */
/*
 * Kernel mapped to physical ram address.
 *
 * M5445x:
 *    Data[0]: 0xF0000000 -> 0xFFFFFFFF	System regs
 *    Data[1]: 0x40000000 -> 0x4FFFFFFF	SDRAM - uncached
 *    Code[0]: Not Mapped
 *    Code[1]: 0x40000000 -> 0x4FFFFFFF	SDRAM - cached
 *
 * M547x/M548x
 *    Data[0]: 0xF0000000 -> 0xFFFFFFFF	System regs
 *    Data[1]: 0x00000000 -> 0x0FFFFFFF	SDRAM - uncached
 *    Code[0]: Not Mapped
 *    Code[1]: 0x00000000 -> 0x0FFFFFFF	SDRAM - cached
 */
#if defined(CONFIG_M5445X)
#define ACR0_DEFAULT	#0xF00FA048   /* System Regs uncached/precise */
#define ACR1_DEFAULT	#0x400FA028   /* SDRAM cached/copyback */
#define ACR2_DEFAULT	#0x00000000   /* Not mapped */
#define ACR3_DEFAULT	#0x400FA028   /* SDRAM cached/copyback */
#elif defined(CONFIG_M547X_8X)
#define ACR0_DEFAULT	#0xF00FA048   /* System Regs */
#define ACR1_DEFAULT	#0x000FA028   /* SDRAM cached/copy-back */
#define ACR2_DEFAULT	#0x00000000   /* Not mapped */
#define ACR3_DEFAULT	#0x000FA028   /* Instruction cached/copy-back */
#endif
#endif

/* ACR mapping for FPGA (maps 0) */
#define ACR0_FPGA       #0x000FA048   /* ACR0 enable FPGA */

/* Several macros to make the writing of subroutines easier:
 * - func_start marks the beginning of the routine which setups the frame
 *   register and saves the registers, it also defines another macro
 *   to automatically restore the registers again.
 * - func_return marks the end of the routine and simply calls the prepared
 *   macro to restore registers and jump back to the caller.
 * - func_define generates another macro to automatically put arguments
 *   onto the stack call the subroutine and cleanup the stack again.
 */

.macro	load_symbol_address	symbol,register
	movel	#\symbol,\register
.endm
	
.macro	func_start	name,saveregs,savesize,stack=0
L(\name):
	linkw	%a6,#-\stack
	subal	#(\savesize),%sp
	moveml	\saveregs,%sp@
.set	stackstart,-\stack

.macro	func_return_\name
	moveml	%sp@,\saveregs
	addal	#(\savesize),%sp
	unlk	%a6
	rts
.endm
.endm

.macro	func_return	name
	func_return_\name
.endm

.macro	func_call	name
	jbsr	L(\name)
.endm

.macro	move_stack	nr,arg1,arg2,arg3,arg4
.if	\nr
	move_stack	"(\nr-1)",\arg2,\arg3,\arg4
	movel	\arg1,%sp@-
.endif
.endm

.macro	func_define	name,nr=0
.macro	\name	arg1,arg2,arg3,arg4
	move_stack	\nr,\arg1,\arg2,\arg3,\arg4
	func_call	\name
.if	\nr
	lea	%sp@(\nr*4),%sp
.endif
.endm
.endm

func_define	serial_putc,1

.macro	putc	ch
	pea	\ch
	func_call	serial_putc
	addql	#4,%sp
.endm

.macro	dputc	ch
#ifdef DEBUG
	putc	\ch
#endif
.endm

func_define	putn,1

.macro	dputn	nr
#ifdef DEBUG
	putn	\nr
#endif
.endm

/*
	mmu_map  -  creates a new TLB entry

	virt_addr      Must be on proper boundary
	phys_addr      Must be on proper boundary
	itlb           MMUOR_ITLB if instruction TLB or 0
	asid           address space ID
	shared_global  MMUTR_SG if shared between different ASIDs or 0
	size_code      MMUDR_SZ1M  1 MB
	               MMUDR_SZ4K  4 KB
	               MMUDR_SZ8K  8 KB
	               MMUDR_SZ16M 16 MB
	cache_mode     MMUDR_INC   instruction non-cacheable
                       MMUDR_IC    instruction cacheable
                       MMUDR_DWT   data writethrough
	               MMUDR_DCB   data copyback
	               MMUDR_DNCP  data non-cacheable, precise
	               MMUDR_DNCIP data non-cacheable, imprecise
	super_prot     MMUDR_SP if user mode generates exception or 0
	readable       MMUDR_R if permits read access (data TLB) or 0
	writable       MMUDR_W if permits write access (data TLB) or 0
	executable     MMUDR_X if permits execute access (instruction TLB) or 0
	locked         MMUDR_LK prevents TLB entry from being replaced or 0
	temp_data_reg  a data register to use for temporary values
*/
.macro mmu_map	virt_addr,phys_addr,itlb,asid,shared_global,size_code,cache_mode,super_prot,readable,writable,executable,locked,temp_data_reg
	/* Set up search of TLB. */
	movel	#(\virt_addr+1), \temp_data_reg
	movel	\temp_data_reg, MMUAR
	/* Search.  */
	movel	#(MMUOR_STLB + MMUOR_ADR +\itlb), \temp_data_reg
	movew	\temp_data_reg, (MMUOR)
	/* Set up tag value.  */
	movel	#(\virt_addr + \asid + \shared_global + MMUTR_V), \temp_data_reg
	movel	\temp_data_reg, MMUTR
	/* Set up data value.  */
	movel	#(\phys_addr + \size_code + \cache_mode + \super_prot + \readable + \writable + \executable + \locked), \temp_data_reg
	movel	\temp_data_reg, MMUDR
	/* Save it.  */
	movel	#(MMUOR_ACC + MMUOR_UAA + \itlb), \temp_data_reg
	movew	\temp_data_reg, (MMUOR)
.endm	/* mmu_map */

.macro mmu_unmap	virt_addr,itlb,temp_data_reg
	/* Set up search of TLB. */
	movel	#(\virt_addr+1), \temp_data_reg
	movel	\temp_data_reg, MMUAR
	/* Search.  */
	movel	#(MMUOR_STLB + MMUOR_ADR +\itlb), \temp_data_reg
	movew	\temp_data_reg, (MMUOR)
	/* Test for hit.  */
	movel	MMUSR,\temp_data_reg
	btst	#MMUSR_HITN,\temp_data_reg
	beq	1f
	/* Read the TLB.  */
	movel	#(MMUOR_RW + MMUOR_ACC +\itlb), \temp_data_reg
	movew	\temp_data_reg, (MMUOR)
	movel	MMUSR,\temp_data_reg
	/* Set up tag value.  */
	movel	#0, \temp_data_reg
	movel	\temp_data_reg, MMUTR
	/* Set up data value.  */
	movel	#0, \temp_data_reg
	movel	\temp_data_reg, MMUDR
	/* Save it.  */
	movel	#(MMUOR_ACC + MMUOR_UAA + \itlb), \temp_data_reg
	movew	\temp_data_reg, (MMUOR)
1:	
.endm	/* mmu_unmap */

/* .text */
.section ".text.head","ax"
ENTRY(_stext)
/* Version numbers of the bootinfo interface -- if we later pass info
 * from boot ROM we might want to put something real here.
 *
 * The area from _stext to _start will later be used as kernel pointer table
 */
	bras	1f	/* Jump over bootinfo version numbers */

	.long	BOOTINFOV_MAGIC
	.long	0
1:	jmp	__start

.equ	kernel_pg_dir,_stext
.equ	.,_stext+0x1000

ENTRY(_start)
	jra	__start
__INIT
ENTRY(__start)
/* Save the location of u-boot info - cmd line, bd_info, etc. */
	movel	%a7,%a4		/* Don't use %a4 before cf_early_init */
	addl	#0x00000004,%a4	/* offset past top */
	addl	#(PAGE_OFFSET-CONFIG_SDRAM_BASE),%a4	/* high mem offset */

/* Setup initial stack pointer */
	movel	#CONFIG_SDRAM_BASE+0x1000,%sp

/* Setup usp */
	subl	%a0,%a0
	movel	%a0,%usp

#if defined(CONFIG_M5445X)
#if defined(CONFIG_SRAM)
	movel	#(CONFIG_SRAM_BASE+0x221), %d0
#else
	movel	#0x80000000, %d0
#endif
	movec	%d0, %rambar1
#elif defined(CONFIG_M547X_8X)
	movel   #MCF_MBAR, %d0
	movec   %d0, %mbar
	move.l  #(MCF_RAMBAR0 + 0x21), %d0
	movec   %d0, %rambar0
	move.l  #(MCF_RAMBAR1 + 0x21), %d0
	movec   %d0, %rambar1
#endif

	movew	#0x2700,%sr

/* reset cache */
	movel	#(CF_CACR_ICINVA + CF_CACR_DCINVA),%d0
	movecl	%d0,%cacr

	movel	#(MMU_BASE+1),%d0
	movecl	%d0,%mmubar
	movel	#MMUOR_CA,%a0  			/* Clear tlb entries */
	movew	%a0,(MMUOR)
	movel	#(MMUOR_CA + MMUOR_ITLB),%a0 	/* Use ITLB for searches */
	movew	%a0,(MMUOR)
	movel	#0,%a0 				/* Clear Addr Space User ID */
	movecl	%a0,%asid 

/* setup ACRs */
	movel	ACR0_DEFAULT, %d0  		/* ACR0 (DATA) setup */
	movec   %d0, %acr0
	nop
	movel	ACR1_DEFAULT, %d0  		/* ACR1 (DATA) setup */
	movec   %d0, %acr1
	nop
	movel	ACR2_DEFAULT, %d0  		/* ACR2 (CODE) setup */
	movec   %d0, %acr2
	nop
	movel	ACR3_DEFAULT, %d0  		/* ACR3 (CODE) setup */
	movec   %d0, %acr3
	nop

	/* If you change the memory size to another value make a matching 
	   change in paging_init(cf-mmu.c) to zones_size[]. */

#if CONFIG_SDRAM_BASE != PAGE_OFFSET
#if defined(CONFIG_M5445X)
	/* Map 256MB as code */
	mmu_map	(PAGE_OFFSET+0*0x1000000),  (PHYS_OFFSET+0*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+1*0x1000000),  (PHYS_OFFSET+1*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+2*0x1000000),  (PHYS_OFFSET+2*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+3*0x1000000),  (PHYS_OFFSET+3*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+4*0x1000000),  (PHYS_OFFSET+4*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+5*0x1000000),  (PHYS_OFFSET+5*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+6*0x1000000),  (PHYS_OFFSET+6*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+7*0x1000000),  (PHYS_OFFSET+7*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+8*0x1000000),  (PHYS_OFFSET+8*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+9*0x1000000),  (PHYS_OFFSET+9*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+10*0x1000000), (PHYS_OFFSET+10*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+11*0x1000000), (PHYS_OFFSET+11*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+12*0x1000000), (PHYS_OFFSET+12*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+13*0x1000000), (PHYS_OFFSET+13*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+14*0x1000000), (PHYS_OFFSET+14*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+15*0x1000000), (PHYS_OFFSET+15*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_IC,  MMUDR_SP, \
		0, 0, MMUDR_X, MMUDR_LK, %d0

	/* Map 256MB as data also */
	mmu_map	(PAGE_OFFSET+0*0x1000000),  (PHYS_OFFSET+0*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+1*0x1000000),  (PHYS_OFFSET+1*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+2*0x1000000),  (PHYS_OFFSET+2*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+3*0x1000000),  (PHYS_OFFSET+3*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+4*0x1000000),  (PHYS_OFFSET+4*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+5*0x1000000),  (PHYS_OFFSET+5*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+6*0x1000000),  (PHYS_OFFSET+6*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+7*0x1000000),  (PHYS_OFFSET+7*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+8*0x1000000),  (PHYS_OFFSET+8*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+9*0x1000000),  (PHYS_OFFSET+9*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+10*0x1000000), (PHYS_OFFSET+10*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+11*0x1000000), (PHYS_OFFSET+11*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+12*0x1000000), (PHYS_OFFSET+12*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+13*0x1000000), (PHYS_OFFSET+13*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+14*0x1000000), (PHYS_OFFSET+14*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
	mmu_map	(PAGE_OFFSET+15*0x1000000), (PHYS_OFFSET+15*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0

	/* Map ATA registers -- sacrifice a data TLB due to the hw design */
	mmu_map	(0x90000000), (0x90000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0

#elif defined(CONFIG_M547X_8X)

	/* Map first 8 MB as code */
	mmu_map (PAGE_OFFSET+0*1024*1024),  (0*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+1*1024*1024),  (1*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+2*1024*1024),  (2*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+3*1024*1024),  (3*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+4*1024*1024),  (4*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+5*1024*1024),  (5*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+6*1024*1024),  (6*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+7*1024*1024),  (7*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, MMUDR_X, \
		MMUDR_LK, %d0

	/* Map first 8 MB as data */
	mmu_map (PAGE_OFFSET+0*1024*1024),  (0*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+1*1024*1024),  (1*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+2*1024*1024),  (2*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+3*1024*1024),  (3*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+4*1024*1024),  (4*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+5*1024*1024),  (5*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+6*1024*1024),  (6*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
	mmu_map (PAGE_OFFSET+7*1024*1024),  (7*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, MMUDR_LK, %d0
#endif
	/*
	 * Do unity mapping to enable the MMU.  Map first chunk of memory
	 * in place as code/data.  The TLBs will be deleted after the MMU is
	 * enabled and we are executing in high memory.
	 */

#if defined(CONFIG_M5445X)
	/* Map first 16 MB as code */
	mmu_map	(PHYS_OFFSET+0*0x1000000), (PHYS_OFFSET+0*0x1000000), \
		MMUOR_ITLB, 0, MMUTR_SG, MMUDR_SZ16M, MMUDR_INC,  MMUDR_SP, 0, \
		0, MMUDR_X, 0, %d0
	/* Map first 16 MB as data too  */
	mmu_map	(PHYS_OFFSET+0*0x1000000), (PHYS_OFFSET+0*0x1000000), 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, 0, %d0
#elif defined(CONFIG_M547X_8X)
	/* Map first 4 MB as code */
	mmu_map (0*1024*1024), (0*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, \
		MMUDR_X, 0, %d0
	mmu_map (1*1024*1024), (1*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, \
		MMUDR_X, 0, %d0
	mmu_map (2*1024*1024), (2*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, \
		MMUDR_X, 0, %d0
	mmu_map (3*1024*1024), (3*1024*1024), MMUOR_ITLB, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_IC,  MMUDR_SP, 0, 0, \
		MMUDR_X, 0, %d0

	/* Map first 4 MB as data too */
	mmu_map (0*1024*1024), (0*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DCB, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, 0, %d0
	mmu_map (1*1024*1024), (1*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DCB, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, 0, %d0
	mmu_map (2*1024*1024), (2*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DCB, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, 0, %d0
	mmu_map (3*1024*1024), (3*1024*1024), 0, 0, \
		MMUTR_SG, MMUDR_SZ1M, MMUDR_DCB, MMUDR_SP, MMUDR_R, \
		MMUDR_W, 0, 0, %d0
#endif
#endif /* CONFIG_SDRAM_BASE != PAGE_OFFSET */

	/* Turn on MMU */
	movel	#(MMUCR_EN),%a0
	movel	%a0,MMUCR
	nop	/* This synchs the pipeline after a write to MMUCR */

	movel	#__running_high,%a0  /* Get around PC-relative addressing. */
	jmp	%a0@

ENTRY(__running_high)
	load_symbol_address _stext,%sp
	movel	L(memory_start),%a0
	movel	%a0,availmem
	load_symbol_address L(phys_kernel_start),%a0
	load_symbol_address _stext,%a1
	subl	#_stext,%a1
	addl	#PAGE_OFFSET,%a1
	movel	%a1,%a0@

/* zero bss */
	lea	_sbss,%a0
	lea	_ebss,%a1
	clrl	%d0
_loop_bss:
	movel	%d0,(%a0)+
	cmpl	%a0,%a1
	bne	_loop_bss

	/* Unmap unity mappings */
#if CONFIG_SDRAM_BASE != PAGE_OFFSET
#if defined(CONFIG_M5445X)
	mmu_unmap (PHYS_OFFSET+0*0x1000000), MMUOR_ITLB, %d0
	mmu_unmap (PHYS_OFFSET+0*0x1000000), 0, %d0
#elif defined(CONFIG_M547X_8X)
	mmu_unmap (PHYS_OFFSET+0*0x1000000), MMUOR_ITLB, %d0
	mmu_unmap (PHYS_OFFSET+1*0x1000000), MMUOR_ITLB, %d0
	mmu_unmap (PHYS_OFFSET+2*0x1000000), MMUOR_ITLB, %d0
	mmu_unmap (PHYS_OFFSET+3*0x1000000), MMUOR_ITLB, %d0
	mmu_unmap (PHYS_OFFSET+0*0x1000000), 0, %d0
	mmu_unmap (PHYS_OFFSET+1*0x1000000), 0, %d0
	mmu_unmap (PHYS_OFFSET+2*0x1000000), 0, %d0
	mmu_unmap (PHYS_OFFSET+3*0x1000000), 0, %d0
#endif
#endif /* CONFIG_SDRAM_BASE != PAGE_OFFSET */

/* create dma memory mirror TLB mapping */
#if defined(CONFIG_M5445X)
	mmu_map CONFIG_DMA_BASE, \
		CONFIG_SDRAM_BASE, 0, 0, \
		MMUTR_SG, MMUDR_SZ16M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, MMUDR_W, \
		0, MMUDR_LK, %d0
#elif defined(CONFIG_M547X_8X)
        mmu_map (CONFIG_DMA_BASE + 0*1024*1024), \
                (CONFIG_SDRAM_BASE + 0*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 1*1024*1024), \
                (CONFIG_SDRAM_BASE + 1*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 2*1024*1024), \
                (CONFIG_SDRAM_BASE + 2*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 3*1024*1024), \
                (CONFIG_SDRAM_BASE + 3*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 4*1024*1024), \
                (CONFIG_SDRAM_BASE + 4*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 5*1024*1024), \
                (CONFIG_SDRAM_BASE + 5*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 6*1024*1024), \
                (CONFIG_SDRAM_BASE + 6*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
        mmu_map (CONFIG_DMA_BASE + 7*1024*1024), \
                (CONFIG_SDRAM_BASE + 7*1024*1024), 0, 0, \
                MMUTR_SG, MMUDR_SZ1M, MMUDR_DNCP, MMUDR_SP, MMUDR_R, \
                MMUDR_W, 0, MMUDR_LK, %d0
#endif

/* Setup initial stack pointer */
	lea	init_task,%a2 
	lea	init_thread_union+THREAD_SIZE,%sp
	subl	%a6,%a6		/* clear a6 for gdb */

#ifdef CONFIG_MCF_USER_HALT
/* Setup debug control reg to allow halts from user space */
	lea	wdbg_uhe,%a0
	wdebug	(%a0)
#endif

	movel	%a4,uboot_info_stk /* save uboot info to variable */
	jsr	cf_early_init
	jmp	start_kernel

.section ".text.head","ax"
set_context:
func_start	set_context,%d0,(1*4)
	movel	12(%sp),%d0
	movec	%d0,%asid
func_return	set_context

#ifdef CONFIG_M5445X
/*
 * set_fpga(addr,val) on the M5445X
 *
 * Map in 0x00000000 -> 0x0fffffff and then do the write.
 */
set_fpga:
	movew	%sr,%d1
	movew	#0x2700,%sr
	movel	ACR0_FPGA, %d0
	movec   %d0, %acr0
	nop
	moveal	4(%sp),%a0
	movel	8(%sp),%a0@
	movel	ACR0_DEFAULT, %d0
	movec   %d0, %acr0
	nop
	movew	%d1,%sr
	rts
#endif

	.data
	.align	4

availmem:
	.long	0
L(phys_kernel_start):
	.long	PAGE_OFFSET
L(kernel_end):
	.long	0
L(memory_start):
	.long	PAGE_OFFSET_RAW

#ifdef CONFIG_MCF_USER_HALT
/*
 * Enable User Halt Enable in the debug control register.
 */
wdbg_uhe:
	.word	0x2c80	/* DR0 */
	.word	0x00b0	/* 31:16 */
	.word	0x0400	/* 15:0 -- enable UHE */
	.word	0x0000	/* unused */
#endif


