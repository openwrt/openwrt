/*
 *  Atheros AP83 board support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/input.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>
#include <asm/mach-ar71xx/platform.h>

static void __init ap83_setup(void)
{
	ar71xx_add_device_mdio(0xfffffffe);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0x1;

	ar71xx_add_device_eth(0);
}

MIPS_MACHINE(MACH_AR71XX_AP83, "Atheros AP83", ap83_setup);
