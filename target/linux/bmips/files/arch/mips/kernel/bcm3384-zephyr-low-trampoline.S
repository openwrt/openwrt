/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>

#define ZEPHYR_KERNEL_ENTRY		0x80010000

	.section .rodata.bcm3384_zephyr_trampoline, "a"
	.balign 4

	.set	push
	.set	mips32
	.set	noreorder

EXPORT(bcm3384_zephyr_trampoline)
	mfc0	t0, CP0_CONFIG
	li	t1, ~CONF_CM_CMASK
	and	t0, t0, t1
	ori	t0, t0, CONF_CM_CACHABLE_NONCOHERENT
	mtc0	t0, CP0_CONFIG
	nop
	nop
	nop

	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	a3, zero

	li	t0, ZEPHYR_KERNEL_ENTRY
	jr	t0
	nop
EXPORT(bcm3384_zephyr_trampoline_end)

	.set	pop
