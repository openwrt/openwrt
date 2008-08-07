/*
 *  Cellvision/SparkLAN boards
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>

#include <asm/bootinfo.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_board.h>
#include <asm/mach-adm5120/adm5120_platform.h>
#include <asm/mach-adm5120/adm5120_irq.h>

#define CELLVISION_GPIO_FLASH_A20	ADM5120_GPIO_PIN5
#define CELLVISION_GPIO_DEV_MASK	(1 << CELLVISION_GPIO_FLASH_A20)

#ifdef CONFIG_MTD_PARTITIONS
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
#endif /* CONFIG_MTD_PARTITIONS */

static struct adm5120_pci_irq cas771_pci_irqs[] __initdata = {
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI0),
	PCIIRQ(3, 0, 1, ADM5120_IRQ_PCI1),
	PCIIRQ(3, 2, 3, ADM5120_IRQ_PCI2)
};

static struct gpio_led cas771_gpio_leds[] __initdata = {
	GPIO_LED_STD(ADM5120_GPIO_PIN0, "cam_flash",	NULL),
	/* GPIO PIN3 is the reset */
	GPIO_LED_STD(ADM5120_GPIO_PIN6, "access",	NULL),
	GPIO_LED_STD(ADM5120_GPIO_P0L1, "status",	NULL),
	GPIO_LED_STD(ADM5120_GPIO_P0L2, "diag",		NULL),
};

static void switch_bank_gpio5(unsigned bank)
{
	switch (bank) {
	case 0:
		gpio_set_value(CELLVISION_GPIO_FLASH_A20, 0);
		break;
	case 1:
		gpio_set_value(CELLVISION_GPIO_FLASH_A20, 1);
		break;
	}
}

static void __init cellvision_generic_setup(void)
{
	/* setup flash A20 line */
	gpio_request(CELLVISION_GPIO_FLASH_A20, NULL);
	gpio_direction_output(CELLVISION_GPIO_FLASH_A20, 0);

	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_add_device_flash(0);
}

static void __init cas6xx_setup(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(cas6xx_partitions);
	adm5120_flash0_data.parts = cas6xx_partitions;
#endif /* CONFIG_MTD_PARTITIONS */
	cellvision_generic_setup();

	adm5120_add_device_switch(1, NULL);
}

ADM5120_BOARD(MACH_ADM5120_CAS630, "Cellvision CAS-630/630W", cas6xx_setup);
ADM5120_BOARD(MACH_ADM5120_CAS670, "Cellvision CAS-670/670W", cas6xx_setup);

static void __init cas7xx_setup(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(cas7xx_partitions);
	adm5120_flash0_data.parts = cas7xx_partitions;
#endif /* CONFIG_MTD_PARTITIONS */

	adm5120_add_device_switch(1, NULL);
}

ADM5120_BOARD(MACH_ADM5120_CAS700, "Cellvision CAS-700/700W", cas7xx_setup);
ADM5120_BOARD(MACH_ADM5120_CAS790, "Cellvision CAS-790", cas7xx_setup);
ADM5120_BOARD(MACH_ADM5120_CAS861, "Cellvision CAS-861/861W", cas7xx_setup);

static void __init cas771_setup(void)
{
	cas7xx_setup();
	adm5120_add_device_gpio_leds(ARRAY_SIZE(cas771_gpio_leds),
					cas771_gpio_leds);
	adm5120_pci_set_irq_map(ARRAY_SIZE(cas771_pci_irqs), cas771_pci_irqs);
}

ADM5120_BOARD(MACH_ADM5120_CAS771, "Cellvision CAS-771/771W", cas771_setup);

static u8 nfs_vlans[6] __initdata = { /* TODO: not tested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

static void __init nfs_setup(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(cas6xx_partitions);
	adm5120_flash0_data.parts = cas6xx_partitions;
#endif /* CONFIG_MTD_PARTITIONS */

	cellvision_generic_setup();
	adm5120_add_device_switch(5, nfs_vlans);

	/* TODO: add PCI IRQ map */
}

ADM5120_BOARD(MACH_ADM5120_NFS101U, "Cellvision NFS-101U/101WU", nfs_setup);
