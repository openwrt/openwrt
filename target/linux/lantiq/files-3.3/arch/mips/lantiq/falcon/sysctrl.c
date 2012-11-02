/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 * Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <linux/ioport.h>
#include <linux/export.h>
#include <linux/clkdev.h>
#include <asm/delay.h>

#include <lantiq_soc.h>

#include "devices.h"
#include "../clk.h"

/* infrastructure control register */
#define SYS1_INFRAC		0x00bc
/* Configuration fuses for drivers and pll */
#define STATUS_CONFIG		0x0040

/* GPE frequency selection */
#define GPPC_OFFSET		24
#define GPEFREQ_MASK		0x00000C0
#define GPEFREQ_OFFSET		10
/* Clock status register */
#define LTQ_SYSCTL_CLKS		0x0000
/* Clock enable register */
#define LTQ_SYSCTL_CLKEN	0x0004
/* Clock clear register */
#define LTQ_SYSCTL_CLKCLR	0x0008
/* Activation Status Register */
#define LTQ_SYSCTL_ACTS		0x0020
/* Activation Register */
#define LTQ_SYSCTL_ACT		0x0024
/* Deactivation Register */
#define LTQ_SYSCTL_DEACT	0x0028
/* reboot Register */
#define LTQ_SYSCTL_RBT		0x002c
/* CPU0 Clock Control Register */
#define LTQ_SYS1_CPU0CC         0x0040
/* clock divider bit */
#define LTQ_CPU0CC_CPUDIV       0x0001

static struct resource ltq_sysctl_res[] = {
	MEM_RES("sys1", LTQ_SYS1_BASE_ADDR, LTQ_SYS1_SIZE),
	MEM_RES("syseth", LTQ_SYS_ETH_BASE_ADDR, LTQ_SYS_ETH_SIZE),
	MEM_RES("sysgpe", LTQ_SYS_GPE_BASE_ADDR, LTQ_SYS_GPE_SIZE),
};

static struct resource ltq_status_res =
	MEM_RES("status", LTQ_STATUS_BASE_ADDR, LTQ_STATUS_SIZE);
static struct resource ltq_ebu_res =
	MEM_RES("ebu", LTQ_EBU_BASE_ADDR, LTQ_EBU_SIZE);

static void __iomem *ltq_sysctl[3];
static void __iomem *ltq_status_membase;
void __iomem *ltq_sys1_membase;
void __iomem *ltq_ebu_membase;

#define ltq_reg_w32(m, x, y)	ltq_w32((x), ltq_sysctl[m] + (y))
#define ltq_reg_r32(m, x)	ltq_r32(ltq_sysctl[m] + (x))
#define ltq_reg_w32_mask(m, clear, set, reg)	\
		ltq_reg_w32(m, (ltq_reg_r32(m, reg) & ~(clear)) | (set), reg)

#define ltq_status_w32(x, y)	ltq_w32((x), ltq_status_membase + (y))
#define ltq_status_r32(x)	ltq_r32(ltq_status_membase + (x))

static inline void
ltq_sysctl_wait(struct clk *clk,
		unsigned int test, unsigned int reg)
{
	int err = 1000000;

	do {} while (--err && ((ltq_reg_r32(clk->module, reg)
					& clk->bits) != test));
	if (!err)
		pr_err("module de/activation failed %d %08X %08X %08X\n",
				clk->module, clk->bits, test,
				ltq_reg_r32(clk->module, reg) & clk->bits);
}

static int
ltq_sysctl_activate(struct clk *clk)
{
	ltq_reg_w32(clk->module, clk->bits, LTQ_SYSCTL_CLKEN);
	ltq_reg_w32(clk->module, clk->bits, LTQ_SYSCTL_ACT);
	ltq_sysctl_wait(clk, clk->bits, LTQ_SYSCTL_ACTS);
	return 0;
}

static void
ltq_sysctl_deactivate(struct clk *clk)
{
	ltq_reg_w32(clk->module, clk->bits, LTQ_SYSCTL_CLKCLR);
	ltq_reg_w32(clk->module, clk->bits, LTQ_SYSCTL_DEACT);
	ltq_sysctl_wait(clk, 0, LTQ_SYSCTL_ACTS);
}

static int
ltq_sysctl_clken(struct clk *clk)
{
	ltq_reg_w32(clk->module, clk->bits, LTQ_SYSCTL_CLKEN);
	ltq_sysctl_wait(clk, clk->bits, LTQ_SYSCTL_CLKS);
	return 0;
}

static void
ltq_sysctl_clkdis(struct clk *clk)
{
	ltq_reg_w32(clk->module, clk->bits, LTQ_SYSCTL_CLKCLR);
	ltq_sysctl_wait(clk, 0, LTQ_SYSCTL_CLKS);
}

static void
ltq_sysctl_reboot(struct clk *clk)
{
	unsigned int act;
	unsigned int bits;

	act = ltq_reg_r32(clk->module, LTQ_SYSCTL_ACT);
	bits = ~act & clk->bits;
	if (bits != 0) {
		ltq_reg_w32(clk->module, bits, LTQ_SYSCTL_CLKEN);
		ltq_reg_w32(clk->module, bits, LTQ_SYSCTL_ACT);
		ltq_sysctl_wait(clk, bits, LTQ_SYSCTL_ACTS);
	}
	ltq_reg_w32(clk->module, act & clk->bits, LTQ_SYSCTL_RBT);
	ltq_sysctl_wait(clk, clk->bits, LTQ_SYSCTL_ACTS);
}

/* enable the ONU core */
static void
ltq_gpe_enable(void)
{
	unsigned int freq;
	unsigned int status;

	/* if if the clock is already enabled */
	status = ltq_reg_r32(SYSCTL_SYS1, SYS1_INFRAC);
	if (status & (1 << (GPPC_OFFSET + 1)))
		return;

	if (ltq_status_r32(STATUS_CONFIG) == 0)
		freq = 1; /* use 625MHz on unfused chip */
	else
		freq = (ltq_status_r32(STATUS_CONFIG) &
			GPEFREQ_MASK) >>
			GPEFREQ_OFFSET;

	/* apply new frequency */
	ltq_reg_w32_mask(SYSCTL_SYS1, 7 << (GPPC_OFFSET + 1),
		freq << (GPPC_OFFSET + 2) , SYS1_INFRAC);
	udelay(1);

	/* enable new frequency */
	ltq_reg_w32_mask(SYSCTL_SYS1, 0, 1 << (GPPC_OFFSET + 1), SYS1_INFRAC);
	udelay(1);
}

static inline void
clkdev_add_sys(const char *dev, unsigned int module,
				unsigned int bits)
{
	struct clk *clk = kzalloc(sizeof(struct clk), GFP_KERNEL);

	clk->cl.dev_id = dev;
	clk->cl.con_id = NULL;
	clk->cl.clk = clk;
	clk->module = module;
	clk->bits = bits;
	clk->activate = ltq_sysctl_activate;
	clk->deactivate = ltq_sysctl_deactivate;
	clk->enable = ltq_sysctl_clken;
	clk->disable = ltq_sysctl_clkdis;
	clk->reboot = ltq_sysctl_reboot;
	clkdev_add(&clk->cl);
}

void __init
ltq_soc_init(void)
{
	int i;

	for (i = 0; i < 3; i++)
		ltq_sysctl[i] = ltq_remap_resource(&ltq_sysctl_res[i]);

	ltq_sys1_membase = ltq_sysctl[0];
	ltq_status_membase = ltq_remap_resource(&ltq_status_res);
	ltq_ebu_membase = ltq_remap_resource(&ltq_ebu_res);

	ltq_gpe_enable();

	/* get our 3 static rates for cpu, fpi and io clocks */
	if (ltq_sys1_r32(LTQ_SYS1_CPU0CC) & LTQ_CPU0CC_CPUDIV)
		clkdev_add_static(CLOCK_200M, CLOCK_100M, CLOCK_200M);
	else
		clkdev_add_static(CLOCK_400M, CLOCK_100M, CLOCK_200M);

	/* add our clock domains */
	clkdev_add_sys("falcon_gpio.0", SYSCTL_SYSETH, ACTS_PADCTRL0 | ACTS_P0);
	clkdev_add_sys("falcon_gpio.1", SYSCTL_SYS1, ACTS_PADCTRL1 | ACTS_P1);
	clkdev_add_sys("falcon_gpio.2", SYSCTL_SYSETH, ACTS_PADCTRL2 | ACTS_P2);
	clkdev_add_sys("falcon_gpio.3", SYSCTL_SYS1, ACTS_PADCTRL3 | ACTS_P3);
	clkdev_add_sys("falcon_gpio.4", SYSCTL_SYS1, ACTS_PADCTRL4 | ACTS_P4);
	clkdev_add_sys("ltq_asc.1", SYSCTL_SYS1, ACTS_ASC1_ACT);
	clkdev_add_sys("i2c-falcon.0", SYSCTL_SYS1, ACTS_I2C_ACT);
}
