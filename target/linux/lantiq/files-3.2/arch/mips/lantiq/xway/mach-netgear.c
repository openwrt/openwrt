/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/phy.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <lantiq_soc.h>
#include <irq.h>

#include "../machtypes.h"
#include "devices.h"

static struct ltq_pci_data ltq_pci_data = {
	.clock	= PCI_CLOCK_INT,
	.gpio	= PCI_GNT1 | PCI_REQ1,
	.irq	= {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode = PHY_INTERFACE_MODE_MII,
};

static struct mtd_partition easy98000_nor_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x40000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x40000,
		.size	= 0x40000,	/* 2 sectors for redundant env. */
	},
	{
		.name	= "linux",
		.offset	= 0x80000,
		.size	= 0xF80000,	/* map only 16 MiB */
	},
};

static struct flash_platform_data easy98000_spi_flash_platform_data = {
	.name = "sflash",
	.parts = easy98000_nor_partitions,
	.nr_parts = ARRAY_SIZE(easy98000_nor_partitions)
};

static struct spi_board_info spi_info __initdata = {
	.modalias		= "m25p80",
	.bus_num		= 0,
	.chip_select		= 3,
	.max_speed_hz		= 10 * 1000 * 1000,
	.mode			= SPI_MODE_3,
	.platform_data		= &easy98000_spi_flash_platform_data
};

struct ltq_spi_platform_data ltq_spi_data = {
	.num_chipselect = 4,
};

static void __init dgn3500_init(void)
{
	ltq_register_pci(&ltq_pci_data);
	ltq_register_etop(&ltq_eth_data);
	ltq_register_spi(&ltq_spi_data, &spi_info, 1);
}

MIPS_MACHINE(LANTIQ_MACH_DGN3500B,
	     "DGN3500B",
	     "Netgear DGN3500B",
	      dgn3500_init);
