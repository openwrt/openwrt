/*
 *  Linksys WRT160NL board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wrt160nl_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "kernel",
		.offset		= 0x040000,
		.size		= 0x0e0000,
	} , {
		.name		= "filesytem",
		.offset		= 0x120000,
		.size		= 0x6c0000,
	} , {
		.name		= "nvram",
		.offset		= 0x7e0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "ART",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x040000,
		.size		= 0x7a0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wrt160nl_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wrt160nl_partitions,
        .nr_parts       = ARRAY_SIZE(wrt160nl_partitions),
#endif
};

static struct spi_board_info wrt160nl_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data  = &wrt160nl_flash_data,
	}
};

static void __init wrt160nl_setup(void)
{
	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = 0xf;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_spi(NULL, wrt160nl_spi_info,
			      ARRAY_SIZE(wrt160nl_spi_info));

	ar71xx_add_device_usb();
	ar91xx_add_device_wmac();

	/* TODO: LEDs, buttons */
}

MIPS_MACHINE(AR71XX_MACH_WRT160NL, "Linksys WRT160NL", wrt160nl_setup);
