/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/asm.h>
#include <asm/cpu.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>

#define BCM3384_VIPER_ALIAS_OFFSET	0x08000000

/*
 * BCM3384 stock firmware loads this image through the Viper/BMIPS4350, then
 * Linux proper runs on Zephyr/BMIPS5000. Inline this at __kernel_entry so the
 * first executed instruction can split those two paths before the generic
 * kernel entry jumps to its linked address.
 */
	.macro	bcm3384_zephyr_wrapper_entry kernel_entry
	.set	push
	.set	noreorder

	mfc0	t0, CP0_PRID
	li	t1, PRID_IMP_MASK
	and	t0, t0, t1
	li	t1, PRID_IMP_BMIPS5000
	bne	t0, t1, 1f
	nop

	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	a3, zero

	mfc0	t0, CP0_CONFIG
	li	t1, ~CONF_CM_CMASK
	and	t0, t0, t1
	ori	t0, t0, CONF_CM_CACHABLE_NONCOHERENT
	mtc0	t0, CP0_CONFIG
	nop
	nop
	nop

	j	\kernel_entry
	nop

1:
	mfc0	t0, CP0_CONFIG
	li	t1, ~CONF_CM_CMASK
	and	t0, t0, t1
	ori	t0, t0, CONF_CM_UNCACHED
	mtc0	t0, CP0_CONFIG
	nop
	nop
	nop

	li	sp, 0x8feff000

	PTR_LA	t0, bcm3384_wrapper_main
	li	t1, BCM3384_VIPER_ALIAS_OFFSET
	or	t0, t0, t1
	jr	t0
	nop

	.set	pop
	.endm
