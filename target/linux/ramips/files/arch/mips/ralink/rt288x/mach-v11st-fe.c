/*
 *  Ralink V11ST-FE board support
 *
 *  Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/ethtool.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define V11ST_FE_GPIO_STATUS_LED	12
#define V11ST_FE_GPIO_BUTTON_WPS	0

#define V11ST_FE_KEYS_POLL_INTERVAL	20
#define V11ST_FE_KEYS_DEBOUNCE_INTERVAL	(3 * V11ST_FE_KEYS_POLL_INTERVAL)

static struct gpio_led v11st_fe_leds_gpio[] __initdata = {
	{
		.name		= "v11st-fe:green:status",
		.gpio		= V11ST_FE_GPIO_STATUS_LED,
		.active_low	= 1,
	}
};

static struct gpio_keys_button v11st_fe_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = V11ST_FE_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= V11ST_FE_GPIO_BUTTON_WPS,
	}
};

static struct mtd_partition v11st_fe_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "u-boot-env",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "factory",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size   = 0x140000,
	}, {
		.name	= "rootfs",
		.offset = 0x190000,
		.size   = 0x210000,
	}
};

static void __init rt_v11st_fe_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0);

	rt288x_flash0_data.nr_parts = ARRAY_SIZE(v11st_fe_partitions);
	rt288x_flash0_data.parts = v11st_fe_partitions;
	rt288x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(v11st_fe_leds_gpio),
				  v11st_fe_leds_gpio);

	ramips_register_gpio_buttons(-1, V11ST_FE_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(v11st_fe_gpio_buttons),
				     v11st_fe_gpio_buttons);

	rt288x_register_wifi();

	/* Board is connected to an IC+ IP175C Fast Ethernet switch */
	rt288x_eth_data.speed = SPEED_100;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_eth_data.phy_mask = BIT(0);
	rt288x_register_ethernet();

	rt288x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_V11ST_FE, "V11ST-FE", "Ralink V11ST-FE", rt_v11st_fe_init);
