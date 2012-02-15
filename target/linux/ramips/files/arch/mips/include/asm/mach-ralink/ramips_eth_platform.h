/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   based on Ralink SDK3.3
 *   Copyright (C) 2009 John Crispin <blogic@openwrt.org>
 */

#ifndef _RAMIPS_ETH_PLATFORM_H
#define _RAMIPS_ETH_PLATFORM_H

#include <linux/phy.h>

struct ramips_eth_platform_data
{
	unsigned char mac[6];
	void (*reset_fe)(void);
	int min_pkt_len;
	unsigned long sys_freq;

	int speed;
	int duplex;
	int tx_fc;
	int rx_fc;

	u32 phy_mask;
	phy_interface_t phy_if_mode;
};

#endif /* _RAMIPS_ETH_PLATFORM_H */

