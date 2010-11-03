/*
 *  Atheros AR913x SoC built-in WMAC device support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
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

#include "dev-ar913x-wmac.h"

static struct ath9k_platform_data ar913x_wmac_data = {
	.led_pin = -1,
};
static char ar913x_wmac_mac[6];

static struct resource ar913x_wmac_resources[] = {
	{
		.start	= AR91XX_WMAC_BASE,
		.end	= AR91XX_WMAC_BASE + AR91XX_WMAC_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= AR71XX_CPU_IRQ_IP2,
		.end	= AR71XX_CPU_IRQ_IP2,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device ar913x_wmac_device = {
	.name		= "ath9k",
	.id		= -1,
	.resource	= ar913x_wmac_resources,
	.num_resources	= ARRAY_SIZE(ar913x_wmac_resources),
	.dev = {
		.platform_data = &ar913x_wmac_data,
	},
};

void __init ar913x_add_device_wmac(u8 *cal_data, u8 *mac_addr)
{
	if (cal_data)
		memcpy(ar913x_wmac_data.eeprom_data, cal_data,
		       sizeof(ar913x_wmac_data.eeprom_data));

	if (mac_addr) {
		memcpy(ar913x_wmac_mac, mac_addr, sizeof(ar913x_wmac_mac));
		ar913x_wmac_data.macaddr = ar913x_wmac_mac;
	}

	ar71xx_device_stop(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	platform_device_register(&ar913x_wmac_device);
}
