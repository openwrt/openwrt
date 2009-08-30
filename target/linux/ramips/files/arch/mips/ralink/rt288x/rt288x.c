/*
 * Ralink RT288x SoC specific setup
 *
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
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

#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

unsigned char rt288x_sys_type[RT288X_SYS_TYPE_LEN];

unsigned long rt288x_cpu_freq;
EXPORT_SYMBOL_GPL(rt288x_cpu_freq);

unsigned long rt288x_sys_freq;
EXPORT_SYMBOL_GPL(rt288x_sys_freq);

void __iomem * rt288x_sysc_base;
void __iomem * rt288x_memc_base;

void __init rt288x_detect_sys_type(void)
{
	u32 n0;
	u32 n1;
	u32 id;

	n0 = rt288x_sysc_rr(SYSC_REG_CHIP_NAME0);
	n1 = rt288x_sysc_rr(SYSC_REG_CHIP_NAME1);
	id = rt288x_sysc_rr(SYSC_REG_CHIP_ID);

	snprintf(rt288x_sys_type, RT288X_SYS_TYPE_LEN,
		"Ralink %c%c%c%c%c%c%c%c id:%u rev:%u",
		(char) (n0 & 0xff), (char) ((n0 >> 8) & 0xff),
		(char) ((n0 >> 16) & 0xff), (char) ((n0 >> 24) & 0xff),
		(char) (n1 & 0xff), (char) ((n1 >> 8) & 0xff),
		(char) ((n1 >> 16) & 0xff), (char) ((n1 >> 24) & 0xff),
		(id >> CHIP_ID_ID_SHIFT) & CHIP_ID_ID_MASK,
		(id & CHIP_ID_REV_MASK));
}

void __init rt288x_detect_sys_freq(void)
{
	u32	t;

	t = rt288x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
	t = ((t >> SYSTEM_CONFIG_CPUCLK_SHIFT) & SYSTEM_CONFIG_CPUCLK_MASK);

	switch (t) {
	case SYSTEM_CONFIG_CPUCLK_250:
		rt288x_cpu_freq = 250000000;
		break;
	case SYSTEM_CONFIG_CPUCLK_266:
		rt288x_cpu_freq = 266666667;
		break;
	case SYSTEM_CONFIG_CPUCLK_280:
		rt288x_cpu_freq = 280000000;
		break;
	case SYSTEM_CONFIG_CPUCLK_300:
		rt288x_cpu_freq = 300000000;
		break;
	}

	rt288x_sys_freq = rt288x_cpu_freq / 2;
}

