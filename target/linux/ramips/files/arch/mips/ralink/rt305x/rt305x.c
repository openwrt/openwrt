/*
 * Ralink RT305x SoC specific setup
 *
 * Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

unsigned char rt305x_sys_type[RT305X_SYS_TYPE_LEN];

unsigned long rt305x_cpu_freq;
EXPORT_SYMBOL_GPL(rt305x_cpu_freq);

unsigned long rt305x_sys_freq;
EXPORT_SYMBOL_GPL(rt305x_sys_freq);

void __iomem * rt305x_intc_base;
void __iomem * rt305x_sysc_base;
void __iomem * rt305x_memc_base;

void __init rt305x_detect_sys_type(void)
{
	u32 n0;
	u32 n1;
	u32 id;

	n0 = rt305x_sysc_rr(SYSC_REG_CHIP_NAME0);
	n1 = rt305x_sysc_rr(SYSC_REG_CHIP_NAME1);
	id = rt305x_sysc_rr(SYSC_REG_CHIP_ID);

	snprintf(rt305x_sys_type, RT305X_SYS_TYPE_LEN,
		"Ralink %c%c%c%c%c%c%c%c id:%u rev:%u",
		(char) (n0 & 0xff), (char) ((n0 >> 8) & 0xff),
		(char) ((n0 >> 16) & 0xff), (char) ((n0 >> 24) & 0xff),
		(char) (n1 & 0xff), (char) ((n1 >> 8) & 0xff),
		(char) ((n1 >> 16) & 0xff), (char) ((n1 >> 24) & 0xff),
		(id >> CHIP_ID_ID_SHIFT) & CHIP_ID_ID_MASK,
		(id & CHIP_ID_REV_MASK));
}

void __init rt305x_detect_sys_freq(void)
{
	u32	t;

	t = rt305x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
	t = ((t >> SYSTEM_CONFIG_CPUCLK_SHIFT) & SYSTEM_CONFIG_CPUCLK_MASK);

	switch (t) {
	case SYSTEM_CONFIG_CPUCLK_320:
		rt305x_cpu_freq = 320000000;
		break;
	case SYSTEM_CONFIG_CPUCLK_384:
		rt305x_cpu_freq = 384000000;
		break;
	}

	rt305x_sys_freq = rt305x_cpu_freq / 3;
}
