/*
 *  Realtek RLX based SoC built-in ethernet
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/sizes.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>

#include "common.h"
#include "dev-eth.h"

static struct re865x_platform_data re865x_data = {
	.mac_addr = {0x00, 0x23, 0x45, 0x67, 0x89, 0xab}
};

static struct resource realtek_eth_resource[] = {
	{
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "nic_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_NIC_BASE,
		.end	= REALTEK_NIC_BASE + REALTEK_NIC_SIZE - 1,
	},
	{
		.name	= "sw_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_SWCORE_BASE,
		.end	= REALTEK_SWCORE_BASE + REALTEK_SWCORE_SIZE - 1,
	},
	{
		.name	= "swtbl_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_SWTABLE_BASE,
		.end	= REALTEK_SWTABLE_BASE + REALTEK_SWTABLE_SIZE - 1,
	},
	{
		.name	= "swtbl_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_SWTABLE_BASE,
		.end	= REALTEK_SWTABLE_BASE + REALTEK_SWTABLE_SIZE - 1,
	}
};

static struct platform_device realtek_eth_device = {
	.name			= "re865x",
	.id			= -1,
	.resource		= realtek_eth_resource,
	.num_resources		= ARRAY_SIZE(realtek_eth_resource),
	.dev = {
		.platform_data	= &re865x_data,
	}
};

void __init realtek_register_eth(void)
{
	realtek_eth_resource[0].start = realtek_eth_resource[0].end = REALTEK_CPU_IRQ(6);
	platform_device_register(&realtek_eth_device);
}
