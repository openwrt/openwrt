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

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"

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

	ar91xx_add_device_wmac();
}

MIPS_MACHINE(AR71XX_MACH_AP81, "Atheros AP81", ap81_setup);
