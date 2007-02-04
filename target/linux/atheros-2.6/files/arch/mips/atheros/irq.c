/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

/*
 * Interrupt support for AR531X WiSOC.
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/pm.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/irq.h>
#include <asm/bootinfo.h>
#include <asm/mipsregs.h>
#include <asm/irq_cpu.h>
#include "ar531x.h"


/* ARGSUSED */
irqreturn_t
spurious_irq_handler(int cpl, void *dev_id)
{
    /* 
    printk("spurious_irq_handler: %d  cause=0x%8.8x  status=0x%8.8x\n",
	   cpl, cause_intrs, status_intrs); 
    */
	return IRQ_NONE;
}

/* ARGSUSED */
irqreturn_t
spurious_misc_handler(int cpl, void *dev_id)
{
    /*
    printk("spurious_misc_handler: 0x%x isr=0x%8.8x imr=0x%8.8x\n",
	   cpl, ar531x_isr, ar531x_imr);
    */
	return IRQ_NONE;
}

static struct irqaction spurious_irq  = {
	.handler	= spurious_irq_handler,
	.flags		= SA_INTERRUPT,
	.name		= "spurious_irq",
};

static struct irqaction spurious_misc  = {
	.handler	= spurious_misc_handler,
	.flags		= SA_INTERRUPT,
	.name		= "spurious_misc",
};

asmlinkage void plat_irq_dispatch(void)
{
#ifdef CONFIG_ATHEROS_AR5312
	if (mips_machtype == MACH_ATHEROS_AR5312)
		ar5312_irq_dispatch();
#endif
#ifdef CONFIG_ATHEROS_AR5315
	if (mips_machtype == MACH_ATHEROS_AR5315)
		ar5315_irq_dispatch();
#endif
}

void __init arch_init_irq(void)
{
	clear_c0_status(ST0_IM);
	mips_cpu_irq_init(0);

	/* Initialize interrupt controllers */
#ifdef CONFIG_ATHEROS_AR5312
	if (mips_machtype == MACH_ATHEROS_AR5312)
		ar5312_misc_intr_init(AR531X_MISC_IRQ_BASE);
#endif
#ifdef CONFIG_ATHEROS_AR5315
	if (mips_machtype == MACH_ATHEROS_AR5315)
		ar5315_misc_intr_init(AR531X_MISC_IRQ_BASE);
#endif

	/* Default "spurious interrupt" handlers */
	setup_irq(AR531X_IRQ_NONE, &spurious_irq);
	setup_irq(AR531X_MISC_IRQ_NONE, &spurious_misc);
}
