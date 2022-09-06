/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Realtek RTL930X SRAM clock setters
 * Copyright (C) 2022 Markus Stockhausen <markus.stockhausen@gmx.de>
 */

#include <asm/mipsregs.h>
#include <dt-bindings/clock/rtl83xx-clk.h>

#include "clk-rtl83xx.h"

#define rSW	$t0
#define rSOC	$t1
#define rMSK	$t2
#define rREG	$t3
#define rTMP1	$t4
#define rTMP2	$t5
#define rCP0	$t6
#define rONE	$t7

.set	noreorder

.globl	rtcl_930x_dram_start
rtcl_930x_dram_start:

/*
 * Functions start here and should avoid access to normal memory. REMARK! Do not
 * forget about stack pointer and dirty caches that might interfere.
 */

.globl	rtcl_930x_dram_set_rate
.ent	rtcl_930x_dram_set_rate
rtcl_930x_dram_set_rate:

#ifdef CONFIG_RTL930X

	/* disable MIPS 34K branch and return prediction */
	mfc0	rCP0, CP0_CONFIG, 7
	ori	rTMP1, rTMP1, 0xC
	mtc0	rTMP1, CP0_CONFIG, 7

	li	rONE, 1

	/* write & read SRAM staging to clear ongoing traffic */
	li	rMSK, RTL_SRAM_BASE_UNCACHED
	lw	rTMP1, 0(rMSK)
	lw	rTMP2, 4(rMSK)
	sw	rTMP1, 0(rMSK)
	sw	rTMP2, 4(rMSK)
	lw	rTMP1, 0(rMSK)
	lw	rTMP2, 4(rMSK)

	li	rSW, RTL_SW_CORE_BASE
	li	rSOC, RTL_SOC_BASE

	/* switch CPU to LXB clock */
	ori	rREG, rSOC, RTL930X_SYS_STATUS
	lw	rTMP1, 0(rREG)
	ins	rTMP1, $0, 0x2, 0x1
	sw	rTMP1, 0(rREG)

	/* set new PLL values */
	ori	rREG, rSW, RTL930X_PLL_CPU_CTRL0
	lw	rTMP1, 0(rREG)
	li	rMSK, 0xfffff00f
	and	rTMP1, rTMP1, rMSK
	sll	$a1, $a1, 4
	or	rTMP1, rTMP1, $a1
	sw	rTMP1, 0(rREG)
	lw	rTMP1, 8(rREG)
	li	rMSK, 0xffffff8f
	and	rTMP1, rTMP1, rMSK
	sll	$a2, $a2, 4
	or	rTMP1, rTMP1, $a2
	sw	rTMP1, 8(rREG)

	/* use trigger depending on model */
	ori	rREG, rSW, RTL930X_MODEL_NAME_INFO
	lw	rTMP1, 0(rREG)
	ext	rTMP2, rTMP1, 0x4, 0x1	
	beqz	rTMP2, trigger_glb_ctrl
	li	rTMP2, 0x3
	ext	rTMP1, rTMP1, 0x0, 0x4	
	bge	rTMP1, rTMP2, trigger_glb_ctrl
	nop
trigger_sw_div_ctrl:
	ori	rREG, rSW, RTL930X_PLL_SW_DIV_CTRL
	lw	rTMP1, 0(rREG)
	ins	rTMP1, $0, 17, 0x1
	sw	rTMP1, 0(rREG)
	ins	rTMP1, rONE, 17, 0x1
	sw	rTMP1, 0(rREG)
	b	wait_pll_ready
	nop
trigger_glb_ctrl:
	ori	rREG, rSW, RTL930X_PLL_GLB_CTRL0
	lw	rTMP1, 0(rREG)
	ins	rTMP1, $0, 30, 0x1
	sw	rTMP1, 0(rREG)
	ins	rTMP1, rONE, 30, 0x1
	sw	rTMP1, 0(rREG)

	ori	rREG, rSW, RTL930X_PLL_GLB_CTRL0
wait_pll_ready:
	lw	rTMP1, 0(rREG)
	ext	rTMP2, rTMP1, 0xF, 0x1
	beqz	rTMP2, wait_pll_ready
	nop

	/* switch CPU to PLL clock */
	ori	rREG, rSOC, RTL930X_SYS_STATUS
	lw	rTMP1, 0(rREG)
	ins	rTMP1, rONE, 0x2, 0x1
	sw	rTMP1, 0(rREG)

	/* stabilize clock */
	li	rTMP1, 5000
wait_clock:
	bnez	rTMP1, wait_clock
	addiu	rTMP1, rTMP1, -1

	/* restore branch prediction */
	mtc0	rCP0, CP0_CONFIG, 7
	jr	$ra
	nop

#else /* !CONFIG_RTL930X */

	jr	$ra
	nop

#endif

.end	rtcl_930x_dram_set_rate

/*
 * End marker. Do not delete.
 */
	.word RTL_SRAM_MARKER
.globl	rtcl_930x_dram_size
rtcl_930x_dram_size:
	.word .-rtcl_930x_dram_start

