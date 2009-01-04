/*
 *  NETGEAR WNR2000 board support
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
static struct mtd_partition wnr2000_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "u-boot-env",
		.offset		= 0x040000,
		.size		= 0x010000,
	} , {
		.name		= "rootfs",
		.offset		= 0x050000,
		.size		= 0x240000,
	} , {
		.name		= "user-config",
		.offset		= 0x290000,
		.size		= 0x010000,
	} , {
		.name		= "uImage",
		.offset		= 0x2a0000,
		.size		= 0x120000,
	} , {
		.name		= "language_table",
		.offset		= 0x3c0000,
		.size		= 0x020000,
	} , {
		.name		= "rootfs_checksum",
		.offset		= 0x3e0000,
		.size		= 0x010000,
	} , {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wnr2000_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wnr2000_partitions,
        .nr_parts       = ARRAY_SIZE(wnr2000_partitions),
#endif
};

static struct spi_board_info wnr2000_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data  = &wnr2000_flash_data,
	}
};

static void __init wnr2000_setup(void)
{
	ar71xx_add_device_spi(NULL, wnr2000_spi_info,
					ARRAY_SIZE(wnr2000_spi_info));

	ar91xx_add_device_wmac();
}

MIPS_MACHINE(AR71XX_MACH_WNR2000, "NETGEAR WNR2000", wnr2000_setup);
