/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>

#include <lantiq_soc.h>
#include <irq.h>

#include "../machtypes.h"
#include "../dev-leds-gpio.h"
#include "../dev-gpio-buttons.h"
#include "devices.h"
#include "dev-dwc_otg.h"

static struct mtd_partition wbmr_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x40000,
	},
	{
		.name	= "uboot-env",
		.offset	= 0x40000,
		.size	= 0x20000,
	},
	{
		.name	= "linux",
		.offset	= 0x60000,
		.size	= 0x1f20000,
	},
	{
		.name	= "calibration",
		.offset	= 0x1fe0000,
		.size	= 0x20000,
	},
};

static struct physmap_flash_data wbmr_flash_data = {
	.nr_parts	= ARRAY_SIZE(wbmr_partitions),
	.parts		= wbmr_partitions,
};

static struct gpio_led
wbmr_leds_gpio[] __initdata = {
	{ .name = "soc:blue:movie", .gpio = 20, .active_low = 1, },
	{ .name = "soc:red:internet", .gpio = 18, .active_low = 1, },
	{ .name = "soc:green:internet", .gpio = 17, .active_low = 1, },
	{ .name = "soc:green:adsl", .gpio = 16, .active_low = 1, },
	{ .name = "soc:green:wlan", .gpio = 15, .active_low = 1, },
	{ .name = "soc:red:security", .gpio = 14, .active_low = 1, },
	{ .name = "soc:green:power", .gpio = 1, .active_low = 1, },
	{ .name = "soc:red:power", .gpio = 5, .active_low = 1, },
	{ .name = "soc:green:usb", .gpio = 28, .active_low = 1, },
};

static struct gpio_keys_button
wbmr_gpio_keys[] __initdata = {
	{
		.desc = "aoss",
		.type = EV_KEY,
		.code = BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 0,
		.active_low = 1,
	},
	{
		.desc = "reset",
		.type = EV_KEY,
		.code = BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 37,
		.active_low = 1,
	},
};

static struct ltq_pci_data ltq_pci_data = {
	.clock      = PCI_CLOCK_INT,
	.gpio   = PCI_GNT1 | PCI_REQ1,
	.irq    = {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode = PHY_INTERFACE_MODE_RGMII,
};

static void __init
wbmr_init(void)
{
#define WMBR_BRN_MAC			0x1fd0024

	ltq_add_device_leds_gpio(-1, ARRAY_SIZE(wbmr_leds_gpio), wbmr_leds_gpio);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL, ARRAY_SIZE(wbmr_gpio_keys), wbmr_gpio_keys);
	ltq_register_nor(&wbmr_flash_data);
	ltq_register_pci(&ltq_pci_data);
	memcpy_fromio(&ltq_eth_data.mac.sa_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + WMBR_BRN_MAC), 6);
	ltq_register_etop(&ltq_eth_data);
	xway_register_dwc(36);
}

MIPS_MACHINE(LANTIQ_MACH_WBMR,
			"WBMR",
			"WBMR",
			wbmr_init);
