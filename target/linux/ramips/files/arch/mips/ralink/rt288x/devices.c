/*
 *  Ralink RT288x SoC platform device registration
 *
 *  Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/etherdevice.h>
#include <linux/err.h>
#include <linux/clk.h>

#include <asm/addrspace.h>

#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

static struct resource rt288x_flash0_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT2880_FLASH0_BASE),
		.end	= KSEG1ADDR(RT2880_FLASH0_BASE) +
			  RT2880_FLASH0_SIZE - 1,
	},
};

static struct platform_device rt288x_flash0_device = {
	.name		= "physmap-flash",
	.resource	= rt288x_flash0_resources,
	.num_resources	= ARRAY_SIZE(rt288x_flash0_resources),
};

static struct resource rt288x_flash1_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT2880_FLASH1_BASE),
		.end	= KSEG1ADDR(RT2880_FLASH1_BASE) +
			  RT2880_FLASH1_SIZE - 1,
	},
};

static struct platform_device rt288x_flash1_device = {
	.name		= "physmap-flash",
	.resource	= rt288x_flash1_resources,
	.num_resources	= ARRAY_SIZE(rt288x_flash1_resources),
};

static int rt288x_flash_instance __initdata;
void __init rt288x_register_flash(unsigned int id,
				  struct physmap_flash_data *pdata)
{
	struct platform_device *pdev;
	u32 t;
	int reg;

	switch (id) {
	case 0:
		pdev = &rt288x_flash0_device;
		reg = MEMC_REG_FLASH_CFG0;
		break;
	case 1:
		pdev = &rt288x_flash1_device;
		reg = MEMC_REG_FLASH_CFG1;
		break;
	default:
		return;
	}

	t = rt288x_memc_rr(reg);
	t = (t >> FLASH_CFG_WIDTH_SHIFT) & FLASH_CFG_WIDTH_MASK;

	switch (t) {
	case FLASH_CFG_WIDTH_8BIT:
		pdata->width = 1;
		break;
	case FLASH_CFG_WIDTH_16BIT:
		pdata->width = 2;
		break;
	case FLASH_CFG_WIDTH_32BIT:
		pdata->width = 4;
		break;
	default:
		printk(KERN_ERR "RT288x: flash bank%u witdh is invalid\n", id);
		return;
	}

	pdev->dev.platform_data = pdata;
	pdev->id = rt288x_flash_instance;

	platform_device_register(pdev);
	rt288x_flash_instance++;
}

static struct resource rt288x_wifi_resources[] = {
	{
		.start	= RT2880_WMAC_BASE,
		.end	= RT2880_WMAC_BASE + 0x3FFFF,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT288X_CPU_IRQ_WNIC,
		.end	= RT288X_CPU_IRQ_WNIC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rt288x_wifi_device = {
	.name			= "rt2800_wmac",
	.resource		= rt288x_wifi_resources,
	.num_resources	= ARRAY_SIZE(rt288x_wifi_resources),
	.dev = {
		.platform_data = NULL,
	}
};

void __init rt288x_register_wifi(void)
{
	platform_device_register(&rt288x_wifi_device);
}

static void rt288x_fe_reset(void)
{
	rt288x_sysc_wr(RT2880_RESET_FE, SYSC_REG_RESET_CTRL);
}

static struct resource rt288x_eth_resources[] = {
	{
		.start	= RT2880_FE_BASE,
		.end	= RT2880_FE_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT288X_CPU_IRQ_FE,
		.end	= RT288X_CPU_IRQ_FE,
		.flags	= IORESOURCE_IRQ,
	},
};

struct ramips_eth_platform_data rt288x_eth_data;
static struct platform_device rt288x_eth_device = {
	.name		= "ramips_eth",
	.resource	= rt288x_eth_resources,
	.num_resources	= ARRAY_SIZE(rt288x_eth_resources),
	.dev = {
		.platform_data = &rt288x_eth_data,
	}
};

void __init rt288x_register_ethernet(void)
{
	struct clk *clk;

	clk = clk_get(NULL, "sys");
	if (IS_ERR(clk))
		panic("unable to get SYS clock, err=%ld", PTR_ERR(clk));

	rt288x_eth_data.sys_freq = clk_get_rate(clk);
	rt288x_eth_data.reset_fe = rt288x_fe_reset;
	rt288x_eth_data.min_pkt_len = 64;

	if (!is_valid_ether_addr(rt288x_eth_data.mac))
		random_ether_addr(rt288x_eth_data.mac);

	platform_device_register(&rt288x_eth_device);
}

static struct resource rt288x_wdt_resources[] = {
	{
		.start	= RT2880_TIMER_BASE,
		.end	= RT2880_TIMER_BASE + RT2880_TIMER_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device rt288x_wdt_device = {
	.name		= "ramips-wdt",
	.id		= -1,
	.resource	= rt288x_wdt_resources,
	.num_resources	= ARRAY_SIZE(rt288x_wdt_resources),
};

void __init rt288x_register_wdt(void)
{
	u32 t;

	/* enable WDT reset output on pin SRAM_CS_N */
	t = rt288x_sysc_rr(SYSC_REG_CLKCFG);
	t |= CLKCFG_SRAM_CS_N_WDT;
	rt288x_sysc_wr(t, SYSC_REG_CLKCFG);

	platform_device_register(&rt288x_wdt_device);
}
