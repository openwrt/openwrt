/*
 *  Atheros AR934x SoC built-in WMAC device support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *
 *  Parts of this file are based on Atheros 2.6.31 BSP
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
#include <linux/ath9k_platform.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "dev-ar934x-wmac.h"

static struct ath9k_platform_data ar934x_wmac_data = {
	.led_pin = -1,
};

static u8 ar934x_wmac_mac[6];

static struct resource ar934x_wmac_resources[] = {
	{
		.start	= AR934X_WMAC_BASE,
		.end	= AR934X_WMAC_BASE + AR934X_WMAC_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= AR71XX_CPU_IRQ_IP2,
		.end	= AR71XX_CPU_IRQ_IP2,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device ar934x_wmac_device = {
	.name		= "ath9k",
	.id		= -1,
	.resource	= ar934x_wmac_resources,
	.num_resources	= ARRAY_SIZE(ar934x_wmac_resources),
	.dev = {
		.platform_data = &ar934x_wmac_data,
	},
};

void __init ar934x_add_device_wmac(u8 *cal_data, u8 *mac_addr)
{
	if (cal_data)
		memcpy(ar934x_wmac_data.eeprom_data, cal_data,
		       sizeof(ar934x_wmac_data.eeprom_data));

	if (mac_addr) {
		memcpy(ar934x_wmac_mac, mac_addr, sizeof(ar934x_wmac_mac));
		ar934x_wmac_data.macaddr = ar934x_wmac_mac;
	}

	platform_device_register(&ar934x_wmac_device);
}
