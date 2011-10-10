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

struct spi_board_info spi_info = {
	.bus_num        = 0,
	.chip_select    = 3,
	.max_speed_hz   = 25000000,
	.modalias       = "mx25l12805d",
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
