/*
 *  $Id$
 *
 *  Edimax boards
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/bootinfo.h>
#include <asm/gpio.h>

#include <adm5120_board.h>
#include <adm5120_irq.h>
#include <adm5120_platform.h>

static struct adm5120_pci_irq br61xx_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
};

static struct mtd_partition br61xx_partitions[] = {
	{
		.name	= "admboot",
		.offset	= 0,
		.size	= 32*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "config",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 32*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *br6104k_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_hcd_device,
};

static struct platform_device *br61x4wg_devices[] __initdata = {
	&adm5120_flash0_device,
};

static void __init br61xx_setup(void) {
	/* setup data for flash0 device */
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(br61xx_partitions);
	adm5120_flash0_data.parts = br61xx_partitions;

	/* TODO: setup mac addresses, if possible */
}

unsigned char br61xx_vlans[6] = {
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

/*--------------------------------------------------------------------------*/

ADM5120_BOARD_START(BR6104K, "Edimax BR-6104K/6104KP")
	.board_setup	= br61xx_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= br61xx_vlans,
	.num_devices	= ARRAY_SIZE(br6104k_devices),
	.devices	= br6104k_devices,
ADM5120_BOARD_END

ADM5120_BOARD_START(BR61x4WG, "Edimax BR-6104WG/6114WG")
	.board_setup	= br61xx_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= br61xx_vlans,
	.num_devices	= ARRAY_SIZE(br61x4wg_devices),
	.devices	= br61x4wg_devices,
	.pci_nr_irqs	= ARRAY_SIZE(br61xx_pci_irqs),
	.pci_irq_map	= br61xx_pci_irqs,
ADM5120_BOARD_END
