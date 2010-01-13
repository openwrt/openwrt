/*
 *  Linksys WRT400N board support
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2009 Imre Kaloz <kaloz@openwrt.org>
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
#include "dev-ap94-pci.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define WRT400N_GPIO_LED_ORANGE	5
#define WRT400N_GPIO_LED_GREEN	4
#define WRT400N_GPIO_LED_POWER	1
#define WRT400N_GPIO_LED_WLAN	0

#define WRT400N_GPIO_BTN_RESET	8
#define WRT400N_GPIO_BTN_WLSEC	3

#define WRT400N_BUTTONS_POLL_INTERVAL	20

#define WRT400N_MAC_ADDR_OFFSET		0x120c
#define WRT400N_CALDATA0_OFFSET		0x1000
#define WRT400N_CALDATA1_OFFSET		0x5000

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wrt400n_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x030000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "env",
		.offset		= 0x030000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "linux",
		.offset		= 0x040000,
		.size		= 0x140000,
	} , {
		.name		= "rootfs",
		.offset		= 0x180000,
		.size		= 0x630000,
	} , {
		.name		= "nvram",
		.offset		= 0x7b0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "factory",
		.offset		= 0x7c0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "language",
		.offset		= 0x7d0000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "caldata",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x040000,
		.size		= 0x770000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wrt400n_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wrt400n_partitions,
        .nr_parts       = ARRAY_SIZE(wrt400n_partitions),
#endif
};

static struct gpio_led wrt400n_leds_gpio[] __initdata = {
	{
		.name		= "wrt400n:green:status",
		.gpio		= WRT400N_GPIO_LED_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wrt400n:amber:aoss",
		.gpio		= WRT400N_GPIO_LED_ORANGE,
		.active_low	= 1,
	}, {
		.name		= "wrt400n:green:wlan",
		.gpio		= WRT400N_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "wrt400n:green:power",
		.gpio		= WRT400N_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static struct gpio_button wrt400n_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= WRT400N_GPIO_BTN_RESET,
		.active_low	= 1,
	} , {
		.desc		= "wlsec",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 3,
		.gpio		= WRT400N_GPIO_BTN_WLSEC,
		.active_low	= 1,
	}
};

static void __init wrt400n_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 mac[6];
	int i;

	memcpy(mac, art + WRT400N_MAC_ADDR_OFFSET, 6);
	for (i = 5; i >= 3; i--)
		if (++mac[i] != 0x00) break;

	ar71xx_set_mac_base(mac);

	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = 0xf;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.has_ar8216 = 1;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&wrt400n_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wrt400n_leds_gpio),
					wrt400n_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WRT400N_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(wrt400n_gpio_buttons),
					wrt400n_gpio_buttons);

	ap94_pci_init(art + WRT400N_CALDATA0_OFFSET, NULL,
		      art + WRT400N_CALDATA1_OFFSET, NULL);
}

MIPS_MACHINE(AR71XX_MACH_WRT400N, "WRT400N", "Linksys WRT400N", wrt400n_setup);
