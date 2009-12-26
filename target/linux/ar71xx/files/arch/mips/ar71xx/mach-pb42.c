/*
 *  Atheros PB42 board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-pb42-pci.h"
#include "dev-usb.h"

#define PB42_BUTTONS_POLL_INTERVAL	20

#define PB42_GPIO_BTN_SW4	8
#define PB42_GPIO_BTN_SW5	3

static struct gpio_button pb42_gpio_buttons[] __initdata = {
	{
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= PB42_GPIO_BTN_SW4,
		.active_low	= 1,
	} , {
		.desc		= "sw5",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= PB42_GPIO_BTN_SW5,
		.active_low	= 1,
	}
};

#define PB42_WAN_PHYMASK	BIT(20)
#define PB42_LAN_PHYMASK	(BIT(16) | BIT(17) | BIT(18) | BIT(19))
#define PB42_MDIO_PHYMASK	(PB42_LAN_PHYMASK | PB42_WAN_PHYMASK)

static void __init pb42_init(void)
{
	ar71xx_add_device_m25p80(NULL);

	ar71xx_add_device_mdio(~PB42_MDIO_PHYMASK);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = PB42_WAN_PHYMASK;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = PB42_LAN_PHYMASK;
	ar71xx_eth1_data.speed = SPEED_100;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_gpio_buttons(-1, PB42_BUTTONS_POLL_INTERVAL,
				       ARRAY_SIZE(pb42_gpio_buttons),
				       pb42_gpio_buttons);

	pb42_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_PB42, "Atheros PB42", pb42_init);
