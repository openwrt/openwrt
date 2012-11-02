/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011 Andrej Vlašić
 *  Copyright (C) 2011 Luka Perkov
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/io.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/string.h>

#include <irq.h>
#include <lantiq_soc.h>
#include <lantiq_platform.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>

#include "../machtypes.h"
#include "dev-wifi-athxk.h"
#include "devices.h"
#include "dev-dwc_otg.h"

#include "mach-gigasx76x.h"

static u8 ltq_ethaddr[6] = { 0 };

static int __init
setup_ethaddr(char *str)
{
	if (!mac_pton(str, ltq_ethaddr))
		memset(ltq_ethaddr, 0, 6);
	return 0;
}
__setup("ethaddr=", setup_ethaddr);


enum {
	UNKNOWN = 0,
	SX761,
	SX762,
	SX763,
};
static u8 board __initdata = SX763;

static int __init
setup_board(char *str)
{
	if (!strcmp(str, "sx761"))
		board = SX761;
	else if (!strcmp(str, "sx762"))
		board = SX762;
	else if (!strcmp(str, "sx763"))
		board = SX763;
	else
		board = UNKNOWN;
	return 0;
}
__setup("board=", setup_board);

static struct mtd_partition gigasx76x_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x10000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x10000,
		.size	= 0x10000,
	},
	{
		.name	= "linux",
		.offset	= 0x20000,
		.size	= 0x7e0000,
	},
};

static struct gpio_led
gigasx76x_gpio_leds[] __initdata = {
	{ .name = "soc:green:voip", .gpio = 216, },
	{ .name = "soc:green:adsl", .gpio = 217, },
	{ .name = "soc:green:usb", .gpio = 218, },
	{ .name = "soc:green:wifi", .gpio = 219, },
	{ .name = "soc:green:phone2", .gpio = 220, },
	{ .name = "soc:green:phone1", .gpio = 221, },
	{ .name = "soc:green:line", .gpio = 222, },
	{ .name = "soc:green:online", .gpio = 223, },
};

static struct gpio_keys_button
gigasx76x_gpio_keys[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 22,
		.active_low	= 1,
	},
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 14,
		.active_low	= 0,
	},
};

static struct physmap_flash_data gigasx76x_flash_data = {
	.nr_parts	= ARRAY_SIZE(gigasx76x_partitions),
	.parts		= gigasx76x_partitions,
};

static struct ltq_pci_data ltq_pci_data = {
	.clock	= PCI_CLOCK_INT,
	.gpio	= PCI_GNT1 | PCI_REQ1,
	.irq	= { [14] = INT_NUM_IM0_IRL0 + 22, },
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_MII,
};

static void __init
gigasx76x_init(void)
{
#define GIGASX76X_USB		29

	ltq_register_gpio_stp();
	ltq_register_nor(&gigasx76x_flash_data);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(gigasx76x_gpio_leds), gigasx76x_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL, ARRAY_SIZE(gigasx76x_gpio_keys), gigasx76x_gpio_keys);
	xway_register_dwc(GIGASX76X_USB);

	if (!is_valid_ether_addr(ltq_ethaddr))
		random_ether_addr(ltq_ethaddr);

	memcpy(&ltq_eth_data.mac.sa_data, ltq_ethaddr, 6);
	ltq_register_etop(&ltq_eth_data);
	if (board == SX762) 
		ltq_register_ath5k(sx762_eeprom_data, ltq_ethaddr);
	else
		ltq_register_ath5k(sx763_eeprom_data, ltq_ethaddr);
}

MIPS_MACHINE(LANTIQ_MACH_GIGASX76X,
			"GIGASX76X",
			"GIGASX76X - Gigaset SX761,SX762,SX763",
			gigasx76x_init);
