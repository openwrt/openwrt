/*
 *  Atheros AR71xx SoC specific platform definitions
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_AR71XX_PLATFORM_H
#define __ASM_MACH_AR71XX_PLATFORM_H

#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/phy.h>
#include <linux/spi/spi.h>
#include <linux/leds.h>
#include <linux/gpio_buttons.h>

struct ag71xx_platform_data {
	phy_interface_t	phy_if_mode;
	u32		phy_mask;
	int		speed;
	int		duplex;
	u32		reset_bit;
	u32		flush_reg;
	u32		mii_if;
	u8		mac_addr[ETH_ALEN];
};

struct ag71xx_mdio_platform_data {
	u32		phy_mask;
};

struct ar71xx_spi_platform_data {
	unsigned	bus_num;
	unsigned	num_chipselect;
	u32		(*get_ioc_base)(u8 chip_select, int cs_high, int is_on);
};

#define AR71XX_SPI_CS_INACTIVE	0
#define AR71XX_SPI_CS_ACTIVE	1

extern void ar71xx_add_device_spi(struct ar71xx_spi_platform_data *pdata,
				struct spi_board_info const *info,
				unsigned n) __init;

extern void ar71xx_set_mac_base(unsigned char *mac) __init;
extern void ar71xx_parse_mac_addr(char *mac_str) __init;

extern struct ag71xx_platform_data ar71xx_eth0_data;
extern struct ag71xx_platform_data ar71xx_eth1_data;
extern void ar71xx_add_device_eth(unsigned int id) __init;

extern void ar71xx_add_device_mdio(u32 phy_mask) __init;

extern void ar71xx_add_device_leds_gpio(int id,
				   unsigned num_leds,
				   struct gpio_led *leds) __init;

extern void ar71xx_add_device_gpio_buttons(int id,
				   unsigned poll_interval,
				   unsigned nbuttons,
				   struct gpio_button *buttons) __init;

#endif /* __ASM_MACH_AR71XX_PLATFORM_H */
