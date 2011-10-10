/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011 Andrej Vlašić
 *  Copyright (C) 2011 Luka Perkov
 *
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
#include <linux/ath5k_platform.h>
#include <linux/pci.h>
#include <linux/phy.h>

#include <irq.h>

#include <lantiq_soc.h>
#include <lantiq_platform.h>

#include "../machtypes.h"
#include "../dev-leds-gpio.h"
#include "devices.h"
#include "dev-dwc_otg.h"

static struct mtd_partition gigasx76x_partitions[] =
{
	{
		.name	= "secondary_env",
		.offset	= 0xe000,
		.size	= 0x2000,
	},
	{
		.name	= "secondary_boot",
		.offset	= 0x10000,
		.size	= 0x10000,
	},
	{
		.name	= "uboot",
		.offset	= 0x20000,
		.size	= 0x30000,
	},
	{
		.name	= "linux",
		.offset	= 0x50000,
		.size	= 0x7a0000,
	},
	{
		.name	= "board_config",
		.offset	= 0x7f0000,
		.size	= 0x10000,
	},
};

static struct gpio_led
gigasx76x_leds_gpio[] __initdata = {
	{ .name = "soc:green:usb", .gpio = 202, },
	{ .name = "soc:green:wlan", .gpio = 203, },
	{ .name = "soc:green:phone2", .gpio = 204, },
	{ .name = "soc:green:phone1", .gpio = 205, },
	{ .name = "soc:green:line", .gpio = 206, },
	{ .name = "soc:green:online", .gpio = 207, },
};


static struct physmap_flash_data gigasx76x_flash_data = {
	.nr_parts	= ARRAY_SIZE(gigasx76x_partitions),
	.parts		= gigasx76x_partitions,
};

static struct ltq_pci_data ltq_pci_data = {
	.clock      = PCI_CLOCK_INT,
	.gpio   = PCI_GNT1 | PCI_REQ1,
	.irq    = {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
  .mii_mode = PHY_INTERFACE_MODE_MII,
};

static void __init
gigasx76x_init(void)
{
#define GIGASX76X_USB			29

	ltq_register_gpio_stp();
	ltq_register_nor(&gigasx76x_flash_data);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_etop(&ltq_eth_data);
	xway_register_dwc(GIGASX76X_USB);
	ltq_register_tapi();
	ltq_register_madwifi_eep();
	ltq_add_device_leds_gpio(-1, ARRAY_SIZE(gigasx76x_leds_gpio), gigasx76x_leds_gpio);
}

MIPS_MACHINE(LANTIQ_MACH_GIGASX76X,
			"GIGASX76X",
			"GIGASX76X - Gigaset SX761,SX762,SX763",
			gigasx76x_init);
