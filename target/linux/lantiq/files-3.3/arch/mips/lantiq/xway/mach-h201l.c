/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2012 Luka Perkov <openwrt@lukaperkov.net>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/string.h>

#include <lantiq_soc.h>
#include <lantiq_platform.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>

#include "../machtypes.h"
#include "devices.h"
#include "dev-dwc_otg.h"

static u8 ltq_ethaddr[6] = { 0 };

static int __init
setup_ethaddr(char *str)
{
	if (!mac_pton(str, ltq_ethaddr))
		memset(ltq_ethaddr, 0, 6);
	return 0;
}
__setup("ethaddr=", setup_ethaddr);

static struct mtd_partition h201l_partitions[] __initdata =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x20000,
		.size	= 0x10000,
	},
	{
		.name	= "linux",
		.offset = 0x30000,
		.size   = 0x7d0000,
	},
};

static struct physmap_flash_data h201l_flash_data __initdata = {
	.nr_parts	= ARRAY_SIZE(h201l_partitions),
	.parts		= h201l_partitions,
};

static struct gpio_led
h201l_leds_gpio[] __initdata = {
};

static struct gpio_keys_button
h201l_gpio_keys[] __initdata = {
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_RMII,
};

static void __init
h201l_init(void)
{
	ltq_register_gpio_stp();
	ltq_register_nor(&h201l_flash_data);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(h201l_leds_gpio), h201l_leds_gpio);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL, ARRAY_SIZE(h201l_gpio_keys), h201l_gpio_keys);

	if (!is_valid_ether_addr(ltq_ethaddr))
		random_ether_addr(ltq_ethaddr);

	memcpy(&ltq_eth_data.mac.sa_data, ltq_ethaddr, 6);
	ltq_register_etop(&ltq_eth_data);

	xway_register_dwc(-1);
}

MIPS_MACHINE(LANTIQ_MACH_H201L,
			"H201L",
			"ZTE ZXV10 H201L",
			h201l_init);
