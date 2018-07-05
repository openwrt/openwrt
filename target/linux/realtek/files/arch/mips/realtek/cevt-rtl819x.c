/*
 *  Realtek RTL819X specific clock event routines
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>

#include <asm/addrspace.h>
#include <asm/time.h>
#include <asm/c-lexra.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

static u32 tc_max_count;
static u32 tc_frequency;
static u32 tc_data_shift;

static void __init rtl8196c_tc0_init(void)
{
	u32 val;

	if (soc_is_rtl8196c_rev_a()) {
		tc_data_shift = RTL8196C_TC_DATA_REVA_SHIFT;
		tc_max_count = RTL8196C_TC_DATA_REVA_MASK >> 1;
	} else {
		tc_data_shift = RTL8196C_TC_DATA_REVB_SHIFT;
		tc_max_count = RTL8196C_TC_DATA_REVB_MASK >> 1;
	}

	val = __raw_readl(realtek_tc_base + REALTEK_TC_REG_CTRL);
	val &= ~(REALTEK_TC_CTRL_TC0_EN | REALTEK_TC_CTRL_TC0_MODE);
	__raw_writel(val, realtek_tc_base + REALTEK_TC_REG_CTRL);

	val = __raw_readl(realtek_tc_base + REALTEK_TC_REG_IR);
	val |= REALTEK_TC_IR_TC0_EN | REALTEK_TC_IR_TC0_PENDING;
	__raw_writel(val, realtek_tc_base + REALTEK_TC_REG_IR);
}

static void __init rtl819x_tc0_init(void)
{
	tc_frequency = realtek_get_sys_clk_rate("timer");

	if (soc_is_rtl8196c())
		rtl8196c_tc0_init();
	else
		BUG();
}


static int rtl819x_timer_set_next_event(unsigned long delta, struct clock_event_device *evt)
{
	u32 val;

	__raw_writel(delta << tc_data_shift, realtek_tc_base + REALTEK_TC_REG_DATA0);

	val = __raw_readl(realtek_tc_base + REALTEK_TC_REG_CTRL);
	val &= ~REALTEK_TC_CTRL_TC0_EN;
	__raw_writel(val, realtek_tc_base + REALTEK_TC_REG_CTRL);
	val |= REALTEK_TC_CTRL_TC0_EN;
	__raw_writel(val, realtek_tc_base + REALTEK_TC_REG_CTRL);

	return 0;
}

void rtl819x_timer_event_handler(struct clock_event_device *dev)
{
}

static struct clock_event_device rtl819x_clockevent = {
	.name		= "RTL819X TC0",
	.features	= CLOCK_EVT_FEAT_ONESHOT,
	.set_next_event	= rtl819x_timer_set_next_event,
	.event_handler	= rtl819x_timer_event_handler,
};


static irqreturn_t rtl819x_timer_interrupt(int irq, void *dev_id)
{
	u32 tc0_irs;
	struct clock_event_device *cd = &rtl819x_clockevent;
	
	/* TC0 interrupt acknowledge */
	tc0_irs = __raw_readl(realtek_tc_base + REALTEK_TC_REG_IR);
	tc0_irs |= REALTEK_TC_IR_TC0_PENDING;
	__raw_writel(tc0_irs, realtek_tc_base + REALTEK_TC_REG_IR);

	cd->event_handler(cd);

	return IRQ_HANDLED;
}

static struct irqaction rtl819x_tc0_irqaction = {
	.handler	= rtl819x_timer_interrupt,
	.flags		= IRQF_TIMER | IRQF_SHARED,
	.name		= "timer",
};

int __init rtl819x_clockevent_init(void)
{
	struct clock_event_device *cd;

	rtl819x_tc0_init();

	cd = &rtl819x_clockevent;

	clockevent_set_clock(cd, tc_frequency);

	cd->rating = 100;
	cd->irq = REALTEK_SOC_IRQ_CLOCK_TIMER;
	cd->max_delta_ns = clockevent_delta2ns(tc_max_count, cd);
	cd->min_delta_ns = clockevent_delta2ns(0x300, cd);
	cd->cpumask = cpumask_of(0);

	clockevents_register_device(&rtl819x_clockevent);

	setup_irq(REALTEK_SOC_IRQ_CLOCK_TIMER, &rtl819x_tc0_irqaction);

	return 0;
}
