/*
 *  Ralink RT305x SoC platform device registration
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
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

#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>
#include "devices.h"

static struct resource rt305x_flash0_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT305X_FLASH0_BASE),
		.end	= KSEG1ADDR(RT305X_FLASH0_BASE) +
			  RT305X_FLASH0_SIZE - 1,
	},
};

static struct platform_device rt305x_flash0_device = {
	.name		= "physmap-flash",
	.resource	= rt305x_flash0_resources,
	.num_resources	= ARRAY_SIZE(rt305x_flash0_resources),
};

static struct resource rt305x_flash1_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT305X_FLASH1_BASE),
		.end	= KSEG1ADDR(RT305X_FLASH1_BASE) +
			  RT305X_FLASH1_SIZE - 1,
	},
};

static struct platform_device rt305x_flash1_device = {
	.name		= "physmap-flash",
	.resource	= rt305x_flash1_resources,
	.num_resources	= ARRAY_SIZE(rt305x_flash1_resources),
};

static int rt305x_flash_instance __initdata;
void __init rt305x_register_flash(unsigned int id,
				  struct physmap_flash_data *pdata)
{
	struct platform_device *pdev;
	u32 t;
	int reg;

	switch (id) {
	case 0:
		pdev = &rt305x_flash0_device;
		reg = MEMC_REG_FLASH_CFG0;
		break;
	case 1:
		pdev = &rt305x_flash1_device;
		reg = MEMC_REG_FLASH_CFG1;
		break;
	default:
		return;
	}

	t = rt305x_memc_rr(reg);
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
		printk(KERN_ERR "RT305x: flash bank%u witdh is invalid\n", id);
		return;
	}

	pdev->dev.platform_data = pdata;
	pdev->id = rt305x_flash_instance;

	platform_device_register(pdev);
	rt305x_flash_instance++;
}
