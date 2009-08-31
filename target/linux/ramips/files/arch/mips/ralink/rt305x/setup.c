/*
 * Ralink RT305x SoC specific setup
 *
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
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
#include <linux/serial_8250.h>

#include <asm/mips_machine.h>
#include <asm/reboot.h>
#include <asm/time.h>

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "machine.h"

enum rt305x_mach_type rt305x_mach;

static void rt305x_restart(char *command)
{
	rt305x_sysc_wr(RT305X_RESET_SYSTEM, SYSC_REG_RESET_CTRL);
	while (1)
		if (cpu_wait)
			cpu_wait();
}

static void rt305x_halt(void)
{
	while (1)
		if (cpu_wait)
			cpu_wait();
}

static void __init rt305x_early_serial_setup(void)
{
	struct uart_port p;
	int err;

	memset(&p, 0, sizeof(p));
	p.flags		= UPF_SKIP_TEST;
	p.iotype	= UPIO_AU;
	p.uartclk	= rt305x_sys_freq;
	p.regshift	= 2;
	p.type		= PORT_16550A;

	p.mapbase	= RT305X_UART0_BASE;
	p.membase	= ioremap_nocache(p.mapbase, RT305X_UART0_SIZE);
	p.line		= 0;
	p.irq		= RT305X_INTC_IRQ_UART0;

	err = early_serial_setup(&p);
	if (err)
		printk(KERN_ERR "RT305x: early UART0 registration failed %d\n",
			err);

	p.mapbase	= RT305X_UART1_BASE;
	p.membase	= ioremap_nocache(p.mapbase, RT305X_UART1_SIZE);
	p.line		= 1;
	p.irq		= RT305X_INTC_IRQ_UART1;

	err = early_serial_setup(&p);
	if (err)
		printk(KERN_ERR "RT305x: early UART1 registration failed %d\n",
			err);
}

unsigned int __cpuinit get_c0_compare_irq(void)
{
	return CP0_LEGACY_COMPARE_IRQ;
}

void __init ramips_soc_setup(void)
{
	rt305x_sysc_base = ioremap_nocache(RT305X_SYSC_BASE, PAGE_SIZE);
	rt305x_memc_base = ioremap_nocache(RT305X_MEMC_BASE, PAGE_SIZE);

	rt305x_detect_sys_type();
	rt305x_detect_sys_freq();

	printk(KERN_INFO "%s running at %lu.%02lu MHz\n", ramips_sys_type,
		rt305x_cpu_freq / 1000000,
		(rt305x_cpu_freq % 1000000) * 100 / 1000000);

	_machine_restart = rt305x_restart;
	_machine_halt = rt305x_halt;
	pm_power_off = rt305x_halt;

	rt305x_early_serial_setup();
}

void __init plat_time_init(void)
{
	mips_hpt_frequency = rt305x_cpu_freq / 2;
}

static int __init rt305x_machine_setup(void)
{
	mips_machine_setup(rt305x_mach);

	return 0;
}

arch_initcall(rt305x_machine_setup);
