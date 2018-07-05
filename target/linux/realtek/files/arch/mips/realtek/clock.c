/*
 *  Realtek RLX based boards common routines
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
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
#include <linux/clkdev.h>
#include <linux/clk-provider.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

static struct clk *clks[4];

static struct clk *__init realtek_add_sys_clkdev(
	const char *id, unsigned long rate)
{
	struct clk *clk;
	int err;

	clk = clk_register_fixed_rate(NULL, id, NULL, 0, rate);
	if (!clk)
		panic("failed to allocate %s clock structure", id);

	err = clk_register_clkdev(clk, id, NULL);
	if (err)
		panic("unable to register %s clock device", id);

	return clk;
}

#define RTL8196C_DEFAULT_BUS_CLOCK_RATE		200000000

static const u32 rtl8196c_cpu_clk[] =
	{250000000, 270000000, 290000000, 310000000, 330000000, 350000000, 370000000, 390000000};

static const u32 rtl8196c_dram_clk[] =
	{65625000, 78125000, 125000000, 150000000, 156250000, 168750000, 193750000, 200000000};

static void __init rtl8196c_clocks_init(void)
{
	unsigned long cpu_rate;
	unsigned long dram_rate;
	unsigned long bus_rate;
	unsigned long timer_rate;
	u32 bs;
	u32 cpu_clk_sel;
	u32 cpu_clk_div_sel;
	u32 sdr_clk_sel;
	u32 bus_clk_sel;
	u32 timer_clk_div;

	bs = realtek_sys_read(REALTEK_SYS_REG_BOOTSTRAP);

	cpu_clk_sel = (bs >> RTL8196C_BOOTSTRAP_CPU_FREQ_SHIFT) & RTL8196C_BOOTSTRAP_CPU_FREQ_MASK;
	cpu_clk_div_sel = bs & RTL8196C_BOOTSTRAP_CPU_FREQ_DIV;
	sdr_clk_sel = (bs >> RTL8196C_BOOTSTRAP_SDRAM_CLK_SEL_SHIFT) & RTL8196C_BOOTSTRAP_SDRAM_CLK_SEL_MASK;
	bus_clk_sel = bs & RTL8196C_BOOTSTRAP_CLKLX_FROM_CLKM;

	cpu_rate = rtl8196c_cpu_clk[cpu_clk_sel];

	if (cpu_clk_div_sel)
		cpu_rate /= 2;

	dram_rate = rtl8196c_dram_clk[sdr_clk_sel];

	if (bus_clk_sel)
		bus_rate = dram_rate;
	else
		bus_rate = RTL8196C_DEFAULT_BUS_CLOCK_RATE;

	clks[0] = realtek_add_sys_clkdev("cpu", cpu_rate);
	clks[1] = realtek_add_sys_clkdev("dram", dram_rate);
	clks[2] = realtek_add_sys_clkdev("bus", bus_rate);

	clk_add_alias("uart", NULL, "bus", NULL);

	if (soc_is_rtl8196c_rev_a())
		timer_clk_div = 20;
	else
		timer_clk_div = 2;

	timer_rate = bus_rate / timer_clk_div;

	clks[3] = realtek_add_sys_clkdev("timer", timer_rate);

	clk_add_alias("wdt", NULL, "timer", NULL);

	__raw_writel(timer_clk_div << REALTEK_TC_CLOCK_DIV_FACTOR_SHIFT, realtek_tc_base + REALTEK_TC_REG_CLOCK_DIV);
}

void __init realtek_clocks_init(void)
{
	if (soc_is_rtl8196c())
		rtl8196c_clocks_init();
	else
		BUG();
}

unsigned long __init
realtek_get_sys_clk_rate(const char *id)
{
	struct clk *clk;
	unsigned long rate;

	clk = clk_get(NULL, id);
	if (IS_ERR(clk))
		panic("unable to get %s clock, err=%d", id, (int) PTR_ERR(clk));

	rate = clk_get_rate(clk);
	clk_put(clk);

	return rate;
}
