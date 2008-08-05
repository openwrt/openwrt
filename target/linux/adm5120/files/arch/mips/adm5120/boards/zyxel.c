/*
 *  $Id$
 *
 *  ZyXEL Prestige P-334/P-335 boards
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
#include <adm5120_platform.h>
#include <adm5120_irq.h>

#define P33X_GPIO_DEV_MASK	(1 << ADM5120_GPIO_PIN5)

static void switch_bank_gpio5(unsigned bank)
{
	switch (bank) {
	case 0:
		gpio_set_value(ADM5120_GPIO_PIN5, 0);
		break;
	case 1:
		gpio_set_value(ADM5120_GPIO_PIN5, 1);
		break;
	}
}

static struct adm5120_pci_irq p33x_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
};

static struct mtd_partition p33x_partitions[] = {
	{
		.name	= "bootbase",
		.offset	= 0,
		.size	= 16*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "rom",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 16*1024,
	} , {
		.name	= "bootext",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 96*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "trx",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	} , {
		.name	= "firmware",
		.offset	= 32*1024,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *p334_devices[] __initdata = {
	&adm5120_flash0_device,
};

static struct platform_device *p335_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_hcd_device,
};

static void __init p33x_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(p33x_partitions);
	adm5120_flash0_data.parts = p33x_partitions;

	adm5120_gpiodev_resource.start &= ~P33X_GPIO_DEV_MASK;
	/* TODO: setup mac address */
}

unsigned char p33x_vlans[6] __initdata = {
	/* FIXME: untested */
	0x50, 0x48, 0x44, 0x42, 0x41, 0x00
};

ADM5120_BOARD_START(P334WT, "ZyXEL Prestige 334WT")
	.board_setup	= p33x_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= p33x_vlans,
	.num_devices	= ARRAY_SIZE(p334_devices),
	.devices	= p334_devices,
	.pci_nr_irqs	= ARRAY_SIZE(p33x_pci_irqs),
	.pci_irq_map	= p33x_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(P335, "ZyXEL Prestige 335/335WT")
	.board_setup	= p33x_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= p33x_vlans,
	.num_devices	= ARRAY_SIZE(p335_devices),
	.devices	= p335_devices,
	.pci_nr_irqs	= ARRAY_SIZE(p33x_pci_irqs),
	.pci_irq_map	= p33x_pci_irqs,
ADM5120_BOARD_END
