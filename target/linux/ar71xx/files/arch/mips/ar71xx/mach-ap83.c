/*
 *  Atheros AP83 board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"

#define AP83_GPIO_LED_WLAN	6
#define AP83_GPIO_LED_POWER	14
#define AP83_GPIO_LED_JUMPSTART	15
#define AP83_GPIO_BTN_JUMPSTART	12
#define AP83_GPIO_BTN_RESET	21

#define AP83_GPIO_VSC7385_CS	1
#define AP83_GPIO_VSC7385_MISO	3
#define AP83_GPIO_VSC7385_MOSI	16
#define AP83_GPIO_VSC7385_SCK	17

static struct gpio_led ap83_leds_gpio[] __initdata = {
	{
		.name		= "ap83:green:jumpstart",
		.gpio		= AP83_GPIO_LED_JUMPSTART,
		.active_low	= 0,
	}, {
		.name		= "ap83:green:power",
		.gpio		= AP83_GPIO_LED_POWER,
		.active_low	= 0,
	}, {
		.name		= "ap83:green:wlan",
		.gpio		= AP83_GPIO_LED_WLAN,
		.active_low	= 0,
	},
};

static struct gpio_button ap83_gpio_buttons[] __initdata = {
	{
		.desc		= "soft_reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= AP83_GPIO_BTN_RESET,
		.active_low	= 1,
	} , {
		.desc		= "jumpstart",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= AP83_GPIO_BTN_JUMPSTART,
		.active_low	= 1,
	}
};

static struct spi_gpio_platform_data ap83_spi_data = {
	.miso	= AP83_GPIO_VSC7385_MISO,
	.mosi	= AP83_GPIO_VSC7385_MOSI,
	.sck	= AP83_GPIO_VSC7385_SCK,
	.num_chipselect = 1,
};

static struct platform_device ap83_spi_device = {
	.name		= "spi_gpio",
	.id		= 0,
	.dev		= {
		.platform_data = &ap83_spi_data,
	}
};

static void __init ap83_setup(void)
{
	ar71xx_add_device_mdio(0xfffffffe);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0x1;

	ar71xx_add_device_eth(0);

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x0;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(1);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ap83_leds_gpio),
					ap83_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, 20, ARRAY_SIZE(ap83_gpio_buttons),
					ap83_gpio_buttons);

	ar71xx_add_device_usb();

	ar91xx_add_device_wmac();

	platform_device_register(&ap83_spi_device);
}

MIPS_MACHINE(AR71XX_MACH_AP83, "Atheros AP83", ap83_setup);
