/*
 *  Netgear WNDR3700 board support
 *
 *  Copyright (C) 2009 Marco Porsch
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
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
#include <linux/input.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "devices.h"

#define WNDR3700_GPIO_LED_WPS_ORANGE	0
#define WNDR3700_GPIO_LED_POWER_ORANGE	1
#define WNDR3700_GPIO_LED_POWER_GREEN	2
#define WNDR3700_GPIO_LED_WPS_GREEN	4

#define WNDR3700_GPIO_BTN_WPS		3
#define WNDR3700_GPIO_BTN_RESET		8
#define WNDR3700_GPIO_BTN_WIFI		11

#define WNDR3700_BUTTONS_POLL_INTERVAL    20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wndr3700_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x050000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "env",
		.offset		= 0x050000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "rootfs",
		.offset		= 0x070000,
		.size		= 0x720000,
	} , {
		.name		= "config",
		.offset		= 0x790000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config_bak",
		.offset		= 0x7a0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "pot",
		.offset		= 0x7b0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "traffic_meter",
		.offset		= 0x7c0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "language",
		.offset		= 0x7d0000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "caldata",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wndr3700_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wndr3700_partitions,
        .nr_parts       = ARRAY_SIZE(wndr3700_partitions),
#endif
};

static struct ar71xx_pci_irq wndr3700_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}, {
		.slot	= 1,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV1,
	}
};

static struct spi_board_info wndr3700_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data  = &wndr3700_flash_data,
	}
};

static struct gpio_led wndr3700_leds_gpio[] __initdata = {
	{
		.name		= "wndr3700:green:power",
		.gpio		= WNDR3700_GPIO_LED_POWER_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:orange:power",
		.gpio		= WNDR3700_GPIO_LED_POWER_ORANGE,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:green:wps",
		.gpio		= WNDR3700_GPIO_LED_WPS_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:orange:wps",
		.gpio		= WNDR3700_GPIO_LED_WPS_ORANGE,
		.active_low	= 1,
	}
};

static struct gpio_button wndr3700_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_RESET,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_WPS,
	} , {
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_2,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_WIFI,
	}
};

static void __init wndr3700_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_set_mac_base(mac);
	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0xf;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_pci_init(ARRAY_SIZE(wndr3700_pci_irqs), wndr3700_pci_irqs);

	ar71xx_add_device_spi(NULL, wndr3700_spi_info,
			      ARRAY_SIZE(wndr3700_spi_info));

        ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wndr3700_leds_gpio),
				    wndr3700_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WNDR3700_BUTTONS_POLL_INTERVAL,
				      ARRAY_SIZE(wndr3700_gpio_buttons),
				      wndr3700_gpio_buttons);
}

MIPS_MACHINE(AR71XX_MACH_WNDR3700, "NETGEAR WNDR3700", wndr3700_setup);
