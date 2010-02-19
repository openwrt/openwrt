/*
 *  Atheros AP91 reference board ethernet initialization
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "devices.h"

void __init ap91_eth_init(u8 *mac_addr)
{
	if (mac_addr)
		ar71xx_set_mac_base(mac_addr);

	/* WAN port */
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = 0x0;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.fifo_cfg1 = 0x0fff0000;
	ar71xx_eth0_data.fifo_cfg2 = 0x00001fff;
	ar71xx_eth0_data.fifo_cfg3 = 0x008001ff;

	/* LAN ports */
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x0;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;
	ar71xx_eth1_data.fifo_cfg1 = 0x0fff0000;
	ar71xx_eth1_data.fifo_cfg2 = 0x00001fff;
	ar71xx_eth1_data.fifo_cfg3 = 0x008001ff;

	ar71xx_add_device_mdio(0x0);
	ar71xx_add_device_eth(1);
	ar71xx_add_device_eth(0);
}
