/*
 *  Ralink RT3662/RT3883 clock API
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
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
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>
#include "common.h"

struct clk {
	unsigned long rate;
};

static struct clk rt3883_cpu_clk;
static struct clk rt3883_sys_clk;
static struct clk rt3883_wdt_clk;
static struct clk rt3883_uart_clk;

void __init rt3883_clocks_init(void)
{
	u32 syscfg0;
	u32 clksel;
	u32 ddr2;

	syscfg0 = rt3883_sysc_rr(RT3883_SYSC_REG_SYSCFG0);
	clksel = ((syscfg0 >> RT3883_SYSCFG0_CPUCLK_SHIFT) &
		  RT3883_SYSCFG0_CPUCLK_MASK);
	ddr2 = syscfg0 & RT3883_SYSCFG0_DRAM_TYPE_DDR2;

	switch (clksel) {
	case RT3883_SYSCFG0_CPUCLK_250:
		rt3883_cpu_clk.rate = 250000000;
		rt3883_sys_clk.rate = (ddr2) ? 125000000 : 83000000;
		break;
	case RT3883_SYSCFG0_CPUCLK_384:
		rt3883_cpu_clk.rate = 384000000;
		rt3883_sys_clk.rate = (ddr2) ? 128000000 : 96000000;
		break;
	case RT3883_SYSCFG0_CPUCLK_480:
		rt3883_cpu_clk.rate = 480000000;
		rt3883_sys_clk.rate = (ddr2) ? 160000000 : 120000000;
		break;
	case RT3883_SYSCFG0_CPUCLK_500:
		rt3883_cpu_clk.rate = 500000000;
		rt3883_sys_clk.rate = (ddr2) ? 166000000 : 125000000;
		break;
	}

	rt3883_wdt_clk.rate = rt3883_sys_clk.rate;
	rt3883_uart_clk.rate = 40000000;
}

struct clk *clk_get(struct device *dev, const char *id)
{
	if (!strcmp(id, "sys"))
		return &rt3883_sys_clk;

	if (!strcmp(id, "cpu"))
		return &rt3883_cpu_clk;

	if (!strcmp(id, "wdt"))
		return &rt3883_wdt_clk;

	if (!strcmp(id, "uart"))
		return &rt3883_uart_clk;

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
