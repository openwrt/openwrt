/*
 *  Atheros AP121 board support
 *
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
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
#include "dev-ar9xxx-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"

#define AP121_GPIO_LED_WLAN		0
#define AP121_GPIO_LED_USB		1

#define AP121_GPIO_BTN_JUMPSTART	11
#define AP121_GPIO_BTN_RESET		12

#define AP121_KEYS_POLL_INTERVAL	20	/* msecs */
#define AP121_KEYS_DEBOUNCE_INTERVAL	(3 * AP121_KEYS_POLL_INTERVAL)

#define AP121_MAC0_OFFSET	0x0000
#define AP121_MAC1_OFFSET	0x0006
#define AP121_CALDATA_OFFSET	0x1000
#define AP121_WMAC_MAC_OFFSET	0x1002

#define AP121_MINI_GPIO_LED_WLAN	0
#define AP121_MINI_GPIO_BTN_JUMPSTART	12
#define AP121_MINI_GPIO_BTN_RESET	11

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition ap121_parts[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "rootfs",
		.offset		= 0x010000,
		.size		= 0x130000,
	},
	{
		.name		= "uImage",
		.offset		= 0x140000,
		.size		= 0x0a0000,
	},
	{
		.name		= "NVRAM",
		.offset		= 0x1e0000,
		.size		= 0x010000,
	},
	{
		.name		= "ART",
		.offset		= 0x1f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	},
};
#define ap121_nr_parts		ARRAY_SIZE(ap121_parts)

static struct mtd_partition ap121_mini_parts[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "u-boot-env",
		.offset		= 0x040000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "rootfs",
		.offset		= 0x050000,
		.size		= 0x2b0000,
	},
	{
		.name		= "uImage",
		.offset		= 0x300000,
		.size		= 0x0e0000,
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

#define ap121_mini_nr_parts	ARRAY_SIZE(ap121_parts)

#else
#define ap121_parts		NULL
#define ap121_nr_parts		0
#define ap121_mini_parts	NULL
#define ap121_mini_nr_parts	0
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data ap121_flash_data = {
	.parts		= ap121_parts,
	.nr_parts	= ap121_nr_parts,
};

static struct gpio_led ap121_leds_gpio[] __initdata = {
	{
		.name		= "ap121:green:usb",
		.gpio		= AP121_GPIO_LED_USB,
		.active_low	= 0,
	},
	{
		.name		= "ap121:green:wlan",
		.gpio		= AP121_GPIO_LED_WLAN,
		.active_low	= 0,
	},
};

static struct gpio_keys_button ap121_gpio_keys[] __initdata = {
	{
		.desc		= "jumpstart button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AP121_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP121_GPIO_BTN_JUMPSTART,
		.active_low	= 1,
	},
	{
		.desc		= "reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = AP121_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP121_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct gpio_led ap121_mini_leds_gpio[] __initdata = {
	{
		.name		= "ap121:green:wlan",
		.gpio		= AP121_MINI_GPIO_LED_WLAN,
		.active_low	= 0,
	},
};

static struct gpio_keys_button ap121_mini_gpio_keys[] __initdata = {
	{
		.desc		= "jumpstart button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AP121_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP121_MINI_GPIO_BTN_JUMPSTART,
		.active_low	= 1,
	},
	{
		.desc		= "reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = AP121_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP121_MINI_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init ap121_common_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_add_device_m25p80(&ap121_flash_data);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, art + AP121_MAC0_OFFSET, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, art + AP121_MAC1_OFFSET, 0);

	/* WAN port */
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = BIT(4);

	/* LAN ports */
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;
	ar71xx_eth1_data.has_ar7240_switch = 1;

	ar71xx_add_device_mdio(0, 0x0);
	ar71xx_add_device_eth(1);
	ar71xx_add_device_eth(0);

	ar9xxx_add_device_wmac(art + AP121_CALDATA_OFFSET,
			       art + AP121_WMAC_MAC_OFFSET);
}

static void __init ap121_setup(void)
{
	ap121_flash_data.parts = ap121_parts;
	ap121_flash_data.nr_parts = ap121_nr_parts;

	ap121_common_setup();

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ap121_leds_gpio),
					ap121_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, AP121_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ap121_gpio_keys),
					 ap121_gpio_keys);

	ar71xx_add_device_usb();
}

static void __init ap121_mini_setup(void)
{
	ap121_flash_data.parts = ap121_mini_parts;
	ap121_flash_data.nr_parts = ap121_mini_nr_parts;

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ap121_mini_leds_gpio),
					ap121_mini_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, AP121_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ap121_mini_gpio_keys),
					 ap121_mini_gpio_keys);

	ap121_common_setup();
}

MIPS_MACHINE(AR71XX_MACH_AP121, "AP121", "Atheros AP121",
	     ap121_setup);

MIPS_MACHINE(AR71XX_MACH_AP121_MINI, "AP121-MINI", "Atheros AP121-MINI",
	     ap121_mini_setup);
