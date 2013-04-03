/*
 * Ralink RT3662/3883 SoC specific platform device definitions
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RT3883_DEVICES_H
#define _RT3883_DEVICES_H

struct physmap_flash_data;
struct spi_board_info;

extern struct physmap_flash_data rt3883_flash0_data;
extern struct physmap_flash_data rt3883_flash1_data;
void rt3883_register_pflash(unsigned int id);

extern struct ramips_nand_platform_data rt3883_nand_data;
void rt3883_register_nand(void);

extern struct ramips_eth_platform_data rt3883_eth_data;
void rt3883_register_ethernet(void);
void rt3883_register_usbhost(void);

extern struct rt2x00_platform_data rt3883_wlan_data;
void rt3883_register_wlan(void);
void rt3883_register_wdt(bool enable_reset);

void rt3883_register_spi(struct spi_board_info *info, int n);

#endif  /* _RT3883_DEVICES_H */
