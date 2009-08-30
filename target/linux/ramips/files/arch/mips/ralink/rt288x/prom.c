/*
 *  Ralink RT288x SoC specific prom routines
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/serial_reg.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

void __init prom_init(void)
{
	printk(KERN_DEBUG "prom: fw_arg0=%08x, fw_arg1=%08x, "
			"fw_arg2=%08x, fw_arg3=%08x\n",
			(unsigned int)fw_arg0, (unsigned int)fw_arg1,
			(unsigned int)fw_arg2, (unsigned int)fw_arg3);

	rt288x_mach_type = RT288X_MACH_GENERIC;
}

void __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
}

