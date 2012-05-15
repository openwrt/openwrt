/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/phy.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/flash.h>

#include <lantiq_soc.h>
#include <irq.h>

#include "../machtypes.h"
#include "devices.h"
#include "dev-ifxhcd.h"
#include "dev-gpio-leds.h"
#include "dev-gpio-buttons.h"

static struct mtd_partition fritz7320_partitions[] = {
	{
		.name	= "urlader",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "linux",
		.offset	= 0x20000,
		.size	= 0xf60000,
	},
	{
		.name	= "tffs (1)",
		.offset	= 0xf80000,
		.size	= 0x40000,
	},
	{
		.name	= "tffs (2)",
		.offset	= 0xfc0000,
		.size	= 0x40000,
	},
};

static struct physmap_flash_data fritz7320_flash_data = {
	.nr_parts	= ARRAY_SIZE(fritz7320_partitions),
	.parts		= fritz7320_partitions,
};

static struct gpio_led
fritz7320_gpio_leds[] __initdata = {
	{ .name = "soc:green:power", .gpio = 44, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:internet", .gpio = 47, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:dect", .gpio = 38, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wlan", .gpio = 37, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:dual1", .gpio = 35, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:dual2", .gpio = 45, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_keys_button
fritz7320_gpio_keys[] __initdata = {
	{
		.desc = "wifi",
		.type = EV_KEY,
		.code = BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 1,
		.active_low = 1,
	},
	{
		.desc = "dect",
		.type = EV_KEY,
		.code = BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 2,
		.active_low = 1,
	},
};

static struct ltq_pci_data ltq_pci_data = {
	.clock	= PCI_CLOCK_INT,
	.gpio	= PCI_GNT1 | PCI_REQ1,
	.irq	= {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_RMII,
};

static int usb_pins[2] = { 50, 51 };

static void __init
fritz7320_init(void)
{
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
		ARRAY_SIZE(fritz7320_gpio_keys), fritz7320_gpio_keys);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(fritz7320_gpio_leds), fritz7320_gpio_leds);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_etop(&ltq_eth_data);
	ltq_register_nor(&fritz7320_flash_data);
	xway_register_hcd(usb_pins);
}

MIPS_MACHINE(LANTIQ_MACH_FRITZ7320,
			"FRITZ7320",
			"FRITZ!BOX 7320",
			fritz7320_init);
