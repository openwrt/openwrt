/*
 * Ralink RT3662/RT3883 SoC specific setup
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>

#include <asm/mips_machine.h>
#include <asm/reboot.h>
#include <asm/time.h>

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>
#include "common.h"

static void rt3883_restart(char *command)
{
	rt3883_sysc_wr(RT3883_RSTCTRL_SYS, RT3883_SYSC_REG_RSTCTRL);
	while (1)
		if (cpu_wait)
			cpu_wait();
}

static void rt3883_halt(void)
{
	while (1)
		if (cpu_wait)
			cpu_wait();
}

unsigned int __cpuinit get_c0_compare_irq(void)
{
	return CP0_LEGACY_COMPARE_IRQ;
}

void __init ramips_soc_setup(void)
{
	struct clk *clk;

	rt3883_sysc_base = ioremap_nocache(RT3883_SYSC_BASE, PAGE_SIZE);
	rt3883_memc_base = ioremap_nocache(RT3883_MEMC_BASE, PAGE_SIZE);

	rt3883_detect_sys_type();
	rt3883_clocks_init();

	clk = clk_get(NULL, "cpu");
	if (IS_ERR(clk))
		panic("unable to get CPU clock, err=%ld", PTR_ERR(clk));

	printk(KERN_INFO "%s running at %lu.%02lu MHz\n", ramips_sys_type,
		clk_get_rate(clk) / 1000000,
		(clk_get_rate(clk) % 1000000) * 100 / 1000000);

	_machine_restart = rt3883_restart;
	_machine_halt = rt3883_halt;
	pm_power_off = rt3883_halt;

	clk = clk_get(NULL, "uart");
	if (IS_ERR(clk))
		panic("unable to get UART clock, err=%ld", PTR_ERR(clk));

	ramips_early_serial_setup(0, RT3883_UART0_BASE, clk_get_rate(clk),
				  RT3883_INTC_IRQ_UART0);
	ramips_early_serial_setup(1, RT3883_UART1_BASE, clk_get_rate(clk),
				  RT3883_INTC_IRQ_UART1);
}

void __init plat_time_init(void)
{
	struct clk *clk;

	clk = clk_get(NULL, "cpu");
	if (IS_ERR(clk))
		panic("unable to get CPU clock, err=%ld", PTR_ERR(clk));

	mips_hpt_frequency = clk_get_rate(clk) / 2;
}
