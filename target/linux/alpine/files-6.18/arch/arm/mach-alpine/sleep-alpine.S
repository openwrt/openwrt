/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Annapurna labs cpu-idle handler. */

#include <linux/linkage.h>

ENTRY(alpine_suspend_finish)
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #(1 << 2)		@ Disable the C bit
	mcr	p15, 0, r0, c1, c0, 0
	mov r8, lr		@backup lr
	bl v7_flush_dcache_louis
	mov lr, r8
	mrc	p15, 0, r0, c1, c0, 1
	bic	r0, r0, #(1 << 6)		@ Disable SMP bit
	mcr	p15, 0, r0, c1, c0, 1
	isb
	dsb
		@This should shutdown
	wfi
		@We didn't shutdown. probably - we have a pending interrupt.
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #(1 << 2)		@ Enable the C bit
	mcr	p15, 0, r0, c1, c0, 0
	mrc	p15, 0, r0, c1, c0, 1
	orr	r0, r0, #(1 << 6)		@ Enable SMP bit
	mcr	p15, 0, r0, c1, c0, 1
	isb
	dsb
	mov pc, lr
ENDPROC(alpine_suspend_finish)
