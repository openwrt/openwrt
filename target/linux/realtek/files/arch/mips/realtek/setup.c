/*
 *  Realtek RLX based SoC specific setup
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bootmem.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/sizes.h>

#include <asm/bootinfo.h>
#include <asm/idle.h>
#include <asm/time.h>
#include <asm/reboot.h>
#include <asm/mips_machine.h>
#include <asm/prom.h>
#include <asm/c-lexra.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"
#include "serial.h"
#include "gpio.h"
#include "pci.h"
#include "machtypes.h"

#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-eth.h"

#define REALTEK_SOC_TYPE_LEN	64

static char realtek_soc_type[REALTEK_SOC_TYPE_LEN];

static void realtek_restart(char *command)
{
	local_irq_disable();

	__raw_writel(0, realtek_tc_base + REALTEK_TC_REG_WATCHDOG);
	for (;;)
		if (cpu_wait)
			cpu_wait();
}

static void realtek_halt(void)
{
	while (1)
		cpu_wait();
}

static void __init realtek_detect_soc_type(void)
{
	char *chip = "????";
	u32 chiprev;

	chiprev = realtek_sys_read(REALTEK_SYS_REG_REVISION);

	switch (chiprev) {
	case SOC_ID_RTL8196C_REV_A:
		rtl_soc = RTL_SOC_8196C_REV_A;
		chip = "8196C rev A";
		break;

	case SOC_ID_RTL8196C_REV_B:
		rtl_soc = RTL_SOC_8196C_REV_B;
		chip = "8196C rev B";
		break;

	default:
		panic("Realtek: unknown SoC, id:0x%08x", chiprev);
	}

	sprintf(realtek_soc_type, "Realtek RTL%s", chip);
	
	pr_info("Realtek SoC: %s\n", realtek_soc_type);
}

const char *get_system_type(void)
{
	return realtek_soc_type;
}

void __init realtek_set_memory_region(void)
{
	void __iomem *mc_base;
	u32 dram_cfg;
	u32 bank, col, row, dbus_width, memsize;

	mc_base = ioremap_nocache(REALTEK_MC_BASE, REALTEK_MC_SIZE);

	dram_cfg = __raw_readl(mc_base + REALTEK_MC_REG_DRAM_CFG);

	dbus_width = (dram_cfg >> REALTEK_DRAM_DBUS_WIDTH_SHIFT) & REALTEK_DRAM_DBUS_WIDTH_MASK;
	bank = 1 + ((dram_cfg >> REALTEK_DRAM_BANK_WIDTH_SHIFT) & REALTEK_DRAM_BANK_WIDTH_MASK);
	col = 8 + ((dram_cfg >> REALTEK_DRAM_COL_WIDTH_SHIFT) & REALTEK_DRAM_COL_WIDTH_MASK);
	row = 11 + ((dram_cfg >> REALTEK_DRAM_ROW_WIDTH_SHIFT) & REALTEK_DRAM_ROW_WIDTH_MASK);

	memsize = 1 << (bank + col + row + dbus_width);
	
	pr_debug("Memory: %lluMB of RAM detected from Memory Controller\n",
		((unsigned long long) memsize) / SZ_1M);

	add_memory_region(0, memsize, BOOT_MEM_RAM);
}

void __init plat_mem_setup(void)
{
	realtek_sys_base = ioremap_nocache(REALTEK_SYS_BASE, REALTEK_SYS_SIZE);
	realtek_tc_base = ioremap_nocache(REALTEK_TC_BASE, REALTEK_TC_SIZE);
	
	realtek_detect_soc_type();

	realtek_set_memory_region();

	_machine_restart = realtek_restart;
	_machine_halt = realtek_halt;
	pm_power_off = realtek_halt;
}

void __init plat_time_init(void)
{
	unsigned long cpu_clk_rate;
	unsigned long dram_clk_rate;
	unsigned long bus_clk_rate;

	realtek_clocks_init();

	cpu_clk_rate = realtek_get_sys_clk_rate("cpu");
	dram_clk_rate = realtek_get_sys_clk_rate("dram");
	bus_clk_rate = realtek_get_sys_clk_rate("bus");

	pr_info("Clocks: CPU:%lu.%03luMHz, DRAM:%lu.%03luMHz, Bus:%lu.%03luMHz\n",
		cpu_clk_rate / 1000000, (cpu_clk_rate / 1000) % 1000,
		dram_clk_rate / 1000000, (dram_clk_rate / 1000) % 1000,
		bus_clk_rate / 1000000, (bus_clk_rate / 1000) % 1000);

	rtl819x_clockevent_init();
	rtl819x_clocksource_init();
}

void __init plat_lexra_cache_init(void)
{
	/* currently nothing to do */
}

static int __init realtek_setup(void)
{
	realtek_gpio_init();
	realtek_register_uart();
	realtek_register_pci();
	realtek_register_usb();

	mips_machine_setup();

	return 0;
}

arch_initcall(realtek_setup);

static void __init realtek_generic_init(void)
{
	realtek_register_m25p80(NULL);
	realtek_register_eth();
}

MIPS_MACHINE(REALTEK_MACH_GENERIC,
	     "Generic",
	     "Generic Realtek RLX based board",
	     realtek_generic_init);
