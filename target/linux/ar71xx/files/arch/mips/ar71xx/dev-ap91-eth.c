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
#include "dev-dsa.h"
#include "dev-ap91-eth.h"

static struct dsa_chip_data ap91_dsa_chip = {
	.port_names[0]  = "cpu",
	.port_names[1]  = "lan1",
	.port_names[2]  = "lan2",
	.port_names[3]  = "lan3",
	.port_names[4]  = "lan4",
};

static struct dsa_platform_data ap91_dsa_data = {
	.nr_chips	= 1,
	.chip		= &ap91_dsa_chip,
};

static void ap91_eth_set_port_name(unsigned port, const char *name)
{
	if (port < 1 || port > 5)
		return;

	if (name)
		ap91_dsa_chip.port_names[port] = (char *) name;
}

void __init ap91_eth_init(u8 *mac_addr, const char *port_names[])
{
	if (mac_addr)
		ar71xx_set_mac_base(mac_addr);

	if (port_names) {
		int i;

		for (i = 0; i < AP91_ETH_NUM_PORT_NAMES; i++)
			ap91_eth_set_port_name(i + 1, port_names[i]);
	}

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

	ar71xx_add_device_dsa(1, &ap91_dsa_data);
}
