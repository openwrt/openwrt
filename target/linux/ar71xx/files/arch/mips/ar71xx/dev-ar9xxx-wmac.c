/*
 *  Atheros AR9XXX SoCs built-in WMAC device support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros 2.6.15/2.6.31 BSP
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

#include "dev-ar9xxx-wmac.h"

static struct ath9k_platform_data ar9xxx_wmac_data = {
	.led_pin = -1,
};
static char ar9xxx_wmac_mac[6];

static struct resource ar9xxx_wmac_resources[] = {
	{
		/* .start and .end fields are filled dynamically */
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= AR71XX_CPU_IRQ_IP2,
		.end	= AR71XX_CPU_IRQ_IP2,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device ar9xxx_wmac_device = {
	.name		= "ath9k",
	.id		= -1,
	.resource	= ar9xxx_wmac_resources,
	.num_resources	= ARRAY_SIZE(ar9xxx_wmac_resources),
	.dev = {
		.platform_data = &ar9xxx_wmac_data,
	},
};

static void ar913x_wmac_init(void)
{
	ar71xx_device_stop(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	ar9xxx_wmac_resources[0].start = AR91XX_WMAC_BASE;
	ar9xxx_wmac_resources[0].end = AR91XX_WMAC_BASE + AR91XX_WMAC_SIZE - 1;
}

static void ar934x_wmac_init(void)
{
	ar9xxx_wmac_resources[0].start = AR934X_WMAC_BASE;
	ar9xxx_wmac_resources[0].end = AR934X_WMAC_BASE + AR934X_WMAC_SIZE - 1;
}

void __init ar9xxx_add_device_wmac(u8 *cal_data, u8 *mac_addr)
{
	switch (ar71xx_soc) {
	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar913x_wmac_init();
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ar934x_wmac_init();
		break;

	default:
		BUG();
	}

	if (cal_data)
		memcpy(ar9xxx_wmac_data.eeprom_data, cal_data,
		       sizeof(ar9xxx_wmac_data.eeprom_data));

	if (mac_addr) {
		memcpy(ar9xxx_wmac_mac, mac_addr, sizeof(ar9xxx_wmac_mac));
		ar9xxx_wmac_data.macaddr = ar9xxx_wmac_mac;
	}

	platform_device_register(&ar9xxx_wmac_device);
}
