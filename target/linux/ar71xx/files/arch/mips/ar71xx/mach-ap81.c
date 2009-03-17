/*
 *  Atheros AP81 board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2009 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/input.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"

#define AP81_GPIO_BTN_SW4	12
#define AP81_GPIO_BTN_SW1	21

#define AP81_BUTTONS_POLL_INTERVAL	20

static struct gpio_button ap81_gpio_buttons[] __initdata = {
	{
		.desc		= "sw1",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= AP81_GPIO_BTN_SW1,
		.active_low	= 1,
	} , {
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= AP81_GPIO_BTN_SW4,
		.active_low	= 1,
	}
};

static void __init ap81_setup(void)
{
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

	ar71xx_add_device_usb();

	ar71xx_add_device_gpio_buttons(-1, AP81_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(ap81_gpio_buttons),
					ap81_gpio_buttons);

	ar91xx_add_device_wmac();
}

MIPS_MACHINE(AR71XX_MACH_AP81, "Atheros AP81", ap81_setup);
