/*
 *  AzureWave AW-NR580 board support
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

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-pb42-pci.h"
#include "dev-leds-gpio.h"

#define AW_NR580_GPIO_LED_READY_RED	0
#define AW_NR580_GPIO_LED_WLAN		1
#define AW_NR580_GPIO_LED_READY_GREEN	2
#define AW_NR580_GPIO_LED_WPS_GREEN	4
#define AW_NR580_GPIO_LED_WPS_AMBER	5

#define AW_NR580_GPIO_BTN_WPS		3
#define AW_NR580_GPIO_BTN_RESET		11

#define AW_NR580_BUTTONS_POLL_INTERVAL	20

static struct gpio_led aw_nr580_leds_gpio[] __initdata = {
	{
		.name		= "aw-nr580:red:ready",
		.gpio		= AW_NR580_GPIO_LED_READY_RED,
		.active_low	= 0,
	}, {
		.name		= "aw-nr580:green:ready",
		.gpio		= AW_NR580_GPIO_LED_READY_GREEN,
		.active_low	= 0,
	}, {
		.name		= "aw-nr580:green:wps",
		.gpio		= AW_NR580_GPIO_LED_WPS_GREEN,
		.active_low	= 0,
	}, {
		.name		= "aw-nr580:amber:wps",
		.gpio		= AW_NR580_GPIO_LED_WPS_AMBER,
		.active_low	= 0,
	}, {
		.name		= "aw-nr580:green:wlan",
		.gpio		= AW_NR580_GPIO_LED_WLAN,
		.active_low	= 0,
	}
};

static struct gpio_button aw_nr580_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= AW_NR580_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 3,
		.gpio		= AW_NR580_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static void __init aw_nr580_setup(void)
{
	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);

	pb42_pci_init();

	ar71xx_add_device_m25p80(NULL);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(aw_nr580_leds_gpio),
					aw_nr580_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, AW_NR580_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(aw_nr580_gpio_buttons),
					aw_nr580_gpio_buttons);
}

MIPS_MACHINE(AR71XX_MACH_AW_NR580, "AW-NR580", "AzureWave AW-NR580",
	     aw_nr580_setup);
