/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/pm.h>
#include <linux/module.h>
#include <asm/reboot.h>

#include <lantiq_soc.h>
#include "../machtypes.h"
#include <base_reg.h>
#include <sys1_reg.h>
#include <boot_reg.h>
#include <ebu_reg.h>

static struct svip_reg_sys1 *const sys1 = (struct svip_reg_sys1 *)LTQ_SYS1_BASE;
static struct svip_reg_ebu *const ebu = (struct svip_reg_ebu *)LTQ_EBU_BASE;

#define CPLD_CMDREG3  ((volatile unsigned char*)(KSEG1 + 0x120000f3))
extern void switchip_reset(void);

static void ltq_machine_restart(char *command)
{
	printk(KERN_NOTICE "System restart\n");
	local_irq_disable();

	if (mips_machtype == LANTIQ_MACH_EASY33016 ||
	    mips_machtype == LANTIQ_MACH_EASY336) {
		/* We just use the CPLD function to reset the entire system as a
		   workaround for the switch reset problem */
		local_irq_disable();
		ebu_w32(0x120000f1, addr_sel_2);
		ebu_w32(0x404027ff, con_2);

		if (mips_machtype == LANTIQ_MACH_EASY336)
			/* set bit 0 to reset SVIP */
			*CPLD_CMDREG3 = (1<<0);
		else
			/* set bit 7 to reset SVIP, set bit 3 to reset xT */
			*CPLD_CMDREG3 = (1<<7) | (1<<3);
	} else {
		*LTQ_BOOT_RVEC(0) = 0;
		/* reset all except PER, SUBSYS and CPU0 */
		sys1_w32(0x00043F3E, rreqr);
		/* release WDT0 reset */
		sys1_w32(0x00000100, rrlsr);
		/* restore reset value for clock enables */
		sys1_w32(~0x0c000040, clkclr);
		/* reset SUBSYS (incl. DDR2) and CPU0 */
		sys1_w32(0x00030001, rbtr);
	}

	for (;;)
		;
}

static void ltq_machine_halt(void)
{
	printk(KERN_NOTICE "System halted.\n");
	local_irq_disable();
	for (;;)
		;
}

static void ltq_machine_power_off(void)
{
	printk(KERN_NOTICE "Please turn off the power now.\n");
	local_irq_disable();
	for (;;)
		;
}

/* This function is used by the watchdog driver */
int ltq_reset_cause(void)
{
	return 0;
}
EXPORT_SYMBOL_GPL(ltq_reset_cause);

static int __init mips_reboot_setup(void)
{
	_machine_restart = ltq_machine_restart;
	_machine_halt = ltq_machine_halt;
	pm_power_off = ltq_machine_power_off;
	return 0;
}

arch_initcall(mips_reboot_setup);
