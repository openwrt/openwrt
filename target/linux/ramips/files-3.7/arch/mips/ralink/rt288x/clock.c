/*
 *  Ralink RT288X clock API
 *
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include "common.h"

struct clk {
	unsigned long rate;
};

static struct clk rt288x_cpu_clk;
static struct clk rt288x_sys_clk;
static struct clk rt288x_wdt_clk;
static struct clk rt288x_uart_clk;

void __init rt288x_clocks_init(void)
{
	u32	t;

	t = rt288x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
	t = ((t >> SYSTEM_CONFIG_CPUCLK_SHIFT) & SYSTEM_CONFIG_CPUCLK_MASK);

	switch (t) {
	case SYSTEM_CONFIG_CPUCLK_250:
		rt288x_cpu_clk.rate = 250000000;
		break;
	case SYSTEM_CONFIG_CPUCLK_266:
		rt288x_cpu_clk.rate = 266666667;
		break;
	case SYSTEM_CONFIG_CPUCLK_280:
		rt288x_cpu_clk.rate = 280000000;
		break;
	case SYSTEM_CONFIG_CPUCLK_300:
		rt288x_cpu_clk.rate = 300000000;
		break;
	}

	rt288x_sys_clk.rate = rt288x_cpu_clk.rate / 2;
	rt288x_uart_clk.rate = rt288x_sys_clk.rate;
	rt288x_wdt_clk.rate = rt288x_sys_clk.rate;
}

/*
 * Linux clock API
 */
struct clk *clk_get(struct device *dev, const char *id)
{
	if (!strcmp(id, "sys"))
		return &rt288x_sys_clk;

	if (!strcmp(id, "cpu"))
		return &rt288x_cpu_clk;

	if (!strcmp(id, "wdt"))
		return &rt288x_wdt_clk;

	if (!strcmp(id, "uart"))
		return &rt288x_uart_clk;

	return ERR_PTR(-ENOENT);
}
EXPORT_SYMBOL(clk_get);

int clk_enable(struct clk *clk)
{
	return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
	return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);

void clk_put(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_put);
