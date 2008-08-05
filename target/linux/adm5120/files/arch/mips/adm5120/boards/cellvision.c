/*
 *  $Id$
 *
 *  Cellvision/SparkLAN boards
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

#define CAS6XX_GPIO_DEV_MASK	(1 << ADM5120_GPIO_PIN5)
#define CAS7XX_GPIO_DEV_MASK	(1 << ADM5120_GPIO_PIN5)
#define NFS_GPIO_DEV_MASK	(1 << ADM5120_GPIO_PIN5)

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

static struct adm5120_pci_irq cas771_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
	PCIIRQ(3, 0, 1, ADM5120_IRQ_PCI1),
	PCIIRQ(3, 2, 3, ADM5120_IRQ_PCI2)
};

static struct mtd_partition cas6xx_partitions[] = {
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
		.name	= "nvfs1",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 64*1024,
	} , {
		.name	= "nvfs2",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 64*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct mtd_partition cas7xx_partitions[] = {
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
		.name	= "nvfs",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 128*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *cas6xx_devices[] __initdata = {
	&adm5120_flash0_device,
};

static struct platform_device *cas7xx_devices[] __initdata = {
	&adm5120_flash0_device,
};

static void __init cas6xx_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL);	/* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(cas6xx_partitions);
	adm5120_flash0_data.parts = cas6xx_partitions;

	adm5120_gpiodev_resource.start &= ~CAS6XX_GPIO_DEV_MASK;

	/* TODO: setup mac address */
}

static void __init cas7xx_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL);	/* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(cas7xx_partitions);
	adm5120_flash0_data.parts = cas7xx_partitions;

	adm5120_gpiodev_resource.start &= ~CAS7XX_GPIO_DEV_MASK;

	/* TODO: setup mac address */
}

static void __init nfs_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL);	/* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(cas6xx_partitions);
	adm5120_flash0_data.parts = cas6xx_partitions;

	adm5120_gpiodev_resource.start &= ~NFS_GPIO_DEV_MASK;

	/* TODO: setup mac address */
}

unsigned char nfs_vlans[6] = { /* TODO: not tested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

/*--------------------------------------------------------------------------*/

ADM5120_BOARD_START(CAS630, "Cellvision CAS-630/630W")
	.board_setup	= cas6xx_setup,
	.eth_num_ports	= 1,
	.num_devices	= ARRAY_SIZE(cas6xx_devices),
	.devices	= cas6xx_devices,
	/* TODO: PCI IRQ map */
ADM5120_BOARD_END

ADM5120_BOARD_START(CAS670, "Cellvision CAS-670/670W")
	.board_setup	= cas6xx_setup,
	.eth_num_ports	= 1,
	.num_devices	= ARRAY_SIZE(cas6xx_devices),
	.devices	= cas6xx_devices,
	/* TODO: PCI IRQ map */
ADM5120_BOARD_END

ADM5120_BOARD_START(CAS700, "Cellvision CAS-700/700W")
	.board_setup	= cas7xx_setup,
	.eth_num_ports	= 1,
	.num_devices	= ARRAY_SIZE(cas7xx_devices),
	.devices	= cas7xx_devices,
	/* TODO: PCI IRQ map */
ADM5120_BOARD_END

ADM5120_BOARD_START(CAS771, "Cellvision CAS-771/771W")
	.board_setup	= cas7xx_setup,
	.eth_num_ports	= 1,
	.num_devices	= ARRAY_SIZE(cas7xx_devices),
	.devices	= cas7xx_devices,
	.pci_nr_irqs	= ARRAY_SIZE(cas771_pci_irqs),
	.pci_irq_map	= cas771_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(CAS790, "Cellvision CAS-790")
	.board_setup	= cas7xx_setup,
	.eth_num_ports	= 1,
	.num_devices	= ARRAY_SIZE(cas7xx_devices),
	.devices	= cas7xx_devices,
	/* TODO: PCI IRQ map */
ADM5120_BOARD_END

ADM5120_BOARD_START(CAS861, "Cellvision CAS-861/861W")
	.board_setup	= cas7xx_setup,
	.eth_num_ports	= 1,
	.num_devices	= ARRAY_SIZE(cas7xx_devices),
	.devices	= cas7xx_devices,
	/* TODO: PCI IRQ map */
ADM5120_BOARD_END

ADM5120_BOARD_START(NFS101U, "Cellvision NFS-101U/101WU")
	.board_setup	= nfs_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= nfs_vlans,
	.num_devices	= ARRAY_SIZE(cas6xx_devices),
	.devices	= cas6xx_devices,
	/* TODO: PCI IRQ map */
ADM5120_BOARD_END
