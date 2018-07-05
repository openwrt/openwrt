/*
 *  Realtek RTL819X specific clock source routines
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/clocksource.h>
#include <linux/sched_clock.h>
#include <linux/io.h>

#include <asm/addrspace.h>
#include <asm/time.h>
#include <asm/c-lexra.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

static u32 tc_frequency;
static u32 tc_data_shift;

static __init void rtl8196c_tc1_init(void)
{
	u32 tc1_data;
	u32 val;

	if (soc_is_rtl8196c_rev_a()) {
		tc_data_shift = RTL8196C_TC_DATA_REVA_SHIFT;
		tc1_data = RTL8196C_TC_DATA_REVA_MASK;
	} else {
		tc_data_shift = RTL8196C_TC_DATA_REVB_SHIFT;
		tc1_data = RTL8196C_TC_DATA_REVB_MASK;
	}

	__raw_writel(tc1_data << tc_data_shift, realtek_tc_base + REALTEK_TC_REG_DATA1);

	val = __raw_readl(realtek_tc_base + REALTEK_TC_REG_CTRL);
	val |= REALTEK_TC_CTRL_TC1_EN | REALTEK_TC_CTRL_TC1_MODE;
	__raw_writel(val, realtek_tc_base + REALTEK_TC_REG_CTRL);

	val = __raw_readl(realtek_tc_base + REALTEK_TC_REG_IR);
	val |= REALTEK_TC_IR_TC1_PENDING;
	val &= ~REALTEK_TC_IR_TC1_EN;
	__raw_writel(val, realtek_tc_base + REALTEK_TC_REG_IR);
}

static __init void rtl819x_tc1_init(void)
{
	tc_frequency = realtek_get_sys_clk_rate("timer");

	if (soc_is_rtl8196c())
		rtl8196c_tc1_init();
	else
		BUG();
}

static u64 rtl819x_tc1_count_read(struct clocksource *cs)
{
	return __raw_readl(realtek_tc_base + REALTEK_TC_REG_COUNT1) >> tc_data_shift;
}

static u64 __maybe_unused notrace rtl819x_read_sched_clock(void)
{
	return __raw_readl(realtek_tc_base + REALTEK_TC_REG_COUNT1) >> tc_data_shift;
}

static struct clocksource rtl819x_clocksource = {
	.name		= "RTL819X TC1",
	.read		= rtl819x_tc1_count_read,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

int __init rtl819x_clocksource_init(void)
{
	rtl819x_tc1_init();

	rtl819x_clocksource.rating = 200;
	rtl819x_clocksource.mask = CLOCKSOURCE_MASK(32 - tc_data_shift),

	clocksource_register_hz(&rtl819x_clocksource, tc_frequency);

#ifndef CONFIG_CPU_FREQ
	sched_clock_register(rtl819x_read_sched_clock, 32 - tc_data_shift, tc_frequency);
#endif

	return 0;
}
