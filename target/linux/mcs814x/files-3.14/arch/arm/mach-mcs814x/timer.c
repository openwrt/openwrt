/*
 * Moschip MCS814x timer routines
 *
 * Copyright (C) 2012, Florian Fainelli <florian@openwrt.org>
 *
 * Licensed under GPLv2
 */
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>

#include <asm/mach/time.h>
#include <mach/mcs814x.h>

/* Timer block registers */
#define TIMER_VAL	0x00
#define TIMER_CTL	0x04
#define  TIMER_CTL_EN	0x01
#define  TIMER_CTL_DBG	0x02

static u32 last_reload;
static u32 timer_correct;
static u32 clock_rate;
static u32 timer_reload_value;
static void __iomem *mcs814x_timer_base;

static inline u32 ticks2usecs(u32 x)
{
	return x / (clock_rate / 1000000);
}

/*
 * Returns number of ms since last clock interrupt.  Note that interrupts
 * will have been disabled by do_gettimeoffset()
 */
static u32 mcs814x_gettimeoffset(void)
{
	u32 ticks = readl_relaxed(mcs814x_timer_base + TIMER_VAL);

	if (ticks < last_reload)
		return ticks2usecs(ticks + (u32)(0xffffffff - last_reload));
	else
		return ticks2usecs(ticks - last_reload);
}


static irqreturn_t mcs814x_timer_interrupt(int irq, void *dev_id)
{
	u32 count = readl_relaxed(mcs814x_timer_base + TIMER_VAL);

	/* take into account delay up to this moment */
	last_reload = count + timer_correct + timer_reload_value;

	if (last_reload < timer_reload_value) {
		last_reload = timer_reload_value;
	} else {
		if (timer_correct == 0)
			timer_correct = readl_relaxed(mcs814x_timer_base + TIMER_VAL) - count;
	}
	writel_relaxed(last_reload, mcs814x_timer_base + TIMER_VAL);

	timer_tick();

	return IRQ_HANDLED;
}

static struct irqaction mcs814x_timer_irq = {
	.name		= "mcs814x-timer",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= mcs814x_timer_interrupt,
};

static struct of_device_id mcs814x_timer_ids[] = {
	{ .compatible = "moschip,mcs814x-timer" },
	{ /* sentinel */ },
};

static void __init mcs814x_of_timer_init(void)
{
	struct device_node *np;
	const unsigned int *intspec;

	np = of_find_matching_node(NULL, mcs814x_timer_ids);
	if (!np)
		panic("unable to find compatible timer node in dtb");

	mcs814x_timer_base = of_iomap(np, 0);
	if (!mcs814x_timer_base)
		panic("unable to remap timer cpu registers");

	intspec = of_get_property(np, "interrupts", NULL);
	if (!intspec)
		panic("no interrupts property for timer");

	mcs814x_timer_irq.irq = be32_to_cpup(intspec);
}

void __init mcs814x_timer_init(void)
{
	struct clk *clk;

	arch_gettimeoffset = mcs814x_gettimeoffset;

	clk = clk_get_sys("timer0", NULL);
	if (IS_ERR_OR_NULL(clk))
		panic("unable to get timer0 clock");

	clock_rate = clk_get_rate(clk);

	mcs814x_of_timer_init();

	pr_info("Timer frequency: %d (kHz)\n", clock_rate / 1000);

	timer_reload_value = 0xffffffff - (clock_rate / HZ);

	/* disable timer */
	writel_relaxed(~TIMER_CTL_EN, mcs814x_timer_base + TIMER_CTL);
	writel_relaxed(timer_reload_value, mcs814x_timer_base + TIMER_VAL);
	last_reload = timer_reload_value;

	setup_irq(mcs814x_timer_irq.irq, &mcs814x_timer_irq);
	/* enable timer, stop timer in debug mode */
	writel_relaxed(TIMER_CTL_EN | TIMER_CTL_DBG,
		mcs814x_timer_base + TIMER_CTL);
}
