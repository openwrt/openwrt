/*
 *  Atheros AR71xx SoC device definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __AR71XX_DEVICES_H
#define __AR71XX_DEVICES_H

#include <asm/mach-ar71xx/platform.h>

struct platform_device;

void ar71xx_add_device_spi(struct ar71xx_spi_platform_data *pdata,
			   struct spi_board_info const *info,
			   unsigned n) __init;

extern unsigned char ar71xx_mac_base[] __initdata;
void ar71xx_parse_mac_addr(char *mac_str) __init;
void ar71xx_init_mac(unsigned char *dst, const unsigned char *src,
		     int offset) __init;

struct ar71xx_eth_pll_data {
	u32	pll_10;
	u32	pll_100;
	u32	pll_1000;
};

extern struct ar71xx_eth_pll_data ar71xx_eth0_pll_data;
extern struct ar71xx_eth_pll_data ar71xx_eth1_pll_data;

extern struct ag71xx_platform_data ar71xx_eth0_data;
extern struct ag71xx_platform_data ar71xx_eth1_data;
extern struct platform_device ar71xx_eth0_device;
extern struct platform_device ar71xx_eth1_device;
void ar71xx_add_device_eth(unsigned int id) __init;

extern struct ag71xx_switch_platform_data ar71xx_switch_data;

extern struct platform_device ar71xx_mdio0_device;
extern struct platform_device ar71xx_mdio1_device;
void ar71xx_add_device_mdio(unsigned int id, u32 phy_mask) __init;

void ar71xx_add_device_uart(void) __init;

void ar71xx_add_device_wdt(void) __init;

#endif /* __AR71XX_DEVICES_H */
