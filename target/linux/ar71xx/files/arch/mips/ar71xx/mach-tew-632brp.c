/*
 *  TrendNET TEW-632BRP board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ar913x-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "nvram.h"

#define TEW_632BRP_GPIO_LED_STATUS	1
#define TEW_632BRP_GPIO_LED_WPS		3
#define TEW_632BRP_GPIO_LED_WLAN	6
#define TEW_632BRP_GPIO_BTN_WPS		12
#define TEW_632BRP_GPIO_BTN_RESET	21

#define TEW_632BRP_BUTTONS_POLL_INTERVAL	20

#define TEW_632BRP_CONFIG_ADDR	0x1f020000
#define TEW_632BRP_CONFIG_SIZE	0x10000

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tew_632brp_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config",
		.offset		= 0x020000,
		.size		= 0x010000,
	} , {
		.name		= "kernel",
		.offset		= 0x030000,
		.size		= 0x0d0000,
	} , {
		.name		= "rootfs",
		.offset		= 0x100000,
		.size		= 0x2f0000,
	} , {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x030000,
		.size		= 0x3c0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tew_632brp_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = tew_632brp_partitions,
        .nr_parts       = ARRAY_SIZE(tew_632brp_partitions),
#endif
};

static struct gpio_led tew_632brp_leds_gpio[] __initdata = {
	{
		.name		= "tew-632brp:green:status",
		.gpio		= TEW_632BRP_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "tew-632brp:blue:wps",
		.gpio		= TEW_632BRP_GPIO_LED_WPS,
		.active_low	= 1,
	}, {
		.name		= "tew-632brp:green:wlan",
		.gpio		= TEW_632BRP_GPIO_LED_WLAN,
		.active_low	= 1,
	}
};

static struct gpio_button tew_632brp_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= TEW_632BRP_GPIO_BTN_RESET,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 3,
		.gpio		= TEW_632BRP_GPIO_BTN_WPS,
	}
};

#define TEW_632BRP_LAN_PHYMASK	BIT(0)
#define TEW_632BRP_WAN_PHYMASK	BIT(4)
#define TEW_632BRP_MDIO_MASK	(~(TEW_632BRP_LAN_PHYMASK | \
				   TEW_632BRP_WAN_PHYMASK))

static void __init tew_632brp_setup(void)
{
	const char *config = (char *) KSEG1ADDR(TEW_632BRP_CONFIG_ADDR);
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 mac[6];
	u8 *wlan_mac = NULL;

	if (nvram_parse_mac_addr(config, TEW_632BRP_CONFIG_SIZE,
			         "lan_mac=", mac) == 0) {
		ar71xx_set_mac_base(mac);
		wlan_mac = mac;
	}

	ar71xx_add_device_mdio(TEW_632BRP_MDIO_MASK);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = TEW_632BRP_LAN_PHYMASK;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = TEW_632BRP_WAN_PHYMASK;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&tew_632brp_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tew_632brp_leds_gpio),
					tew_632brp_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, TEW_632BRP_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(tew_632brp_gpio_buttons),
					tew_632brp_gpio_buttons);

	ar913x_add_device_wmac(eeprom, wlan_mac);
}

MIPS_MACHINE(AR71XX_MACH_TEW_632BRP, "TEW-632BRP", "TRENDnet TEW-632BRP",
	     tew_632brp_setup);
