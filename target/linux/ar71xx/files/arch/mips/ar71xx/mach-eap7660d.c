/*
 *  Senao EAP7660D board support
 *
 *  Copyright (C) 2010 Daniel Golle <daniel.golle@gmail.com>
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-pb42-pci.h"
#include "dev-m25p80.h"

#define EAP7660D_BUTTONS_POLL_INTERVAL	20
#define EAP7660D_GPIO_DS4		7
#define EAP7660D_GPIO_DS5		2
#define EAP7660D_GPIO_DS7		0
#define EAP7660D_GPIO_DS8		4
#define EAP7660D_GPIO_SW1		3
#define EAP7660D_GPIO_SW3		8
#define EAP7660D_PHYMASK		BIT(20)

static struct gpio_led eap7660d_leds_gpio[] __initdata = {
	{
		.name		= "eap7660d:green:ds8",
		.gpio		= EAP7660D_GPIO_DS8,
		.active_low	= 0,
	},
	{
		.name		= "eap7660d:green:ds5",
		.gpio		= EAP7660D_GPIO_DS5,
		.active_low	= 0,
	},
	{
		.name		= "eap7660d:green:ds7",
		.gpio		= EAP7660D_GPIO_DS7,
		.active_low	= 0,
	},
	{
		.name		= "eap7660d:green:ds4",
		.gpio		= EAP7660D_GPIO_DS4,
		.active_low	= 0,
	}
};

static struct gpio_button eap7660d_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= EAP7660D_GPIO_SW1,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= EAP7660D_GPIO_SW3,
		.active_low	= 1,
	}
};

static void __init eap7660d_setup(void)
{
	ar71xx_add_device_mdio(~EAP7660D_PHYMASK);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = EAP7660D_PHYMASK;
	ar71xx_add_device_eth(0);
	ar71xx_add_device_m25p80(NULL);
	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(eap7660d_leds_gpio),
					eap7660d_leds_gpio);
	ar71xx_add_device_gpio_buttons(-1, EAP7660D_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(eap7660d_gpio_buttons),
					eap7660d_gpio_buttons);
	pb42_pci_init();
};

MIPS_MACHINE(AR71XX_MACH_EAP7660D, "EAP7660D", "Senao EAP7660D",
	     eap7660d_setup);
