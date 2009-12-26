/*
 *  Linksys WRT160NL board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ar913x-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define WRT160NL_GPIO_LED_POWER		14
#define WRT160NL_GPIO_LED_WPS_AMBER	9
#define WRT160NL_GPIO_LED_WPS_BLUE	8
#define WRT160NL_GPIO_LED_WLAN		6

#define WRT160NL_GPIO_BTN_WPS		7
#define WRT160NL_GPIO_BTN_RESET		21

#define WRT160NL_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wrt160nl_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "kernel",
		.offset		= 0x040000,
		.size		= 0x0e0000,
	} , {
		.name		= "filesytem",
		.offset		= 0x120000,
		.size		= 0x6c0000,
	} , {
		.name		= "nvram",
		.offset		= 0x7e0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "ART",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x040000,
		.size		= 0x7a0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wrt160nl_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wrt160nl_partitions,
        .nr_parts       = ARRAY_SIZE(wrt160nl_partitions),
#endif
};

static struct gpio_led wrt160nl_leds_gpio[] __initdata = {
	{
		.name		= "wrt160nl:blue:power",
		.gpio		= WRT160NL_GPIO_LED_POWER,
		.active_low	= 1,
		.default_trigger = "default-on",
	}, {
		.name		= "wrt160nl:amber:wps",
		.gpio		= WRT160NL_GPIO_LED_WPS_AMBER,
		.active_low	= 1,
	}, {
		.name		= "wrt160nl:blue:wps",
		.gpio		= WRT160NL_GPIO_LED_WPS_BLUE,
		.active_low	= 1,
	}, {
		.name		= "wrt160nl:blue:wlan",
		.gpio		= WRT160NL_GPIO_LED_WLAN,
		.active_low	= 1,
	}
};

static struct gpio_button wrt160nl_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= WRT160NL_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= WRT160NL_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static void __init wrt160nl_setup(void)
{
	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = 0x01;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&wrt160nl_flash_data);

	ar71xx_add_device_usb();
	ar913x_add_device_wmac();

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wrt160nl_leds_gpio),
					wrt160nl_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WRT160NL_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(wrt160nl_gpio_buttons),
					wrt160nl_gpio_buttons);
}

MIPS_MACHINE(AR71XX_MACH_WRT160NL, "Linksys WRT160NL", wrt160nl_setup);
