/*
 * Ralink RT288x SoC specific setup
 *
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
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
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

static void rt288x_restart(char *command)
{
	rt288x_sysc_wr(RT2880_RESET_SYSTEM, SYSC_REG_RESET_CTRL);
	while (1)
		if (cpu_wait)
			cpu_wait();
}

static void rt288x_halt(void)
{
	while (1)
		cpu_wait();
}

static void __init rt288x_early_serial_setup(void)
{
	struct uart_port p;
	int err;

	memset(&p, 0, sizeof(p));
	p.flags		= UPF_SKIP_TEST;
	p.iotype	= UPIO_AU;
	p.uartclk	= rt288x_sys_freq;
	p.regshift	= 2;
	p.type		= PORT_16550A;

	p.mapbase	= RT2880_UART0_BASE;
	p.membase	= ioremap_nocache(p.mapbase, RT2880_UART0_SIZE);
	p.line		= 0;
	p.irq		= RT2880_INTC_IRQ_UART0;

	err = early_serial_setup(&p);
	if (err)
		printk(KERN_ERR "RT288x: early UART0 registration failed %d\n",
			err);

	p.mapbase	= RT2880_UART1_BASE;
	p.membase	= ioremap_nocache(p.mapbase, RT2880_UART1_SIZE);
	p.line		= 1;
	p.irq		= RT2880_INTC_IRQ_UART1;

	err = early_serial_setup(&p);
	if (err)
		printk(KERN_ERR "RT288x: early UART1 registration failed %d\n",
			err);
}

unsigned int __cpuinit get_c0_compare_irq(void)
{
	return CP0_LEGACY_COMPARE_IRQ;
}

void __init ramips_soc_setup(void)
{
	rt288x_sysc_base = ioremap_nocache(RT2880_SYSC_BASE, RT2880_SYSC_SIZE);
	rt288x_memc_base = ioremap_nocache(RT2880_MEMC_BASE, RT2880_MEMC_SIZE);

	rt288x_detect_sys_type();
	rt288x_detect_sys_freq();

	printk(KERN_INFO "%s running at %lu.%02lu MHz\n", ramips_sys_type,
		rt288x_cpu_freq / 1000000,
		(rt288x_cpu_freq % 1000000) * 100 / 1000000);

	_machine_restart = rt288x_restart;
	_machine_halt = rt288x_halt;
	pm_power_off = rt288x_halt;

	rt288x_early_serial_setup();
}

void __init plat_time_init(void)
{
	mips_hpt_frequency = rt288x_cpu_freq / 2;
}
