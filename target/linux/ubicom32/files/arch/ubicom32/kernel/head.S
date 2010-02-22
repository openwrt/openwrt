/*
 * arch/ubicom32/kernel/head.S
 *	<TODO: Replace with short file description>
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
#include <linux/sys.h>
#include <linux/linkage.h>
#include <asm/asm-offsets.h>
#include <asm/page_offset.h>
#define __ASM__
#include <asm/ip5000.h>


#define SRC_AN A3
#define DST_AN A4

#define PARAM_DN D0
#define TMP_DN D15
#define TMP2_DN D14

/*
 * The following code is placed at the start of the Linux section of memory.
 * This is the primary entry point for Linux.
 *
 * However, we also want the syscall entry/exit code to be at a fixed address.
 * So we take the primary entry point and reserve 16 bytes.  That address is
 * where the system_call entry point exists.  This 16 bytes basically allows
 * us to jump around the system_call entry point code to the actual startup
 * code.
 *
 * Linux Memory Map (see vlinux.lds.S):
 * 0x40400000 - Primary Entry Point for Linux (jump around code below).
 * 0x40400010 - Old syscall Entry Point.
 */

	.sect	.skip_syscall, "ax", @progbits
	.global	__skip_syscall_section
__skip_syscall_section:
	moveai		A3, #%hi(_start)
	lea.1		A3, %lo(_start)(A3)
	ret		A3
/*
 * __os_node_offset contains the offset from KERNELBASE to the os_node, it is
 * not intended to be used by anything except the boot code.
 */
__os_node_offset:
.long	(_os_node - KERNELSTART)

.text
.global	_start

/*
 * start()
 *	This is the start of the Linux kernel.
 */
_start:
	move.4		SCRATCHPAD1, #0


/*
 * Setup the range registers... the loader has setup a few, but we will go ahead
 * and correct them for our own limits. Note that once set these are never
 * changed again.  The ranges are as follows
 *
 *  D_RANGE0 - io block (set up by loaded)
 *
 *  I_RANGE0 and D_RANGE1 - kernel/ultra loader address space bottom of ocm-> top
 *	of ram typically 0x3ffc0000 - 0x440000000
 *  I_RANGE1 - kernel / userspace transition area (aka syscalls, context switches)
 *	typically 0x3FFC0030 - ~0x3FFC0200
 *  I_RANGE2 / D_RANGE2 - slab area
 *	typically 0x40A00000 - ~0x44000000
 *  I_RANGE3
 *	old system call interface if enabled.
 *
 *   D_RANGE3, D_RANGE4 - unused.
 */
	moveai		SRC_AN, #%hi(PAGE_OFFSET_RAW)
	lea.4		SRC_AN, %lo(PAGE_OFFSET_RAW)(SRC_AN)
	move.4		D_RANGE1_LO, SRC_AN
	move.4		I_RANGE0_LO, SRC_AN

; don't try to calculate I_RANGE_HI, see below
;	moveai		SRC_AN, #%hi(___init_end-4)
;	lea.4		SRC_AN, %lo(___init_end-4)(SRC_AN)
;	move.4		I_RANGE0_HI, SRC_AN

	moveai		SRC_AN, #%hi(SDRAMSTART + CONFIG_MIN_RAMSIZE-4)
	lea.4		SRC_AN, %lo(SDRAMSTART + CONFIG_MIN_RAMSIZE-4)(SRC_AN)
	move.4		D_RANGE1_HI, SRC_AN

; for now allow the whole ram to be executable as well so we don't run into problems
; once we load user more code.
	move.4		I_RANGE0_HI, SRC_AN

#ifdef CONFIG_PROTECT_KERNEL
; when kernel protection is enabled, we only open up syscall and non kernel text
; for userspace apps, for now only irange registers registers 1 and 2 are used for userspace.

	;; syscall range
	moveai		SRC_AN, #%hi(__syscall_text_run_begin)
	lea.4		SRC_AN, %lo(__syscall_text_run_begin)(SRC_AN)
	move.4		I_RANGE1_LO, SRC_AN
	moveai		SRC_AN, #%hi(__syscall_text_run_end)
	lea.4		SRC_AN, %lo(__syscall_text_run_end)(SRC_AN)
	move.4		I_RANGE1_HI, SRC_AN

	;; slab instructions
	moveai		SRC_AN, #%hi(_edata)
	lea.4		SRC_AN, %lo(_edata)(SRC_AN)
	move.4		I_RANGE2_LO, SRC_AN
	;; End of DDR is already in range0 hi so just copy it.
	move.4		I_RANGE2_HI, I_RANGE0_HI

#ifdef CONFIG_OLD_40400010_SYSTEM_CALL
	;; create a small hole for old syscall location
	moveai		SRC_AN, #%hi(0x40400000)
	lea.4		I_RANGE3_LO, 0x10(SRC_AN)
	lea.4		I_RANGE3_HI, 0x14(SRC_AN)
#endif
	;; slab data (same as slab instructions but starting a little earlier).
	moveai		SRC_AN, #%hi(_data_protection_end)
	lea.4		SRC_AN, %lo(_data_protection_end)(SRC_AN)
	move.4		D_RANGE2_LO, SRC_AN
	move.4		D_RANGE2_HI, I_RANGE0_HI

;; enable ranges
	;; skip I_RANGE0_EN
	move.4		I_RANGE1_EN, #-1
	move.4		I_RANGE2_EN, #-1
#ifdef CONFIG_OLD_40400010_SYSTEM_CALL
	move.4		I_RANGE3_EN, #-1
#else
	move.4		I_RANGE3_EN, #0
#endif
	;; skip D_RANGE0_EN or D_RANGE1_EN
	move.4		D_RANGE2_EN, #-1
	move.4		D_RANGE3_EN, #0
	move.4		D_RANGE4_EN, #0
#endif

;
; If __ocm_free_begin is smaller than __ocm_free_end the
; setup OCM text and data ram banks properly
;
	moveai		DST_AN, #%hi(__ocm_free_begin)
	lea.4		TMP_DN, %lo(__ocm_free_begin)(DST_AN)
	moveai		DST_AN, #%hi(__ocm_free_end)
	lea.4		TMP2_DN, %lo(__ocm_free_end)(DST_AN)
	sub.4		#0, TMP2_DN, TMP_DN
	jmple.f		2f
	moveai		DST_AN, #%hi(__data_begin)
	lea.4		TMP_DN, %lo(__data_begin)(DST_AN)
	moveai		DST_AN, #%hi(OCMSTART)
	lea.4		TMP2_DN, %lo(OCMSTART)(DST_AN)
	sub.4		TMP_DN, TMP_DN, TMP2_DN
	lsr.4		TMP_DN, TMP_DN, #15
	lsl.4		TMP_DN, #1, TMP_DN
	moveai		DST_AN, #%hi(OCMC_BASE)
	add.4		OCMC_BANK_MASK(DST_AN), #-1, TMP_DN
	pipe_flush	0
2:
;
; Load .ocm_text
;
	moveai          DST_AN, #%hi(__ocm_text_run_end)
	lea.4           TMP_DN, %lo(__ocm_text_run_end)(DST_AN)
	moveai          DST_AN, #%hi(__ocm_text_run_begin)
	lea.4           DST_AN, %lo(__ocm_text_run_begin)(DST_AN)
	moveai          SRC_AN, #%hi(__ocm_text_load_begin)
	lea.4           SRC_AN, %lo(__ocm_text_load_begin)(SRC_AN)
	jmpt.t          2f

1:      move.4          (DST_AN)4++, (SRC_AN)4++

2:      sub.4           #0, DST_AN, TMP_DN
	jmpne.t         1b
;
; Load .syscall_text
;
	moveai          DST_AN, #%hi(__syscall_text_run_end)
	lea.4           TMP_DN, %lo(__syscall_text_run_end)(DST_AN)
	moveai          DST_AN, #%hi(__syscall_text_run_begin)
	lea.4           DST_AN, %lo(__syscall_text_run_begin)(DST_AN)
	moveai          SRC_AN, #%hi(__syscall_text_load_begin)
	lea.4           SRC_AN, %lo(__syscall_text_load_begin)(SRC_AN)
	jmpt.t          2f

1:	move.4          (DST_AN)4++, (SRC_AN)4++

2:	sub.4           #0, DST_AN, TMP_DN
	jmpne.t         1b

;
; Load .ocm_data
;
	moveai          DST_AN, #%hi(__ocm_data_run_end)
	lea.4           TMP_DN, %lo(__ocm_data_run_end)(DST_AN)
	moveai          DST_AN, #%hi(__ocm_data_run_begin)
	lea.4           DST_AN, %lo(__ocm_data_run_begin)(DST_AN)
	moveai          SRC_AN, #%hi(__ocm_data_load_begin)
	lea.4           SRC_AN, %lo(__ocm_data_load_begin)(SRC_AN)
	jmpt.t          2f

1:      move.4          (DST_AN)4++, (SRC_AN)4++

2:      sub.4           #0, DST_AN, TMP_DN
	jmpne.t         1b

; Clear .bss
;
	moveai		SRC_AN, #%hi(_ebss)
	lea.4		TMP_DN, %lo(_ebss)(SRC_AN)
	moveai		DST_AN, #%hi(_sbss)
	lea.4		DST_AN, %lo(_sbss)(DST_AN)
	jmpt.t		2f

1:	move.4		(DST_AN)4++, #0

2:	sub.4		#0, DST_AN, TMP_DN
	jmpne.t		1b

; save our parameter to devtree (after clearing .bss)
	moveai		DST_AN, #%hi(devtree)
	lea.4		DST_AN, %lo(devtree)(DST_AN)
	move.4		(DST_AN), PARAM_DN

	moveai		sp, #%hi(init_thread_union)
	lea.4		sp, %lo(init_thread_union)(sp)
	movei		TMP_DN, #ASM_THREAD_SIZE
	add.4		sp, sp, TMP_DN
	move.4		-4(sp)++, #0 ; nesting level = 0
	move.4		-4(sp)++, #1 ; KERNEL_THREAD

;; ip3k-elf-gdb backend now sets scratchpad3 to 1 when either continue
;; or single step commands are issued. scratchpad3 is set to 0 when the
;; debugger detaches from the board.
	move.4		TMP_DN, scratchpad3
	lsl.4		TMP_DN, TMP_DN, #0x0
	jmpeq.f		_jump_to_start_kernel
_ok_to_set_break_points_in_linux:
;; THREAD_STALL
	move.4		mt_dbg_active_clr,#-1
;; stalling the threads isn't instantaneous.. need to flush the pipe.
	pipe_flush	0
	pipe_flush	0

_jump_to_start_kernel:
	moveai		SRC_AN, #%hi(start_kernel)
	lea.4		SRC_AN, %lo(start_kernel)(SRC_AN)
	ret		SRC_AN
