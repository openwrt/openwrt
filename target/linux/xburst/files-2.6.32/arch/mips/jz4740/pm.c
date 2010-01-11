/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4740 SoC power management support
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/init.h>
#include <linux/pm.h>
#include <linux/delay.h>
#include <linux/suspend.h>
#include <asm/mach-jz4740/regs.h>

extern void jz4740_intc_suspend(void);
extern void jz4740_intc_resume(void);

static int jz_pm_enter(suspend_state_t state)
{
	unsigned long nfcsr = REG_EMC_NFCSR;
	uint32_t scr = REG_CPM_SCR;

	/* Disable nand flash */
	REG_EMC_NFCSR = ~0xff;

 	udelay(100);

	/*stop udc and usb*/
	REG_CPM_SCR &= ~( 1<<6 | 1<<7);
	REG_CPM_SCR |= 0<<6 | 1<<7;

	jz4740_intc_suspend();

 	/* Enter SLEEP mode */
	REG_CPM_LCR &= ~CPM_LCR_LPM_MASK;
	REG_CPM_LCR |= CPM_LCR_LPM_SLEEP;
	__asm__(".set\tmips3\n\t"
		"wait\n\t"
		".set\tmips0");

	/* Restore to IDLE mode */
	REG_CPM_LCR &= ~CPM_LCR_LPM_MASK;
	REG_CPM_LCR |= CPM_LCR_LPM_IDLE;

	/* Restore nand flash control register */
	REG_EMC_NFCSR = nfcsr;

	jz4740_intc_resume();

	/* Restore sleep control register */
	REG_CPM_SCR = scr;

	return 0;
}

static struct platform_suspend_ops jz_pm_ops = {
	.valid		= suspend_valid_only_mem,
	.enter		= jz_pm_enter,
};

/*
 * Initialize power interface
 */
int __init jz_pm_init(void)
{
	suspend_set_ops(&jz_pm_ops);
	return 0;

}
late_initcall(jz_pm_init);
