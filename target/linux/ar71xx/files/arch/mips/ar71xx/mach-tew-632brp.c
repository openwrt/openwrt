/*
 *  TrendNET TEW-632BRP board support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
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
#include <asm/mach-ar71xx/platform.h>

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tew_632brp_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config",
		.offset		= 0x020000,
		.size		= 0x010000,
	} , {
		.name		= "kernel",
		.offset		= 0x030000,
		.size		= 0x0c0000,
	} , {
		.name		= "rootfs",
		.offset		= 0x0f0000,
		.size		= 0x2f0000,
	} , {
		.name		= "art",
		.offset		= 0x3e0000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x030000,
		.size		= 0x3b0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tew_632brp_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = tew_632brp_partitions,
        .nr_parts       = ARRAY_SIZE(tew_632brp_partitions),
#endif
};

static struct spi_board_info tew_632brp_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data  = &tew_632brp_flash_data,
	}
};

static void __init tew_632brp_setup(void)
{
	ar71xx_add_device_mdio(0xfffffffe);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = 0x0;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_spi(NULL, tew_632brp_spi_info,
					ARRAY_SIZE(tew_632brp_spi_info));
}

MIPS_MACHINE(AR71XX_MACH_TEW_632BRP, "TRENDnet TEW-632BRP", tew_632brp_setup);
