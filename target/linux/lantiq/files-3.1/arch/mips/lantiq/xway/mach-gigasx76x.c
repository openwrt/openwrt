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
#include <linux/ath5k_platform.h>
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/io.h>
#include <linux/string.h>

#include <irq.h>
#include <lantiq_soc.h>
#include <lantiq_platform.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>

#include "../machtypes.h"
#include "devices.h"
#include "dev-dwc_otg.h"

#define UBOOT_ENV_OFFSET	0x010000
#define UBOOT_ENV_SIZE		0x010000

static struct mtd_partition gigasx76x_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x000000,
		.size	= 0x010000,
	},
	{
		.name	= "uboot_env",
		.offset	= UBOOT_ENV_OFFSET,
		.size	= UBOOT_ENV_SIZE,
	},
	{
		.name	= "linux",
		.offset	= 0x020000,
		.size	= 0x7d0000,
	},
	{
		.name	= "board_config",
		.offset	= 0x7f0000,
		.size	= 0x010000,
	},
};

static struct gpio_led
gigasx76x_gpio_leds[] __initdata = {
	{ .name = "soc:green:usb", .gpio = 202, },
	{ .name = "soc:green:wlan", .gpio = 203, },
	{ .name = "soc:green:phone2", .gpio = 204, },
	{ .name = "soc:green:phone1", .gpio = 205, },
	{ .name = "soc:green:line", .gpio = 206, },
	{ .name = "soc:green:online", .gpio = 207, },
	{ .name = "soc:green:voip", .gpio = 208, },
};

static struct gpio_keys_button
gigasx76x_gpio_keys[] __initdata = {
{
	.desc	= "restart",
	.type	= EV_KEY,
	.code	= KEY_RESTART,
	.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
	.gpio	= 14,
	.active_low	= 1,
},
{
	.desc	= "wps",
	.type	= EV_KEY,
	.code	= KEY_WPS_BUTTON,
	.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
	.gpio	= 22,
	.active_low	= 1,
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

static char __init *get_uboot_env_var(char *haystack, int haystack_len, char *needle, int needle_len) {
	int i;
	for (i = 0; i <= haystack_len - needle_len; i++) {
		if (memcmp(haystack + i, needle, needle_len) == 0) {
			return haystack + i + needle_len;
		}
	}
	return NULL;
}

/*
 * gigasx76x_parse_hex_* are not uniq. in arm/orion there are also duplicates:
 * dns323_parse_hex_*
 * TODO: one day write a patch for this :)
 */
static int __init gigasx76x_parse_hex_nibble(char n) {
	if (n >= '0' && n <= '9')
		return n - '0';

	if (n >= 'A' && n <= 'F')
		return n - 'A' + 10;

	if (n >= 'a' && n <= 'f')
		return n - 'a' + 10;

	return -1;
}

static int __init gigasx76x_parse_hex_byte(const char *b) {
	int hi;
	int lo;

	hi = gigasx76x_parse_hex_nibble(b[0]);
	lo = gigasx76x_parse_hex_nibble(b[1]);

	if (hi < 0 || lo < 0)
		return -1;

	return (hi << 4) | lo;
}

static int __init gigasx76x_register_ethernet(void) {
	u_int8_t addr[6];
	int i;
	char *uboot_env_page;
	char *mac;

	uboot_env_page = ioremap(LTQ_FLASH_START + UBOOT_ENV_OFFSET, UBOOT_ENV_SIZE);
	if (!uboot_env_page)
		return -ENOMEM;

	mac = get_uboot_env_var(uboot_env_page, UBOOT_ENV_SIZE, "\0ethaddr=", 9);

	if (!mac) {
	goto error_fail;
	}

	/* Sanity check the string we're looking at */
	for (i = 0; i < 5; i++) {
	if (*(mac + (i * 3) + 2) != ':') {
		goto error_fail;
		}
	}

	for (i = 0; i < 6; i++) {
		int byte;
		byte = gigasx76x_parse_hex_byte(mac + (i * 3));
		if (byte < 0) {
			goto error_fail;
		}
		addr[i] = byte;
	}

	iounmap(uboot_env_page);
	printk("GIGASX76X: Found ethernet MAC address: ");
	for (i = 0; i < 6; i++)
		printk("%.2x%s", addr[i], (i < 5) ? ":" : ".\n");

	memcpy(&ltq_eth_data.mac.sa_data, addr, 6);
	ltq_register_etop(&ltq_eth_data);

	return 0;

	error_fail:
		iounmap(uboot_env_page);
		return -EINVAL;
}

static void __init gigasx76x_init(void) {
#define GIGASX76X_USB		29
#define GIGASX76X_MADWIFI_ADDR	0xb07f0000

	ltq_register_gpio_stp();
	ltq_register_nor(&gigasx76x_flash_data);
	ltq_register_pci(&ltq_pci_data);
	gigasx76x_register_ethernet();
	xway_register_dwc(GIGASX76X_USB);
	ltq_register_tapi();
	ltq_register_madwifi_eep(GIGASX76X_MADWIFI_ADDR);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(gigasx76x_gpio_leds), gigasx76x_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL, ARRAY_SIZE(gigasx76x_gpio_keys), gigasx76x_gpio_keys);
}

MIPS_MACHINE(LANTIQ_MACH_GIGASX76X, "GIGASX76X", "GIGASX76X - Gigaset SX761,SX762,SX763", gigasx76x_init);
