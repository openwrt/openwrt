/*
 *  $Id$
 *
 *  Compex boards
 *
 *  Copyright (C) 2007-2008 OpenWrt.org
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg at openwrt.org>
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

#define NP27G_GPIO_DEV_MASK	(1 << ADM5120_GPIO_PIN5)
#define NP28G_GPIO_DEV_MASK	( 1 << ADM5120_GPIO_PIN5 \
				| 1 << ADM5120_GPIO_PIN4)

#define WP54_GPIO_DEV_MASK	( 1 << ADM5120_GPIO_PIN5 \
				| 1 << ADM5120_GPIO_PIN3)

static struct adm5120_pci_irq wp54_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
};

static struct adm5120_pci_irq np28g_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
	PCIIRQ(3, 0, 1, ADM5120_IRQ_PCI0),
	PCIIRQ(3, 1, 2, ADM5120_IRQ_PCI1),
	PCIIRQ(3, 2, 3, ADM5120_IRQ_PCI2)
};

static struct mtd_partition wp54g_wrt_partitions[] = {
	{
		.name	= "cfe",
		.offset	= 0,
		.size	= 0x050000,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "trx",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 0x3A0000,
	} , {
		.name	= "nvram",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 0x010000,
	}
};

static struct platform_device *np2xg_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_hcd_device,
};

static struct platform_device *wp54_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_buttons_device,
};

unsigned char np27g_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

unsigned char np28g_vlans[6] __initdata = {
	0x50, 0x42, 0x44, 0x48, 0x00, 0x00
};

unsigned char wp54_vlans[6] __initdata = {
	0x41, 0x42, 0x00, 0x00, 0x00, 0x00
};

/*--------------------------------------------------------------------------*/

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

static void wp54_reset(void)
{
	gpio_set_value(ADM5120_GPIO_PIN3, 0);
}

static void np28g_reset(void)
{
	gpio_set_value(ADM5120_GPIO_PIN4, 0);
}

static void __init np27g_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_gpiodev_resource.start &= ~NP27G_GPIO_DEV_MASK;

	/* TODO: setup mac address */
}

static void __init np28g_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	gpio_request(ADM5120_GPIO_PIN4, NULL); /* for system reset */
	gpio_direction_output(ADM5120_GPIO_PIN4, 1);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;

	adm5120_gpiodev_resource.start &= ~NP28G_GPIO_DEV_MASK;

	/* TODO: setup mac address */
}

static void __init wp54_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	gpio_request(ADM5120_GPIO_PIN3, NULL); /* for system reset */
	gpio_direction_output(ADM5120_GPIO_PIN3, 1);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;

	adm5120_buttons_data.nbuttons = 1;
	adm5120_buttons[0].desc = "reset button";
	adm5120_buttons[0].gpio = ADM5120_GPIO_PIN4;

	adm5120_gpiodev_resource.start &= ~WP54_GPIO_DEV_MASK;

	/* TODO: setup mac address */
}

static void __init wp54_wrt_setup(void)
{
	wp54_setup();

	adm5120_flash0_data.nr_parts = ARRAY_SIZE(wp54g_wrt_partitions);
	adm5120_flash0_data.parts = wp54g_wrt_partitions;

	/* TODO: setup mac address */
}

/*--------------------------------------------------------------------------*/

ADM5120_BOARD_START(NP27G, "Compex NetPassage 27G")
	.board_setup	= np27g_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= np27g_vlans,
	.num_devices	= ARRAY_SIZE(np2xg_devices),
	.devices	= np2xg_devices,
	/* TODO: add PCI IRQ map */
ADM5120_BOARD_END

ADM5120_BOARD_START(NP28G, "Compex NetPassage 28G")
	.board_setup	= np28g_setup,
	.board_reset	= np28g_reset,
	.eth_num_ports	= 4,
	.eth_vlans	= np28g_vlans,
	.num_devices	= ARRAY_SIZE(np2xg_devices),
	.devices	= np2xg_devices,
	.pci_nr_irqs	= ARRAY_SIZE(np28g_pci_irqs),
	.pci_irq_map	= np28g_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(WP54AG, "Compex WP54AG")
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
	.pci_nr_irqs	= ARRAY_SIZE(wp54_pci_irqs),
	.pci_irq_map	= wp54_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(WP54G, "Compex WP54G")
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
	.pci_nr_irqs	= ARRAY_SIZE(wp54_pci_irqs),
	.pci_irq_map	= wp54_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(WP54G_WRT, "Compex WP54G-WRT")
	.board_setup	= wp54_wrt_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
	.pci_nr_irqs	= ARRAY_SIZE(wp54_pci_irqs),
	.pci_irq_map	= wp54_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(WPP54AG, "Compex WPP54AG")
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
	.pci_nr_irqs	= ARRAY_SIZE(wp54_pci_irqs),
	.pci_irq_map	= wp54_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(WPP54G, "Compex WPP54G")
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
	.pci_nr_irqs	= ARRAY_SIZE(wp54_pci_irqs),
	.pci_irq_map	= wp54_pci_irqs,
ADM5120_BOARD_END
