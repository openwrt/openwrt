/*
 *  Ralink RT288x SoC platform device registration
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
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

#include <asm/addrspace.h>

#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

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
