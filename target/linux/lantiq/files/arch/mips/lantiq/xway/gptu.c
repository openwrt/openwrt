/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/pm.h>
#include <linux/export.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/reboot.h>

#include <lantiq_soc.h>
#include "../clk.h"

#include "../devices.h"

#define ltq_gptu_w32(x, y)	ltq_w32((x), ltq_gptu_membase + (y))
#define ltq_gptu_r32(x)		ltq_r32(ltq_gptu_membase + (x))


/* the magic ID byte of the core */
#define GPTU_MAGIC	0x59
/* clock control register */
#define GPTU_CLC	0x00
/* id register */
#define GPTU_ID		0x08
/* interrupt node enable */
#define GPTU_IRNEN	0xf4
/* interrupt control register */
#define GPTU_IRCR	0xf8
/* interrupt capture register */
#define GPTU_IRNCR	0xfc
/* there are 3 identical blocks of 2 timers. calculate register offsets */
#define GPTU_SHIFT(x)	(x % 2 ? 4 : 0)
#define GPTU_BASE(x)	(((x >> 1) * 0x20) + 0x10)
/* timer control register */
#define GPTU_CON(x)	(GPTU_BASE(x) + GPTU_SHIFT(x) + 0x00)
/* timer auto reload register */
#define GPTU_RUN(x)	(GPTU_BASE(x) + GPTU_SHIFT(x) + 0x08)
/* timer manual reload register */
#define GPTU_RLD(x)	(GPTU_BASE(x) + GPTU_SHIFT(x) + 0x10)
/* timer count register */
#define GPTU_CNT(x)	(GPTU_BASE(x) + GPTU_SHIFT(x) + 0x18)

/* GPTU_CON(x) */
#define CON_CNT		BIT(2)
#define CON_EDGE_FALL	BIT(7)
#define CON_SYNC	BIT(8)
#define CON_CLK_INT	BIT(10)

/* GPTU_RUN(x) */
#define RUN_SEN		BIT(0)
#define RUN_RL		BIT(2)

/* set clock to runmode */
#define CLC_RMC		BIT(8)
/* bring core out of suspend */
#define CLC_SUSPEND	BIT(4)
/* the disable bit */
#define CLC_DISABLE	BIT(0)

#define TIMER_INTERRUPT	(INT_NUM_IM3_IRL0 + 22)

enum gptu_timer {
	TIMER1A = 0,
	TIMER1B,
	TIMER2A,
	TIMER2B,
	TIMER3A,
	TIMER3B
};

static struct resource ltq_gptu_resource =
	MEM_RES("GPTU", LTQ_GPTU_BASE_ADDR, LTQ_GPTU_SIZE);

static void __iomem *ltq_gptu_membase;

static irqreturn_t timer_irq_handler(int irq, void *priv)
{
	int timer = irq - TIMER_INTERRUPT;
	ltq_gptu_w32(1 << timer, GPTU_IRNCR);
	return IRQ_HANDLED;
}

static void gptu_hwinit(void)
{
	struct clk *clk = clk_get_sys("ltq_gptu", NULL);
	clk_enable(clk);
	ltq_gptu_w32(0x00, GPTU_IRNEN);
	ltq_gptu_w32(0xff, GPTU_IRNCR);
	ltq_gptu_w32(CLC_RMC | CLC_SUSPEND, GPTU_CLC);
}

static void gptu_hwexit(void)
{
	ltq_gptu_w32(0x00, GPTU_IRNEN);
	ltq_gptu_w32(0xff, GPTU_IRNCR);
	ltq_gptu_w32(CLC_DISABLE, GPTU_CLC);
}

static int ltq_gptu_enable(struct clk *clk)
{
	int ret = request_irq(TIMER_INTERRUPT + clk->bits, timer_irq_handler,
		IRQF_TIMER, "timer", NULL);
	if (ret) {
		pr_err("gptu: failed to request irq\n");
		return ret;
	}

        ltq_gptu_w32(CON_CNT | CON_EDGE_FALL | CON_SYNC | CON_CLK_INT,
		GPTU_CON(clk->bits));
	ltq_gptu_w32(1, GPTU_RLD(clk->bits));
	ltq_gptu_w32(ltq_gptu_r32(GPTU_IRNEN) | clk->bits, GPTU_IRNEN);
	ltq_gptu_w32(RUN_SEN | RUN_RL, GPTU_RUN(clk->bits));
	return 0;
}

static void ltq_gptu_disable(struct clk *clk)
{
	ltq_gptu_w32(0, GPTU_RUN(clk->bits));
	ltq_gptu_w32(0,	GPTU_CON(clk->bits));
	ltq_gptu_w32(0, GPTU_RLD(clk->bits));
	ltq_gptu_w32(ltq_gptu_r32(GPTU_IRNEN) & ~clk->bits, GPTU_IRNEN);
	free_irq(TIMER_INTERRUPT + clk->bits, NULL);
}

static inline void clkdev_add_gptu(const char *con, unsigned int timer)
{
	struct clk *clk = kzalloc(sizeof(struct clk), GFP_KERNEL);

	clk->cl.dev_id = "ltq_gptu";
	clk->cl.con_id = con;
	clk->cl.clk = clk;
	clk->enable = ltq_gptu_enable;
	clk->disable = ltq_gptu_disable;
	clk->bits = timer;
	clkdev_add(&clk->cl);
}

static int __init gptu_setup(void)
{
	/* remap gptu register range */
	ltq_gptu_membase = ltq_remap_resource(&ltq_gptu_resource);
	if (!ltq_gptu_membase)
		panic("Failed to remap gptu memory");

	/* power up the core */
	gptu_hwinit();

	/* the gptu has a ID register */
	if (((ltq_gptu_r32(GPTU_ID) >> 8) & 0xff) != GPTU_MAGIC) {
		pr_err("gptu: failed to find magic\n");
		gptu_hwexit();
		return -ENAVAIL;
	}

	/* register the clocks */
	clkdev_add_gptu("timer1a", TIMER1A);
	clkdev_add_gptu("timer1b", TIMER1B);
	clkdev_add_gptu("timer2a", TIMER2A);
	clkdev_add_gptu("timer2b", TIMER2B);
	clkdev_add_gptu("timer3a", TIMER3A);
	clkdev_add_gptu("timer3b", TIMER3B);

	pr_info("gptu: 6 timers loaded\n");

	return 0;
}

arch_initcall(gptu_setup);
