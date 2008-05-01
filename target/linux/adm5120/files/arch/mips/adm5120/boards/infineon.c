/*
 *  $Id$
 *
 *  Infineon boards
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007,2008 Gabor Juhos <juhosg at openwrt.org>
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

#include <adm5120_defs.h>
#include <adm5120_irq.h>
#include <adm5120_board.h>
#include <adm5120_platform.h>
#include <adm5120_info.h>

static void switch_bank_gpio3(unsigned bank)
{
	switch (bank) {
	case 0:
		gpio_set_value(ADM5120_GPIO_PIN3, 0);
		break;
	case 1:
		gpio_set_value(ADM5120_GPIO_PIN3, 1);
		break;
	}
}

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

static struct mtd_partition easy_partitions[] = {
	{
		.name	= "admboot",
		.offset	= 0,
		.size	= 64*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "boardcfg",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 64*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct adm5120_pci_irq easy5120rt_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
};

static struct platform_device *easy5120pata_devices[] __initdata = {
	&adm5120_flash0_device,
	/* TODO: add VINETIC2 device? */
};

static struct platform_device *easy5120rt_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_hcd_device,
};

static struct platform_device *easy5120wvoip_devices[] __initdata = {
	&adm5120_flash0_device,
	/* TODO: add VINETIC2 device? */
};

static struct platform_device *easy83000_devices[] __initdata = {
	&adm5120_flash0_device,
	/* TODO: add VINAX device? */
};

static void __init easy_setup_pqfp(void)
{
	gpio_request(ADM5120_GPIO_PIN3, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN3, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio3;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(easy_partitions);
	adm5120_flash0_data.parts = easy_partitions;

	/* TODO: setup mac addresses */
}

static void __init easy_setup_bga(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(easy_partitions);
	adm5120_flash0_data.parts = easy_partitions;

	/* TODO: setup mac addresses */
}

unsigned char easy5120rt_vlans[6] = {
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

/*--------------------------------------------------------------------------*/

ADM5120_BOARD_START(EASY5120PATA, "Infineon EASY 5120P-ATA Reference Board")
	.board_setup	= easy_setup_pqfp,
	.eth_num_ports	= 6,
	.num_devices	= ARRAY_SIZE(easy5120pata_devices),
	.devices	= easy5120pata_devices,
ADM5120_BOARD_END

ADM5120_BOARD_START(EASY5120RT, "Infineon EASY 5120-RT Reference Board")
	.board_setup	= easy_setup_bga,
	.eth_num_ports	= 5,
	.eth_vlans	= easy5120rt_vlans,
	.num_devices	= ARRAY_SIZE(easy5120rt_devices),
	.devices	= easy5120rt_devices,
	.pci_nr_irqs	= ARRAY_SIZE(easy5120rt_pci_irqs),
	.pci_irq_map	= easy5120rt_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(EASY5120WVOIP, "Infineon EASY 5120-WVoIP Reference Board")
	.board_setup	= easy_setup_bga,
	.eth_num_ports	= 6,
	.num_devices	= ARRAY_SIZE(easy5120wvoip_devices),
	.devices	= easy5120wvoip_devices,
ADM5120_BOARD_END

ADM5120_BOARD_START(EASY83000, "Infineon EASY 83000 Reference Board")
	.board_setup	= easy_setup_pqfp,
	.eth_num_ports	= 6,
	.num_devices	= ARRAY_SIZE(easy83000_devices),
	.devices	= easy83000_devices,
ADM5120_BOARD_END
