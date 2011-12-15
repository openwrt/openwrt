/*
 *  Atheros AR71xx SoC specific platform data definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
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

struct ag71xx_switch_platform_data {
	int		dummy;
};

struct ag71xx_platform_data {
	phy_interface_t	phy_if_mode;
	u32		phy_mask;
	int		speed;
	int		duplex;
	u32		reset_bit;
	u8		mac_addr[ETH_ALEN];
	struct device	*mii_bus_dev;

	u8		has_gbit:1;
	u8		is_ar91xx:1;
	u8		is_ar7240:1;
	u8		is_ar724x:1;
	u8		has_ar8216:1;

	struct ag71xx_switch_platform_data *switch_data;

	void		(*ddr_flush)(void);
	void		(*set_speed)(int speed);

	u32		fifo_cfg1;
	u32		fifo_cfg2;
	u32		fifo_cfg3;
};

struct ag71xx_mdio_platform_data {
	u32		phy_mask;
	int		is_ar7240;
};

struct ar71xx_ehci_platform_data {
	u8		is_ar91xx;
};

struct ar71xx_spi_platform_data {
	unsigned	bus_num;
	unsigned	num_chipselect;
	u32		(*get_ioc_base)(u8 chip_select, int cs_high, int is_on);
};

#define AR71XX_SPI_CS_INACTIVE	0
#define AR71XX_SPI_CS_ACTIVE	1

#endif /* __ASM_MACH_AR71XX_PLATFORM_H */
