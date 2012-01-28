/*
 *  Atheros AP113 board support
 *
 *  Copyright (C) 2011 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/flash.h>

#include "machtype.h"
#include "devices.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-pb9x-pci.h"
#include "dev-usb.h"

#define AP113_GPIO_LED_USB		0
#define AP113_GPIO_LED_STATUS		1
#define AP113_GPIO_LED_ST		11

#define AP113_GPIO_BTN_JUMPSTART	12

#define AP113_KEYS_POLL_INTERVAL	20	/* msecs */
#define AP113_KEYS_DEBOUNCE_INTERVAL	(3 * AP113_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition ap113_parts[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "rootfs",
		.offset		= 0x010000,
		.size		= 0x300000,
	},
	{
		.name		= "uImage",
		.offset		= 0x300000,
		.size		= 0x3e0000,
	},
	{
		.name		= "NVRAM",
		.offset		= 0x3e0000,
		.size		= 0x010000,
	},
	{
		.name		= "ART",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	},
};
#define ap113_nr_parts		ARRAY_SIZE(ap113_parts)
#else
#define ap113_parts		NULL
#define ap113_nr_parts		0
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data ap113_flash_data = {
	.parts		= ap113_parts,
	.nr_parts	= ap113_nr_parts,
};

static struct gpio_led ap113_leds_gpio[] __initdata = {
	{
		.name		= "ap113:green:usb",
		.gpio		= AP113_GPIO_LED_USB,
		.active_low	= 1,
	},
	{
		.name		= "ap113:green:status",
		.gpio		= AP113_GPIO_LED_STATUS,
		.active_low	= 1,
	},
	{
		.name		= "ap113:green:st",
		.gpio		= AP113_GPIO_LED_ST,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ap113_gpio_keys[] __initdata = {
	{
		.desc		= "jumpstart button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AP113_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP113_GPIO_BTN_JUMPSTART,
		.active_low	= 1,
	},
};

static void __init ap113_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_add_device_m25p80(&ap113_flash_data);

	ar71xx_add_device_mdio(0, ~BIT(0));
	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = BIT(0);

	ar71xx_add_device_eth(0);

	ar71xx_register_gpio_keys_polled(-1, AP113_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ap113_gpio_keys),
					 ap113_gpio_keys);
	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ap113_leds_gpio),
					ap113_leds_gpio);

	ar71xx_add_device_usb();

	pb9x_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_AP113, "AP113", "Atheros AP113",
	     ap113_setup);
